/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/Module.h>

#include <regex>

/************ FuncUnit **********/
// Map that stores all possible modes of a FuncUnit
const std::map<OpGraphOpCode, LLVMMode> FuncUnit::all_modes =
{
    // Adding functionality to the funcunit
    // You can add a mode to the FuncUnit by specifying all_modes.push_back ({"module name", {"functionality"}, "wire name that feeds into the multiplexer"});
    // Functionality can be multiple lines, hence the reason why it is a vector. Each string represents one line, hence the reason it is a vector
	{OpCode::NOP,    {"op_nop",      "nop",          {"assign c = a;"},           "nop_sel"}},
    {OpCode::ADD,    {"op_add",      "add",         {"assign c = a + b ;"},      "add_sel"}},
    {OpCode::SUB,    {"op_sub",      "sub",         {"assign c = a - b;"},      "sub_sel"}},
    {OpCode::MUL,    {"op_multiply", "multiply",    {"assign c = a * b;"},      "mul_sel"}},
    {OpCode::DIV,    {"op_divide",   "divide",      {"assign c = a / b;"},      "div_sel"}},
    {OpCode::AND,    {"op_and",      "and",         {"assign c = a & b;"},      "and_sel"}},
    {OpCode::OR,     {"op_or",       "or",          {"assign c = a | b;"},      "or_sel"}},
    {OpCode::XOR,    {"op_xor",      "xor",         {"assign c = a ^ b;"},      "xor_sel"}},
    {OpCode::SHL,    {"op_shl",      "shl",         {"assign c = a << b;"},     "shl_sel"}},
    {OpCode::LSHR,   {"op_lshr",     "lshr",        {"assign c = a >> b;"},     "lshr_sel"}},
    {OpCode::ASHR,   {"op_ashr",     "ashr",        {"assign c = a >>> b;"},    "ashr_sel"}},
    {OpCode::CONST,  {"op_const",    "const",       {"//const;"},               "const_sel"}},
    {OpCode::LOAD,   {"op_load",     "load",        {"//load;"},                "load_sel"}},
    {OpCode::STORE,  {"op_store",    "store",       {"//store;"},               "store_sel"}},
    {OpCode::ICMP,   {"op_cmp",	     "cmp",	    {"assign c = a == b? 1: 0;"},"cmp_sel"}}
};

// Returns a unique name for a funcunit
std::string FuncUnit::GenericName()
{
    std::string NameToReturn = "func_" + std::to_string(getSize()) + "b"; // Base name is "func"
    for (unsigned i = 0; i < supported_modes.size(); i++)
    {
        // Add a "_(module_name_inside)" to the name
        NameToReturn.append("_");
        NameToReturn.append(all_modes.at(supported_modes.at(i)).OpName);
    }
    return NameToReturn;
}

// FuncUnit constructor
FuncUnit::FuncUnit(std::string name, Location loc, std::vector<OpGraphOpCode> supported_modes_, unsigned size, int II, int latency, bool isElastic)
    : Module(name, loc, size, isElastic)
    , pipeline_mode(II, latency)
    , supported_modes(std::move(supported_modes_))
{
    if (II != 1) {
        std::cout << II << '\n';
        make_and_throw<cgrame_error>([&](auto&& s) { s << "dont support an II other than 1 (given II=" << II << ')'; });
    }

    parameterlist["latency"] = latency;

    // Create the ports
    addPort("in_a", PORT_INPUT, "size", size);
    addPort("in_b", PORT_INPUT, "size", size);

    if (supported_modes.size() > 1) // If there is more than one mode, we need to add a select line, we will also have a mux, so make port "out" a reg
    {
	    addPort("select", PORT_CONFIG, ceil(log2(supported_modes.size())), false);
	    addPort("out", PORT_OUTPUT, "size", size);

	    std::string width_suffix = "_" + std::to_string((int)size) + "b";
	    for (unsigned i = 0; i < supported_modes.size(); i++)
	    {
			if (all_modes.at(supported_modes.at(i)).ModuleName == "op_nop" or all_modes.at(supported_modes.at(i)).ModuleName == "op_not"){
				addSubModule(new CustomModuleSingleInput(
					all_modes.at(supported_modes.at(i)).ModuleName + width_suffix, loc,
					all_modes.at(supported_modes.at(i)).Functionality,
					size), 0.1, static_cast<double>(i)/(supported_modes.size()), 0.9, static_cast<double>(i)/(supported_modes.size()));
			}
			else{
				//create a new submodule to pointer (check if constructor is breaking, or if the add function is breaking)
				addSubModule(new CustomModule(
					all_modes.at(supported_modes.at(i)).ModuleName + width_suffix, loc,
					all_modes.at(supported_modes.at(i)).Functionality,
					size), 0.1, static_cast<double>(i)/(supported_modes.size()), 0.9, static_cast<double>(i)/(supported_modes.size()));
			}
	    }

	    //seperate the adding module and the creating case statement 
	    addSubModule(new CaseStatement("caseStatement", loc, supported_modes.size(), size, latency), 0, 0.5, 0.1, 1);

	    for (unsigned i = 0; i < supported_modes.size(); i++)
	    {
	        addConnection("this.in_a", all_modes.at(supported_modes.at(i)).ModuleName + width_suffix + ".a");

			if (all_modes.at(supported_modes.at(i)).ModuleName != "op_nop" and all_modes.at(supported_modes.at(i)).ModuleName != "op_not"){
				addConnection("this.in_b", all_modes.at(supported_modes.at(i)).ModuleName + width_suffix + ".b");
			}
	    }
	    for (unsigned i = 0; i < supported_modes.size(); i++)
	    {
	        addConnection(all_modes.at(supported_modes.at(i)).ModuleName + width_suffix + ".c", "caseStatement.in" + std::to_string(i));
	    }
	    addConnection("this.select", "caseStatement.select");
	    addConnection("caseStatement.out", "this.out");
    }
    else if (supported_modes.size() == 1)
    {   

        addPort("out", PORT_OUTPUT, "size", size);

        std::string width_suffix = "_" + std::to_string((int)size) + "b";
        if (all_modes.at(supported_modes.at(0)).ModuleName == "op_nop" or all_modes.at(supported_modes.at(0)).ModuleName == "op_not"){
				addSubModule(new CustomModuleSingleInput(
					all_modes.at(supported_modes.at(0)).ModuleName + width_suffix, loc,
					all_modes.at(supported_modes.at(0)).Functionality,
					size), 0.1, static_cast<double>(0)/(supported_modes.size()), 0.9, static_cast<double>(1)/(supported_modes.size()));
			}
		else{
			addSubModule(new CustomModule(
				all_modes.at(supported_modes.at(0)).ModuleName + width_suffix, loc,
				all_modes.at(supported_modes.at(0)).Functionality,
				size), 0.1, static_cast<double>(0)/(supported_modes.size()), 0.9, static_cast<double>(1)/(supported_modes.size()));
			}
    }
    else if (supported_modes.size() < 1) // If there is jsut 1 mode, "out" can be just a wire
    {
        throw cgrame_error("not enough supported modes in the list");
    }
    else // Otherwise for 0 modes, "out" can just be a wire
    {
        addPort("out", PORT_OUTPUT, "size", size);
    }


    // Add position for visualization
    node_relative_position.insert({"in_a", {0.33, 0}});
    node_relative_position.insert({"in_b", {0.67, 0}});
    node_relative_position.insert({"fu", {0.5, 0.33}});
    node_relative_position.insert({"out", {0.5, 0.67}});
}

BitConfig FuncUnit::getBitConfig(
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
            bitConfig.add({ (size_t)bits_needed, BitSetting::DONT_CARE_PREFER_LOW}, cycle);
        } else if (op_and_mrrg_nodes.size() == 1) {
            used_cycles++;
            const auto find_result = std::find(begin(supported_modes), end(supported_modes), begin(op_and_mrrg_nodes)->first->opcode);
            if (find_result == end(supported_modes)) {
                throw cgrame_error("couldn't find op in supported modes list");
            } else {
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

FuncUnit::~FuncUnit()
{
}

MRRG* FuncUnit::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i+= getII())
    {
        // create nodes
        MRRG::NodeDescriptor in_a = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "in_a", {Operands::BINARY_LHS, Operands::BINARY_ANY})).first;

        MRRG::NodeDescriptor in_b = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "in_b", {Operands::BINARY_RHS, Operands::BINARY_ANY})).first;
        MRRG::NodeDescriptor fu  = result.insert(MRRGNode::make_function(this, data_size, i, "fu", getLatency(), supported_modes)).first;

        MRRG::NodeDescriptor out  = result.insert(MRRGNode::make_routing(this, data_size, i, "out")).first;

        result.link(in_a, fu);
        result.link(in_b, fu);
        (void)out;
    }

    for(unsigned i = 0; i < II; i+= getII())
    {
        MRRG::NodeDescriptor fu           = result.getNode(i, "fu");
        MRRG::NodeDescriptor out_next    = result.getNode(MOD_II(i + getLatency()), "out");

        result.link(fu, out_next);
    }

    return result_ptr;
}

MemoryUnit::MemoryUnit(std::string name, Location loc, unsigned size, bool isElastic, bool pred)
    : Module(name, loc, size, isElastic)
    , pred(pred)
{
    // Create the ports
    addPort("addr", PORT_INPUT, "size", size, isElastic); //FIXME: size shouldn't be left as default
    addPort("data_in", PORT_INPUT, "size", size, isElastic);
    addPort("data_out", PORT_OUTPUT, "size", size, isElastic);
    addPort("w_rq", PORT_CONFIG, 1, isElastic);
    if (pred) {
        addPort("pred", PORT_INPUT, 1, isElastic);
    }

    addVerilogPort("addr_to_ram", PORT_OUTPUT, "size", size);
    addVerilogPort("data_in_to_ram", PORT_OUTPUT, "size", size);
    addVerilogPort("data_out_from_ram", PORT_INPUT, "size", size);
    addVerilogPort("w_rq_to_ram", PORT_OUTPUT, "", 1);
    if (isElastic) {
        addVerilogPort("ram_input_ready", PORT_INPUT, "", 1);
        addVerilogPort("ram_input_enable", PORT_OUTPUT, "", 1);
        addVerilogPort("ram_output_ready", PORT_OUTPUT, "", 1);
        addVerilogPort("ram_output_enable", PORT_INPUT, "", 1);
    }

    // Add position for visualization
    node_relative_position.insert({"mem", {0.33, 0.5}});
    node_relative_position.insert({"addr", {0, 0.33}});
    node_relative_position.insert({"data_in", {0, 0.67}});
    node_relative_position.insert({"data_out", {0.67, 0.5}});
}

std::string MemoryUnit::GenericName()
{
    std::string elas = isElastic ? "elastic_" : "";
    return elas + "memUnit_" + std::to_string(getSize()) + "b";
}

void MemoryUnit::GenFunctionality()
{
    std::cout << std::endl;
    std::cout << SET_INDENT << "// ALERT: This module is an unimplemented place holder.\n";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json MemoryUnit::CoreIRGenFunctionality() // Virtual function override
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
    for (auto& port : ports)
    {
        std::string portName = port.second->getName();
        vjson["interface"].push_back(portName);
    }
    for (auto& port : portsToPropagate)
    {
        std::string portName = port.getName();
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

    for (auto& port : portsToPropagate)
    {
        port_type portType = port.pt;
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
        if (!(port.parameter).empty()) // Check if size is parameterized
        {
            std::string portParameterName = port.parameter;
            portSizeString = "[" + portParameterName + "-1:0]";
        }
        else
        {
            portSizeString = "[" + std::to_string(port.size - 1) + ":0]";
        }
        std::string portName = port.getName();
        std::string portDeclaration = portTypeString + " " + portSizeString + " " + portName + ";\n";
        moduleDefinition += std::string(SET_INDENT) + portDeclaration;
    }

    // Functionality
    if (isElastic) {
        moduleDefinition += std::string(SET_INDENT) + "assign addr_to_ram = addr;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign data_in_to_ram = data_in;\n";
        if (pred) {
            moduleDefinition += std::string(SET_INDENT) + "assign w_rq_to_ram = w_rq & pred;\n";
        } else {
            moduleDefinition += std::string(SET_INDENT) + "assign w_rq_to_ram = w_rq;\n";
        }
        moduleDefinition += std::string(SET_INDENT) + "assign ram_input_enable = addr_valid_upstream;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign ram_output_ready = ~data_out_stop_downstream;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign addr_stop_upstream = ~ram_input_ready;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign data_in_stop_upstream = ~ram_input_ready;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign data_out = data_out_from_ram;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign data_out_valid_downstream = ram_output_enable;";
    } else { 
        moduleDefinition += std::string(SET_INDENT) + "assign addr_to_ram = addr;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign data_in_to_ram = data_in;\n";
        if (pred) {
            moduleDefinition += std::string(SET_INDENT) + "assign w_rq_to_ram = w_rq & pred;\n";
        } else {
            moduleDefinition += std::string(SET_INDENT) + "assign w_rq_to_ram = w_rq;\n";
        }
        moduleDefinition += std::string(SET_INDENT) + "assign data_out = data_out_from_ram;";
    }


    vjson["definition"] = moduleDefinition;
    return vjson;
}

BitConfig MemoryUnit::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
    const Mapping& map,
    const ConfigCell& ccell,
    const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_val_node;
    static const std::vector<std::pair<OpGraphOpCode,std::regex>> node_name_checks {
        {OpCode::LOAD,  std::regex("\\.data_out$")},
        {OpCode::STORE, std::regex("\\.data_in$")},
    };

    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromOpNode> opNodesByCycle(mrrg.initiationInterval());
    for (const auto& op_and_mrrg_nodes : mrrg_nodes_from_op_node) {
        for (const auto& mrrg_node : op_and_mrrg_nodes.second) {
            opNodesByCycle[mrrg_node->cycle][op_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int used_cycles = 0;
    // TODO: Fixup for multi-context cases
    for (int cycle = 0; cycle < mrrg.initiationInterval(); cycle++) {
        MRRGNodesFromOpNode mrrg_nodes_from_op_node_in_cycle = opNodesByCycle[cycle];

        if (mrrg_nodes_from_op_node_in_cycle.empty()) {
            bitConfig.add({ BitSetting::LOW }, cycle);

        } else {
            const OpGraphOp& op = [&]() -> decltype(auto) {
                if (mrrg_nodes_from_op_node_in_cycle.size() == 1) {
                    return *(begin(mrrg_nodes_from_op_node_in_cycle)->first);
                } else {
                    throw cgrame_error("expected exactly one op node");
                }
            }();
            used_cycles++;
            // Set bit config for write enable to high if op is store.
            bitConfig.add({ from_bool<BitSetting>(op.opcode == OpCode::STORE) }, cycle);
        }
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}

MRRG* MemoryUnit::createMRRG(unsigned int II)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i++)
    {
        // create FU node
        MRRGNode& fu  = result.getNodeRef(result.insert(MRRGNode::make_function(this, data_size, i, "mem", 1,
            {OpCode::LOAD, OpCode::STORE}
        )).first);
        if (pred) {
            MRRG::NodeDescriptor pred = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "pred", {Operands::PREDICATE})).first;
            result.link(pred, &fu);
        }

        // create input nodes and connections
        MRRG::NodeDescriptor addr = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "addr", {Operands::MEM_ADDR})).first;
        result.link(addr, &fu);

        MRRG::NodeDescriptor data_in = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "data_in", {Operands::MEM_DATA})).first;
        result.link(data_in, &fu);

        // create output nodes and connections
        MRRG::NodeDescriptor data_out = result.insert(MRRGNode::make_routing(this, data_size, MOD_II(i + 1), "data_out")).first;
        result.link(&fu, data_out);
    }

    return result_ptr;
}

MemoryUnit::~MemoryUnit()
{
}

ConstUnit::ConstUnit(std::string name, Location loc, int size, int II, bool isElastic)
    : Module(name, loc, size, isElastic)
{
    addPort("out", PORT_OUTPUT, "size", size, isElastic);
    if (II > 1) 
        addPort("Context", PORT_INPUT, ceil(log2(II)));
    if (isElastic) {
        addSubModule(new DisconnectedSource("dis", loc, 1, 1));
        addConnection("dis.out", "this.out_valid_downstream", false);
    }
        
    // Add position for visualization
    node_relative_position.insert({"const", {0, 0.25}});
    node_relative_position.insert({"out", {0, 0.75}});

    // FIXME:
    // The second argument in the ConfigCell constructor below actually does
    // not make a Verilog connection to the port correctly. But this should
    // only affect Verilog generation flow. The actual Verilog connection
    // assignment is hardcoded in ConstUnit::GenFunctionality().
    addConfig("ConstVal", {"this.out"}, II, false);
    if (II > 1) {
        addConnection("this.Context", "ConstVal.Context", false);
    }
}

std::string ConstUnit::GenericName()
{
    std::string elas = isElastic ? "elastic_" : "";
    return elas + "const_" + std::to_string(getSize()) + "b";
}

void ConstUnit::GenFunctionality()
{
    // FIXME:
    // Once Module::addConfig() is fixed to accept ports as connection, this
    // method should be removed.
    std::cout << std::endl;
    std::cout << SET_INDENT << "assign out = ConstVal_sig;";
    std::cout << std::endl;
}

BitConfig ConstUnit::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
    const Mapping& map,
    const ConfigCell& ccell,
    const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;

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
    for (auto & mrrg_nodes_from_op_node_per_cycle : opNodesByCycle) {
        if (mrrg_nodes_from_op_node_per_cycle.empty()) {
            bitConfig.add({data_size, BitSetting::LOW}, cycle);
        } else {
            if (mrrg_nodes_from_op_node_per_cycle.size() != 1) {
                make_and_throw<std::logic_error>([&](auto&& s) {
                    s << "This opgraph_op node is mapped to multiple MRRG nodes\n";
                });
            }
            else if (std::begin(mrrg_nodes_from_op_node_per_cycle)->first->opcode != OpCode::CONST) {
                make_and_throw<std::logic_error>([&](auto&& s) {
                    s << "A non constant op was mapped to this const unit\n";
                });
            }
            used_cycles++;
            std::vector<BitSetting> result = bitsettings_from_int(std::begin(mrrg_nodes_from_op_node_per_cycle)->first->constVal, this->data_size);
            bitConfig.add(result, cycle);
        }
        ++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}

MRRG* ConstUnit::createMRRG(unsigned int II)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i++)
    {
        // create FU node
        MRRG::NodeDescriptor fu  = result.insert(MRRGNode::make_function(this, data_size, i, "const", 0, {OpCode::CONST}, 1, true)).first;
        // create output nodes and connections
        MRRG::NodeDescriptor data_out = result.insert(MRRGNode::make_routing(this, data_size, i, "out")).first;
        result.link(fu, data_out);
    }

    return result_ptr;
}

ConstUnit::~ConstUnit()
{
}

/************ CaseStatement **********/
CaseStatement::CaseStatement(std::string name, Location loc, unsigned numInputs, unsigned size, int latency, bool isElastic)
    : Module(name, loc, size, isElastic)
    , numInputs(numInputs)
    , latency(latency)
{
    adds_synchronous_circuitry = true;

    addParameter("size", size);
    addParameter("latency", latency);

    for (unsigned i = 0; i < numInputs; i++)
    {
        addPort("in" + std::to_string(i), PORT_INPUT, "size", size);
    }
    addPort("select", PORT_CONFIG, ceil(log2(numInputs)), isElastic);
    addPort("out", PORT_OUTPUT, "size", size, isElastic);
}

std::string CaseStatement::GenericName()
{
    return "caseStatement_" + std::to_string(numInputs) + "in_" + std::to_string(latency) + "latency_" + std::to_string(getSize()) + "b";
}

void CaseStatement::GenFunctionality()
{
    std::cout << "\n" << SET_INDENT << "always @*\n";
    std::cout << SET_DOUBLE_INDENT << "case (select)\n";
    if (getNumInputs() == 1) // We do not want a case statement, because the select variable does not exist
    {
        std::cout << SET_INDENT << "assign computation = in0;\n";
    }
    else
    {
        for (unsigned i = 0; i < getNumInputs(); i++)
            std::cout << SET_TRIPLE_INDENT << i << ": computation = in" << i << ";\n";
        std::cout << SET_TRIPLE_INDENT << "default: computation = {size{1'bx}};\n";
        std::cout << SET_DOUBLE_INDENT << "endcase\n";
    }
    std::cout << '\n';

    std::cout
        << SET_INDENT << "integer i;\n"
        << SET_INDENT << "always @(posedge CGRA_Clock, posedge CGRA_Reset) begin\n"
        << SET_INDENT << "    if (CGRA_Reset) begin\n"
        << SET_INDENT << "        for (i=0; i < latency; i=i+1)\n"
        << SET_INDENT << "            delay_ppln[i] <= {size{1'b0}};\n"
        << SET_INDENT << "    end else begin\n"
        << SET_INDENT << "        for (i=1; i < latency; i=i+1)\n"
        << SET_INDENT << "            delay_ppln[i] <= delay_ppln[i-1];\n"
        << SET_INDENT << "        delay_ppln[0] <= computation;\n"
        << SET_INDENT << "    end\n"
        << SET_INDENT << "end\n"
        << '\n'
        << SET_INDENT << "generate\n"
        << SET_INDENT << "    if (latency > 0)\n"
        << SET_INDENT << "        assign out = delay_ppln[latency-1];\n"
        << SET_INDENT << "    else\n"
        << SET_INDENT << "        assign out = computation;\n"
        << SET_INDENT << "endgenerate\n"
    ;
}

nlohmann::json CaseStatement::CoreIRGenFunctionality() // Virtual function override
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

    // Declare wires
    moduleDefinition += std::string(SET_INDENT) + "reg [size-1:0] computation;\n";
    moduleDefinition += std::string(SET_INDENT) + "reg [size-1:0] delay_ppln [latency-1:0];\n";

    // Functionality
    moduleDefinition += std::string(SET_INDENT) + "always @*\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "case (select)\n";
    for (unsigned i = 0; i < getNumInputs(); i++)
    {
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + std::to_string(i) + ": computation = in" + std::to_string(i) + ";\n";
    }
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "default: computation = {size{1'bx}};\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "endcase\n";

    moduleDefinition += std::string(SET_INDENT) + "integer i;\n";
    moduleDefinition += std::string(SET_INDENT) + "always @(posedge CGRA_Clock, posedge CGRA_Reset) begin\n";
    moduleDefinition +=  std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "for (i=0; i < latency; i=i+1)\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "delay_ppln[i] <= {size{1'b0}};\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end else if (CGRA_Enable) begin\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "for (i=1; i < latency; i=i+1)\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) + "delay_ppln[i] <= delay_ppln[i-1];\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "delay_ppln[0] <= computation;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "end\n\n";

    moduleDefinition += std::string(SET_INDENT) + "generate\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (latency > 0)\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "assign out = delay_ppln[latency-1];\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "assign out = computation;\n";
    moduleDefinition += std::string(SET_INDENT) + "endgenerate";

    if (isElastic){ 
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + "assign select_valid_upstream = 1'b1;\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + "assign out_valid_downstream = 1'b1;\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + "assign select_stop_upstream = out_stop_downstream;\n";
    }

    vjson["definition"] = moduleDefinition;
    return vjson;
}

CaseStatement::~CaseStatement()
{
}
