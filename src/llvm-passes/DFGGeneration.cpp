/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/GetElementPtrTypeIterator.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"

#if LLVM_VERSION_MAJOR >= 14
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#endif

#include <string>
#include <sstream>
#include <list>
#include <map>
#include <algorithm>
#include <cctype>
#include <fstream>

#include <CGRA/OpGraph.h>
#include <CGRA/OpGraphProcedures.h>
#include <CGRA/utility/Collections.h>

using namespace llvm;

namespace
{
    // Used to represent the instructions that are commutative. Candidate for replacement is using llvm::Instruction::isCommutive
    static const std::set<OpGraphOpCode> commutative_ops = { OpCode::ADD,   OpCode::MUL, OpCode::AND, OpCode::OR, OpCode::XOR};

    cl::opt<std::string> inputTagPairs("in-tag-pairs", cl::Positional, cl::desc("<input file that contains tag number and string pairs>"));

    cl::opt<std::string> loopTags("loop-tags", cl::desc("Input a list of loop tag names to generate DFG for"));

    OpGraphOpCode LLVMtoOp(const Instruction& I)
    {
        // MODIFICATION START: 识别自定义函数调用
        // 优先检查指令是否为对我们自定义函数的调用
        if (auto* call_inst = dyn_cast<CallInst>(&I)) {
            Function* called_func = call_inst->getCalledFunction();
            if (called_func && called_func->hasName()) {
                StringRef func_name = called_func->getName();
            
                // 通过函数名直接映射到我们新的DFG操作节点
                if (func_name == "GET_REAL") {
                    return OpCode::GET_REAL;
                }
                else if (func_name == "GET_IMAG") {
                    return OpCode::GET_IMAG;
                }
                else if (func_name == "COMBINE") {
                    return OpCode::COMBINE;
                }
                // 注意：如果还有其他函数调用需要处理，可以在这里添加
                // 如果不是我们关心的函数，则会继续执行下面的 switch
            }
        }
        // MODIFICATION END

        switch(I.getOpcode())
        {
            case Instruction::PHI       :   return OpCode::PHI;
            case Instruction::Select    :   return OpCode::PHI;
            case Instruction::Trunc     :   return OpCode::TRUNC;
            case Instruction::Add       :   return OpCode::ADD;
            case Instruction::FAdd      :   errs() << "Note: converting FAdd to integer op\n"; return OpCode::ADD;
            case Instruction::Sub       :   return OpCode::SUB;
            case Instruction::FSub      :   errs() << "Note: converting FSub to integer op\n"; return OpCode::SUB;
            case Instruction::Shl       :   return OpCode::SHL;
            case Instruction::AShr      :   return OpCode::ASHR;
            case Instruction::LShr      :   return OpCode::LSHR;
            case Instruction::And       :   return OpCode::AND;
            case Instruction::Or        :   return OpCode::OR;
            case Instruction::Xor       :   return OpCode::XOR;
            case Instruction::SRem      :   return OpCode::DIV;
            case Instruction::URem      :   return OpCode::DIV;
            case Instruction::FRem      :   errs() << "Note: converting FRem to integer op\n"; return OpCode::DIV;
            case Instruction::SDiv      :   return OpCode::DIV;
            case Instruction::UDiv      :   return OpCode::DIV;
            case Instruction::FDiv      :   errs() << "Note: converting FDiv to integer op\n"; return OpCode::DIV;
            case Instruction::Mul       :   return OpCode::MUL;
            case Instruction::FMul      :   errs() << "Note: converting FMul to integer op\n"; return OpCode::MUL;
            case Instruction::Load      :   return OpCode::LOAD;
            case Instruction::Store     :   return OpCode::STORE;
            case Instruction::GetElementPtr : return OpCode::GEP;
            case Instruction::ICmp      : return OpCode::ICMP;
            case Instruction::Br        :   return OpCode::BR;
            
            // MODIFICATION START: 允许CallInst通过
            // 对于我们不特殊处理的函数调用，可以当作NOP，或者发出警告
            case Instruction::Call      :   return OpCode::NOP;
            // MODIFICATION END


            // ignore these instructions (mostly casts)
            case Instruction::SExt      :   return OpCode::NOP;
            case Instruction::ZExt      :   return OpCode::NOP;
            case Instruction::FPToUI    :   return OpCode::NOP;
            case Instruction::FPToSI    :   return OpCode::NOP;
            case Instruction::UIToFP    :   return OpCode::NOP;
            case Instruction::SIToFP    :   return OpCode::NOP;
            case Instruction::FPTrunc   :   return OpCode::NOP;
            case Instruction::FPExt     :   return OpCode::NOP;
            case Instruction::PtrToInt  :   return OpCode::NOP;
            case Instruction::IntToPtr  :   return OpCode::NOP;
            case Instruction::BitCast   :   return OpCode::NOP;

            default: errs() << "could not look up:" << I << "\n"; std::abort();
        }
    }

    // ... OpNameMaker struct remains unchanged ...
    struct OpNameMaker {
        std::map<const Value*,std::string> instruction_unique_part = {};
        std::map<std::string, int> other_category_counts = {};

        OpNameMaker(const Loop* L) {
            for (const auto& bb : make_iterator_range(L->block_begin(), L->block_end())) {
                for (const auto& inst : *bb) {
                    // get the name from the IR, or just use the number of instructions encountered so far
                    std::string name = static_cast<std::string>(inst.getName());
                    if (name == "") { name = 'i' + std::to_string(instruction_unique_part.size()) + '_'; }
                    instruction_unique_part.emplace(&inst,std::move(name));
                }
            }
        }

        // base case
        std::string operator()(const Value* val, std::string s_arg = "") {

            // is it one of the pre-generated names?
            const auto lookup_result = instruction_unique_part.find(val);
            if (lookup_result != instruction_unique_part.end()) {
                return lookup_result->second + std::move(s_arg);
            }

            // otherwise, try to get a name from the IR, or make a unique name
            std::string result = static_cast<std::string>(val->getName());

            if (result.empty()) { // use string + number
                std::string category_name = "input"; // default

                // some common cases
                     if (isa<GlobalValue>(val)) { category_name = "global"; }
                else if (isa<Constant>(val))    { category_name = "const"; }
                else if (isa<BasicBlock>(val))  { category_name = "bb"; }

                auto& category_count = other_category_counts[category_name];
                result = category_name + std::to_string(category_count++);
            }

            return result + s_arg;
        }

        // special case
        std::string operator()(const Value* inst, const char* s_arg) {
            return (*this)(inst, std::string(s_arg));
        }

        // special case
        template<typename Arg1, typename... Args>
        std::string operator()(const Value* inst, const std::string& s_arg, Arg1& arg1, Args&... args) {
            return (*this)(inst, s_arg + std::to_string(arg1), args...);
        }

        // special case
        template<typename Arg1, typename... Args>
        std::string operator()(const Value* inst, const char* s_arg, Arg1& arg1, Args&... args) {
            return (*this)(inst, s_arg + std::to_string(arg1), args...);
        }

        // general case, converts first arg to string
        template<typename Arg1, typename... Args>
        std::string operator()(const Value* inst, Arg1& arg1, Args&... args) {
            return (*this)(inst, std::to_string(arg1), args...);
        }
    };


    std::string operandTypeFor(const OpGraphOp* op, int op_num) {
        if (commutative_ops.find(op->opcode) != commutative_ops.end()) {
            return Operands::BINARY_ANY;
        }
        else if (op->opcode == OpCode::LOAD) {
            return Operands::MEM_ADDR;
        }
        else if (op->opcode == OpCode::STORE) {
            if (op_num == 0) {
                return Operands::MEM_DATA;
            }
            else if (op_num == 1) {
                return Operands::MEM_ADDR;
            }
        }
        else if (op->opcode == OpCode::GEP) {
            return "gep_input" + std::to_string(op_num);
        }
        else if (op->opcode == OpCode::BR) {
            switch (op_num) {
                case 0: return "branch_cond";
                case 1: return "branch_true";
                case 2: return "branch_false";
            }
        }
        // MODIFICATION START: 为新操作定义操作数端口类型
        else if (op->opcode == OpCode::GET_REAL || op->opcode == OpCode::GET_IMAG) {
            // 单输入操作，我们约定使用LHS端口
            return Operands::BINARY_LHS;
        }
        else if (op->opcode == OpCode::COMBINE) {
            // 双输入操作
            if (op_num == 0) {
                return Operands::BINARY_LHS;  // real part
            }
            else if (op_num == 1) {
                return Operands::BINARY_RHS;  // imag part
            }
        }
        else if (op->opcode == OpCode::ADDRCAL) {
            // 双输入操作
            if (op_num == 0) {
                return Operands::BINARY_LHS; // base address
            }
            else if (op_num == 1) {
                return Operands::BINARY_RHS; // index
            }
        }
        // MODIFICATION END
        else if (op_num == 0) {
            return Operands::BINARY_LHS;
        }
        else if (op_num == 1) {
            return Operands::BINARY_RHS;
        }

        errs() << "Unhandled case for operand setting"; std::abort();
    }

    // ... try_extract_integral_constant function remains unchanged ...
    std::pair<bool, uint64_t> try_extract_integral_constant(const Value& val) {
        if(const auto& as_constant = dyn_cast<Constant>(&val)) {
            const auto& stripped_const = as_constant->stripPointerCasts();

            if (const auto& as_const_int = dyn_cast<ConstantInt>(stripped_const)) {
                return {true, as_const_int->getZExtValue()};

            } else if (const auto& as_global_var = dyn_cast<GlobalVariable>(stripped_const)) {
                if (as_global_var->hasInitializer()) {
                    try_extract_integral_constant(*as_global_var->getInitializer());
                }
            } else if (const auto& as_constant_expr = dyn_cast<ConstantExpr>(stripped_const)) {
                if (as_constant_expr->isCast()) {
                    return try_extract_integral_constant(*stripped_const->getOperand(0));
                } else {
                   errs() << "Warning: unable to extract a value from a ConstantExpr: " << *stripped_const << "\n";
                }
            } else {
                errs() << "Warning: unable to extract a value from a Constant: " << *stripped_const << "\n";
            }
        }
        return {false, 0};
    }


    struct DfgOutImpl {
        // ... DfgOutImpl constructor and initial parts of runOnLoop remain unchanged ...
        std::vector<OpGraph*> graphs = {};
        std::map<unsigned int, std::string> tag_pairs = {};
        std::vector<std::string> loop_tags = {};
        DfgOutImpl()
        {
            if(!inputTagPairs.empty())
            {
                std::ifstream in(inputTagPairs);
                for(std::string line; std::getline(in, line); )
                {
                    std::stringstream temp_sstream(line);
                    int temp_tag_num;
                    temp_sstream >> temp_tag_num;
                    std::string temp_tag_string;
                    temp_sstream >> temp_tag_string;
                    tag_pairs.emplace(temp_tag_num, std::move(temp_tag_string));
                }
                std::stringstream temp_sstream(loopTags);
                std::string temp_tag_string;
                while(temp_sstream >> temp_tag_string)
                    loop_tags.push_back(temp_tag_string);
            }
            else
                errs() << "Warning: No tag pair is provided as input, no DFG will be generated." << "\n";
        }


        bool runOnLoop(Loop* L)
        {
            // ... Loop tag finding logic remains unchanged ...
            int found_tag_num = 0;
            Instruction* the_loop_tag = nullptr;
            for(auto it = L->getHeader()->begin(); it != L->getHeader()->end(); ++it)
            {
                if(isa<CallInst>(it))
                {
                    Function * func = dyn_cast<CallInst>(it)->getCalledFunction();
                    if((func != nullptr) && (func->getName() == "DFGLOOP_TAG")) {
                        found_tag_num = cast<ConstantInt>(dyn_cast<CallInst>(it)->getArgOperand(0))->getValue().getZExtValue(); // found_tag_num has the the tag number
                        the_loop_tag = &*it;
                    }
                }
            }
            if(!found_tag_num) // If there is no tag associated with this loop
                return false;

            std::string tag_name;

            auto tag_pairs_it = tag_pairs.find(found_tag_num);
            if(tag_pairs_it == tag_pairs.end())
            {
                errs() << "Error: Tag could not be found from the generated script, ignoring this loop." << "\n";
                return false;
            }
            else
            {
                auto tag_string_it = std::find(loop_tags.begin(), loop_tags.end(), tag_pairs_it->second);
                if(tag_string_it == loop_tags.end())
                {
                    return false; // Do not process this loop
                }
                // Process the loop otherwise
                tag_name = *tag_string_it;
            }

            if (!L->getSubLoops().empty()) // only run on innermost loops
                return false;

            unsigned int bb_count = L->getBlocks().size();
            if(bb_count > 1) // More then one basic block within a loop
            {
                errs() << "Error: Loop with tag: " << tag_name << " is not supported. This loop is ignored." << "\n";
                return false;
            }


            // ... Initial graph construction logic remains largely the same ...
            OpNameMaker makeOpName {L};
            OpGraph og;
            auto llvm_value_to_dfg_op = [&]() {
                std::map<const Value*, OpGraph::OpDescriptor> result;
                for(const auto& bb : L->blocks()) {
                    for(const auto& inst : *bb) {
                        if (&inst == the_loop_tag) { continue; }
                        result[&inst] = og.emplace(makeOpName(&inst, inst.getOpcodeName()), 32, LLVMtoOp(inst));
                        for (const auto& operand : inst.operands()) {
                            // MODIFICATION: For CallInst, the last operand is the function itself, skip it.
                            if (isa<CallInst>(inst) && operand == inst.getOperand(inst.getNumOperands() - 1)) {
                                continue;
                            }
                            const auto& operand_as_value = *operand;
                            if (result.find(&operand_as_value) != result.end()) { continue; }
                            const auto* operand_as_inst_ptr = dyn_cast<Instruction>(&operand_as_value);
                            if (operand_as_inst_ptr && L->contains(operand_as_inst_ptr)) { continue; }
                            result[&operand_as_value] = og.insert([&]() -> OpGraphOp {
                                const auto op_name = makeOpName(operand);
                                const auto success_and_value = try_extract_integral_constant(*operand);
                                if (success_and_value.first) {
                                    return {std::move(op_name), 32, OpCode::CONST, (std::int64_t)success_and_value.second};
                                }
                                return {std::move(op_name), 32, OpCode::INPUT};
                            }());
                        }
                    }
                }
                return result;
            }();

            for(const auto& bb : L->blocks()) {
                for(const auto& inst : *bb) {
                    if (&inst == the_loop_tag) { continue; }
                    const auto& op_for_inst = llvm_value_to_dfg_op.at(&inst);
                    int operand_num = -1;
                    for (const auto& operand : inst.operands()) {
                        // MODIFICATION: For CallInst, the last operand is the function itself, skip creating an edge for it.
                        if (isa<CallInst>(inst) && operand == inst.getOperand(inst.getNumOperands() - 1)) {
                            continue;
                        }
                        operand_num += 1;
                        const auto& operand_as_value = *operand;
                        const auto& operand_op = llvm_value_to_dfg_op.at(&operand_as_value);
                        og.link(operand_op, op_for_inst, operandTypeFor(op_for_inst, operand_num));
                    }
                    if(std::any_of(inst.user_begin(), inst.user_end(), [&](const auto& user) {
                        const auto* user_as_instruction_ptr = dyn_cast<Instruction>(&*user);
                        return not user_as_instruction_ptr || not L->contains(user_as_instruction_ptr);
                    })) {
                        og.insert(op_for_inst, OpGraphOp(makeOpName(&inst,"output"), 32, OpCode::OUTPUT), "");
                    }
                }
            }


            verifyAndPrintReport(og, std::cerr, true, true);

            auto dfg_op_to_llvm_value = [&]() {
                std::map<OpGraph::OpDescriptor, const Value*> result;
                for (const auto& value_and_op : llvm_value_to_dfg_op) {
                    result.emplace(value_and_op.second, value_and_op.first);
                }
                return result;
            }();

            // MODIFICATION START: GEP lowering with ADDRCAL specialization
            // This logic replaces the original GEP lowering loop entirely.
            
            // Collect all GEP nodes first to avoid iterator invalidation issues.
            const auto orig_geps = filter_collection(og.opNodes(), [&og](auto&& op) { return og.getNodeRef(op).getOpCode() == OpGraphOpCode::GEP; });
            for (const auto& gep_op : orig_geps) {
                const auto inst_ptr = dyn_cast<GetElementPtrInst>(dfg_op_to_llvm_value.at(gep_op));
                if (not inst_ptr) {
                    errs() << "Warning: Instruction " << *dfg_op_to_llvm_value.at(gep_op)
                           << " for GEP op node " << og.getNodeRef(gep_op).getName() << " is not a GEP. Will not lower\n";
                    continue;
                }
                const auto& inst = *inst_ptr;

                // Case 1: Simple array access like base[index]
                // This corresponds to a GEP with 2 operands: base pointer and index.
                // We assume the element size is handled by the hardware (e.g., implicitly 4 bytes for int).
                if (inst.getNumOperands() == 2) {
                    // Get the DFG nodes for the base address and the index
                    OpGraph::OpDescriptor base_addr = llvm_value_to_dfg_op.at(inst.getOperand(0));
                    OpGraph::OpDescriptor index = llvm_value_to_dfg_op.at(inst.getOperand(1));

                    // Create the new ADDRCAL node
                    const auto addrcal_op = og.emplace(makeOpName(&inst, "addrcal"), 32, OpCode::ADDRCAL);
                    
                    // Connect base and index to the ADDRCAL node
                    og.link(base_addr, addrcal_op, operandTypeFor(addrcal_op, 0)); // base
                    og.link(index, addrcal_op, operandTypeFor(addrcal_op, 1));     // index
                    
                    // Reroute all outgoing edges from the old GEP node to the new ADDRCAL node
                    for (const auto& edge : og.outEdges(gep_op)) {
                        og.link_like(addrcal_op, og.targetOfEdge(edge), edge);
                    }

                    // Update maps to point to the new node
                    llvm_value_to_dfg_op[inst_ptr] = addrcal_op;
                    dfg_op_to_llvm_value[addrcal_op] = inst_ptr;
                
                } else {
                    // Case 2: Complex GEP (structs, multi-dimensional arrays)
                    // Use the original lowering logic which expands it into MULs and ADDs.
                    OpGraph::OpDescriptor tip = llvm_value_to_dfg_op.at(inst.getOperand(0));

                    {gep_type_iterator GTI = gep_type_begin(&inst);
                    int gep_operand_num = 1;
                    for(auto operand_it = std::next(inst.op_begin()); operand_it != inst.op_end(); ++operand_it, ++GTI, ++gep_operand_num) {
                        const auto data_size = inst.getParent()->getModule()->getDataLayout().getTypeAllocSize(GTI.getIndexedType());
                        const auto data_size_op = og.emplace(makeOpName(&inst, "data_size", gep_operand_num), 32, OpCode::CONST, data_size);
                        const auto mult_by_size = og.emplace(makeOpName(&inst, "mul", gep_operand_num), 32, OpCode::MUL);
                        const auto add_to_prev  = og.emplace(makeOpName(&inst, "add", gep_operand_num), 32, OpCode::ADD);
                        og.link(data_size_op, mult_by_size, Operands::BINARY_ANY);
                        og.link(llvm_value_to_dfg_op.at(*operand_it), mult_by_size, Operands::BINARY_ANY);
                        og.link(tip, add_to_prev, Operands::BINARY_ANY);
                        og.link(mult_by_size, add_to_prev, Operands::BINARY_ANY);
                        tip = add_to_prev;
                    }}

                    for (const auto& edge : og.outEdges(gep_op)) {
                        og.link_like(tip, og.targetOfEdge(edge), edge);
                    }
                    
                    llvm_value_to_dfg_op[inst_ptr] = tip;
                    dfg_op_to_llvm_value[tip] = inst_ptr;
                }
    
                // After processing, remove the original GEP node from the graph.
                og.erase(gep_op);
            }
            // MODIFICATION END

            // ... The rest of the file remains unchanged ...
            verifyAndPrintReport(og, std::cerr, true, true);
            og = removeCastNodes(std::move(og));
            verifyAndPrintReport(og, std::cerr, true, true);

            const auto op_print_ranking = [&]() {
                std::map<OpGraph::OpDescriptor, int> result;
                for (const auto& bb : make_iterator_range(L->block_begin(), L->block_end())) {
                    for (const auto& inst : *bb) {
                        const auto lookup_result = llvm_value_to_dfg_op.find(&inst);
                        if (lookup_result == llvm_value_to_dfg_op.end()) { continue; }
                        std::vector<OpGraph::OpDescriptor> to_visit = og.inputOps(lookup_result->second);
                        std::set<OpGraph::OpDescriptor> visited;
                        std::vector<OpGraph::OpDescriptor> dfs_order;
                        while (not to_visit.empty()) {
                            const auto curr = to_visit.back();
                            to_visit.pop_back();
                            if (not visited.insert(curr).second) { continue; }
                            const auto& inst_lookup = dfg_op_to_llvm_value.find(curr);
                            if (inst_lookup != dfg_op_to_llvm_value.end()) {
                                if (const auto inst = dyn_cast<Instruction>(inst_lookup->second)) {
                                    if (L->contains(inst)) { continue; }
                                }
                            }
                            const auto& inputs = og.inputOps(curr);
                            std::copy(inputs.begin(), inputs.end(), std::back_inserter(to_visit));
                            dfs_order.push_back(curr);
                        }
                        for (auto it = dfs_order.rbegin(); it != dfs_order.rend(); ++it) {
                            result.insert({*it, static_cast<int>(result.size())});
                        }
                        result.insert({lookup_result->second, static_cast<int>(result.size())});
                    }
                }
                return result;
            }();
            
            std::ofstream f("graph_" + tag_name + ".dot", std::ios::out);
            og.serialize(f, op_print_ranking);

            return false;
        }
    };
    
    // ... Legacy and New PM Registration remains unchanged ...
    struct LegacyDFGOut : public LoopPass
    {
        static char ID; // Pass identification, replacement for typeid
        DfgOutImpl impl = {};

        LegacyDFGOut() : LoopPass(ID) {}

        virtual bool runOnLoop(Loop* L, LPPassManager&) {
            return impl.runOnLoop(L);
        }

        virtual bool doFinalization()
        {
            return false;
        }
    };
}


/* Legacy PM Registration */
char LegacyDFGOut::ID = 0;
static RegisterPass<LegacyDFGOut> X("dfg-out", "DFG(Data Flow Graph) Output Pass",
        false /* Only looks at CFG */,
        false /* Analysis Pass */);

/* New PM Registration */
#if LLVM_VERSION_MAJOR >= 14
namespace {
struct DfgOut : PassInfoMixin<DfgOut> {
    DfgOutImpl impl = {};
    PreservedAnalyses run(Loop& L, LoopAnalysisManager& , LoopStandardAnalysisResults& , LPMUpdater&) {
        return impl.runOnLoop(&L) ? PreservedAnalyses::none() : PreservedAnalyses::all();
    }
};
} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "DfgOut",
        LLVM_VERSION_STRING,
        [](PassBuilder& PB) {
            PB.registerPipelineParsingCallback([](StringRef Name, LoopPassManager& PM, ArrayRef<PassBuilder::PipelineElement>) {
                if (Name == "dfg-out") {
                    PM.addPass(DfgOut());
                    return true;
                }
                return false;
            });
        }
    };
}
#endif /* LLVM_VERSION_MAJOR >= 14 */