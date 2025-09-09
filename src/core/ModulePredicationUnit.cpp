/*
oftware programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/Module.h>

#include <regex>
// maps for the modes of each type of comparator
const std::map<OpGraphOpCode, LLVMMode> Compare::all_modes =
    {
        {OpCode::CMP, {"op_icmp", "cmp", {"cmp"}, "cmp_sel"}},
};

/************ Compare **********/
Compare::Compare(std::string name, Location loc, unsigned size, int II)
    : Module(name, loc, size)
{
    adds_synchronous_circuitry = true;
    // Create ports
    addPort("in_a", PORT_INPUT, size);
    addPort("in_b", PORT_INPUT, size);
    addPort("out", PORT_OUTPUT, size);
    addPort("select", PORT_CONFIG, 3);

    // add position for visualization
};

// Virtual function that overrides Module::GenericName. Returns generic name of the object
std::string Compare::GenericName() // Virtual function override
{
    return "Compare_" + std::to_string(getSize()) + "b";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json Compare::CoreIRGenFunctionality() // Virtual function override
{
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};

    for (auto &parameter : parameterlist)
    {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = {};
   //vjson["interface"].push_back("CGRA_PHI_Enable");

    for (auto &port : ports)
    {
        std::string portName = port.second->getName();
        vjson["interface"].push_back(portName);
    }

    // module definition
    std::string moduleDefinition;
    
    for (auto &port : ports)
    {
        port_type portType = port.second->pt;
        std::string portTypeString = {};
        if (portType == port_type::PORT_INPUT)
        {
            portTypeString = "input";
        }
        else if (portType == port_type::PORT_OUTPUT)
        {
            portTypeString = "output";
        }
        else if (portType == port_type::PORT_OUTPUT_REG)
        {
            portTypeString = "output reg";
        }
        else
        {
            portTypeString = "inout";
        }
        std::string portSizeString;
        if (!(port.second->parameter).empty()) // Check if size is parameterized
        {
            std::string portParameterName = port.second->parameter;
            portSizeString = "[" + portParameterName + "-1:0]";
        }
        else
        {
            portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
        }
        std::string portName = port.second->getName();
        std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
        moduleDefinition += std::string(SET_INDENT) + portDeclaration;
    }

    // Functionality
    moduleDefinition += std::string(SET_INDENT) + "wire pred;\n";
    moduleDefinition += std::string(SET_INDENT) + "always @(*) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (select == 3'b000) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out <= 1b'0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (select == 3'b001) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out <= in_a == in_b;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (select == 3'b010) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out <= in_a != in_b;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (select == 3'b011) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out <= in_a > in_b;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (select == 3'b100) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out <= in_a >= in_b;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (select == 3'b101) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out <= in_a < in_b;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (select == 3'b110) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out <= in_a <= in_b;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "pred <= 1b'0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out = {pred} \n";

    vjson["definition"] = moduleDefinition;

    return vjson;
}

Compare::~Compare()
{
}

MRRG *Compare::createMRRG(unsigned II = 1)
{

    MRRG *result_ptr = new MRRG(II);
    auto &result = *result_ptr;

    for (unsigned i = 0; i < II; i++)
    {

        // input and function node
        MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in")).first;
        MRRG::NodeDescriptor cmp = result.insert(MRRGNode::make_function(this, data_size, i, "cmp_inst", 0, {OpCode::CMP})).first;
        // output node
        MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out")).first;
        // Linking the function to the inputs and the outputs
        result.link(in, cmp);
        result.link(cmp, out);
    }

    return result_ptr;
}

BitConfig Compare::getBitConfig(
    const MRRG &mrrg, const OpGraph &og,
    const Mapping &map,
    const ConfigCell &ccell,
    const MRRGNodesFromOpNode &mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode &mrrg_nodes_from_val_node) const
{
    (void)ccell;

    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromOpNode> opNodesByCycle(mrrg.initiationInterval());
    for (const auto &op_and_mrrg_nodes : mrrg_nodes_from_op_node)
    {
        for (const auto &mrrg_node : op_and_mrrg_nodes.second)
        {
            opNodesByCycle[mrrg_node->cycle][op_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int cycle = 0;
    for (auto &mrrg_nodes_from_op_node_per_cycle : opNodesByCycle)
    {
        if (mrrg_nodes_from_op_node_per_cycle.empty())
        {
            bitConfig.add({2, BitSetting::LOW}, cycle);
        }
        else
        {
            if (mrrg_nodes_from_op_node_per_cycle.size() != 1)
            {
                make_and_throw<std::logic_error>([&](auto &&s)
                                                 { s << "This opgraph_op node is mapped to multiple MRRG nodes\n"; });
            }
            else if (std::begin(mrrg_nodes_from_op_node_per_cycle)->first->opcode != OpCode::CMP)
            {
                make_and_throw<std::logic_error>([&](auto &&s)
                                                 { s << "A non compare op was mapped to this compare unit\n"; });
            }
            std::string cmpMode = std::begin(mrrg_nodes_from_op_node_per_cycle)->first->cmpMode;
            std::vector<BitSetting> result;
            if (cmpMode.compare("EQ") == 0)
            {
                bitConfig.add(bitsettings_from_int((int)1, 2), cycle);
            }
            else if (cmpMode.compare("NE") == 0)
            {
                bitConfig.add(bitsettings_from_int((int)2, 2), cycle);
            }
            else if (cmpMode.compare("GT") == 0)
            {
                bitConfig.add(bitsettings_from_int((int)3, 2), cycle);
            }
            else if (cmpMode.compare("GE") == 0)
            {
                bitConfig.add(bitsettings_from_int((int)4, 2), cycle);
            }
            else if (cmpMode.compare("LT") == 0)
            {
                bitConfig.add(bitsettings_from_int((int)5, 2), cycle);
            }
            else if (cmpMode.compare("LE") == 0)
            {
                bitConfig.add(bitsettings_from_int((int)6, 2), cycle);
            }
            else
            {
                make_and_throw<std::logic_error>([&](auto &&s)
                                                 { s << "An unrecognized compare instruction mode entered\n"; });
            }
            // bitConfig.add(result, cycle);
        }
        ++cycle;
    }

    return bitConfig;
}

/************ SelMultiplexer **********/
const std::map<OpGraphOpCode, LLVMMode> SelMultiplexer::all_modes =
    {
        {OpCode::SELECT, {"op_sel", "sel", {"sel"}, "sel_sel"}},
        {OpCode::PHI, {"op_phi", "phi", {"phi"}, "phi_sel"}}
    };

/************ SelMultiplexer **********/
SelMultiplexer::SelMultiplexer(std::string name, Location loc, unsigned mux_size_, unsigned size, bool isElastic)
    : Module(name, loc, size, isElastic)
    , mux_size(mux_size_)
    {

    adds_synchronous_circuitry = true;
    if (mux_size <= 1) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "Multiplexer doesn't support mux_size <= 1. mux_size = " << mux_size;
    });}

    // Create ports
    for(int i = 0; i < mux_size; i++) {
        addPort("in" + std::to_string(i), PORT_INPUT, "size", size, isElastic);
    }

    addPort("out", PORT_OUTPUT_REG, "size", size, isElastic);
    
    // select port to control small mux
    addPort("select", PORT_CONFIG, 2* ceil(log2(mux_size)), isElastic);

    // select port to control the bigger mux
    addPort("selectpred", PORT_INPUT, 1, isElastic);

}
// Virtual function that overrides Module::GenericName. Returns generic name of the object
std::string SelMultiplexer::GenericName() // Virtual function override
{
    return "sel_mux_" + std::to_string(mux_size) + "to1_" + std::to_string(getSize()) + "b";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json SelMultiplexer::CoreIRGenFunctionality() // Virtual function override
{
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_"; //module prefix
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist)
    {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = {};
    vjson["interface"].push_back("CGRA_Clock");
    vjson["interface"].push_back("CGRA_Reset");
    vjson["interface"].push_back("CGRA_Enable");

    for (auto& port : ports)
    {
        std::string portName = port.second->getName();
        vjson["interface"].push_back(portName);
    }

    // module definition
    std::string moduleDefinition;
    moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
    moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
    moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";
    //moduleDefinition += std::string(SET_INDENT) + "input CGRA_PHI_Enable;\n";

    for (auto& port : ports)
    {
        port_type portType = port.second->pt;
        std::string portTypeString = {};
        if(portType == port_type::PORT_INPUT)
        {
            portTypeString = "input";
        }
        else if (portType == port_type::PORT_OUTPUT)
        {
            portTypeString = "output";
        }
        else if (portType == port_type::PORT_OUTPUT_REG)
        {
            portTypeString = "output reg";
        }
        else
        {
            portTypeString = "inout";
        }
        std::string portSizeString;
        //std::string portSizeString1b;

        if (!(port.second->parameter).empty()) // Check if size is parameterized
        {
            std::string portParameterName = port.second->parameter;
            portSizeString = "[" + portParameterName + "-1:0]";
            //portSizeString1b = "[" + portParameterName + "0:0]";
        }
        else
        {
            portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
        }
        std::string portName = port.second->getName();

        std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
        moduleDefinition += std::string(SET_INDENT) + portDeclaration;
        
        //std::string portDeclaration1b = portTypeString + " " + portSizeString1b + " " + portName + ";\n";
        //moduleDefinition += std::string(SET_INDENT) + portDeclaration;
    }


    // Functionality 


    int ss = (int)ceil(log2(mux_size));
    std::string selectSize = std::to_string(ss);
  
    moduleDefinition += std::string(SET_INDENT) + "wire [" + std::to_string(ss-1) + ":0] x;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign x = selectpred ? select["+ std::to_string(2*ss-1) + ":" +std::to_string(ss) +"] : select["+ std::to_string(ss-1) + ":0];\n";

    // Functionality
    if (isElastic) {
	    moduleDefinition += std::string(SET_INDENT) + "always @(*)\n";
	    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "case (x)\n";
	    for (int i = 0; i < mux_size; ++i)
	    {
	        std::string stringI = std::to_string(i);
	        moduleDefinition += std::string(SET_TRIPLE_INDENT) + stringI + ":begin\n";
	        moduleDefinition += std::string(SET_QUAD_INDENT) + " out = in" + stringI + ";\n";
	        moduleDefinition += std::string(SET_QUAD_INDENT) + " out_valid_downstream = in" + stringI + "_valid_upstream;\n";
	        for (int j = 0; j < mux_size; ++j){
	    	    std::string stringJ = std::to_string(j);
	    	    if (j == i)
	    	        moduleDefinition += std::string(SET_QUAD_INDENT) + " in" + stringJ + "_stop_upstream = out_stop_downstream;\n";
	    	    else
	    	        moduleDefinition += std::string(SET_QUAD_INDENT) +" in" + stringJ + "_stop_upstream = 1'b1;\n";
	        } 
	        moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end\n";
	    }
	    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "default: out = {size{1'bx}};\n";
	    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "endcase\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "assign select_stop_upstream = out_stop_downstream;\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "assign select_valid_upstream = out_valid_downstream;\n";
    } else {
	    moduleDefinition += std::string(SET_INDENT) + "always @(*)\n";
	    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "case (x)\n";
	    for (int i = 0; i < mux_size; ++i)
	    {
	        std::string stringI = std::to_string(i);
	        moduleDefinition += std::string(SET_TRIPLE_INDENT) + stringI + ": out = in" + stringI + ";\n";
	    }
	    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "default: out = {size{1'bx}};\n";
	    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "endcase";
    }

    vjson["definition"] = moduleDefinition;
    return vjson;
}

SelMultiplexer::~SelMultiplexer()
{
}

MRRG* SelMultiplexer::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i++)
    {
        MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out")).first;
        MRRG::NodeDescriptor mux = result.insert(MRRGNode::make_routing_function(this, data_size, i, "sel_inst", 0, {Operands::BINARY_LHS, Operands::BINARY_RHS, Operands::BINARY_ANY}, {OpCode::SELECT, OpCode::PHI}, 1)).first;
        MRRG::NodeDescriptor selectpred = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "selectpred", {Operands::PREDICATE})).first;
        //MRRG::NodeDescriptor selectpred = result.insert(MRRGNode::make_routing(this, i, "selectpred")).first;
        result.link(mux, out);
        result.link(selectpred, mux);

        for(int j = 0; j < mux_size; j++)
        {
            MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in" + std::to_string(j))).first;

            result.link(in, mux);
        }
    }
    return result_ptr;
}

BitConfig SelMultiplexer::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
    const Mapping& map,
    const ConfigCell& ccell,
    const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_op_node;
    const std::vector<std::regex> required_node_regexes {
        std::regex("\\.out$"),
    };
    const std::regex in_regex("\\.in([0-9]+)$");

    const std::vector<std::regex> required_op_node_regexes {
        std::regex("\\.out$"),
        std::regex("\\.sel_inst"),
    };
    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize value and op nodes mapping by cycle
    std::vector<MRRGNodesFromValNode> valNodesByCycle(mrrg.initiationInterval());
    for (const auto& val_and_mrrg_nodes : mrrg_nodes_from_val_node) {
        for (const auto& mrrg_node : val_and_mrrg_nodes.second) {
            valNodesByCycle[mrrg_node->cycle][val_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    std::vector<MRRGNodesFromOpNode> opNodesByCycle(mrrg.initiationInterval());
    for (const auto& op_and_mrrg_nodes : mrrg_nodes_from_op_node) {
        for (const auto& mrrg_node : op_and_mrrg_nodes.second) {
            opNodesByCycle[mrrg_node->cycle][op_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int cycle = 0; 
    int used_cycles = 0;
    for (int i = 0; i < valNodesByCycle.size(); i++) {    
        std::set<int> inputs_used;
        int left = 0;
        int right = 0;
        if (!opNodesByCycle[i].empty()){
            auto mrrg_nodes_from_op_per_cycle = opNodesByCycle[i];
            
            int required_node_types_found = 0;
            std::vector<OpGraphOp*> phiNodes;

            for (const auto& op_node_and_mrrg_nodes : mrrg_nodes_from_op_per_cycle) {
                for (const auto& req_node_regex : required_op_node_regexes) {
                    for (const auto& mrrg_node : op_node_and_mrrg_nodes.second) {
                        std::smatch match_results;
                        if (std::regex_search(mrrg_node->getHierarchyQualifiedName(), match_results, req_node_regex)) {
                            if (required_node_types_found > 2) {
                                throw cgrame_error("found a node that matched two required node regexes");
                            } else {
                                required_node_types_found++;
                                phiNodes.push_back(op_node_and_mrrg_nodes.first);               
                            }
                        }
                    }
                }
            }
            
            bool leftFound = false;
            bool rightFound = false;
            auto  mrrg_nodes_from_val_per_cycle = valNodesByCycle[i];
            for (const auto& val_node_and_mrrg_nodes : mrrg_nodes_from_val_per_cycle) { //added
                if (required_node_types_found > 0) {
                    for (const auto& mrrg_node : val_node_and_mrrg_nodes.second) {
                        std::smatch match_results;
                        std::regex_search(mrrg_node->getHierarchyQualifiedName(), match_results, in_regex);
                        if (match_results.size() == 2) {
                                if( val_node_and_mrrg_nodes.first->getOperandForOutput(phiNodes[0]) ==  Operands::BINARY_LHS){
                                    if(leftFound) throw cgrame_error("Error: 2 LHS inputs specfied for phi op" + phiNodes[0]->getName());
                                    else{
                                        leftFound = true;
                                        left = stoi(match_results[1].str());
                                    }
                                    //break;
                                }
                                else if ( val_node_and_mrrg_nodes.first->getOperandForOutput(phiNodes[0]) ==  Operands::BINARY_RHS){
                                    if(rightFound) throw cgrame_error("Error: 2 RHS inputs specfied for phi op" + phiNodes[0]->getName());
                                    else{ 
                                        rightFound = true;
                                        right = stoi(match_results[1].str());
                                    }
                                    //break;
                                } else if ( val_node_and_mrrg_nodes.first->getOperandForOutput(phiNodes[0]) ==  Operands::PREDICATE) {
                                    continue;  
                                } else{
                                    throw cgrame_error("Error: Must specify LHS/RHS for phi op " + phiNodes[0]->getName() + "   " + val_node_and_mrrg_nodes.first->getName() + "    " + val_node_and_mrrg_nodes.first->getOperandForOutput(phiNodes[0]));
                                }   
                            inputs_used.insert(stoi(match_results[1].str()));
                        }
                    }
                } else {
                    // ignore the val
                }
            }
        } else {
            auto  mrrg_nodes_from_val_per_cycle = valNodesByCycle[i];
            for (const auto& val_node_and_mrrg_nodes : mrrg_nodes_from_val_per_cycle) {
                std::vector<bool> required_node_types_found;
                for (const auto& req_node_regex : required_node_regexes) {
                    required_node_types_found.push_back(false);
                    for (const auto& mrrg_node : val_node_and_mrrg_nodes.second) {
                        std::smatch match_results;
                        if (std::regex_search(mrrg_node->getHierarchyQualifiedName(), match_results, req_node_regex)) {
                            if (required_node_types_found.back()) {
                                throw cgrame_error("found a node that matched two required node regexes");
                            } else {
                                required_node_types_found.back() = true;
                            }
                        }
                    }
                }

                if (std::all_of(begin(required_node_types_found), end(required_node_types_found), [&](auto&& v) { return v; })) {
                    for (const auto& mrrg_node : val_node_and_mrrg_nodes.second) {
                        std::smatch match_results;
                        std::regex_search(mrrg_node->getHierarchyQualifiedName(), match_results, in_regex);
                        if (match_results.size() == 2) {
                            inputs_used.insert(stoi(match_results[1].str()));
                        }
                    }
                } else {
                    // ignore the val
                }
            }

        }
        if (inputs_used.empty()) {
            bitConfig.add({2* (size_t)std::lround(ceil(log2(mux_size))), BitSetting::DONT_CARE_PREFER_LOW}, cycle);
        } else if (inputs_used.size() == 1) {
            used_cycles++;
            std::vector<BitSetting> first_vec = bitsettings_from_int(*begin(inputs_used), (int)std::lround(ceil(log2(mux_size))));
            std::vector<BitSetting> sec_vec = bitsettings_from_int(*begin(inputs_used), (int)std::lround(ceil(log2(mux_size))));
            first_vec.insert(first_vec.end(), sec_vec.begin(), sec_vec.end() );
            bitConfig.add(first_vec, cycle);
        } else if(inputs_used.size() == 2){            
            used_cycles++;
            std::vector<BitSetting> first_vec = bitsettings_from_int(left, (int)std::lround(ceil(log2(mux_size))));
            int sec = *std::next(inputs_used.begin(), 1);
            std::vector<BitSetting> sec_vec = bitsettings_from_int(right, (int)std::lround(ceil(log2(mux_size))));
            first_vec.insert(first_vec.end(), sec_vec.begin(), sec_vec.end());
            bitConfig.add(first_vec, cycle);
        } else {
            throw cgrame_error("ERROR: predicate MUX has to have two inputs");
        }    
        cycle++;       
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}

/************ EventTransitionTable **********/
/* EventTransitionTable stores the branches of an application within a table. So, a CGRA
 * with supported number of contexts, SII, can map an application with conditional branches. */

const std::map<OpGraphOpCode, LLVMMode> EventTransitionTable::all_modes =
{
        {OpCode::BR, {"op_br", "br", {"br"}, "sel_br"}
    },
};

EventTransitionTable::EventTransitionTable(std::string name, int SII, Location loc)
        : Module(name, loc, 0)
{
    addPort("current_event", PORT_INPUT, 1);

    parameterlist.emplace("size", getSize());
    parameterlist.emplace("contexts", SII);
}

nlohmann::json EventTransitionTable::CoreIRGenFunctionality() {
    auto context_size = (parameterlist["contexts"] == 1) ? "0" : "$clog2(contexts)-1";
    nlohmann::json vjson;

    // Construct Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto &parameter : parameterlist) {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = {};
    vjson["interface"].push_back("ConfigIn");
    vjson["interface"].push_back("ConfigOut");
    vjson["interface"].push_back("Config_Clock");
    vjson["interface"].push_back("Config_Reset");
    vjson["interface"].push_back("CGRA_Clock");
    vjson["interface"].push_back("CGRA_Reset");
    vjson["interface"].push_back("CGRA_Enable");
    //vjson["interface"].push_back("CGRA_PHI_Enable");
    vjson["interface"].push_back("next_state");
    vjson["interface"].push_back("current_state");
    vjson["interface"].push_back("branch");
    

    for (auto& port : ports)
    {
        std::string portName = port.second->getName();
        vjson["interface"].push_back(portName);
    }

    // module definition
    std::string moduleDefinition;
    for (auto& port : ports)
    {
        port_type portType = port.second->pt;
        std::string portTypeString = {};
        if(portType == port_type::PORT_INPUT)
        {
            portTypeString = "input";
        }
        else if (portType == port_type::PORT_OUTPUT)
        {
            portTypeString = "output";
        }
        else if (portType == port_type::PORT_OUTPUT_REG)
        {
            portTypeString = "output reg";
        }
        else
        {
            portTypeString = "inout";
        }
        std::string portSizeString;
        if (!(port.second->parameter).empty()) // Check if size is parameterized
        {
            std::string portParameterName = port.second->parameter;
            portSizeString = "[" + portParameterName + "-1:0]";
        }
        else
        {
            portSizeString = "[" + std::to_string(port.second->size - 1) + ":0]";
        }
        std::string portName = port.second->getName();
        std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
        moduleDefinition += std::string(SET_INDENT) + portDeclaration;
    }
    // module definition
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Clock;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Reset;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Enable;\n";
    //moduleDefinition += std::string(SET_INDENT) +    "input CGRA_PHI_Enable;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input ConfigIn;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input Config_Clock;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input Config_Reset;\n";
    moduleDefinition += std::string(SET_INDENT) +    "output ConfigOut;\n";
    moduleDefinition += std::string(SET_INDENT) +    "output reg [" + context_size + ":0] next_state;\n";
    moduleDefinition += std::string(SET_INDENT) +    "output reg branch;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input [" + context_size + ":0] current_state;\n";

    moduleDefinition += std::string(SET_INDENT) + "reg [" + context_size + "-1:0] event_stt_next_state [" + context_size + "-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "reg [" + context_size + "-1:0] event_stt_state [" + context_size + "-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "reg event_stt_event [" + context_size + "-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "integer i;\n";
   
    moduleDefinition += std::string(SET_INDENT) + "always @(posedge Config_Clock) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (Config_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "for (i = 0; i < " + context_size + "; i = i+1) begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "event_stt_state[i] <= 0;\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "event_stt_event[i] <= 0;\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "event_stt_next_state[i] <= 0;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "for (i = 0; i < " + context_size + "; i = i+1) begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "if(i == 0) begin\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "event_stt_state[i] <= {ConfigIn,event_stt_state[i][" + context_size + "-1:1]};\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "event_stt_event[i] <= event_stt_state[" + context_size + "-1][0];\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "event_stt_next_state[i] <= {event_stt_event[0],event_stt_next_state[i][$clog2(" + context_size + ")-1:1]};\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + " end\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + " else begin\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "event_stt_state[i] <= {event_stt_state[i-1][0],event_stt_state[i][$clog2(" + context_size + ")-1:1]};\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "event_stt_event[i] <= event_stt_event[i-1];\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "event_stt_next_state[i] <= {event_stt_next_state[i-1][0],event_stt_next_state[i][$clog2(" + context_size + ")-1:1]};\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";
   
    moduleDefinition += std::string(SET_INDENT) + "always @(*) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "branch = 1'b0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "for (i = 0; i < " + context_size + "; i = i+1) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "if (event_stt_state[i] == current_state && event_stt_event[i] == current_event) begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "next_state =  event_stt_next_state[i];\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "branch = 1'b1;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";

    moduleDefinition += std::string(SET_INDENT) + "assign config_out = event_stt_next_state[" + context_size + "-1][0];\n";
    
    vjson["definition"] = moduleDefinition;
    return vjson;
}

std::string EventTransitionTable::GenericName()
{
    return "EventTransitionTable";
}

MRRG* EventTransitionTable::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i++)
    {
        MRRG::NodeDescriptor current_event = result.insert(MRRGNode::make_routing(this, data_size, i, "current_event")).first;
        MRRG::NodeDescriptor br = result.insert(MRRGNode::make_routing_function(this, data_size, i, "br_inst", 0, {Operands::BINARY_ANY}, {OpCode::BR}, 1)).first;
        result.link(br, current_event);

    }
    return result_ptr;
}

BitConfig EventTransitionTable::getBitConfig(
    const MRRG &mrrg, const OpGraph &og,
    const Mapping &map,
    const ConfigCell &ccell,
    const MRRGNodesFromOpNode &mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode &mrrg_nodes_from_val_node) const
{
    (void)ccell;

    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromOpNode> opNodesByCycle(mrrg.initiationInterval());
    for (const auto &op_and_mrrg_nodes : mrrg_nodes_from_op_node)
    {
        for (const auto &mrrg_node : op_and_mrrg_nodes.second)
        {
            opNodesByCycle[mrrg_node->cycle][op_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int cycle = 0;
    for (auto &mrrg_nodes_from_op_node_per_cycle : opNodesByCycle)
    {
        
    }

    return bitConfig;
}
