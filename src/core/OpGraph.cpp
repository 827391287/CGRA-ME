/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/
#include <CGRA/OpGraph.h>

#include <CGRA/Exception.h>
#include <CGRA/GraphAlgorithms.h>
#include <CGRA/utility/ConfigGraph.h>
#include <CGRA/MRRGProcedures.h>
#include <CGRA/dotparse.h>

#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <limits>
#include <fstream>


namespace {

/**
 * A vector of pairs of opcode names and opcode enums.
 * Sorted by name. Use opcode_from_string and to_string instead of direct access.
 */
const auto opcode_names = []{ // immediately invoked
    std::vector<std::pair<std::string, OpGraphOpCode>> result {
    {"nop", OpCode::NOP},
    {"input", OpCode::INPUT},
    {"input_pred", OpCode::INPUT_PRED},
    {"output_pred", OpCode::OUTPUT_PRED},
    {"output", OpCode::OUTPUT},
    {"const", OpCode::CONST},
    {"trunc", OpCode::TRUNC},
    {"sext", OpCode::SEXT},
    {"zext", OpCode::ZEXT},
    {"phi", OpCode::PHI},
    {"add", OpCode::ADD},
    {"sub", OpCode::SUB},
    {"mul", OpCode::MUL},
    {"mult", OpCode::MUL},
    {"div", OpCode::DIV},
    {"and", OpCode::AND},
    {"or", OpCode::OR},
    {"xor", OpCode::XOR},
    {"shl", OpCode::SHL},
    {"ashr", OpCode::ASHR},
    {"lshr", OpCode::LSHR},
    {"gep", OpCode::GEP},
    {"icmp", OpCode::ICMP},
    {"load", OpCode::LOAD},
    {"store", OpCode::STORE},
    {"br", OpCode::BR},
    {"sqrt", OpCode::SQRT},
    {"fadd", OpCode::FADD},
    {"fmul", OpCode::FMUL},
    {"fdiv", OpCode::FDIV},
    {"fp2int", OpCode::FP2INT},
    {"mulu_full_lo", OpCode::MULU_FULL_LO},
    {"mulu_half_lo", OpCode::MULU_HALF_LO},
    {"mulu_quart_lo", OpCode::MULU_QUART_LO},
    {"mulu_full_hi", OpCode::MULU_FULL_HI},
    {"mulu_half_hi", OpCode::MULU_HALF_HI},
    {"mulu_quart_hi", OpCode::MULU_QUART_HI},
    {"muls_full_lo", OpCode::MULS_FULL_LO},
    {"muls_half_lo", OpCode::MULS_HALF_LO},
    {"muls_quart_lo", OpCode::MULS_QUART_LO},
    {"muls_full_hi", OpCode::MULS_FULL_HI},
    {"muls_half_hi", OpCode::MULS_HALF_HI},
    {"muls_quart_hi", OpCode::MULS_QUART_HI},
    {"add_full", OpCode::ADD_FULL},
    {"add_half", OpCode::ADD_HALF},
    {"add_quart", OpCode::ADD_QUART},
    {"select", OpCode::SELECT},
    {"int2fp", OpCode::INT2FP},
    };
    std::sort(result.begin(), result.end());
    return result;
}();

/**
 * A lookup from opcode enum to canonical name
 */
const auto opcode_to_name = []{ // immediately invoked
    std::map<OpGraphOpCode, std::string> result;
    for (const auto& name_and_code : opcode_names) {
        result.emplace(name_and_code.second, name_and_code.first);
    }
    return result;
}();

} // end anon namespace

std::ostream& operator<<(std::ostream &os, const OpGraphOpCode &opcode) {
    const auto lookup_result = opcode_to_name.find(opcode);
    if (lookup_result != opcode_to_name.end()) {
        return os << lookup_result->second;
    }

    throw make_from_stream<cgrame_error>([&,fname=__func__](auto&& s) {
        s << fname << "can't print opcode with value " << +opcode;
    });
}

const std::string& to_string(const OpGraphOpCode& opcode) { return opcode_to_name.at(opcode); }

OpGraphOpCode opcode_from_string(const std::string& str) {
    const auto name_comp_less = [](auto&& lhs_elem, auto& rhs_val) {
        const auto comp = [](auto&& lhs, auto& rhs) { return std::tolower(lhs) < std::tolower(rhs); };
        auto first1 = lhs_elem.first.begin();
        auto first2 = rhs_val.begin();
        const auto last1 = lhs_elem.first.end();
        const auto last2 = rhs_val.end();

        // same as C++20's lexicographical compare, from https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare
        for ( ; (first1 != last1) && (first2 != last2); ++first1, (void) ++first2 ) {
            if (comp(*first1, *first2)) return true;
            if (comp(*first2, *first1)) return false;
        }
        return (first1 == last1) && (first2 != last2);
    };

    const auto search_result = std::lower_bound(opcode_names.begin(), opcode_names.end(), str, name_comp_less);

    if (search_result == opcode_names.end() || name_comp_less(std::make_pair(str,0), search_result->first)) {
        throw make_from_stream<std::logic_error>([&,fname=__func__](auto&& s) {
            s << fname << " can't convert '" << str << "' into an opcode";
        });
    } else {
        return search_result->second;
    }
}

std::istream& operator>>(std::istream &is, OpGraphOpCode &opcode) {
    std::string str;
    if (is >> str) {
        opcode = opcode_from_string(str);
    }
    return is;
}

OpGraphNode::~OpGraphNode()
{
}



OpGraphOp::OpGraphOp(std::string name, int bitwidth, OpGraphOpCode code)
    : OpGraphNode(name)
    , bitwidth(bitwidth)
    , opcode(code)
    , input()
    , output(nullptr) {
    if (code == OpCode::CONST) {
        make_and_throw<std::logic_error>([&](auto& s) {
            s << "Trying to create a op graph const without initializing a value";
        });
    }
    if (code == OpCode::CMP) {
        make_and_throw<std::logic_error>([&](auto& s) {
            s << "Trying to create a op graph cmp without initializing a mode";
        });
    }
}

OpGraphOp::OpGraphOp(std::string _name, int _bitwidth, OpGraphOpCode _code, BitConfig* _bitConfig)
    : OpGraphNode(_name)
    , bitwidth(_bitwidth)
    , opcode(_code)
    , bitConfig(_bitConfig)
    , input()
    , output(nullptr) {
    if (_code == OpCode::CONST) {
        make_and_throw<std::logic_error>([&](auto& s) {
            s << "Trying to create a op graph const without initializing a value";
        });
    }
    if (_code == OpCode::CMP) {
        make_and_throw<std::logic_error>([&](auto& s) {
            s << "Trying to create a op graph cmp without initializing a mode";
        });
    }
}

OpGraphOp::OpGraphOp(std::string name, int bitwidth, OpGraphOpCode code, std::int64_t value)
    : OpGraphNode(name)
    , bitwidth(bitwidth)
    , opcode(code)
    , input()
    , output(nullptr)
    , constVal(value) {
    if (code != OpCode::CONST) {
        make_and_throw<std::logic_error>([&](auto& s) {
            s << "Trying to assign a value to an op that is not const";
        });
    }
}

OpGraphOp::OpGraphOp(std::string name, int bitwidth, OpGraphOpCode code, std::string value, bool cmp)
    : OpGraphNode(name)
    , bitwidth(bitwidth)
    , opcode(code)
    , input()
    , output(nullptr) {
    if (cmp) {
        cmpMode = value;
    } else {
        memName = value;
    }
    if (code != OpCode::CMP && code != OpCode::LOAD && code != OpCode::STORE) {
        make_and_throw<std::logic_error>([&](auto& s) {
            s << "Trying to assign a value to an op that is not cmp or memory access";
        });
    }
}

OpGraphOp OpGraphOp::propertyClone(std::string new_name) const {
    if (opcode == OpCode::CONST) {
        return {std::move(new_name), bitwidth, opcode, constVal};
    } else if (opcode == OpCode::CMP) {
        return {std::move(new_name), bitwidth, opcode, cmpMode, true};
    } else if (opcode == OpCode::LOAD || opcode == OpCode::STORE) {
        return {std::move(new_name), bitwidth, opcode, memName, false};
    } else {
        return {std::move(new_name), bitwidth, opcode};
    }
}

bool OpGraphOp::operator==(const OpGraphOp& rhs) const {
    if (*(const OpGraphNode*)this != rhs) { return false; }
    if (this->opcode != rhs.opcode) { return false; }
    if (this->bitwidth != rhs.bitwidth) { return false; }
    if (opcode == OpCode::CONST) {
        return this->constVal == rhs.constVal;
    } else if (opcode == OpCode::CMP) {
        return this->cmpMode.compare(rhs.cmpMode) == 0;
    } else if (opcode == OpCode::LOAD || opcode == OpCode::STORE) {
        return this->memName.compare(rhs.memName) == 0;
    } else {
        return true;
    }
}

OpGraphOp::~OpGraphOp() { }

OpGraphVal::OpGraphVal(std::string name, int bitwidth, int dist, EdgeKind ek)
    : OpGraphNode(name)
    , bitwidth(bitwidth)
    , input(nullptr)
    , output()
    , output_operand()
    , dist(dist)
    , kind(ek) { }

OpGraphVal::OpGraphVal(std::string name, int bitwidth, OpGraphOp* input_op, int dist, EdgeKind ek)
    : OpGraphVal(name, bitwidth, dist, ek) {
    this->input = input_op;
    if (ek == EdgeKind::kDataFlow)
        input_op->output = this;
}

OpGraphVal::~OpGraphVal() { }

std::string OpGraphVal::getOperandForOutput(const OpGraphOp* op) {
    const auto ival_output_pos = std::find(output.begin(), output.end(), op);
    const auto dist = std::distance(output.begin(), ival_output_pos);
    return output_operand[dist];
}

bool OpGraphVal::getPredicateForOutput(const OpGraphOp* op) {
    const auto ival_output_pos = std::find(output.begin(), output.end(), op);
    const auto dist = std::distance(output.begin(), ival_output_pos);
    return output_predicate[dist];
}

int OpGraphVal::getBitwidthForOutput(const OpGraphOp* op) {
    const auto ival_output_pos = std::find(output.begin(), output.end(), op);
    const auto dist = std::distance(output.begin(), ival_output_pos);
    return output_bitwidth[dist];
}

OpGraph::OpGraph()
    : op_nodes{}
    , val_nodes{}
    , ops_by_name{}
    , vals_by_name{}
{
}

const std::vector<OpGraphOp*> OpGraph::empty_op_vector = {};
const std::vector<OpGraphVal*> OpGraph::empty_val_vector = {};
const std::vector<OpGraph::EdgeDescriptor> OpGraph::empty_edge_vector = {};

OpGraph::~OpGraph() { clear(); }

void OpGraph::clear() {
    for (auto& op : op_nodes) {
        delete op;
    }
    for (auto& val : val_nodes) {
        delete val;
    }
    op_nodes.clear();
    val_nodes.clear();
    alias_nodes.clear();
    ops_by_name.clear();
    vals_by_name.clear();
}

OpGraph::OpGraph(const OpGraph& src)
    : op_nodes{}
    , val_nodes{}
    , alias_nodes {}
    , ops_by_name{}
    , vals_by_name{} {
    *this = src;
}

OpGraph& OpGraph::operator=(const OpGraph& src) {
    clear();
    std::map<OpDescriptor, OpDescriptor> src_to_this;
    for (const auto& src_op : src.opNodes()) {
        src_to_this.insert({src_op, insert(src.getNodeRef(src_op).propertyClone())});
    }
    for (const auto& src_op : src.opNodes()) {
        const auto src_val = src.outputVal(src_op);
        int output_index = -1;
        for (const auto& sink_op : src.outputOps(src_val)) {
            output_index += 1;
            link(src_to_this.at(src_op), src_to_this.at(sink_op),
                src.getNodeRef(src_val).output_operand.at(output_index),
                src.getNodeRef(src_val).bitwidth, src.getNodeRef(src_val).dist, src.getNodeRef(src_val).kind);
        }
    }
    return *this;
}

OpGraph::OpDescriptor OpGraph::insert(OpGraphOp op_to_move_in) {
    OpGraphOp* op = new OpGraphOp(std::move(op_to_move_in));

    if (!ops_by_name.emplace(op->name, op).second) {
        make_and_throw<std::logic_error>([&](auto& s) {
            s << "Ops should have unique names. An op with the name " << op->getName() << " already exists";
        });
    }

    op_nodes.push_back(op);

    return op;
}

OpGraph::OpOpInsertResult OpGraph::insert(OpDescriptor driver,
    OpGraphOp fanout_to_move_in,
    std::string operand_group,
    int bitwidth,
    int dist,
    EdgeKind kind) {
    auto fanout_op = insert(std::move(fanout_to_move_in));
    auto val = link(driver, fanout_op, operand_group, bitwidth, dist, kind);

    return { val, fanout_op };
}

OpGraph::ValDescriptor OpGraph::link(OpDescriptor driver,
    OpDescriptor fanout,
    std::string operand_group,
    int bitwidth,
    int dist,
    EdgeKind kind, 
    bool predicate) {
    // create val if it doesn't exist
    OpGraph::ValDescriptor val;
    std::string val_name = getNodeRef(driver).name + "_out" + (kind == EdgeKind::kAlias ? "_alias" : "");
    if (vals_by_name.find(val_name) != vals_by_name.end()) {
        val = vals_by_name[val_name];
    } else if (!getNodeRef(driver).output || kind == EdgeKind::kAlias) {
        val = new OpGraphVal(
            val_name,
            bitwidth,
            &getNodeRef(driver),
            dist,
            kind);

        if (!vals_by_name.emplace(val->name, val).second) {
            make_and_throw<std::logic_error>([&](auto& s) {
                s << "Vals should have unique names. A val with the name " << val->getName() << " already exists";
            });
        }
        if (kind == EdgeKind::kAlias) {
            alias_nodes.push_back(&getNodeRef(val));
        } else {
            val_nodes.push_back(&getNodeRef(val));
        }
    } else {
        val = outputVal(driver);
    }
    auto& driver_val_ref = getNodeRef(val);
    driver_val_ref.output.emplace_back(&getNodeRef(fanout));
    driver_val_ref.output_operand.emplace_back(operand_group);
    driver_val_ref.output_predicate.emplace_back(predicate);
    driver_val_ref.output_bitwidth.emplace_back(bitwidth);

    if (kind != EdgeKind::kAlias)
        getNodeRef(fanout).input.emplace_back(&getNodeRef(val));

    return val;
}

OpGraph::ValDescriptor OpGraph::link(ValDescriptor val, OpDescriptor fanout, std::string operand_group) {
    // setup the link
    auto& driver_val_ref = getNodeRef(val);
    driver_val_ref.output.emplace_back(&getNodeRef(fanout));
    driver_val_ref.output_operand.emplace_back(operand_group);
    getNodeRef(fanout).input.emplace_back(&getNodeRef(val));

    return val;
}

auto OpGraph::link_like(OpDescriptor driver, OpDescriptor fanout, EdgeDescriptor base) -> ValDescriptor {
    return link(driver, fanout, getOperandTag(base), getBitwidth(base), getDist(base), getKind(base));
}

void OpGraph::unLink(ValDescriptor driver_val, OpDescriptor fanout) {
    auto& ival = getNodeRef(driver_val);
    auto& oval = getNodeRef(fanout);
    const auto ival_output_pos = std::find(ival.output.begin(), ival.output.end(), fanout);
    if (ival_output_pos == ival.output.end()) {
        throw make_from_stream<std::logic_error>([&](auto&& s) {
            s << "trying to unlink " << driver_val << " -> "
                << fanout << " but the latter op is not a fanout of the former val";
        });
    }

    // remove the connecting val from fanout's input list
    auto& fo_ins = oval.input;
    const auto oval_input_pos = std::find(fo_ins.begin(), fo_ins.end(), driver_val);

    fo_ins.erase(oval_input_pos);

    // remove fanout from the connecting val
    const auto dist = std::distance(ival.output.begin(), ival_output_pos);
    ival.output.erase(ival_output_pos);
    ival.output_operand.erase(std::next(ival.output_operand.begin(), dist));

    if (ival.output.empty()) {
        defunct_nodes.emplace_back(&ival);
        const auto driver = ival.input;
        getNodeRef(driver).output = nullptr;
        vals_by_name.erase(ival.getName());
        val_nodes.erase(std::find(val_nodes.begin(), val_nodes.end(), driver_val));
    }
}

void OpGraph::erase(OpDescriptor op) {
    const auto pos_in_op_nodes = std::find(op_nodes.begin(), op_nodes.end(), op);
    if (pos_in_op_nodes == op_nodes.end()) {
        throw make_from_stream<std::logic_error>([&](auto&& s) {
            s << "trying to erase op " << op << " which is not part of the OpGraph " << this;
        });
    }

    const auto input_vals_copy = inputVals(op); // this list will be modified in unLink
    for (auto& ival : input_vals_copy) {
        unLink(ival, op);
    }

    const auto output_ops_copy = outputOps(op); // this list will be modified in unLink
    for (auto& oop : output_ops_copy) {
        unLink(outputVal(op), oop);
    }

    op_nodes.erase(pos_in_op_nodes);
    ops_by_name.erase(getNodeRef(op).getName());
    defunct_nodes.emplace_back(&getNodeRef(op));
}

OpGraph::ValDescriptor OpGraph::outputVal(OpDescriptor op_desc) const {
    if (not op_desc) { return null_val; }
    auto& op = getNodeRef(op_desc);
    if (op.output) { return op.output; }
    else { return null_val; }
}

std::vector<OpGraph::ValDescriptor> OpGraph::outputVals(OpDescriptor op_desc) const {
    if (not op_desc) { return {}; }
    return { outputVal(op_desc) };
}

const std::vector<OpGraphOp*>& OpGraph::outputOps(ValDescriptor val) const {
    if (not val) { return empty_op_vector; }
    return getNodeRef(val).output;
}

const std::vector<OpGraphOp*>& OpGraph::outputOps(OpDescriptor op) const {
    return outputOps(outputVal(op));
}

auto OpGraph::inputOps(OpDescriptor op) const -> std::vector<OpDescriptor> {
    std::vector<OpDescriptor> result;
    const auto& ivals = inputVals(op);
    std::transform(ivals.begin(), ivals.end(), std::back_inserter(result), [this](const auto& ival) { return this->inputOp(ival); });
    return result;
}

auto OpGraph::inputOp(ValDescriptor val) const -> OpDescriptor {
    if (not val) { return null_op; }
    return getNodeRef(val).input;
}

const OperandTag& OpGraph::getOperandTag(EdgeDescriptor edge) const {
    return getNodeRef(edge.val).output_operand.at(edge.output_index);
}

int OpGraph::getDist(EdgeDescriptor edge) const {
    return getNodeRef(edge.val).dist;
}

int OpGraph::getBitwidth(EdgeDescriptor edge) const {
    return getNodeRef(edge.val).output_bitwidth.at(edge.output_index);
}

EdgeKind OpGraph::getKind(EdgeDescriptor edge) const {
    return getNodeRef(edge.val).kind;
}

auto OpGraph::targetOfEdge(EdgeDescriptor ed) const -> OpDescriptor {
    if (not ed) { return null_op; }
    return getNodeRef(ed.val).output.at(ed.output_index);
}

auto OpGraph::outEdges(const OpDescriptor& op) const -> std::vector<EdgeDescriptor> {
    std::vector<EdgeDescriptor> result;
    for (const auto& out_val : outputVals(op)) {
        for (int i = 0; i < (std::ptrdiff_t)outputOps(out_val).size(); ++i) {
            result.push_back({out_val, i});
        }
    }
    return result;
}

auto OpGraph::inEdges(const OpDescriptor& op) const -> std::vector<EdgeDescriptor> {
    std::vector<EdgeDescriptor> result;
    for (const auto& target_ival : inputVals(op)) {
        for (const auto& edge_to_target : outEdges(inputOp(target_ival))) {
            if (targetOfEdge(edge_to_target) != op) { continue; }
            result.push_back(edge_to_target);
        }
    }
    return result;
}

const std::vector<OpGraphVal*>& OpGraph::inputVals(OpDescriptor op_desc) const {
    if (!op_desc) {
        return empty_val_vector;
    }
    auto& op = getNodeRef(op_desc);
    return op.input;
}

std::ostream& operator<<(std::ostream& output, const OpGraphOp& op)
{
    output << op.name << "(" << op.opcode <<")";
    return output;  // for multiple << operators.
}

std::ostream& operator<<(std::ostream& output, const OpGraphVal& val)
{
    output << val.name;
    return output;  // for multiple << operators.
}

//Change opnode with select to be scheduled first 
OpSchedule computeASAP(const OpGraph& op_graph)
{
    OpSchedule schedule;
    // create a copy of the list of all op nodes
    auto V = op_graph.opNodes();

    unsigned int num_scheduled = 0;
    // initialize all nodes
    for (auto& v : V)
    {
        if(v->input.size() == 0)
        {
            schedule.cycle[v] = 0;
            v = nullptr; // invalidate from list
            num_scheduled++;
        }
        else
        {
            schedule.cycle[v] = -1;
        }
    }

    int max_latency = 0;

    while(num_scheduled < V.size())
    {
        for (auto& v : V)
        {
            int max = 0;
            bool pred_sched = v;
            for(int i = 0; v && i < (std::ptrdiff_t)v->input.size(); i++) // for all node inputs
            {
                OpGraphVal* in = v->input[i];
                std::cout << "Path latency (" << *(in->input) << ", " << *v << "): " << /* output_latency.at(in).at(i) << */ "\n";
                max = std::max(max, schedule.cycle.at(in->input) /* + op_latency.at(in->input) + output_latency.at(in).at(i) */); // TODO: verify:  latency needs to be the path between predecessor and current node
                if(schedule.cycle.at(in->input) == -1)
                {
                    pred_sched = false;
                    break;
                }
            }

            if(pred_sched) // all predecessors are scheduled
            {
                schedule.cycle.at(v) = max; // TODO: should I factor in current op latency here?
                v = nullptr; // invalidate from list
                num_scheduled++;
                max_latency = std::max(max, max_latency);
            }
        }
    }

    schedule.latency = max_latency;

    return schedule;
}

OpSchedule computeALAP(const OpGraph& op_graph, unsigned int max_cycles)
{
    OpSchedule schedule;
    // create a copy of the list of all op nodes
    auto V = op_graph.opNodes();

    unsigned int num_scheduled = 0;

    // initialize all nodes
    for (auto& v : V)
    {
        if(v->output == nullptr) // TODO: this is probably wrong - this needs to be finding all the output nodes
        {
            schedule.cycle[v] = max_cycles;
            v = nullptr; // invalidate from list
            num_scheduled++;
        }
        else
        {
            schedule.cycle[v] = -1;
        }
    }

    while(num_scheduled < V.size())
    {
        for (auto& v : V)
        {
            int min = 0;
            bool succ_sched = v;
            for(int i = 0; v && i < (std::ptrdiff_t)v->output->output.size(); i++) // for all op outputs
            {
                OpGraphVal* out = v->output;
                OpGraphOp* succ = out->output[i];

                std::cout << "Path latency (" << *v << ", " << *succ << "): " << /* output_latency.at(out).at(i) << */ "\n";
                min = std::min(min, schedule.cycle.at(succ) /* - op_latency.at(v) + output_latency.at(out).at(i) */ );

                if(schedule.cycle.at(succ) == -1)
                {
                    succ_sched = false;
                    break;
                }
            }

            if(succ_sched) // all sucessors are scheduled
            {
                schedule.cycle.at(v) = min; // TODO: should I factor in current op latency here?
                v = nullptr; // invalidate from list
                num_scheduled++;
            }
        }
    }
    return schedule;
}

auto OpGraph::verify() const -> std::vector<VerifyMessage>
{
    std::vector<VerifyMessage> result;
    const auto& add_result = [&](VerifyMessage::Type type, auto&& f) {
        result.emplace_back(VerifyMessage{ type, string_from_stream(std::forward<decltype(f)>(f)) });
    };
    const auto& add_error = [&](auto&& f) { add_result(VerifyMessage::Type::Error, std::forward<decltype(f)>(f)); };
    const auto& add_warning = [&](auto&& f) { add_result(VerifyMessage::Type::Warning, std::forward<decltype(f)>(f)); };
    const auto& add_info = [&](auto&& f) { add_result(VerifyMessage::Type::Info, std::forward<decltype(f)>(f)); };
    (void)add_warning; (void)add_info; // Remove this line when these are used (it silences a warning)

    //Check for null pointer
    for(auto op : this->opNodes()) {
        if (not op) {
            add_error([&](auto&& s) { s << "found null op\n"; });
        }
    }
    for(auto& val : this->valNodes()){
        if (not val) {
            add_error([&](auto&& s) { s << "found null val\n"; });
        }
        const auto expected_size = val->output.size();
        if (val->output_operand.size() != expected_size) {
            add_error([&](auto&& s) { s << "operand index list size does not match number of outputs\n"; });
        }
    }

    // Check to make sure that all double link is set-up correctly from the input side
    for(auto temp_val : this->valNodes())
    {
        if (not temp_val) { continue; }

        auto& output = temp_val->output;
        for(auto temp_op : output)
        {
            if (not temp_op) { continue; }

            auto temp_it = std::find(temp_op->input.begin(), temp_op->input.end(), temp_val);
            if (temp_it == temp_op->input.end()) {
                add_error([&](auto&& s) { s << "expected to find " << *temp_val << " in input list of " << *temp_op; });
            }
        }
    }
    // Check to make sure that all double link is set-up correctly from the output side
    for(auto temp_op : this->opNodes())
    {
        if (not temp_op) { continue; }

        auto& input = temp_op->input;
        for(auto temp_val : input)
        {
            if (not temp_val) { continue; }

            auto temp_it = std::find(temp_val->output.begin(), temp_val->output.end(), temp_op);
            if (temp_it == temp_val->output.end()) {
                add_error([&](auto&& s) { s << "expected to find " << *temp_op << " in output list of " << *temp_val; });
            }
        }
    }

    return result;
}

int OpGraph::getOpIndex (OpGraphOp* op) const {
    auto it = find(op_nodes.begin(), op_nodes.end(), op);
    if (it != op_nodes.end()) {
        return it - op_nodes.begin();
    } else {
        throw cgrame_error ("Operation not found within the opgraph");
    }
}

int OpGraph::getValIndex (OpGraphVal* val) const {
    auto it = find(val_nodes.begin(), val_nodes.end(), val);
    if (it != val_nodes.end()) {
        return it - val_nodes.begin();
    } else {
        throw cgrame_error ("Value not found within the opgraph " + val->getName());
    }
}

int OpGraph::getAliasIndex (OpGraphVal* val) const {
    auto it = find(alias_nodes.begin(), alias_nodes.end(), val);
    if (it != alias_nodes.end()) {
        return it - alias_nodes.begin();
    } else {
        throw cgrame_error ("Alias not found within the opgraph " + val->getName());
    }
}

const OpGraphOp* OpGraph::getOpByIndex (int index) const {
    if (index >= op_nodes.size()) {
        throw cgrame_error ("Index is larger than the number of operations");
    } else {
        return op_nodes[index];
    }
}

const OpGraphVal* OpGraph::getValByIndex (int index) const {
    if (index >= val_nodes.size()) {
        throw cgrame_error ("Index is larger than the number of operations");
    } else {
        return val_nodes[index];
    }
}

bool verifyAndPrintReport(const OpGraph& opgraph, std::ostream& os, bool silent_on_no_errors, bool throw_if_errors) {
    const auto verify_output = opgraph.verify();
    const auto found_errors = analyzeOpgraphVerifyResults(os, verify_output, silent_on_no_errors);
    if (throw_if_errors && found_errors) {
        make_and_throw<cgrame_error>([&](auto&& s) { s << "OpGraph verify check failed! check stdout for results"; });
    } else {
        return not found_errors;
    }
}

bool analyzeOpgraphVerifyResults(std::ostream& os, const std::vector<OpGraph::VerifyMessage>& messages, const bool silent_on_no_errors) {
    using Type = OpGraph::VerifyMessage::Type;

    const auto contains_error = std::any_of(begin(messages), end(messages), [](auto&& msg) { return msg.type == Type::Error; });
    const auto has_messages = !messages.empty();
    const auto print_all_messages = contains_error || not silent_on_no_errors;

    if (print_all_messages) {
        if (contains_error) {
            os << "OpGraph verify FAILED";
        } else {
            os << "OpGraph verify passed";
        }

        if (has_messages) {
            os << ". Begin report:\n";

            for (auto& msg : messages) {
                os << msg.type << ": " << msg.message << '\n';
            }

            os << "End OpGraph verify report\n";
        } else {
            os << ", and nothing to report.\n";
        }

    }

    return contains_error;
}

// This function uses a DFS to find the largest cycle in the graph starting with the input nodes
// It assumes the graph is not disjoint.
// If the graph is acyclic, it returns 0.
static void dfs_visit(int time , int & longest_cycle, std::map<OpGraphOp*, int> & dfs_colour, std::map<OpGraphOp*, int> & dfs_timestamp, OpGraphOp* op)
{
    // colour grey
    dfs_colour[op] = 1;

    dfs_timestamp[op] = time;
    time++;

    //if leaf node, we are done
    if(op->opcode == OpCode::OUTPUT || op->opcode == OpCode::STORE)
    {
        dfs_colour[op] = 2;
        return;
    }

    for(auto & n : op->output->output)
    {
        if(dfs_colour[n] == 0)
        {
            dfs_visit(time, longest_cycle, dfs_colour, dfs_timestamp, n);
        }
        else if(dfs_colour[n] == 1)
        {
            int size = dfs_timestamp[op] - dfs_timestamp[n] + 1;
            //cout << "found cycle at " << *op << " to " << *n << " size: " << size << "\n";
            if(size > longest_cycle)
                longest_cycle = size;
        }
    }
    dfs_colour[op] = 2;
    return;
}

int OpGraph::getMaxCycle()
{
    int result = 0;

    std::map<OpGraphOp*, int> dfs_colour;
    std::map<OpGraphOp*, int> dfs_timestamp;

    // Following Corman et al. page540, 2nd ed.
    // 0 - white
    // 1 - grey
    // 2 - black
    for(auto & n : op_nodes)
        dfs_colour[n] = 0;

    // Loop through all op nodes
    for(auto & op : op_nodes)
    {
        if(dfs_colour[op] == 0)
        {
            //cout << "Visiting input: " << *input_op << "\n";
            dfs_visit(0, result, dfs_colour, dfs_timestamp, op);
        }
    }
    return result;
}

namespace {
    OpGraph::Walk reverse_and_canonicalize(OpGraph::Walk&& cyclic_trail, const OpGraph& opgraph) {
        std::reverse(cyclic_trail.begin(), cyclic_trail.end());
        auto new_begin = std::min_element(cyclic_trail.begin(), cyclic_trail.end(), [&](const auto& lhs, const auto& rhs) {
            return opgraph.getNodeRef(lhs.val).getName() < opgraph.getNodeRef(rhs.val).getName();
        });
        std::rotate(cyclic_trail.begin(), new_begin, cyclic_trail.end());
        return cyclic_trail;
    }
}

// code is based off of computeTrailsToBalance
// TODO: annotate this info directly on the graph, and remove this code
auto OpGraph::edgeLatencies() const -> std::map<EdgeDescriptor,int> {
    // return value. initialize to all zeros
    std::map<EdgeDescriptor,int> result;
    for (const auto& op : this->opNodes()) {
        for (const auto& e : this->outEdges(op)) {
            result[e] = 0;
        }
    }

    std::set<std::vector<EdgeDescriptor>> cycles_found;

    // ops that have been visited by previous iterations
    std::set<OpDescriptor> ops_visited;

    // non-tree and re-convergence edges that have been visited by previous iterations
    // note: re-convergence edges are symmetric with non-tree ones, so we treat them the same here
    std::set<EdgeDescriptor> nte_and_convergence_edges_found;

    // use (ordered) sets -- no hash defined
    // also, we'll be treating hyper-edge output pins as vertices; as-if it were a multi-graph
    const auto galgos = GraphAlgorithms<EdgeDescriptor,
        StandardMapMaker<std::map, EdgeDescriptor>, StandardSetMaker<std::set, EdgeDescriptor>
    >{};

    for (OpDescriptor op : this->opNodes()) {
        if (ops_visited.find(op) != ops_visited.end()) { continue; }

        // start as far up as possible, to make for fewer iterations.
        // note: for an unvisited Op, all Ops reachable by fanin edges must have also not been visited.
        {std::set<OpDescriptor> ancestors;
        while (true) {
            const auto& fanin = this->inputVals(op);
            if (fanin.empty() || not ancestors.insert(op).second) { break; }
            op = this->inputOp(*fanin.begin());
        }}

        struct FindOpNonTreeEdgesVisitor : DoNothingGraphVisitor<EdgeDescriptor> {
            const OpGraph* opgraph = nullptr;
            std::set<OpDescriptor> examined_op_targets = {};
            std::set<EdgeDescriptor> non_tree_edges = {};

            void onExamine(const EdgeDescriptor& e) {
                // if this hyper-edge's target was already visited, save it
                if (not examined_op_targets.insert(opgraph->targetOfEdge(e)).second) {
                    non_tree_edges.insert(e);
                }
            }
        } visitor;
        visitor.opgraph = this;
        visitor.examined_op_targets.insert(op); // handle edges that loop around to op

        // a search with with the hyper-edge output pins as vertices
        // returns a fanin graph representing the search, where fanin.front() defines a spanning tree
        auto search_tree = galgos.wavedBreadthFirstVisit(*this, this->outEdges(op), visitAllVertices(), visitor);

        // each non-tree edge represents one constraint.
        // examine the spanning tree around the target and driver to determine type and members
        for (const auto nte : visitor.non_tree_edges) {
            // only one constraint per nte or convergence edge!
            if (not nte_and_convergence_edges_found.insert(nte).second) { continue; }

            // if there is any tree or non-tree edge that is driven by the target of nte, then it's a cycle
            EdgeDescriptor cycle_edge;
            const auto reverse_search_tree = galgos.wavedBreadthFirstVisit(
                makeReversedGraphFromFaninLists<EdgeDescriptor>(&search_tree), {nte},
                // input_is(this->targetOfEdge(nte))
                [&](auto& e) {
                    const bool found = this->targetOfEdge(nte) == this->inputOp(e.val);
                    if (found) { cycle_edge = e; }
                    return found;
                }
            );
            if (cycle_edge) {
                auto traceback = galgos.singleTraceback(singleItemSet(nte), cycle_edge, reverse_search_tree);
                if (not traceback.success) { throw make_from_stream<cgrame_error>([fname=__func__](auto&& s) {
                    s << fname << " couldn't find original non-tree edge when searching a reverse search tree";
                });}
                if (nte == cycle_edge) {
                    traceback.path.pop_back(); // remove duplicate
                }
                // is it a new cycle?
                if (cycles_found.insert(reverse_and_canonicalize(std::move(traceback.path), *this)).second) {
                    result[nte] = 1;
                }
            }
        }

        // synchronize the global visited set with the one for this iteration
        std::copy(visitor.examined_op_targets.begin(), visitor.examined_op_targets.end(), std::inserter(ops_visited, ops_visited.end()));
    }

    return result;
}

void OpGraph::printDOTwithOps(std::ostream &s) const
{
    using std::to_string;
    unsigned int counter = 0;
    s << "digraph G {\n";

    // print op_nodes
    //  op_node map
    std::map<OpGraphOp*, std::string> opnode_map;
    counter = 0;
    for(auto it = this->opNodes().begin(); it != this->opNodes().end(); it++)
    {
	opnode_map[(*it)] = to_string((*it)->opcode) + to_string(counter++);
	s << opnode_map[(*it)] << "[opcode=" << (*it)->opcode;
	if ((*it)->opcode == OpCode::CONST) {
	    s << ", constVal=" << (*it)->constVal;
	} else if ((*it)->opcode == OpCode::CMP) { 
	    s << ", cmpMode=" << (*it)->cmpMode;
	} else if ((*it)->opcode == OpCode::LOAD || (*it)->opcode == OpCode::STORE){
        s << ", memName=" << (*it)->memName;
    }

	s << "];\n";
    }

    // use val nodes to create all edges
    for(auto it = this->valNodes().begin(); it != this->valNodes().end(); it++)
    {
        std::string inputnode = opnode_map[(*it)->input];
        for(unsigned int o = 0; o < (*it)->output.size(); o++)
        {
            OpGraphOp* op = (*it)->output.at(o);
            std::string operand = (*it)->output_operand.at(o);
            s << inputnode << "->" << opnode_map[op] << "[operand=" << operand << "]; ";
            s << "//" << (*it)->input->name << "->" <<  op->name << "\n";
        }
    }
    s << "}\n";
}

void OpGraph::print_dot(std::ostream &s) const
{
    unsigned int counter = 0;
    s << "digraph G {\n";

    // print op_nodes
    //  op_node map
    std::map<OpGraphOp*, std::string> opnode_map;
    counter = 0;
    for(auto it = this->opNodes().begin(); it != this->opNodes().end(); it++)
    {
        opnode_map[(*it)] = "node" + std::to_string(counter++);
        s << opnode_map[(*it)] << "[opcode=" << (*it)->opcode << "];\n";
    }

    // use val nodes to create all edges
    for(auto it = this->valNodes().begin(); it != this->valNodes().end(); it++)
    {
        std::string inputnode = opnode_map[(*it)->input];
        for(unsigned int o = 0; o < (*it)->output.size(); o++)
        {
            OpGraphOp* op = (*it)->output.at(o);
            std::string operand = (*it)->output_operand.at(o);
            s << inputnode << "->" << opnode_map[op] << "[operand=" << operand << "]; ";
            s << "//" << (*it)->input->name << "->" <<  op->name << "\n";
        }
    }
    s << "}\n";
}

void OpGraph::serialize(std::ostream& s) const {
    // std::vector<OpDescriptor> input_nodes;
    // const auto& on = opNodes();
    // std::copy_if(on.begin(), on.end(), std::back_inserter(input_nodes), [&](const auto& op) {
    //     return fanin(op).empty();
    // });

    // struct V : DoNothingGraphVisitor<OpDescriptor> {
    //     std::map<OpGraph::OpDescriptor, int> op_print_ranking;
    //     onExamine(const auto& op) { op_print_ranking.insert(op, static_cast<int>(op_print_ranking.size())); }
    //     shouldIgnore(const auto& op) const { return op_print_ranking.count(op) != 0; }
    // };

    // for (const auto& seed_op : input_nodes) {
    //     wavedBreadthFirstVisit();
    // }

    // return serialize(s, op_print_ranking);
    return serialize(s, {});
}

void OpGraph::serialize(std::ostream& s, const std::map<OpDescriptor,int>& op_print_ranking) const {

    // print ordering. If no value specified, is tied for last
    const auto get_ranking = [&](OpDescriptor op) {
        const auto lookup = op_print_ranking.find(op);
        if (lookup == op_print_ranking.end()) {
            return std::numeric_limits<int>::max();
        } else {
            return lookup->second;
        }
    };

    // print in order of given ranking. Stable, so ties go to memory order
    auto sorted_op_nodes = opNodes();
    std::stable_sort(sorted_op_nodes.begin(), sorted_op_nodes.end(), [&,this](const auto& lhs, const auto& rhs) {
        return get_ranking(lhs) < get_ranking(rhs);
    });

    s << "digraph G {\n";

    for (const auto& op_desc : sorted_op_nodes) {
        s << "    "; printDotID(s, getNodeRef(op_desc).getName()) << " [opcode=" << getNodeRef(op_desc).opcode;
        if (getNodeRef(op_desc).opcode == OpCode::CONST) {
            s << ", constVal="; printDotID(s, std::to_string(getNodeRef(op_desc).constVal));
        } else if (getNodeRef(op_desc).opcode == OpCode::CMP) {
            s << ", cmpMode="; printDotID(s, getNodeRef(op_desc).cmpMode);
	    } else if (getNodeRef(op_desc).opcode == OpCode::STORE || getNodeRef(op_desc).opcode == OpCode::LOAD ) {
            s << ", memName="; printDotID(s, getNodeRef(op_desc).memName);
	    }
        s << "];\n";
    }

    for (const auto& src_op_desc : sorted_op_nodes) {
        const auto& src_op = getNodeRef(src_op_desc);

        // sort out edges by op rank, with ties broken by output_index. Stable, so ties go to memory order
        auto sorted_out_edges = outEdges(src_op_desc);
        std::stable_sort(sorted_out_edges.begin(), sorted_out_edges.end(), [&,this](const auto& lhs, const auto& rhs) {
            const auto& lhs_rank = get_ranking(this->targetOfEdge(lhs));
            const auto& rhs_rank = get_ranking(this->targetOfEdge(rhs));
            if (lhs_rank == rhs_rank) {
                return lhs.output_index < rhs.output_index;
            } else {
                return lhs_rank < rhs_rank;
            }
        });

        for (const auto& out_edge : sorted_out_edges) {
            const auto& sink_op = getNodeRef(targetOfEdge(out_edge));
            const auto& val = getNodeRef(out_edge.val);
            const auto& operand_tag = val.output_operand.at(out_edge.output_index);
            const bool has_operand_tag = not operand_tag.empty();
            const bool has_attributes = has_operand_tag; // && anything new
            s << "    " ; printDotID(s, src_op.getName()) << " -> " ; printDotID(s, sink_op.getName());
            if (has_attributes) {
                s << " [";
                if (has_operand_tag) {
                    s << "operand="; printDotID(s, operand_tag);
                }
                s << "]";
            }
            s << ";\n";
        }
    }

    s << "}\n";
}

bool operator==(const OpGraph& lhs, const OpGraph& rhs) {
    if (&lhs == &rhs) { return true; }

    // check ops same
    if (lhs.ops_by_name.size() != rhs.ops_by_name.size()) { return false; }
    for (const auto& lhs_name_and_opdesc : lhs.ops_by_name) {
        // find the node with the same name
        const auto rhs_search_result = rhs.ops_by_name.find(lhs_name_and_opdesc.first);
        if (rhs_search_result == rhs.ops_by_name.end()) { return false; }

        // check if properties are the same
        const auto& lhs_node = lhs.getNodeRef(lhs_name_and_opdesc.second);
        const auto& rhs_node = rhs.getNodeRef(rhs_search_result->second);
        if (not (lhs_node == rhs_node)) { return false; }

        // check if fanout is the same
        const auto& lhs_out_edges = lhs.outEdges(lhs_name_and_opdesc.second);
        const auto& rhs_out_edges = rhs.outEdges(rhs_search_result->second);
        const auto& mismatch_result = std::mismatch(
            lhs_out_edges.begin(), lhs_out_edges.end(),
            rhs_out_edges.begin(), rhs_out_edges.end(),
            [&](const auto& lhs_edge, const auto& rhs_edge) {
                // shouldn't be any nulls in the first place... but check anyway
                if (not (bool)lhs_edge || not (bool)rhs_edge) { return false; }

                // get, then compare, the val with the same name (if it exists)
                // and check if the operand index is the same
                const auto& lhs_val = lhs.getNodeRef(lhs_edge.val);
                const auto& rhs_val_lookup = rhs.vals_by_name.find(lhs_val.name);
                if (rhs_val_lookup == rhs.vals_by_name.end()) { return false; }
                const auto& rhs_val = rhs.getNodeRef(rhs_edge.val);
                return lhs_val == rhs_val && (
                    lhs_val.output_operand.at(lhs_edge.output_index)
                    == rhs_val.output_operand.at(rhs_edge.output_index)
                );
            }
        );
        if (mismatch_result != std::make_pair(lhs_out_edges.end(), rhs_out_edges.end())) { return false; }
    }

    // check vals same
    if (lhs.vals_by_name.size() != rhs.vals_by_name.size()) { return false; }
    for (const auto& lhs_name_and_valdesc : lhs.vals_by_name) {
        const auto rhs_search_result = rhs.vals_by_name.find(lhs_name_and_valdesc.first);
        if (rhs_search_result == rhs.vals_by_name.end()) { return false; }
        const auto& lhs_node = lhs.getNodeRef(lhs_name_and_valdesc.second);
        const auto& rhs_node = rhs.getNodeRef(rhs_search_result->second);
        if (not (lhs_node == rhs_node)) { return false; }
    }

    return true;
}

OpGraphTransformResult filter(
    const OpGraph& src,
    const std::set<OpGraph::OpDescriptor>& allowed_ops
) {
    OpGraphTransformResult result;

    // add all the ops
    for (const auto& src_op : src.opNodes()) {
        if (allowed_ops.end() == allowed_ops.find(src_op)) { continue; }
        const auto& dest_op = result.transform_result.insert(src.getNodeRef(src_op).propertyClone());
        result.forward_mappings.emplace(src_op, dest_op);
        result.reverse_mappings.emplace(dest_op, src_op);
    }

    // add all the vals
    for (const auto& src_op : src.opNodes()) {
        if (allowed_ops.end() == allowed_ops.find(src_op)) { continue; }
        const auto& dest_op = result.transform_result.asOp(result.forward_mappings.at(src_op));
        const auto& src_val = src.getNodeRef(src.outputVal(src_op));
        int output_idex = 0;
        for (const auto& src_oop : src.outputOps(src_op)) {
            if (allowed_ops.end() == allowed_ops.find(src_oop)) { continue; }
            const auto& dest_oop = result.transform_result.asOp(result.forward_mappings.at(src_oop));
            const auto& dest_oval = result.transform_result.link(dest_op, dest_oop,
                src_val.output_operand.at(output_idex), src_val.bitwidth, src_val.dist, src_val.kind);
            result.forward_mappings.emplace(src.outputVal(src_op), dest_oval);  // subsequent iterations will do nothing
            result.reverse_mappings.emplace(dest_oval, src.outputVal(src_op));  // subsequent iterations will do nothing
            output_idex += 1;
        }
    }

    return result;
}

std::set<OpGraph::OpDescriptor> findNDownstreamOps(
    const OpGraph& opgraph,
    const std::vector<OpGraphOp*>& starting_points,
    const std::ptrdiff_t n_ops
) {
    using ODesc = OpGraph::OpDescriptor;
    const auto g_algos = GraphAlgorithms<ODesc>{};

    struct Visitor : DoNothingGraphVisitor<ODesc> {
        std::ptrdiff_t n_ops = -1; // better than uninitialized?
        std::set<ODesc> ops = {};
        bool have_enough = false;

        void onExamine(const ODesc& op) { if (!have_enough) { ops.insert(op); } }
        void onWaveEnd() { have_enough = (std::ptrdiff_t)ops.size() > n_ops; }
        bool shouldStop() { return have_enough; }
    } v;
    v.n_ops = n_ops; // no aggregate init here, until c++17

    std::set<OpGraph::OpDescriptor> starting_points_as_set(starting_points.begin(), starting_points.end());
    g_algos.wavedBreadthFirstVisit(opgraph, starting_points_as_set, visitAllVertices(), v);

    return std::move(v.ops);
}

// in the cpp file, as LLVM disables RTTI.
// Somehow, the linker figures out how to make the code work together?
// The RTTI data is still present (would get a link error otherwise)...
//  Probably crash if called from LLVM plugin code
auto  OpGraph::asOp(NodeDescriptor ndesc) const ->  OpDescriptor { return dynamic_cast<OpDescriptor>(ndesc);  }
auto OpGraph::asVal(NodeDescriptor ndesc) const -> ValDescriptor { return dynamic_cast<ValDescriptor>(ndesc); }

std::ostream& operator<<(std::ostream& os, const OpGraph::EdgeDescriptor& ed) {
    if (ed) { os << ed.val->name; } else { os << "nullptr"; }
    return os << "-" << ed.output_index;
}
std::ostream& operator<<(std::ostream& os, const OpGraph::VerifyMessage::Type& vm_type) {
    using Type = OpGraph::VerifyMessage::Type;
    switch (vm_type) {
        case Type::Info:      os << "Info";      break;
        case Type::Warning:   os << "Warning";   break;
        case Type::Error:     os << "Error";     break;
        default: os << "OpGraphVMTNotImplementedByPrinter" << +vm_type; break;
    }
    return os;
}
