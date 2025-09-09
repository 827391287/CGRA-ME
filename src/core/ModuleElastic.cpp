#include <CGRA/Module.h>
#include <CGRA/ModuleElastic.h>
#include <CGRA/ModuleFPUnit.h>
#include <regex>

/* Elastic Buffer (FIFO Implementation) */
ElasticBufferFifo::ElasticBufferFifo(std::string name, Location loc, int depth_, int size, bool hasEnable_, bool isElastic)
	: Module(name, loc, size, isElastic), eb_depth(depth_), hasEnable(hasEnable_)
{
    adds_synchronous_circuitry = true;
    if (isElastic == false) {make_and_throw<cgrame_error>([&](auto&& s) {
        s << "Elastic buffer needs to be marked as elastic ";
    });}

    // Create ports
    addPort("data_in", PORT_INPUT, "size", size, isElastic);
    addPort("data_out", PORT_OUTPUT, "size", size, isElastic);
    if (hasEnable_) { 
	    addPort("enable", PORT_CONFIG, 1, isElastic);
    }

    // Visualization
    node_relative_position.insert({"data_in", {0, 0}});
    node_relative_position.insert({"data_out", {0, 0.5}});
}

// Virtual function that overrides Module::GenericName. Returns generic name for the object.
std::string ElasticBufferFifo::GenericName()
{
    std::string enable = hasEnable ? "enable_" : "";
	return "elastic_buffer_" + enable + std::to_string(getSize()) + "b";
}

// Virtual function that overrides Module::CoreIRGetFunctionality. Generates the functionality for this module.
nlohmann::json ElasticBufferFifo::CoreIRGenFunctionality()
{	
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_";
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
    moduleDefinition += std::string(SET_INDENT) + "parameter DATA_WIDTH = " + std::to_string(getSize()) + ";\n";
    moduleDefinition += std::string(SET_INDENT) + "parameter DEPTH = " + std::to_string(getDepth()) + ";\n";
    moduleDefinition += std::string(SET_INDENT) + "parameter ADDRESS_WIDTH = $clog2(DEPTH);\n";
    moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
    moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
    moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";
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
    moduleDefinition += std::string(SET_INDENT) + "wire eb_enable;\n";
    if (hasEnable) {
	    moduleDefinition += std::string(SET_INDENT) + "assign eb_enable = enable;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign enable_valid_upstream = data_in_valid_upstream;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign enable_stop_upstream = data_in_stop_upstream;\n";
    } else { 
        moduleDefinition += std::string(SET_INDENT) + "assign eb_enable = 'b1;\n";
    }
    moduleDefinition += std::string(SET_INDENT) + "wire fifo_full, fifo_almost_full, fifo_empty, fifo_almost_empty;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [DATA_WIDTH-1:0] fifo_read_data;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [ADDRESS_WIDTH:0] fifo_word_count;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire fifo_write_en, fifo_read_en;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign fifo_write_en = (data_in_valid_upstream & eb_enable) & ~fifo_full;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign fifo_read_en = ~fifo_empty & ~data_out_stop_downstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign data_in_stop_upstream = fifo_full;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign data_out_valid_downstream = ~fifo_empty;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign data_out = fifo_read_data & {DATA_WIDTH{~fifo_empty}};\n";
    moduleDefinition += std::string(SET_INDENT) + "// Instantiate FIFO\n";
    moduleDefinition += std::string(SET_INDENT) + "fwft_fifo FIFO\n";
    moduleDefinition += std::string(SET_INDENT) + "(\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".reset ( CGRA_Reset ),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".clk ( CGRA_Clock ),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".full ( fifo_full ),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".write_en ( fifo_write_en ),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".write_data ( data_in ),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".empty ( fifo_empty ),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".read_en ( fifo_read_en ),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".read_data ( fifo_read_data ),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".usedw ( fifo_word_count )\n";
    moduleDefinition += std::string(SET_INDENT) + ");\n";
    moduleDefinition += std::string(SET_INDENT) + "defparam FIFO.width = DATA_WIDTH;\n";
    moduleDefinition += std::string(SET_INDENT) + "defparam FIFO.widthad = ADDRESS_WIDTH;\n";
    moduleDefinition += std::string(SET_INDENT) + "defparam FIFO.depth = DEPTH;\n";
    vjson["definition"] = moduleDefinition;
    return vjson;
}

BitConfig ElasticBufferFifo::getBitConfig(
        const MRRG& mrrg, const OpGraph & og,
	const Mapping& map,
        const ConfigCell& ccell,
        const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
        const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_op_node;
    BitConfig bitConfig(mrrg.initiationInterval());
    // Use the m_in node to check if this register is being used
    const std::regex in_regex("\\.data_in$");

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromValNode> valNodesByCycle(mrrg.initiationInterval());
    for (const auto& val_and_mrrg_nodes : mrrg_nodes_from_val_node) {
        for (const auto& mrrg_node : val_and_mrrg_nodes.second) {
            valNodesByCycle[mrrg_node->cycle][val_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int cycle = 0;
    int used_cycles = 0;
    for (auto & mrrg_nodes_from_val_by_cycle : valNodesByCycle) {
        if (mrrg_nodes_from_val_by_cycle.empty()) {
            bitConfig.add({BitSetting::LOW}, cycle);
        } else {
	    bool enableBuffer = false;
	    for (auto & op_val_and_nodes: mrrg_nodes_from_val_by_cycle) {
		for (auto & mrrg_node : op_val_and_nodes.second) {
		    auto name = mrrg_node->getHierarchyQualifiedName();
		    if (regex_search(name, in_regex)) {
			// Considering doing an error check here to see if "m_in" shows up twice
			enableBuffer = true;
			break;
		    }
		}
	    }

	    if (!enableBuffer) {
		    bitConfig.add({(size_t) 1, BitSetting::LOW}, cycle);
	    } else {
            used_cycles++;
		    bitConfig.add({(size_t) 1, BitSetting::HIGH}, cycle);
	    }
	}
	++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}



ElasticBufferFifo::~ElasticBufferFifo()
{	// Empty destructor
}

MRRG* ElasticBufferFifo::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for (unsigned i = 0; i < II; i++) 
    { 
	    // Create nodes, model an EB as a zero delay wire
	    MRRG::NodeDescriptor data_in = result.insert(MRRGNode::make_routing(this, data_size, i, "data_in", 0)).first;
	    MRRG::NodeDescriptor buff = result.insert(MRRGNode::make_routing(this,  data_size, i, "buffer", 0)).first;
	    MRRG::NodeDescriptor data_out = result.insert(MRRGNode::make_routing(this,  data_size, i, "data_out", 0)).first;

	    // Connect nodes
	    result.link(data_in, buff);
        result.link(buff, data_out);
    }
    return result_ptr;
}

/* Elastic Eager Fork  With Branch*/
const std::map<OpGraphOpCode, LLVMMode> ElasticForkBranch::all_modes =
    {
        {OpCode::SELECT, {"op_br", "branch", {"br"}, "br_br"}}
    };
ElasticForkBranch::ElasticForkBranch(std::string name, Location loc, int size, int fanout_, bool isElastic)
	: Module(name, loc, size, isElastic), fanout(fanout_)
{
     if (isElastic == false) {make_and_throw<cgrame_error>([&](auto&& s) {
        s << "Elastic eager fork needs to be marked as elastic ";
    });}
    adds_synchronous_circuitry = true;
    isElastic = true;
    // Check module legality
    if (fanout <= 1) {make_and_throw<cgrame_error>([&](auto&& s) {
	    s << "Elastic fork doesn't support fanout of <=1. Requested fanout was " << fanout;
    });}

    // Create ports
    addPort("in", PORT_INPUT, "size", size, isElastic);
    addPort("condition", PORT_INPUT, 1, isElastic);

    for (int i = 0; i < fanout; i++) {
	    addPort("out" + std::to_string(i), PORT_OUTPUT, "size", size, isElastic);
    }
    
    addPort("enable_downstream", PORT_CONFIG, fanout * 2 + 1, isElastic);
    addParameter("FANOUT", fanout);
}

std::string ElasticForkBranch::GenericName()
{
	return "elastic_fork_branch_fanout" + std::to_string(fanout) + "_size_" + std::to_string(data_size);
}

nlohmann::json ElasticForkBranch::CoreIRGenFunctionality()
{
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_";
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

	// Functionality 
    // Internal Variables
    moduleDefinition += std::string(SET_INDENT) + "wire enable_condition;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [(FANOUT-1):0] set_t_f;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [(FANOUT-1):0] en_downstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [(FANOUT-1):0] valid_downstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [(FANOUT-1):0] stop_downstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "reg [(FANOUT-1):0] state_regs;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [(FANOUT-1):0] masked_state_regs;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [(FANOUT-1):0] internal_stops;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [(FANOUT-1):0] muxed_condition;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire internal_vas;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire mask_valid_cond;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire mask_cond;\n";

    // Combinational assignments
    // Set the configuration bits
    moduleDefinition += std::string(SET_INDENT) + "assign enable_condition = enable_downstream[0];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign en_downstream = enable_downstream[FANOUT:1];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign set_t_f = enable_downstream[FANOUT*2:FANOUT+1];\n";

    // Mask the condition to pass through
    moduleDefinition += std::string(SET_INDENT) + "assign mask_cond = !enable_condition | condition;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign mask_valid_cond = !enable_condition | condition_valid_upstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign condition_stop_upstream = enable_condition & |internal_stops | (|({FANOUT{mask_valid_cond}} & masked_state_regs & {FANOUT{!in_valid_upstream}}));\n";
    
    // mux condition to set the true false condition depending if it is enabled or not
    moduleDefinition += std::string(SET_INDENT) + "assign muxed_condition = enable_condition ? set_t_f & {FANOUT{mask_cond}} & en_downstream | ~set_t_f & {FANOUT{~mask_cond}} & en_downstream : {FANOUT{1'b1}};\n";

    // Set up the interantional conditions for the stop and valid
    moduleDefinition += std::string(SET_INDENT) + "assign internal_stops = (stop_downstream & masked_state_regs);\n";
    moduleDefinition += std::string(SET_INDENT) + "assign internal_vas = (|internal_stops) & in_valid_upstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign masked_state_regs = state_regs & en_downstream & {FANOUT{CGRA_Enable}};\n";

    // Output assignments
    moduleDefinition += std::string(SET_INDENT) + "assign in_stop_upstream = |internal_stops  | (|({FANOUT{!mask_valid_cond}} & masked_state_regs & {FANOUT{in_valid_upstream}}));\n";
    moduleDefinition += std::string(SET_INDENT) + "assign valid_downstream = masked_state_regs & {FANOUT{in_valid_upstream}} & muxed_condition & {FANOUT{mask_valid_cond}};\n";

    // Internal register logic
    moduleDefinition += std::string(SET_INDENT) + "always @(posedge CGRA_Clock, posedge CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "state_regs <= 'h1;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "state_regs <= internal_stops | {FANOUT{~internal_vas}};\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";

	for (int i = 0; i < fanout; i++)
		moduleDefinition += std::string(SET_INDENT) + "assign out" + std::to_string(i) + "_valid_downstream = valid_downstream["+std::to_string(i)+"];\n";
	moduleDefinition += "\n";

	for (int i = 0; i < fanout; i++)
		moduleDefinition += std::string(SET_INDENT) + "assign stop_downstream["+std::to_string(i)+"] = out" + std::to_string(i) + "_stop_downstream;\n";
	moduleDefinition += "\n";

	for (int i = 0; i < fanout; i++)
	    moduleDefinition += std::string(SET_INDENT) + "assign out" + std::to_string(i) + " = in & {32{CGRA_Enable}};\n";;
    moduleDefinition += std::string(SET_INDENT) + "assign enable_downstream_valid_upstream = in_valid_upstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign enable_downstream_stop_upstream = in_stop_upstream;\n";
	vjson["definition"] = moduleDefinition;
    return vjson;
}

ElasticForkBranch::~ElasticForkBranch()
{	// Empty destructor
}

MRRG* ElasticForkBranch::createMRRG(unsigned II = 1)
{

    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for (unsigned i = 0; i < II; i++) 
    { 
	    // Create nodes and link them, we make this module look like a demux to the mapper
	    MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in", 0)).first;
        MRRG::NodeDescriptor cond = result.insert(MRRGNode::make_operand_pin(this, 1, i, "condition", {Operands::PREDICATE})).first;
        MRRG::NodeDescriptor fork = result.insert(MRRGNode::make_routing_function(this, data_size, i, "fork", 0, {Operands::BINARY_LHS, Operands::BINARY_RHS, Operands::BINARY_ANY}, {OpCode::BR}, 1)).first;
	    MRRG::NodeDescriptor fork_out = result.insert(MRRGNode::make_routing(this, data_size, i, "fork_out", 0, true)).first;
	    result.link(cond, fork);
	    result.link(in, fork);
        result.link(fork, fork_out);
	    for (int j = 0; j < fanout; j++) {
	        MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out" + std::to_string(j), 0)).first;
	        result.link(fork_out,   out);
	    }
    }

    return result_ptr;
}

BitConfig ElasticForkBranch::getBitConfig(
	const MRRG& mrrg, const OpGraph & og,
	const Mapping& map,
	const ConfigCell& ccell,
	const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
	const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_op_node;
    const std::vector<std::regex> required_node_regexes {
            std::regex("\\.fork$"), std::regex("\\.in$"),
    };
    const std::regex out_regex("\\.out([0-9]+)$");
    BitConfig bitConfig(mrrg.initiationInterval());

    const std::vector<std::regex> required_op_node_regexes {
        std::regex("\\.fork$"), 
        std::regex("\\.in$"),
        std::regex("\\.condition$")
    };
    // Organize nodes mapping by cycle
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
    bool opNode = false;
    int false_index = -1;
    int true_index = -1;
    
     for (int i = 0; i < valNodesByCycle.size(); i++) { 
        std::unordered_set<int> inputs_used;
        if (!opNodesByCycle[i].empty()) {
            auto mrrg_nodes_from_op_per_cycle = opNodesByCycle[i];

            int required_node_types_found = 0;
            std::vector<OpGraphOp*> brNodes;
            opNode = true;
            for (const auto& op_node_and_mrrg_nodes : mrrg_nodes_from_op_per_cycle) {
                for (const auto& req_node_regex : required_op_node_regexes) {
                    for (const auto& mrrg_node : op_node_and_mrrg_nodes.second) {
                        std::smatch match_results;
                        if (std::regex_search(mrrg_node->getHierarchyQualifiedName(), match_results, req_node_regex)) {
                            if (required_node_types_found > 2) {
                                throw cgrame_error("found a node that matched two required node regexes");
                            } else {
                                required_node_types_found++;
                                brNodes.push_back(op_node_and_mrrg_nodes.first);               
                            }
                        }
                    }
                }
            }
            auto  mrrg_nodes_from_val_per_cycle = valNodesByCycle[i];
            for (const auto& val_node_and_mrrg_nodes : mrrg_nodes_from_val_per_cycle) { //added
                if (required_node_types_found > 0) {
                    for (const auto& mrrg_node : val_node_and_mrrg_nodes.second) {
                        std::smatch match_results;
                        std::regex_search(mrrg_node->getHierarchyQualifiedName(), match_results, out_regex);
                        if (match_results.size() == 2) {
                            std::vector<MRRG::NodeDescriptor> vals_mapped = map.getMappingList(val_node_and_mrrg_nodes.first);
                            std::queue<const MRRGNode*> to_visit;
                            MRRG::NodeDescriptor last_node = mrrg_node;
                            to_visit.push(mrrg_node);
                            while (!to_visit.empty()) {
                                auto curr = to_visit.front();
                                to_visit.pop();
                                for (auto node : mrrg.fanout(curr)) {
                                    if (std::find(vals_mapped.begin(), vals_mapped.end(), node) != vals_mapped.end()) {
                                        auto it = std::find(vals_mapped.begin(), vals_mapped.end(), node);
                                        last_node = *it;
                                        to_visit.push(*it);
                                    }
                                }
                            }
                            auto f_node = mrrg.fanout(last_node)[0];
                            if (f_node->type != MRRGNode_Type::MRRG_NODE_FUNCTION && f_node->type != MRRGNode_Type::MRRG_NODE_ROUTING_FUNCTION) {
                                throw cgrame_error("Cannot find the connection for the val ");
                            }
                            int size_vector = og.opNodes().size();
                            OpGraph::OpDescriptor op = nullptr;
                            // loop through all op nodes to find same MRRG node as 'this'
                            // may be very slow
                            for (int i = 0; i < size_vector; i++) {
                                const auto current_node = map.getSingleMapping(og.opNodes()[i]);
                                if (current_node == f_node) {
                                    op = og.opNodes()[i];
                                    break;
                                }
                            }
                            if (op == nullptr) throw cgrame_error("Cannot find the op ");
                            //const OpGraphOp* output = map.getSingleMapping(f_node);
                            // Need to find the  condition of the input used
                            if( val_node_and_mrrg_nodes.first->getPredicateForOutput(op) ==  false){
                                false_index = stoi(match_results[1].str());
                            } else if ( val_node_and_mrrg_nodes.first->getPredicateForOutput(op) ==  true){
                                true_index = stoi(match_results[1].str());
                            } else{
                                throw cgrame_error("Error: Must specify must specify branch  " + val_node_and_mrrg_nodes.first->getName() + "  predicate value  ");
                            } 
                            inputs_used.insert(stoi(match_results[1].str()));
                        } else {

                        }
                    }
                }
            }
        } else {
            auto  mrrg_nodes_from_val_per_cycle = valNodesByCycle[i];
            for (const auto& val_node_and_mrrg_nodes : mrrg_nodes_from_val_per_cycle) {
                // Check for required nodes: .demux and .in
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

                // Check for which outputs are used in the mapping
                if (std::all_of(begin(required_node_types_found), end(required_node_types_found), [&](auto&& v) { return v; })) {
                    for (const auto& mrrg_node : val_node_and_mrrg_nodes.second) {
                        std::smatch match_results;
                        std::regex_search(mrrg_node->getHierarchyQualifiedName(), match_results, out_regex);
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
            bitConfig.add({(size_t) fanout *2 + 1, BitSetting::LOW}, cycle);
            
        } else if (opNode) {
            if (inputs_used.size() != 2) throw cgrame_error("Branch cannot point to more than 2 outputs");
            if (true_index == -1 || false_index == -1) throw cgrame_error("ERROR Branch: must specify the true and false predicates of the branch");
            // Create the bit settings,  in a one-hot manner
            used_cycles++;
            unsigned bit_settings = 0;
            bit_settings |= (1U << (unsigned) 0);
            for (const auto& input_num : inputs_used) {
                bit_settings |= (1U << (unsigned) input_num + 1);
                if (true_index == input_num)
                    bit_settings |= (1U << (unsigned) input_num + fanout + 1);
            }
            bitConfig.add(bitsettings_from_int((int) bit_settings, fanout * 2 + 1), cycle);

        } else {
            // Create the bit settings,  in a one-hot manner
            used_cycles++;
            unsigned bit_settings = 0;
            for (const auto& input_num : inputs_used) {
                bit_settings |= (1U << (unsigned) input_num + 1);
            }
            bitConfig.add(bitsettings_from_int((int) bit_settings, fanout * 2 + 1), cycle);
        }
        ++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}

/* Elastic Eager Fork */
ElasticEagerFork::ElasticEagerFork(std::string name, Location loc, int size, int fanout_, int unit_fork_fanout_, bool isElastic)
	: Module(name, loc, size, isElastic), fanout(fanout_), unit_fork_fanout(unit_fork_fanout_)
{
     if (isElastic == false) {make_and_throw<cgrame_error>([&](auto&& s) {
        s << "Elastic eager fork needs to be marked as elastic ";
    });}
    adds_synchronous_circuitry = true;
    isElastic = true;
    // Check module legality
    if (fanout <= 1) {make_and_throw<cgrame_error>([&](auto&& s) {
	    s << "Elastic fork doesn't support fanout of <=1. Requested fanout was " << fanout;
    });}
    if (unit_fork_fanout <= 1) {make_and_throw<cgrame_error>([&](auto&& s) {
	    s << "Elastic fork doesn't support unit fanout of <=1. Requested unit fanout was " << unit_fork_fanout;
	});}

    // Create ports
    addPort("in", PORT_INPUT, "size", size, isElastic);

    for (int i = 0; i < fanout; i++) {
	    addPort("out" + std::to_string(i), PORT_OUTPUT, "size", size, isElastic);
    }
    
    addPort("enable_downstream", PORT_CONFIG, fanout, isElastic);
}

std::string ElasticEagerFork::GenericName()
{
	return "elastic_eager_fork_1to" + std::to_string(fanout) + "_size_" + std::to_string(data_size);
}

nlohmann::json ElasticEagerFork::CoreIRGenFunctionality()
{
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_";
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

	// Functionality 
	int fork_layers = std::ceil(std::log(fanout) / std::log(unit_fork_fanout));
    moduleDefinition += std::string(SET_INDENT) + "wire stop_upstream;";
	moduleDefinition += std::string(SET_INDENT) + "elastic_eager_fork #(";
	moduleDefinition += ".FANOUT(" + std::to_string(fanout) + "), ";
	moduleDefinition += ".FORK_LAYERS(" + std::to_string(fork_layers) + "), ";
	moduleDefinition += ".UNIT_FORK_FANOUT(" + std::to_string(unit_fork_fanout) + ")) efork (\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".reset(CGRA_Reset),\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".valid_upstream(in_valid_upstream),\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".stop_upstream(stop_upstream),\n";

	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".valid_downstream({";
	for (int i = fanout-1; i > 0; i--)
		moduleDefinition += "out" + std::to_string(i) + "_valid_downstream, ";
	moduleDefinition += "out0_valid_downstream}),\n";	
	
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".stop_downstream({";
	for (int i = fanout-1; i > 0; i--)
		moduleDefinition += "out" + std::to_string(i) + "_stop_downstream, ";
	moduleDefinition += "out0_stop_downstream}),\n";


	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".enable_downstream(enable_downstream)\n";
	moduleDefinition += std::string(SET_INDENT) + ");\n";

	moduleDefinition += "\n";
	for (int i = 0; i < fanout; i++)
	    moduleDefinition += std::string(SET_INDENT) + "assign out" + std::to_string(i) + " = in & {32{CGRA_Enable}};\n";
        moduleDefinition += std::string(SET_INDENT) + "assign in_stop_upstream = stop_upstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign enable_downstream_valid_upstream = in_valid_upstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign enable_downstream_stop_upstream = in_stop_upstream;\n";
	vjson["definition"] = moduleDefinition;
    return vjson;
}

ElasticEagerFork::~ElasticEagerFork()
{	// Empty destructor
}

MRRG* ElasticEagerFork::createMRRG(unsigned II = 1)
{

    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for (unsigned i = 0; i < II; i++) 
    { 
	    // Create nodes and link them, we make this module look like a demux to the mapper
	    MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in", 0)).first;
	    MRRG::NodeDescriptor fork = result.insert(MRRGNode::make_routing(this, data_size, i, "fork", 0, true)).first;
	    result.link(in, fork);
	    for (int j = 0; j < fanout; j++) {
	        MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out" + std::to_string(j), 0)).first;
	        result.link(fork,   out);
	    }
    }

    return result_ptr;
}

BitConfig ElasticEagerFork::getBitConfig(
	const MRRG& mrrg, const OpGraph & og,
	const Mapping& map,
	const ConfigCell& ccell,
	const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
	const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_op_node;
    const std::vector<std::regex> required_node_regexes {
            std::regex("\\.fork$"), std::regex("\\.in$"),
    };
    const std::regex in_regex("\\.out([0-9]+)$");
    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromValNode> valNodesByCycle(mrrg.initiationInterval());
    for (const auto& val_and_mrrg_nodes : mrrg_nodes_from_val_node) {
        for (const auto& mrrg_node : val_and_mrrg_nodes.second) {
            valNodesByCycle[mrrg_node->cycle][val_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }
    
    int cycle = 0;
    int used_cycles = 0;
    for (auto & mrrg_nodes_from_val_per_cycle : valNodesByCycle) {
        std::unordered_set<int> inputs_used;

        for (const auto& val_node_and_mrrg_nodes : mrrg_nodes_from_val_per_cycle) {
            // Check for required nodes: .demux and .in
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

            // Check for which outputs are used in the mapping
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

        if (inputs_used.empty()) {
            bitConfig.add({(size_t) fanout, BitSetting::LOW}, cycle);
            
        } else {
            // Create the bit settings,  in a one-hot manner
            used_cycles++;
            unsigned bit_settings = 0;
            for (const auto& input_num : inputs_used) {
                bit_settings |= (1U << (unsigned) input_num);
            }
            bitConfig.add(bitsettings_from_int((int) bit_settings, fanout), cycle);
        }
        ++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}

/* Elastic Join */
ElasticJoin::ElasticJoin(std::string name, Location loc, int size, int fanin_, bool isElastic)
	: Module(name, loc, size, isElastic), fanin(fanin_)
{
	adds_synchronous_circuitry = false;
    if (isElastic == false) {make_and_throw<cgrame_error>([&](auto&& s) {
        s << "Elastic eager fork needs to be marked as elastic ";
    });}
	// Check module legality
	if (fanin <= 1) {make_and_throw<cgrame_error>([&](auto&& s) {
		s << "Elastic join doesn't support fanin of <=1. Requested fanin was " << fanin;
	});}

	// Create ports
	for (int i = 0; i < fanin; i++) {
		addPort("in" + std::to_string(i), PORT_INPUT, "size", size, isElastic);
		node_relative_position.insert({"in" + std::to_string(i), {static_cast<double>(i)/fanin, 0.25}});

		addPort("out" + std::to_string(i), PORT_OUTPUT, "size", size);
        node_relative_position.insert({"out", {static_cast<double>(i)/fanin, 0.5}});
	}
    addPort("out_stop_downstream", PORT_INPUT, 1);
    addPort("out_valid_downstream", PORT_OUTPUT, 1);
	addPort("enable", PORT_CONFIG, fanin, isElastic);
}

std::string ElasticJoin::GenericName()
{
	return "elastic_join_" + std::to_string(fanin) + "to1" + "_size_" + std::to_string(data_size);
}

nlohmann::json ElasticJoin::CoreIRGenFunctionality()
{
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist)
    {
	vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = {};

    for (auto& port : ports)
    {
	std::string portName = port.second->getName();
	vjson["interface"].push_back(portName);
    }

    // module definition
    std::string moduleDefinition;
    moduleDefinition += "parameter FANIN = " + std::to_string(fanin) + ";\n";
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


    moduleDefinition += std::string(SET_INDENT) + "wire [(FANIN-1):0] valid_upstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [(FANIN-1):0] stop_upstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign valid_upstream = {";
    for (int i = fanin-1; i > 0; i--)
	moduleDefinition += "in" + std::to_string(i) + "_valid_upstream, ";
    moduleDefinition += "in0_valid_upstream};\n";	

    moduleDefinition += std::string(SET_INDENT) + "assign  {";
    for (int i = fanin-1; i > 0; i--)
	moduleDefinition += "in" + std::to_string(i) + "_stop_upstream, ";
    moduleDefinition += "in0_stop_upstream} = stop_upstream;\n";
    // Internal masked upstream valids
    moduleDefinition +=  std::string(SET_INDENT) + "wire [(FANIN-1):0] masked_valid_upstream;\n";
    moduleDefinition +=  std::string(SET_INDENT) + "assign masked_valid_upstream = valid_upstream | (~enable);\n";
    moduleDefinition +=  std::string(SET_INDENT) + "assign out_valid_downstream = &masked_valid_upstream;\n"; 
    moduleDefinition +=  std::string(SET_INDENT) + "assign stop_upstream =  {FANIN{out_stop_downstream}} | (masked_valid_upstream & {FANIN{~(out_valid_downstream)}});\n";

    moduleDefinition += "\n";
    for (int i = 0; i < fanin; i++)
	    moduleDefinition += std::string(SET_INDENT) + "assign out" + std::to_string(i) + " = in" + std::to_string(i) + ";\n";
    moduleDefinition += std::string(SET_INDENT) + "assign enable_valid_upstream = out_valid_downstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign enable_stop_upstream = out_stop_downstream;\n";

    vjson["definition"] = moduleDefinition;
    return vjson;
}

ElasticJoin::~ElasticJoin()
{	// Empty destructor
}

MRRG* ElasticJoin::createMRRG(unsigned II = 1)
{
    

    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for (unsigned i = 0; i < II; i++) 
    { 
	    // Create nodes and link them, we make this module look like a demux to the mapper
	    for (int j = 0; j < fanin; j++) {
	        MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out" + std::to_string(j), 0)).first;
	        MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in" + std::to_string(j), 0)).first;
	        result.link(in, out);
	    }
        MRRG::NodeDescriptor out_result = result.insert(MRRGNode::make_routing(this, data_size, i, "out_result", 0)).first;
	    MRRG::NodeDescriptor in_result = result.insert(MRRGNode::make_routing(this, data_size, i, "in_result", 0)).first;
	    result.link(in_result, out_result);
    }

    return result_ptr;
}

BitConfig ElasticJoin::getBitConfig(
	const MRRG& mrrg, const OpGraph & og,
	const Mapping& map,
	const ConfigCell& ccell,
	const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
	const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_op_node;
    const std::vector<std::regex> required_node_regexes {
        std::regex("\\.out([0-9]+)$"),
    };
    const std::regex in_regex("\\.in([0-9]+)$");
    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromValNode> valNodesByCycle(mrrg.initiationInterval());
    for (const auto& val_and_mrrg_nodes : mrrg_nodes_from_val_node) {
        for (const auto& mrrg_node : val_and_mrrg_nodes.second) {
            valNodesByCycle[mrrg_node->cycle][val_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int cycle = 0;
    int used_cycles = 0;
    for (auto & mrrg_nodes_from_val_per_cycle : valNodesByCycle) {
        std::unordered_set<int> inputs_used;

        for (const auto& val_node_and_mrrg_nodes : mrrg_nodes_from_val_per_cycle) {
            // Check for required nodes
            std::vector<bool> required_node_types_found;
            for (const auto& req_node_regex : required_node_regexes) {
                required_node_types_found.push_back(false);
                for (const auto& mrrg_node : val_node_and_mrrg_nodes.second) {
                    std::smatch match_results;
                    if (std::regex_search(mrrg_node->getHierarchyQualifiedName(), match_results, req_node_regex)) {
                        //if (required_node_types_found.back()) {
                        //    throw cgrame_error("found a node that matched two required node regexes");
                        //} else {
                            required_node_types_found.back() = true;
                        //}
                    }
                }
            }

            // Check for which inputs are used in the mapping
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

        if (inputs_used.empty()) {
            bitConfig.add({(size_t) fanin, BitSetting::DONT_CARE_PREFER_LOW}, cycle);
        } else {
            // Create the bit settings,  in a one-hot manner
            used_cycles++;
            unsigned bit_settings = 0;
            for (const auto& input_num : inputs_used) {
                bit_settings |= (1U << (unsigned) input_num);
            }
            bitConfig.add(bitsettings_from_int((int) bit_settings, fanin), cycle);
        }
        ++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}


/* Elastic Diverge */
ElasticDiverge::ElasticDiverge(std::string name, Location loc, int size, int fanout_)
	: Module(name, loc, size), fanout(fanout_)
{
	adds_synchronous_circuitry = true;
	isElastic = true;
	// Check module legality
	if (fanout <= 1) {make_and_throw<cgrame_error>([&](auto&& s) {
		s << "Elastic diverge doesn't support fanout of <=1. Requested fanout was " << fanout;
	});}

	// Create ports
	// TODO: visualization	
	addPort("valid_upstream", PORT_INPUT, 1);
	addPort("stop_upstream", PORT_OUTPUT, 1);
	addPort("data_in", PORT_INPUT, "size", size);
	for (int i = 0; i < fanout; i++) {
		addPort("valid_downstream" + std::to_string(i), PORT_OUTPUT, 1);
		addPort("stop_downstream" + std::to_string(i), PORT_INPUT, 1);
		addPort("data_out" + std::to_string(i), PORT_OUTPUT, "size", size);
	}
	addPort("enable_downstream", PORT_INPUT, fanout);
}

std::string ElasticDiverge::GenericName()
{
	return "elastic_diverge_1to" + std::to_string(fanout) + "_" + std::to_string(getSize()) + "b";
}

nlohmann::json ElasticDiverge::CoreIRGenFunctionality()
{
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_";
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
    moduleDefinition += std::string(SET_INDENT) + "parameter DATA_WIDTH = " + std::to_string(getSize()) + ";\n";
    moduleDefinition += std::string(SET_INDENT) + "parameter FANOUT = " + std::to_string(fanout) + ";\n";
    moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
    moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
    moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";
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

    moduleDefinition += std::string(SET_INDENT) + "wire [(FANOUT-1):0] valid_downstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [(FANOUT-1):0] stop_downstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [DATA_WIDTH*FANOUT-1:0] data_out;\n";
    
    moduleDefinition += std::string(SET_INDENT) + "assign valid_downstream = {";
    for (int i = fanout-1; i > 0; i--)
	moduleDefinition += "valid_downstream" + std::to_string(i) + ", ";
    moduleDefinition += "valid_downstream0};\n";	
    moduleDefinition += std::string(SET_INDENT) + "assign stop_downstream = {";
    for (int i = fanout-1; i > 0; i--)
	moduleDefinition += "stop_downstream" + std::to_string(i) + ", ";
    moduleDefinition += "stop_downstream0};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {";
    for (int i = fanout-1; i > 0; i--)
	moduleDefinition += "data_out" + std::to_string(i) + ", ";
    moduleDefinition += "data_out0} = data_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [FANOUT-1:0] arbiter_request, arbiter_grant;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign arbiter_request = ~stop_downstream & enable_downstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "// Arbiter module, which we use to select in the case of multiple upstream modules ready\n";
    moduleDefinition += std::string(SET_INDENT) + "round_robin_arbiter #(FANOUT) arbiter\n";
    moduleDefinition += std::string(SET_INDENT) + "    (\n";
    moduleDefinition += std::string(SET_INDENT) + "     .clk(CGRA_Clock),\n";
    moduleDefinition += std::string(SET_INDENT) + "     .reset(CGRA_Reset),\n";
    moduleDefinition += std::string(SET_INDENT) + "     .req_in(arbiter_request),\n";
    moduleDefinition += std::string(SET_INDENT) + "     .grant_final(arbiter_grant),\n";
    moduleDefinition += std::string(SET_INDENT) + "     .memory_controller_waitrequest(~valid_upstream) // Upstream valid being low should pause the arbiter\n";
    moduleDefinition += std::string(SET_INDENT) + "    );\n";
    moduleDefinition += std::string(SET_INDENT) + "assign stop_upstream = ~(|arbiter_grant);\n";
    moduleDefinition += std::string(SET_INDENT) + "assign valid_downstream = arbiter_grant & {FANOUT{valid_upstream}};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign data_out = (valid_downstream) ? {FANOUT{data_in}} : 'bx;\n";
    vjson["definition"] = moduleDefinition;
    return vjson;
}

ElasticDiverge::~ElasticDiverge()
{	// Empty destructor
}

MRRG* ElasticDiverge::createMRRG(unsigned II = 1)
{
	// TODO: generate MRRG from module
	return new MRRG(II);
}

BitConfig ElasticDiverge::getBitConfig(
	const MRRG& mrrg, const OpGraph & og,
	const Mapping& map,
	const ConfigCell& ccell,
	const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
	const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
	// TODO: generate bitstream for module based on MRRG connectivity
    return NULL;
}

/************ ElasticSelMultiplexer **********/
const std::map<OpGraphOpCode, LLVMMode> ElasticSelMultiplexer::all_modes =
    {
        {OpCode::SELECT, {"op_sel", "sel", {"sel"}, "sel_sel"}},
        {OpCode::PHI, {"op_phi", "phi", {"phi"}, "phi_sel"}}
    };

/************ ElasticSelMultiplexer **********/
ElasticSelMultiplexer::ElasticSelMultiplexer(std::string name, Location loc, unsigned mux_size_, unsigned size, bool isElastic)
    : Module(name, loc, size, isElastic)
    , mux_size(mux_size_)
    {
    isElastic = true;
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
    addPort("select", PORT_CONFIG, mux_size, isElastic);
}
// Virtual function that overrides Module::GenericName. Returns generic name of the object
std::string ElasticSelMultiplexer::GenericName() // Virtual function override
{
    return "elastic_sel_mux_" + std::to_string(mux_size) + "to1_" + std::to_string(getSize()) + "b";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json ElasticSelMultiplexer::CoreIRGenFunctionality() // Virtual function override
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
	moduleDefinition += std::string(SET_INDENT) + "always @(*) begin\n";
	for (int i = 0; i < mux_size; ++i)
	{
	    std::string stringI = std::to_string(i);
	    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if(select[" + stringI + "] && in" + stringI + "_valid_upstream)begin\n";
	    moduleDefinition += std::string(SET_TRIPLE_INDENT) + " out = in" + stringI + ";\n";
	    moduleDefinition += std::string(SET_TRIPLE_INDENT) + " out_valid_downstream = in" + stringI + "_valid_upstream;\n";
	    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else ";
	}
    moduleDefinition += "begin\n";
	moduleDefinition += std::string(SET_TRIPLE_INDENT) + " out_valid_downstream = 0;\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
	moduleDefinition += std::string(SET_INDENT) + "end\n";
	for (int j = 0; j < mux_size; ++j){
		std::string stringJ = std::to_string(j);
		moduleDefinition += std::string(SET_INDENT) + "assign in" + stringJ + "_stop_upstream = out_stop_downstream & select[" + stringJ + "] ;\n";
	} 
    moduleDefinition += std::string(SET_INDENT) + "assign select_stop_upstream = out_stop_downstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign select_valid_upstream = out_valid_downstream;\n";
    

    vjson["definition"] = moduleDefinition;
    return vjson;
}

ElasticSelMultiplexer::~ElasticSelMultiplexer()
{
}

MRRG* ElasticSelMultiplexer::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i++)
    {
        MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out")).first;
        MRRG::NodeDescriptor mux = result.insert(MRRGNode::make_routing_function(this, data_size, i, "sel_inst", 0, {Operands::BINARY_LHS, Operands::BINARY_RHS, Operands::BINARY_ANY}, {OpCode::SELECT, OpCode::PHI}, 1)).first;
        result.link(mux, out);
        for(int j = 0; j < mux_size; j++)
        {
            MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in" + std::to_string(j))).first;

            result.link(in, mux);
        }
    }
    return result_ptr;
}

BitConfig ElasticSelMultiplexer::getBitConfig(
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
    bool op = false;
    for (int i = 0; i < valNodesByCycle.size(); i++) {    
        std::set<int> inputs_used;
        if (!opNodesByCycle[i].empty()){
            op = true;
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
            
            auto  mrrg_nodes_from_val_per_cycle = valNodesByCycle[i];
            for (const auto& val_node_and_mrrg_nodes : mrrg_nodes_from_val_per_cycle) { //added
                if (required_node_types_found > 0) {
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
            bitConfig.add({(size_t) mux_size, BitSetting::LOW}, cycle);
            
        } else if (inputs_used.size() > 1 && !op) {
            throw cgrame_error("Elastic Select Mux has more than input but it is not used as an select or phi operations");
        } else {
            // Create the bit settings,  in a one-hot manner
            used_cycles++;
            unsigned bit_settings = 0;
            for (const auto& input_num : inputs_used) {
                bit_settings |= (1U << (unsigned) input_num);
            }
            bitConfig.add(bitsettings_from_int((int) bit_settings, mux_size), cycle);
        }
        cycle++;       
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}


/* Elastic Merge */
ElasticMerge::ElasticMerge(std::string name, Location loc, int size, int fanin_)
	: Module(name, loc, size), fanin(fanin_)
{
	adds_synchronous_circuitry = true;
    isElastic = true;	
	// Check module legality
	if (fanin <= 1) {make_and_throw<cgrame_error>([&](auto&& s) {
		s << "Elastic merge doesn't support fanin of <=1. Requested fanin was " << fanin;
	});}

	// Create ports
	for (int i = 0; i < fanin; i++) {
		addPort("valid_upstream" + std::to_string(i), PORT_INPUT, 1);
		addPort("stop_upstream" + std::to_string(i), PORT_OUTPUT, 1);
		addPort("data_in" + std::to_string(i), PORT_INPUT, "size", size);
		node_relative_position.insert({"data_in" + std::to_string(i), {static_cast<double>(i)/fanin, 0.25}});
	}
	addPort("valid_downstream", PORT_OUTPUT, 1);
	addPort("stop_downstream", PORT_INPUT, 1);
	addPort("data_out", PORT_OUTPUT, "size", size);
	addPort("enable_upstream", PORT_INPUT, fanin);
    node_relative_position.insert({"data_out", {0.25, 0.5}});
    node_relative_position.insert({"merge", {0.5, 0.5}});
}

std::string ElasticMerge::GenericName()
{
	return "elastic_merge_" + std::to_string(fanin) + "to1" + "_size_" + std::to_string(data_size);
}

nlohmann::json ElasticMerge::CoreIRGenFunctionality()
{
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_";
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

	// Functionality 
	moduleDefinition += std::string(SET_INDENT) + "elastic_merge #(";
	moduleDefinition += ".FANIN(" + std::to_string(fanin) + "), ";
	moduleDefinition += ".DATA_WIDTH(" + std::to_string(getSize()) + ")) emerge (\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".clk(CGRA_Clock),\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".reset(CGRA_Reset),\n";

	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".valid_upstream({";
	for (int i = fanin-1; i > 0; i--)
		moduleDefinition += "valid_upstream" + std::to_string(i) + ", ";
	moduleDefinition += "valid_upstream0}),\n";	
	
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".stop_upstream({";
	for (int i = fanin-1; i > 0; i--)
		moduleDefinition += "stop_upstream" + std::to_string(i) + ", ";
	moduleDefinition += "stop_upstream0}),\n";
	
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".valid_downstream(valid_downstream),\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".stop_downstream(stop_downstream),\n";

	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".data_in({";
	for (int i = fanin-1; i > 0; i--)
		moduleDefinition += "data_in" + std::to_string(i) + ", ";
	moduleDefinition += "data_in0}),\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".data_out(data_out),\n";


	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".enable_upstream(enable_upstream)\n";
	moduleDefinition += std::string(SET_INDENT) + ");\n";

	vjson["definition"] = moduleDefinition;
    return vjson;
}

ElasticMerge::~ElasticMerge()
{	// Empty destructor
}

MRRG* ElasticMerge::createMRRG(unsigned II = 1)
{

    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for (unsigned i = 0; i < II; i++) 
    { 
	// Create nodes and link them, we make this module look like a demux to the mapper
	MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "data_out", 0)).first;
	MRRG::NodeDescriptor merge = result.insert(MRRGNode::make_routing(this, data_size, i, "merge", 0)).first;
	result.link(merge, out);
	for (int j = 0; j < fanin; j++) {
	    MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "data_in" + std::to_string(j), 0)).first;
	    result.link(in, merge);
	}
    } 

    return result_ptr;
}

BitConfig ElasticMerge::getBitConfig(
	const MRRG& mrrg, const OpGraph & og,
	const Mapping& map,
	const ConfigCell& ccell,
	const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
	const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_op_node;
    const std::vector<std::regex> required_node_regexes {
            std::regex("\\.merge$"), std::regex("\\.data_out$"),
    };
    const std::regex in_regex("\\.data_in([0-9]+)$");
    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromValNode> valNodesByCycle(mrrg.initiationInterval());
    for (const auto& val_and_mrrg_nodes : mrrg_nodes_from_val_node) {
        for (const auto& mrrg_node : val_and_mrrg_nodes.second) {
            valNodesByCycle[mrrg_node->cycle][val_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int cycle = 0;
    int used_cycles = 0;
    for (auto & mrrg_nodes_from_val_per_cycle : valNodesByCycle) {
        std::unordered_set<int> inputs_used;

        for (const auto& val_node_and_mrrg_nodes : mrrg_nodes_from_val_per_cycle) {
            // Check for required nodes: .demux and .in
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

            // Check for which outputs are used in the mapping
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

        if (inputs_used.empty()) {
            bitConfig.add({(size_t) fanin, BitSetting::DONT_CARE_PREFER_LOW}, cycle);
        } else {
            // Create the bit settings,  in a one-hot manner
            used_cycles++;
            unsigned bit_settings = 0;
            for (const auto& input_num : inputs_used) {
                bit_settings |= (1U << (unsigned) input_num);
            }
            bitConfig.add(bitsettings_from_int((int) bit_settings, fanin), cycle);
        }
        ++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}


const std::map<OpGraphOpCode, LLVMMode> FPUnit_wrapper::all_modes =
{ 
 {OpCode::FADD,    {"op_add",      "add",         {"8"},      "add_sel"}}, // janders changed to 8 from Adham's 7
  {OpCode::FMUL,    {"op_multiply", "multiply",    {"0"},      "mul_sel"}},
  {OpCode::FDIV,    {"op_divide",   "divide",      {"20"},      "div_sel"}},
  {OpCode::SQRT,   {"op_sqrt",    "sqrt",        {"17"},       "sqrt_sel"}},
};

//module definition
FPUnit_wrapper::FPUnit_wrapper(std::string name, Location loc, int max_latency, std::vector<OpGraphOpCode> supported_modes_) 
    : Module(name, loc), max_latency(max_latency), supported_modes(std::move(supported_modes_))
{
    adds_synchronous_circuitry = true; //Wrapper has sequential elements
    isElastic = true;
     //Create ports
    addPort("opcode", PORT_INPUT, ceil(log2(supported_modes.size())));
    addPort("valid_upstream", PORT_INPUT, 1);
    addPort("stop_downstream", PORT_INPUT, 1);
    addPort("valid_downstream", PORT_OUTPUT_REG, 1);
    addPort("stop_upstream", PORT_OUTPUT_REG, 1); 
}

//Virtual function that override Module::GenericName. Returns generic name for the wrapper
std::string FPUnit_wrapper::GenericName()
{
  return "fpunit_wrapper_size_" + std::to_string(data_size);
}

//Virtual function that overrides Module::CoreIRGetFunctionality. Generates functionality for the wrapper
nlohmann::json FPUnit_wrapper::CoreIRGenFunctionality()
{
  nlohmann::json vjson;

  //Create Header
  vjson["prefix"] = "cgrame_";
  //No parameters defined
  vjson["interface"] = {};
  vjson["interface"].push_back("CGRA_Clock");
  vjson["interface"].push_back("CGRA_Reset");
  vjson["interface"].push_back("CGRA_Enable");

  for (auto& port : ports)
  { 
    std::string portName = port.second->getName();
    vjson["interface"].push_back(portName);
  }

  //module definition
  std::string moduleDefinition;
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Clock;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Reset;\n";
  moduleDefinition += std::string(SET_INDENT) + "input CGRA_Enable;\n";
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
    
  // Functionality
    moduleDefinition += std::string(SET_INDENT) + "reg [" + std::to_string(max_latency) + ":0] register;\n";
    moduleDefinition += std::string(SET_INDENT) + "reg [" + std::to_string((int)(ceil(log2(supported_modes.size()))-1)) + ":0] curr_opcode;\n";
    moduleDefinition += "\n"; 
    moduleDefinition += std::string(SET_INDENT) + "always@(posedge CGRA_Clock) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "stop_upstream <= 0;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "curr_opcode <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (opcode != curr_opcode) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "if (|register) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + std::string(SET_DOUBLE_INDENT) + "stop_upstream <= 1;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end else begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + std::string(SET_DOUBLE_INDENT) + "stop_upstream <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + std::string(SET_DOUBLE_INDENT) + "curr_opcode <= opcode;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (opcode == curr_opcode) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "curr_opcode <= opcode;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (~stop_downstream) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "stop_upstream <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (stop_downstream) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "stop_upstream <= 1;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";
    moduleDefinition += "\n"; 
    moduleDefinition += std::string(SET_INDENT) + "always@(posedge CGRA_Clock) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "register <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (~stop_downstream) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "register <= {valid_upstream, register[" + std::to_string(max_latency) + ":1]};\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (stop_downstream) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "register <= register;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";
    moduleDefinition += "\n";
    moduleDefinition += std::string(SET_INDENT) + "always@(*) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "valid_downstream <= 0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else begin\n";
    for (unsigned i = 0; i < supported_modes.size(); i++)
    {
      if (i == 0) {
	moduleDefinition += std::string(SET_TRIPLE_INDENT) + "if (curr_opcode == " + std::to_string(i) + ") begin\n";
      }
      else {
      	moduleDefinition += std::string(SET_TRIPLE_INDENT) + "else if (curr_opcode == " + std::to_string(i) + ") begin\n";
      }
      if (std::stoi(all_modes.at(supported_modes.at(i)).Functionality.at(0)) != 0) {
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + std::string(SET_INDENT) + "if (~register[" + std::to_string((max_latency - std::stoi(all_modes.at(supported_modes.at(i)).Functionality.at(0))) + 1) + "]) begin\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + std::string(SET_TRIPLE_INDENT) + "valid_downstream <= 0;\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + std::string(SET_INDENT) + "end\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + std::string(SET_INDENT) + "else if (register[" + std::to_string(max_latency - std::stoi((all_modes.at(supported_modes.at(i)).Functionality.at(0))) + 1) + "]) begin\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + std::string(SET_TRIPLE_INDENT) + "valid_downstream <= 1;\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + std::string(SET_INDENT) + "end\n";
      } else {
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + std::string(SET_TRIPLE_INDENT) + "valid_downstream <= valid_upstream;\n";
      }
      moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end\n";
    }
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n";

    vjson["definition"] = moduleDefinition;
  return vjson;

}

//Class destructor
FPUnit_wrapper::~FPUnit_wrapper()
{
  //Empty Destructor
}



BitConfig FPUnit_wrapper::getBitConfig(
  const MRRG& mrrg, const OpGraph & og,
  const Mapping& map,
  const ConfigCell& ccell,
  const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
  const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
  (void)ccell;
  (void)mrrg_nodes_from_val_node;
  const auto bits_needed = std::lround(ceil(log2(supported_modes.size())));

  BitConfig bitConfig(mrrg.initiationInterval());

  // Organize nodes mapping by cycle
  std::vector<MRRGNodesFromOpNode> opNodesByCycle(mrrg.initiationInterval());
  for (const auto& op_and_mrrg_nodes : mrrg_nodes_from_op_node) {
    for (const auto& mrrg_node : op_and_mrrg_nodes.second) {
      opNodesByCycle[mrrg_node->cycle][op_and_mrrg_nodes.first].insert(mrrg_node);
    }
  }

  int cycle = 0;
  int used_cycles = 0;
  for (auto & op_and_mrrg_nodes : opNodesByCycle) {
    if (op_and_mrrg_nodes.empty()) {
      bitConfig.add({ (size_t)bits_needed, BitSetting::DONT_CARE_PREFER_LOW }, cycle);
    } else if (op_and_mrrg_nodes.size() == 1) {
        const auto find_result = std::find(begin(supported_modes), end(supported_modes), begin(op_and_mrrg_nodes)->first->opcode);
        if (find_result == end(supported_modes)) {
          throw cgrame_error("couldn't find op in supported modes list");
        } else {
            used_cycles++;
            bitConfig.add( bitsettings_from_int(std::distance(begin(supported_modes), find_result), bits_needed), cycle);
        }

    } else {
        throw cgrame_error("expect either 0 or 1 op nodes");
    }
    ++cycle;
  }
  bitConfig.setUsedCycles(used_cycles);
  return bitConfig;
}

//Creating Module Routing Resource Graph
MRRG* FPUnit_wrapper::createMRRG(unsigned II = 1)
{

    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for (unsigned i = 0; i < II; i++) 
    { 
	    MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out" + std::to_string(0), 0)).first;
	    MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in" + std::to_string(0), 0)).first;
	    result.link(in, out);
    }

    return result_ptr;
}

/* Elastic Variable-Latency Unit wrapper */
ElasticVLUWrapper::ElasticVLUWrapper(std::string name, Location loc, int size, int input_count_)
	: Module(name, loc, size), input_count(input_count_)
{
	adds_synchronous_circuitry = false;
    isElastic = true;
	// Create Ports
	for (int i = 0; i < input_count; i++) {
        addPort("data_in" + std::to_string(i), PORT_INPUT, "size", size);
        node_relative_position.insert({"data_in" + std::to_string(i), {0, static_cast<double>(i)/input_count}});
    }
	addPort("data_out", PORT_OUTPUT, "size", size);
    node_relative_position.insert({"data_out", {0.75, 0.5}});

	addPort("valid_upstream", PORT_INPUT, 1);
	addPort("stop_upstream", PORT_OUTPUT, 1);
	addPort("valid_downstream", PORT_OUTPUT, 1);
	addPort("stop_downstream", PORT_INPUT, 1);

	addPort("vlu_go", PORT_OUTPUT, 1);
	addPort("vlu_ready", PORT_INPUT, 1);
	addPort("vlu_done", PORT_INPUT, 1);
	addPort("vlu_ack", PORT_OUTPUT, 1);
	for (int i = 0; i < input_count; i++) {
        addPort("vlu_data_in" + std::to_string(i), PORT_OUTPUT, "size", size);
        node_relative_position.insert({"vlu_data_in" + std::to_string(i), {0.25, static_cast<double>(i) / input_count}});
    }
	addPort("vlu_data_out", PORT_INPUT, "size", size);
    node_relative_position.insert({"vlu_data_out", {0.5, 0.5}});
}

std::string ElasticVLUWrapper::GenericName()
{
	return "elastic_vluwrapper_" + std::to_string(getSize()) + "b";
}

nlohmann::json ElasticVLUWrapper::CoreIRGenFunctionality()
{
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist)
    {
	vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = {};

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

    // Functionality
    moduleDefinition += std::string(SET_INDENT) + "elastic_vlu_wrapper #(";
    moduleDefinition += ".DATA_WIDTH(" + std::to_string(getSize()) + "), ";
    moduleDefinition += ".INPUT_COUNT(" + std::to_string(2) + ")) vlu_wrapper (\n";

    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".data_in({";
    for (int i = input_count-1; i > 0; i--)
	moduleDefinition += "data_in" + std::to_string(i) + ", ";
    moduleDefinition += "data_in0}),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".data_out(data_out),\n";

    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".valid_upstream(valid_upstream),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".stop_upstream(stop_upstream),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".valid_downstream(valid_downstream),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".stop_downstream(stop_downstream),\n";

    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".vlu_go(vlu_go),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".vlu_ready(vlu_ready),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".vlu_done(vlu_done),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".vlu_ack(vlu_ack),\n";

    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".vlu_data_in({";
    for (int i = input_count-1; i > 0; i--)
	moduleDefinition += "vlu_data_in" + std::to_string(i) + ", ";
    moduleDefinition += "vlu_data_in0}),\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".vlu_data_out(vlu_data_out)\n";

    moduleDefinition +=  std::string(SET_INDENT) + ");\n";

    vjson["definition"] = moduleDefinition;
    return vjson;
}

ElasticVLUWrapper::~ElasticVLUWrapper()
{	// Empty destructor
}

MRRG* ElasticVLUWrapper::createMRRG(unsigned II = 1)
{

    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for (unsigned i = 0; i < II; i++) 
    { 
	    // Create nodes and link them, we make this module look like a demux to the mapper
	    MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "data_out", 0)).first;
	    MRRG::NodeDescriptor vlu_out = result.insert(MRRGNode::make_routing(this, data_size, i, "vlu_data_out", 0)).first;
	    result.link(vlu_out, out);
	    for (int j = 0; j < input_count; j++) {
	        MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "data_in" + std::to_string(j), 0)).first;
	        MRRG::NodeDescriptor vlu_in = result.insert(MRRGNode::make_routing(this, data_size, i, "vlu_data_in" + std::to_string(j), 0)).first;
	        result.link(in, vlu_in);
	    }
    }

    return result_ptr;
}

std::string ElasticFuncUnit::GenericName()
{
    return "elastic_func_unit_" + name + "_size_" + std::to_string(data_size);
}

// Writing this function since the connection among the functional unit and join are not consistent with the rest of the archtecture
// Hence this wrapper code is needed
ElasticFuncUnit::ElasticFuncUnit(std::string name, Location loc, std::vector<OpGraphOpCode> supported_modes_, unsigned size, int II, int latency, bool predExist, bool isElastic, int contexts) 
    : Module(name, loc, size) {
    // If pred exist then token inducer is not needed since there should be no operation feeding into itself 
    // PORTS
	addPort("in_a", PORT_INPUT, "size", size, isElastic);
	addPort("in_b", PORT_INPUT, "size", size, isElastic);
	addPort("out", PORT_OUTPUT, "size", size, isElastic);
    
    // Function modules 
    addSubModule(new ElasticJoin(name + "_join", loc, 32, 2), 0.25, 0.3, 0.5, 0.1);
    addSubModule(new FuncUnit(name + "_ALU", loc, supported_modes_, size, II, latency, isElastic), 0, 0.75, 0.5, 0.25);
    
    // Add Config cells
    addConfig(name + "_ALUconfig", {name + "_ALU.select"}, II, false);
    addConfig(name + "_InputJoinConfig", {name + "_join.enable"}, II, isElastic);

    //Connections among modules 
    connectPorts("this.in_a", name + "_join.in0", isElastic);
    connectPorts("this.in_b", name + "_join.in1", isElastic);
    addConnection(name + "_join.out0", name + "_ALU.in_a");
    addConnection(name + "_join.out1", name + "_ALU.in_b");
    
    if (predExist) {
        addConnection(name + "_ALU.out", "this.out");
        addConnection(name + "_join.out_valid_downstream", "this.out_valid_downstream", false);
        addConnection("this.out_stop_downstream", name + "_join.out_stop_downstream", false);
        return;
    }
    
    // Adding token inducer if pred does not exist
    addSubModule(new ElasticTokenInducer(name + "_token_inducer", loc, 32, II), 0.5, 0.45, 0.5, 0.05);
    addConfig(new ElasticConfigCell(name + "_adapterToken", II), {name + "_token_inducer.token"});
    addConnection(name + "_ALU.out", name +  "_token_inducer.data_in");
    addConnection(name + "_join.out_valid_downstream", name + "_token_inducer.data_in_valid_upstream", false);
    addConnection(name + "_token_inducer.data_in_stop_upstream", name + "_join.out_stop_downstream", false);
    if ( II > 1) {
	    addConnection(name + "adapterToken.contexts_used", name + "_token_inducer.contexts_used", false);
    }

    connectPorts(name + "_token_inducer.data_out", "this.out", isElastic);
}

std::string ElasticFPUnit::GenericName()
{
    return "elastic_fp_unit_" + name + "_size_" + std::to_string(data_size);
}

// Writing this function since the connection among the functional unit and join are not consistent with the rest of the archtecture
// Hence this wrapper code is needed
ElasticFPUnit::ElasticFPUnit(std::string name, Location loc, std::vector<OpGraphOpCode> supported_modes_, unsigned size, int II, int latency, bool predExist, bool isElastic, int contexts) 
    : Module(name, loc, size) {
    // If pred exist then token inducer is not needed since there should be no operation feeding into itself 
    // PORTS
	addPort("in_a", PORT_INPUT, "size", size, isElastic);
	addPort("in_b", PORT_INPUT, "size", size, isElastic);
	addPort("out", PORT_OUTPUT, "size", size, isElastic);
    addSubModule(new FPUnit(name +"_FPALU", loc, supported_modes_, size, II, latency), 0.5, 0.4, 0.5, 0.1);
    // FP wrapper for the elastic protocol.  This is needed because the FloPoCo FP units are non-elastic
    addSubModule(new FPUnit_wrapper(name + "_fp_wrapper", loc, 22, supported_modes_), 1.0, 0.4, 0.5, 0.1);
    
    // Function modules 
    addSubModule(new ElasticJoin(name + "_join", loc, 32, 2), 0.25, 0.3, 0.5, 0.1);
    
    // Add Config cells
    addConfig(name + "_FPUnitConfig",{name + "_FPALU.opcode", name + "_fp_wrapper.opcode"}, II, false); 
    addConfig(name + "_InputJoinConfig", {name + "_join.enable"}, II, isElastic);

    //Connections among modules 
    connectPorts("this.in_a", name + "_join.in0", isElastic);
    connectPorts("this.in_b", name + "_join.in1", isElastic);
    addConnection(name + "_join.out0", name + "_FPALU.in_a");
    addConnection(name + "_join.out1", name + "_FPALU.in_b");
    addConnection(name + "_join.out_valid_downstream", name +"_fp_wrapper.valid_upstream", false);
    addConnection(name + "_fp_wrapper.stop_upstream", name + "_join.out_stop_downstream", false);
    addConnection("this.out_stop_downstream", name + "_FPALU.FP_clk_en", false); // OK add clock enable for FloPoCo cores
    
    if (predExist) {
        addConnection(name + "_FPALU.out", "this.out");
        addConnection( name + "_fp_wrapper.valid_downstream", "this.out_valid_downstream", false);
        addConnection("this.out_stop_downstream", name + "_fp_wrapper.stop_downstream", false);
        return;
    }
    
    // Adding token inducer if pred does not exist
    addSubModule(new ElasticTokenInducer(name + "_token_inducer", loc, 32, II), 0.5, 0.45, 0.5, 0.05);
    addConfig(new ElasticConfigCell(name + "_adapterToken", II), {name + "_token_inducer.token"});
    addConnection(name + "_FPALU.out", name +  "_token_inducer.data_in");
    addConnection(name + "_fp_wrapper.valid_downstream", name + "_token_inducer.data_in_valid_upstream", false);
    addConnection(name + "_token_inducer.data_in_stop_upstream", name + "_fp_wrapper.stop_downstream", false);
    if ( II > 1) {
	    addConnection(name + "adapterToken.contexts_used", name + "_token_inducer.contexts_used", false);
    }

    connectPorts(name + "_token_inducer.data_out", "this.out", isElastic);
}

/* Elastic FU-VLU adapter */
ElasticTokenInducer::ElasticTokenInducer(std::string name, Location loc, int size, int II, bool isElastic)
	: Module(name, loc, size, isElastic)
{
    iteration_interval = II;
    adds_synchronous_circuitry = true;
    if (isElastic == false) {make_and_throw<cgrame_error>([&](auto&& s) {
        s << "Elastic eager fork needs to be marked as elastic ";
    });}
    // Create ports
    addPort("data_in", PORT_INPUT, "size", size, isElastic);
    addPort("data_out", PORT_OUTPUT, "size", size, isElastic);

    addPort("token", PORT_CONFIG, 1, isElastic);
    if (II > 1) {
        addPort("contexts_used", PORT_INPUT, ceil(log2(II)));
    }
    // Visualization
    node_relative_position.insert({"data_in", {0, 0}});
    node_relative_position.insert({"data_out", {0, 0}});
}

std::string ElasticTokenInducer::GenericName()
{
	return "elastic_fu_vlu_adapter_" + std::to_string(getSize()) + "b";
}

nlohmann::json ElasticTokenInducer::CoreIRGenFunctionality()
{
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_";
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

    // Functionality
    moduleDefinition += std::string(SET_INDENT) + "reg ["+std::to_string((int)ceil(log2(iteration_interval))) + "-1:0] counter;\n";
	moduleDefinition += std::string(SET_INDENT) + "reg pass;\n\n";
    moduleDefinition += std::string(SET_INDENT) + "always @(posedge CGRA_Clock, posedge CGRA_Reset) begin\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset == 'b1) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "pass <= 'b0;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "counter <= 'b0;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (~data_out_stop_downstream) begin\n";
    if (iteration_interval > 1) {
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "if("+std::to_string((int)ceil(log2(iteration_interval))) + " > 0)\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "if (counter > contexts_used) begin \n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "pass <= 1'b0;\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "end else begin\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "pass <= token;\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "counter <= counter + 1'b1;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end else begin\n";
    }
    moduleDefinition += std::string(SET_QUAD_INDENT) + "if (counter == 1) begin \n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "pass <= 1'b0;\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "end else begin\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "pass <= token;\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "counter <= counter + 1'b1;\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "end\n";
    if (iteration_interval > 1) {
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end\n";
    }
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n\n";   

    moduleDefinition += std::string(SET_INDENT) + "assign data_in_stop_upstream = data_out_stop_downstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign data_out_valid_downstream = data_in_valid_upstream | pass;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign token_stop_upstream = data_in_stop_upstream;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign token_valid_upstream = 'b1;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign data_out = data_in;\n";

	vjson["definition"] = moduleDefinition;
    return vjson;
}

ElasticTokenInducer::~ElasticTokenInducer()
{	// Empty destructor
}

MRRG* ElasticTokenInducer::createMRRG(unsigned II = 1)
{

    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for (unsigned i = 0; i < II; i++) 
    { 
	    // Create nodes, model as a 0-delay wire
	    MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "data_in", 0)).first;
	    MRRG::NodeDescriptor token_inducer = result.insert(MRRGNode::make_routing(this, data_size, i, "token_inducer", 1)).first;
	    MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "data_out", 0)).first;

	    // Connect nodes
	    const auto& nodes_this_cycle = makeNodeGetterForCycle(result, i);
	    result.link(nodes_this_cycle("data_in"), nodes_this_cycle("data_out"));
    }

    return result_ptr;
}

BitConfig ElasticTokenInducer::getBitConfig(
	const MRRG& mrrg, const OpGraph & og,
	const Mapping& map,
	const ConfigCell& ccell,
	const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
	const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_op_node;
    const std::regex in_regex("\\.data_in$");
    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromValNode> valNodesByCycle(mrrg.initiationInterval());

    // create an index from (Module, OpGraphVal) to a set of MRRG nodes mapped to that OpGraphVal within that Module
    std::set<const MRRGNode*> mrrgnodes_mapped_to;
    for(auto* val : og.valNodes()) {
        for(auto* n : map.getMappingList(val)) {
            mrrgnodes_mapped_to.insert(n);
        }
    }

    for(auto* op : map.getOpGraph().opNodes()) {
        for (auto* n : map.getMappingList(op)) {
            mrrgnodes_mapped_to.insert(n);
        }
    }


    for (const auto& val_and_mrrg_nodes : mrrg_nodes_from_val_node) {
        for (const auto& mrrg_node : val_and_mrrg_nodes.second) {
            valNodesByCycle[mrrg_node->cycle][val_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }
    
    int cycle = 0;
    int used_cycles = 0;
    for (const auto & mrrg_nodes_from_val_per_cycle : valNodesByCycle) {
	if (mrrg_nodes_from_val_per_cycle.empty()) {
            bitConfig.add({BitSetting::LOW}, cycle);
    } else {
	    bool enableToken;
	    for (const auto & op_val_and_nodes: mrrg_nodes_from_val_per_cycle) {
		for (const auto & mrrg_node : op_val_and_nodes.second) {
		    auto name = mrrg_node->getHierarchyQualifiedName();
            
		    if (regex_search(name, in_regex)) {
			    // Considering doing an error check here to see if "m_in" shows up twice
			    std::set<const MRRGNode*> visited;
			    enableToken = isNodeInCycle(mrrg_node, mrrg_node, visited, mrrgnodes_mapped_to);
		    }
		}
	    }
	    if (enableToken) {
            used_cycles++;
		    bitConfig.add({BitSetting::HIGH}, cycle);
	    } else {
		    bitConfig.add({BitSetting::LOW}, cycle);
	    }
    }
        ++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}

bool ElasticTokenInducer::isNodeInCycle( const MRRGNode* originNode, const MRRGNode* thisNode, std::set<const MRRGNode*>& visited, std::set<const MRRGNode*>& mrrgnodes_mapped_to) const { 

    // Mark the current node as visited
    visited.insert(thisNode);
 
    // Recur for all the fanout vertices
    for (const auto& fanoutNode : thisNode->fanout){
         
        // If a fanout node is not visited, then recur for that node
        if (visited.find(fanoutNode) == visited.end() && mrrgnodes_mapped_to.find(fanoutNode) != mrrgnodes_mapped_to.end())
        {
           if (isNodeInCycle(originNode, fanoutNode, visited, mrrgnodes_mapped_to))
              return true;
        }
        // If a fanout node is visited and
        // is the origin node,then there exists a cycle that the node is part of.
        else if (fanoutNode == originNode) {
	        return true;
	    }
    }
    return false;
}

/* Elastic Register File */
ElasticRegisterFile::ElasticRegisterFile(std::string name, Location loc, int numInputPorts_, int numOutputPorts_, int log2Registers_, int size, int contexts, int eb_depth_)
	: Module(name, loc, size)
	, numInputPorts(numInputPorts_)
	, numOutputPorts(numOutputPorts_)
	, log2Registers(log2Registers_)
	, eb_depth(eb_depth_)
{
    adds_synchronous_circuitry = true;
    isElastic = true;

    if (log2Registers < 1) { make_and_throw<cgrame_error>([&](auto&& s) {
	    s << "RegisterFile doesn't support less than 2 registers. log2Registers = " << log2Registers;
	    });}

    // Generating input ports
    for (int i = 0; i < numInputPorts; i++)
    {
	    addPort("in" + std::to_string(i), PORT_INPUT, size, isElastic); // input data
	    addSubModule(new ElasticEagerFork("rf_input_fork" + std::to_string(i), loc, size,  1 << log2Registers, 2), 0.1 + static_cast<double>(i)/numOutputPorts, 0.1, 1.0/numInputPorts, 0.3);
	    addConfig("rf_INPUTFork" + std::to_string(i) + "Config", {"rf_input_fork" + std::to_string(i) + ".enable_downstream"}, contexts, isElastic);
	    connectPorts ("this.in" + std::to_string(i), "rf_input_fork" + std::to_string(i) + ".in", isElastic); 
    }

    // Generating output ports
    for (int i = 0; i < numOutputPorts; i++)
    {
	    addPort("out" + std::to_string(i), PORT_OUTPUT_REG,  size, isElastic); // output ports/registers
    }

    // Generating input muxes and registers
    for (int i = 0; i < (1 << log2Registers); ++i) {
	    addSubModule(new Register("reg" + std::to_string(i), loc, size, isElastic), 0.1 + static_cast<double>(i)/(1 << log2Registers), 0.3, 1.0/(1 << log2Registers),0.2);
	    addConfig(name + "Reg" + std::to_string(i) + "Config", {"reg" + std::to_string(i) + ".enable"}, contexts, isElastic);

	    if (numOutputPorts > 1) {
	        addSubModule(new ElasticEagerFork("rf_output_fork" + std::to_string(i), loc, size,  numOutputPorts, 2), 0.1 + i ,0.1, 1.0/numInputPorts, 0.3);
	        addConfig("rf_OUTPUTFork" + std::to_string(i) + "Config", {"rf_output_fork" + std::to_string(i) + ".enable_downstream"}, contexts, isElastic);
	        connectPorts ("reg" + std::to_string(i) + ".out", "rf_output_fork" + std::to_string(i) + ".in", isElastic); 
	    }
	    if (numInputPorts > 1) {
	        addSubModule(new Multiplexer("in_mux" + std::to_string(i), loc, numInputPorts, size, isElastic), 0.1 + static_cast<double>(i)/numOutputPorts, 0.1, 1.0/numInputPorts, 0.3);
	        addConfig(name + "InMux" + std::to_string(i) + "Config", {"in_mux" + std::to_string(i) + ".select"}, contexts, isElastic);

	        // Connect input mux to reg
	        connectPorts ("in_mux" + std::to_string(i) + ".out", "reg" + std::to_string(i) + ".in", isElastic);
	        for (int j = 0; j < numInputPorts; ++j) {
	    	    connectPorts ("rf_input_fork" + std::to_string(j) + ".out" + std::to_string(i), "in_mux" + std::to_string(i) + ".in" + std::to_string(j), isElastic);
	        }
	    }
	    else {
	        connectPorts ("rf_input_fork0.out" + std::to_string(i), "reg" + std::to_string(i) + ".in", isElastic);
	    } 
    }

    // Generating output muxes
    for (int i = 0; i < numOutputPorts; ++i) {
        addSubModule(new Multiplexer("out_mux" + std::to_string(i), loc, (1 << log2Registers), size, isElastic), 0.1 + static_cast<double>(i)/numOutputPorts, 0.7, 1.0/numOutputPorts, 0.3);
        addConfig(name + "OutMux" + std::to_string(i) + "Config", {"out_mux" + std::to_string(i) + ".select"}, contexts, isElastic);

	    // Connect every register to this output mux
	    for (int j = 0; j < (1 << log2Registers); ++j) {

	        if ( numOutputPorts > 1) {
	    	    connectPorts ("rf_output_fork" + std::to_string(j) + ".out" + std::to_string(i), "out_mux" + std::to_string(i) + ".in" + std::to_string(j), isElastic);
	        } else {
	    	    connectPorts ("reg" + std::to_string(j) + ".out", "out_mux" + std::to_string(i) + ".in" + std::to_string(j), isElastic);
	        }
	    }
        connectPorts("out_mux" + std::to_string(i) + ".out", "this.out" + std::to_string(i), isElastic);
    }

    // Setting positions for nodes
    for (int i = 0; i < numInputPorts; i++) {
	    setNodePosition("in" + std::to_string(i), static_cast<double>(i*log2Registers+1)/(numInputPorts*log2Registers+2), 0.1);
    }

    for (int i = 0; i < numOutputPorts; ++i) {
	    setNodePosition("out" + std::to_string(i), static_cast<double>(i+1)/(numOutputPorts+2), 0.95);
    }

}

std::string ElasticRegisterFile::GenericName()
{
    return "elastic_registerFile_" + std::to_string(numInputPorts) + "in_" + std::to_string(numOutputPorts) + "out_" + std::to_string(getSize()) + "b";
}

ElasticRegisterFile::~ElasticRegisterFile()
{
}

// CoreIR Implementation of GenFunctionality
nlohmann::json ElasticRegisterFile::CoreIRGenFunctionality() // Virtual function override
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

    // Functionality
    moduleDefinition += std::string(SET_INDENT) + "// Setting the always blocks and inside registers\n";
    moduleDefinition += std::string(SET_INDENT) + "reg [size-1:0] register[2**log2regs-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "always@(posedge CGRA_Clock, posedge CGRA_Reset)\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if(CGRA_Reset)\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "begin : RESET\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "integer i;\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "for (i = 0; i < 2**log2regs; i = i+1)\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) + "register[i] = 0;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (CGRA_Enable)\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "begin\n";
    for (int i = 0; i < numOutputPorts; i++)
        moduleDefinition += std::string(SET_QUAD_INDENT) + "out" + std::to_string(i) + " = " + "register[address_out" + std::to_string(i) + "];\n";
    for (int i = 0; i < numInputPorts; i++)
    {
        moduleDefinition += std::string(SET_QUAD_INDENT) + "if(WE" + std::to_string(i) + ")\n";
        moduleDefinition += std::string(SET_PENTA_INDENT) + "register[address_in" + std::to_string(i) + "] = in" + std::to_string(i) + ";\n";
    }
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end";

    vjson["definition"] = moduleDefinition;
    return vjson;
}

/* Elastic Memory port */
ElasticMemPort::ElasticMemPort(std::string name, Location loc, int size, int eb_depth_, int num_connections_, bool pred, int contexts)
	: Module(name, loc, size)
	, eb_depth(eb_depth_)
	, num_connections(num_connections_)
{
    isElastic = true;
    // Legality checks
    if(num_connections < 2) { make_and_throw<cgrame_error>([&](auto&& s) {
	    s << "ElasticMemPort doesn't support num_connections < 2. num_connections = " << num_connections;
	    });}

    // Add ports
    for (int i = 0; i < num_connections; i++) {
	    addPort("in" + std::to_string(i), PORT_INPUT, "size", size, isElastic);
	    addPort("out" + std::to_string(i), PORT_OUTPUT, "size", size, isElastic);
    }

    // Add Input forks
    for (int i = 0; i < num_connections; i++) {
	    addSubModule(new ElasticEagerFork("fork_in" + std::to_string(i), loc, size, 2, 2, isElastic), 0, 0, 0, 0);
	    addConfig("ForkIn" + std::to_string(i) + "Config", {"fork_in" + std::to_string(i) + ".enable_downstream"}, contexts, isElastic);
	    connectPorts("this.in" + std::to_string(i), "fork_in" + std::to_string(i) + ".in", isElastic);
    }

    addSubModule(new ElasticBufferFifo("mem_eb_in" , loc, eb_depth, 32), 0.0, 0.4, 0.5, 0.1);
    addSubModule(new ElasticBufferFifo("mem_eb_addr", loc, eb_depth, 32), 0.0, 0.4, 0.5, 0.1);

    //addSubModule(new ElasticBufferFifoWithEnable("mem_eb_out" , eb_depth, 32, false), 0.0, 0.4, 0.5, 0.1);
    // Output Fork
    addSubModule(new ElasticEagerFork("fork_out", loc, size, num_connections, 2), 0, 0, 0, 0);
    
    //MemUnit instantiation
    addSubModule(new MemoryUnit("mem_unit", loc, size, isElastic, pred), 0, 0, 0, 0);

    if (pred) {
        for (int i = 0; i < num_connections; i++)
            addPort("pred" + std::to_string(i), PORT_INPUT, 1, isElastic);
        addSubModule(new Multiplexer("mux_pred", loc, num_connections, 1, isElastic), 0.6, 0.4, 0.5, 0.2);
        for(int i = 0; i < num_connections; i++)
            connectPorts("this.pred" + std::to_string(i), "mux_pred.in" + std::to_string(i), isElastic);
        connectPorts("mux_pred.out", "mem_unit.pred", isElastic);
        addConfig("MuxPred", {"mux_pred.select"}, contexts, isElastic);
    }

    addConfig("EBEnableIn", {"mem_eb_in.enable"}, contexts, isElastic);
    addConfig("EBEnableAddr", {"mem_eb_addr.enable"}, contexts, isElastic);
    //addConfig(new ConfigCell("EBEnableOut"), {"mem_eb_out.enable"});

    // Add address, data MUXes
    addSubModule(new Multiplexer("mux_addr", loc, num_connections, size, isElastic), 0.6, 0.2, 0.5, 0.2);
    addSubModule(new Multiplexer("mux_data", loc, num_connections, size, isElastic), 0.6, 0.4, 0.5, 0.2);
    addSubModule(new Multiplexer("mux_in_out", loc, 2, size, isElastic), 0.6, 0.4, 0.5, 0.2);
    addSubModule(new DeMux("demux_in_out", loc, 2, 32, isElastic), 0.0, 0.6, 0.5, 0.1);

    addConfig("ForkOutConfig", {"fork_out.enable_downstream"}, contexts, isElastic);
    addConfig("MuxInOut", {"mux_in_out.select"}, contexts, isElastic);
    addConfig("DeMuxInOut", {"demux_in_out.select"}, contexts, isElastic);
    addConfig("MuxAddr", {"mux_addr.select"}, contexts, isElastic);
    addConfig("MuxData", {"mux_data.select"}, contexts, isElastic);
    addConfig("WriteEnable", {"mem_unit.w_rq"}, contexts, isElastic);
    for (int i = 0; i < num_connections; i++) {
	    connectPorts("fork_in" + std::to_string(i) + ".out0", "mux_addr.in" + std::to_string(i), isElastic);
	    connectPorts("fork_in" + std::to_string(i) + ".out1", "mux_data.in" + std::to_string(i), isElastic);
    }

    connectPorts("mux_data.out", "mux_in_out.in0", isElastic);
    connectPorts("mem_unit.data_out", "mux_in_out.in1", isElastic);
    connectPorts("mux_in_out.out", "mem_eb_in.data_in", isElastic);
    connectPorts("mem_eb_in.data_out", "demux_in_out.in", isElastic);
    connectPorts("demux_in_out.out0", "mem_unit.data_in", isElastic);
    connectPorts("demux_in_out.out1", "fork_out.in", isElastic); 
    connectPorts("mux_addr.out", "mem_eb_addr.data_in", isElastic);
    connectPorts("mem_eb_addr.data_out", "mem_unit.addr", isElastic);

    for (int i = 0; i < num_connections; i++) {
	    connectPorts("fork_out.out" + std::to_string(i) , "this.out" + std::to_string(i), isElastic);
    }
}

std::string ElasticMemPort::GenericName()
{
	return "elastic_memport_" + std::to_string(num_connections) + "conn_" + std::to_string(getSize()) + "b";
} 

ElasticMemPort::~ElasticMemPort()
{
}

std::string ElasticCrossbar::GenericName()
{
    return "elastic_crossbar_" + std::to_string(num_inputs) + "in_" + std::to_string(num_outputs) + "out" + "_size_" + std::to_string(data_size);
}

ElasticCrossbar::ElasticCrossbar(std::string name, Location loc, int num_inputs, int num_outputs, int data_size, int type, bool predExist, std::string pred_type, int contexts)
    : Module(name, loc, data_size)
    , num_inputs(num_inputs)
    , num_outputs(num_outputs)
{
    bool isFull = pred_type.compare("full") == 0 & predExist;
    bool isPartial = pred_type.compare("partial") == 0 & predExist;
    if (pred_type.compare("full") != 0 & pred_type.compare("partial") != 0) {
        std::cout << pred_type << std::endl;    
        throw cgrame_error("Predication type should be full or partial");
    }
    isElastic = true;
    if (type < 0 || type > 5) { make_and_throw<cgrame_error>([&](auto&& s) {
	    s << "Need to insert type of elastic crossbar " << type <<"\n";
	    s << "0: no merge and no diverge \n";
	    s << "1: just merge and no diverge \n";
	    s << "2: no merge and just all diverge \n";
	    s << "3: no merge and just diverge for ALU \n";
	    s << "4: merge and all diverge \n";
	    s << "5: merge and diverge for ALU \n";
	    });}
    int iter = 0;
    if (type == 3 || type == 5) { 
	iter = 2;
    } else if (type == 2 || type == 4) { 
	iter = num_inputs;
    }
    isElastic = true;
    if (type > 1 && type < 6) { 

	    for(int i = (num_inputs - iter); i < num_inputs; i++) {
	        // Demux to choose between fork/diverge
	        addSubModule(new DeMux("out_demux" + std::to_string(i), loc, 2, data_size, isElastic), 0.0, 0.6, 0.5, 0.1);

	        // Output diverge
	        addSubModule(new ElasticDiverge("output_diverge" + std::to_string(i), loc, data_size, num_outputs), 0.5, 0.7, 0.5, 0.1);

	        // Output muxes
	        for (int j = 0; j < num_outputs; j++) {
	    	    addSubModule(new Multiplexer("out_mux" + std::to_string(i) + "_" + std::to_string(j), loc, 2, data_size, isElastic), 0.0, 0.9, 0.5, 0.1);
	        }
	        addConfig("DivergeSelectConfig" + std::to_string(i), {"output_diverge" +  std::to_string(i) + ".enable_downstream"}, contexts, isElastic);

	        std::vector<std::string> mux_out_config_conns;
	        mux_out_config_conns.push_back("out_demux" + std::to_string(i) + ".select");
	        for (int j = 0; j < num_outputs; j++) {
	    	    mux_out_config_conns.push_back("out_mux" + std::to_string(i) + "_" + std::to_string(j) + ".select");
	        }
	        addConfig("MuxOutConfig" + std::to_string(i), mux_out_config_conns, contexts, isElastic);
	    }
    }

    // 1. Create num_output muxes (each num_input in size)
    for(int i = 0; i < num_outputs; i++) {
	    if (type == 0 || type == 2 || type == 3) {
	        // Connects to regA for HyCube
            if (isFull){
                //addSubModule(new SelMultiplexer("mux_" + std::to_string(i), loc, num_inputs, data_size, isElastic), 0.2, static_cast<double>(i)/num_outputs, 0.6, 0.8/num_outputs);
                addSubModule(new ElasticSelMultiplexer("mux_" + std::to_string(i), loc, num_inputs, data_size, isElastic), 0.2, static_cast<double>(i)/num_outputs, 0.6, 0.8/num_outputs);
            }else if (isPartial) {
                addSubModule(new SelMultiplexer("mux_" + std::to_string(i), loc, num_inputs, data_size, isElastic), 0.2, static_cast<double>(i)/num_outputs, 0.6, 0.8/num_outputs);
            }
            else{
                addSubModule(new Multiplexer("mux_" + std::to_string(i), loc, num_inputs, data_size, isElastic), 0.2, static_cast<double>(i)/num_outputs, 0.6, 0.8/num_outputs);
            }

        }else if (type == 1 || type == 4 || type == 5)
	        addSubModule(new ElasticMerge("merge_" + std::to_string(i), loc, data_size, num_inputs), 0.2, static_cast<double>(i)/num_outputs, 0.6, 0.8/num_outputs);
    }

    // 2. Create the configuration cells for each mux
    for(int i = 0; i < num_outputs; i++)
    {
	    if (type == 0 || type == 2 || type == 3)
	        addConfig("Mux" + std::to_string(i) + "config", {"mux_" + std::to_string(i) + ".select"}, contexts, isElastic);
	    else if (type == 1 || type == 4 || type == 5)
	        addConfig("Merge" + std::to_string(i) + "config", {"merge_" + std::to_string(i) + ".enable_upstream"}, contexts, isElastic);
    }

    // 3. Wiring and Ports
    // 3A. Create input and output ports
    // Input ports:
    for(int i = 0; i < num_inputs; i++)
    {
	    addPort("in" + std::to_string(i), PORT_INPUT, data_size, isElastic);
        if (isFull) {
	        addSubModule(new ElasticForkBranch("fork_crossbar" + std::to_string(i), loc, data_size, num_outputs, true), 0, 0, 0, 0);
        } else {
	        addSubModule(new ElasticEagerFork("fork_crossbar" + std::to_string(i), loc, data_size, num_outputs, 2), 0, 0, 0, 0);
        }
	    addConfig("ForkCrossbar" + std::to_string(i) + "Config", {"fork_crossbar" + std::to_string(i) + ".enable_downstream"}, contexts, isElastic);
	    node_relative_position.insert({"in" + std::to_string(i), {0, static_cast<double>(i)/num_inputs}});  // Add position for visualization
    }
    // If pred exist a port to get in the select predicate
    if (predExist){ 
        addPort("pred_in", PORT_INPUT, 1, isElastic);
        addSubModule(new ElasticEagerFork("fork_predicate", loc, 1, num_inputs, 2), 0, 0, 0, 0);
        addConfig("ForkPredicateConf", {"fork_predicate.enable_downstream"}, contexts, isElastic);
        connectPorts("this.pred_in", "fork_predicate.in", isElastic);
    }

    // Output ports:
    for(int i = 0; i < num_outputs; i++)
    {
	    addPort("out" + std::to_string(i), PORT_OUTPUT, data_size, isElastic);
	    node_relative_position.insert({"out" + std::to_string(i), {1, static_cast<double>(i)/num_outputs}});    // Add position for visualization
    }

    for(int i = 0; i < num_inputs; i++)
    {
        if (isFull) {
            connectPorts("fork_predicate.out" + std::to_string(i), "fork_crossbar" + std::to_string(i) + ".condition", isElastic);
        }
	    if (iter == num_inputs || iter >= (num_inputs - i)) {
	        connectPorts("this.in"+std::to_string(i), "out_demux" + std::to_string(i) + ".in", isElastic); 
	        // Fork/Diverge select demux
	        connectPorts("out_demux" + std::to_string(i) + ".out0", "fork_crossbar" + std::to_string(i) + ".in", isElastic);
	        connectPorts("out_demux" + std::to_string(i) + ".out1", "output_diverge" + std::to_string(i) + ".data_in", isElastic); // TODO: change to true when diverge implemented

	        // Output fork connections
	        for (int j = 0; j < (num_outputs); j++) {
	    	    // Downstream connections (upstream already connected)
	    	    connectPorts("fork_crossbar" + std::to_string(i) + ".out" + std::to_string(j), "out_mux" + std::to_string(i) + "_" + std::to_string(j) + ".in0", isElastic);
	        }

	        // Output diverge connections
	        for (int j = 0; j < (num_outputs); j++) {
	    	// Diverge -> eb connections
	    	    connectPorts("output_diverge" + std::to_string(i) + ".data_out" + std::to_string(j), "out_mux" + std::to_string(i) + "_" + std::to_string(j) + ".in1", isElastic); // TODO: change to true when diverge implemented
	        }

	    } else {
	        connectPorts("this.in"+std::to_string(i), "fork_crossbar" + std::to_string(i) + ".in", isElastic); 
	    }
    }


    // 3B. Wiring ports to muxes
    // Crossbar input ports to mux inputs:
    for(int j = 0; j < num_outputs; j++)
    {
        if (isPartial) {
            connectPorts("fork_predicate.out" + std::to_string(j), "mux_" + std::to_string(j) + ".selectpred", isElastic);
        }
	    for(int i = 0; i < num_inputs; i++)
	    {
	        if ( type == 0 || type == 2 || type == 3) {
	    	    if (iter == num_inputs || iter >= (num_inputs - i)) {
	    	        connectPorts("out_mux" + std::to_string(i) + "_" + std::to_string(j) +  ".out", "mux_" + std::to_string(j) + ".in" + std::to_string(i), isElastic);
	    	    } else {
	    	        connectPorts("fork_crossbar" + std::to_string(i) + ".out" + std::to_string(j), "mux_" + std::to_string(j) + ".in" + std::to_string(i), isElastic);
	    	    }
	        } else if ( type == 1 || type == 4 || type == 5) { 
	    	    if (iter == num_inputs || iter >= (num_inputs - i)) {
	    	        connectPorts("out_mux" + std::to_string(i) + "_" + std::to_string(j) +  ".out", "merge_" + std::to_string(j) + ".data_in" + std::to_string(i), isElastic);
	    	    } else {
	    	        connectPorts("fork_crossbar" + std::to_string(i) + ".out" + std::to_string(j), "merge_" + std::to_string(j) + ".data_in" + std::to_string(i), isElastic);
	    	    }
	        }
	    }
    }
	   
    // Mux outputs to crossbar output ports:
    for(int i = 0; i < num_outputs; i++)
    {
	    if (type == 0 || type == 2 || type == 3) {
	        connectPorts("mux_" + std::to_string(i) + ".out", "this.out" + std::to_string(i), isElastic);
	    } else if (type == 1 || type == 4 || type == 5) {
	        connectPorts("merge_" + std::to_string(i) + ".data_out", "this.out" + std::to_string(i), isElastic);
	    }
    }
}

/************ ConfigCell **********/
ElasticConfigCell::ElasticConfigCell(std::string name, int II, Location loc)
    : ConfigCell(name, II)
{
    configCell_II = II;
    addPort("valid", PORT_INPUT, 1);
    addPort("stop", PORT_INPUT, 1);
    if (configCell_II > 1){
        addPort("contexts_used", PORT_OUTPUT, ceil(log2(configCell_II)));
    }
}

nlohmann::json ElasticConfigCell::CoreIRGenFunctionality()
{
    nlohmann::json vjson;
    auto context_size = (parameterlist["contexts"] == 1) ? "0" : "$clog2(contexts)-1";
    // Construct Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist)
    {
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
    if (configCell_II > 1) {
        vjson["interface"].push_back("contexts_used");
    }
    vjson["interface"].push_back("select");
    vjson["interface"].push_back("valid");
    vjson["interface"].push_back("stop");

    // module definition
    std::string moduleDefinition;
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Clock;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Reset;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input CGRA_Enable;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input ConfigIn;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input Config_Clock;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input Config_Reset;\n";
    moduleDefinition += std::string(SET_INDENT) +    "output ConfigOut;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input valid;\n";
    moduleDefinition += std::string(SET_INDENT) +    "input stop;\n";
    moduleDefinition += std::string(SET_INDENT) +    "output [size-1:0] select;\n";
    moduleDefinition += std::string(SET_INDENT) +    "reg [size-1:0] config_reg [contexts-1:0];\n\n";
    moduleDefinition += std::string(SET_INDENT) +    "reg [" + context_size + ":0] context_counter;\n";
    if (configCell_II > 1){
        moduleDefinition += std::string(SET_INDENT) +    "output reg [" + context_size + ":0] contexts_used;\n";
    } else {
        moduleDefinition += std::string(SET_INDENT) +    "reg [" + context_size + ":0] contexts_used;\n";
    }
    moduleDefinition += std::string(SET_INDENT) +    "integer i;\n\n";

    moduleDefinition += std::string(SET_INDENT) +    "always @(posedge Config_Clock, posedge Config_Reset)\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (Config_Reset)\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +           "for (i = 0; i < contexts; i = i+1) begin\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) +              "config_reg[i] <= 'd0;\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +           "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +           "for (i = 0; i < contexts; i = i+1) begin\n";
    moduleDefinition += std::string(SET_PENTA_INDENT) +              "if(i == 0) begin\n";
    if (parameterlist["size"] > 1) {
        if (parameterlist["contexts"] > 1) {
            if (parameterlist["contexts"] == 2){
                moduleDefinition += std::string(SET_HEXA_INDENT) +                   "contexts_used <= ConfigIn;\n";   
                moduleDefinition += std::string(SET_HEXA_INDENT) +                   "config_reg[i] <= {contexts_used,config_reg[i][size-1:1]};\n"; 
            } else {
                moduleDefinition += std::string(SET_HEXA_INDENT) +                   "contexts_used <= {ConfigIn,contexts_used[" + context_size + ":1]};\n";
                moduleDefinition += std::string(SET_HEXA_INDENT) +                   "config_reg[i] <= {contexts_used[0],config_reg[i][size-1:1]};\n";
            }
            
        }
        else {
            moduleDefinition += std::string(SET_HEXA_INDENT) +                   "config_reg[i] <= {ConfigIn,config_reg[i][size-1:1]};\n";
        }
        moduleDefinition += std::string(SET_PENTA_INDENT) +              "end\n";
        moduleDefinition += std::string(SET_PENTA_INDENT) +              "else begin\n";
        moduleDefinition += std::string(SET_HEXA_INDENT) +                   "config_reg[i] <= {config_reg[i-1][0],config_reg[i][size-1:1]};\n";
    }
    else {
        if (parameterlist["contexts"]> 1) {
            if (parameterlist["contexts"] == 2){
                moduleDefinition += std::string(SET_HEXA_INDENT) +                   "contexts_used <= ConfigIn;\n";    
                moduleDefinition += std::string(SET_HEXA_INDENT) +                   "config_reg[i] <= contexts_used;\n";
            } else {
                moduleDefinition += std::string(SET_HEXA_INDENT) +                   "contexts_used <= {ConfigIn,contexts_used[" + context_size + ":1]};\n";
                moduleDefinition += std::string(SET_HEXA_INDENT) +                   "config_reg[i] <= contexts_used[0];\n";
            }
        }
        else {
            moduleDefinition += std::string(SET_HEXA_INDENT) +                   "config_reg[i] <= ConfigIn;\n";
        }
        moduleDefinition += std::string(SET_PENTA_INDENT) +              "end\n";
        moduleDefinition += std::string(SET_PENTA_INDENT) +              "else begin\n";
        moduleDefinition += std::string(SET_HEXA_INDENT) +                   "config_reg[i] <= config_reg[i-1];\n";
    }

    moduleDefinition += std::string(SET_PENTA_INDENT) +              "end\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +           "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "end\n\n";
    if (parameterlist["contexts"]> 1) {
        moduleDefinition += std::string(SET_INDENT) +    "always @(posedge CGRA_Clock, posedge CGRA_Reset)\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if(CGRA_Reset)\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "begin\n";
        moduleDefinition += std::string(SET_QUAD_INDENT) +           "context_counter <= 'd0;\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "end\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (valid && !stop) begin\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) +  "if(context_counter > contexts_used)\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "begin\n";
        moduleDefinition += std::string(SET_QUAD_INDENT) +           "context_counter <= 'd0;\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "end\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if(CGRA_Enable)\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "begin\n";
        moduleDefinition += std::string(SET_QUAD_INDENT) + "if(context_counter == contexts_used)\n";
        moduleDefinition += std::string(SET_PENTA_INDENT) +     "begin\n";
        moduleDefinition += std::string(SET_HEXA_INDENT) +           "context_counter <= 'd0;\n";
        moduleDefinition += std::string(SET_PENTA_INDENT) +     "end\n";
        moduleDefinition += std::string(SET_QUAD_INDENT) + "else\n";
        moduleDefinition += std::string(SET_PENTA_INDENT) +           "context_counter <= context_counter + 'b1;\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "end\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) +     "end\n";
        moduleDefinition += std::string(SET_INDENT) +    "assign select = config_reg[context_counter];\n";
    } else {
        moduleDefinition += std::string(SET_INDENT) +    "assign select = config_reg[0];\n";
    }
    moduleDefinition += std::string(SET_INDENT) +    "assign ConfigOut = config_reg[contexts-1][0];";

    vjson["definition"] = moduleDefinition;
    return vjson;
}

std::string ElasticConfigCell::GenericName()
{
    return "elastic_configcell_context_size" + std::to_string(getStorageSize())+ "II"+std::to_string(configCell_II);
}

/* Elastic Lazy Fork */
ElasticLazyFork::ElasticLazyFork(std::string name, Location loc, int size, int fanout_, int unit_fork_fanout_, int impl_type_)
	: Module(name, loc, size), fanout(fanout_), unit_fork_fanout(unit_fork_fanout_), impl_type(impl_type_)
{
	adds_synchronous_circuitry = false;
    isElastic = true;
	// Check module legality
	if (fanout <= 1) {make_and_throw<cgrame_error>([&](auto&& s) {
		s << "Elastic fork doesn't support fanout of <=1. Requested fanout was " << fanout;
	});}
	if (unit_fork_fanout <= 1) {make_and_throw<cgrame_error>([&](auto&& s) {
		s << "Elastic fork doesn't support unit fanout of <=1. Requested unit fanout was " << unit_fork_fanout;
	});}

	// Create ports
    addPort("valid_upstream", PORT_INPUT, 1);
    addPort("stop_upstream", PORT_OUTPUT, 1);
    addPort("in", PORT_INPUT, "size", size);
    node_relative_position.insert({"in", {0.25, 0.25}});
    node_relative_position.insert({"fork", {0.5, 0.25}});
    for (int i = 0; i < fanout; i++) {
        addPort("valid_downstream" + std::to_string(i), PORT_OUTPUT, 1);
        addPort("stop_downstream" + std::to_string(i), PORT_INPUT, 1);
        addPort("out" + std::to_string(i), PORT_OUTPUT, "size", size);
        node_relative_position.insert({"out" + std::to_string(i), {static_cast<double>(i)/fanout, 0.5}});
    }
    addPort("enable_downstream", PORT_INPUT, fanout);
}

std::string ElasticLazyFork::GenericName()
{
	return "elastic_lazy_fork_1to" + std::to_string(fanout) + "_size_" + std::to_string(data_size);
}

nlohmann::json ElasticLazyFork::CoreIRGenFunctionality()
{
    nlohmann::json vjson;

    // Create Header
    vjson["prefix"] = "cgrame_";
    vjson["parameters"] = {};
    for (auto& parameter : parameterlist)
    {
        vjson["parameters"].push_back(parameter.first);
    }
    vjson["interface"] = {};

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

	// Functionality 
	moduleDefinition += std::string(SET_INDENT) + "elastic_lazy_fork #(";
	moduleDefinition += ".FANOUT(" + std::to_string(fanout) + "), ";
	moduleDefinition += ".UNIT_FORK_FANOUT(" + std::to_string(unit_fork_fanout) + "), ";
	moduleDefinition += ".IMPLEMENTATION_TYPE(" + std::to_string(impl_type) + ") efork (\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".valid_upstream(valid_upstream),\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".stop_upstream(stop_upstream),\n";

	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".valid_downstream({";
	for (int i = fanout-1; i > 0; i--)
		moduleDefinition += "valid_downstream" + std::to_string(i) + ", ";
	moduleDefinition += "valid_downstream0}),\n";	
	
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".stop_downstream({";
	for (int i = fanout-1; i > 0; i--)
		moduleDefinition += "stop_downstream" + std::to_string(i) + ", ";
	moduleDefinition += "stop_downstream0}),\n";

	moduleDefinition += std::string(SET_DOUBLE_INDENT) + ".enable_downstream(enable_downstream)\n";
	moduleDefinition += std::string(SET_INDENT) + ");\n";

    moduleDefinition += "\n";
    for (int i = 0; i < fanout; i++)
        moduleDefinition += std::string(SET_INDENT) + "assign out" + std::to_string(i) + " = in;\n";

	vjson["definition"] = moduleDefinition;
    return vjson;
}

ElasticLazyFork::~ElasticLazyFork()
{	// Empty destructor
}

MRRG* ElasticLazyFork::createMRRG(unsigned II = 1)
{

    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for (unsigned i = 0; i < II; i++) 
    { 
	    // Create nodes and link them, we make this module look like a demux to the mapper
	    MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in", 0)).first;
	    MRRG::NodeDescriptor fork = result.insert(MRRGNode::make_routing(this, data_size, i, "fork", 0)).first;
    	result.link(in, fork);
	    for (int j = 0; j < fanout; j++) {
	        MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out" + std::to_string(j), 0)).first;
	        result.link(fork, out);
	    }
    }

    return result_ptr;
}

BitConfig ElasticLazyFork::getBitConfig(
	const MRRG& mrrg, const OpGraph & og,
	const Mapping& map,
	const ConfigCell& ccell,
	const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
	const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_op_node;
    const std::vector<std::regex> required_node_regexes {
            std::regex("\\.fork$"), std::regex("\\.in$"),
    };
    const std::regex in_regex("\\.out([0-9]+)$");
    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromValNode> valNodesByCycle(mrrg.initiationInterval());
    for (const auto& val_and_mrrg_nodes : mrrg_nodes_from_val_node) {
        for (const auto& mrrg_node : val_and_mrrg_nodes.second) {
            valNodesByCycle[mrrg_node->cycle][val_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int cycle = 0;
    int used_cycles = 0;
    for (auto & mrrg_nodes_from_val_per_cycle : valNodesByCycle) {
        std::unordered_set<int> inputs_used;

        for (const auto& val_node_and_mrrg_nodes : mrrg_nodes_from_val_per_cycle) {
            // Check for required nodes: .demux and .in
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

            // Check for which outputs are used in the mapping
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

        if (inputs_used.empty()) {
            bitConfig.add({(size_t) fanout, BitSetting::DONT_CARE_PREFER_LOW}, cycle);
        } else {
            // Create the bit settings,  in a one-hot manner
            used_cycles++;
            unsigned bit_settings = 0;
            for (const auto& input_num : inputs_used) {
                bit_settings |= (1U << (unsigned) input_num);
            }
            bitConfig.add(bitsettings_from_int((int) bit_settings, fanout), cycle);
        }
        ++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}
