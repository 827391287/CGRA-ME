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
#include <unordered_set>
#include <unordered_map>

/************ Register **********/
Register::Register(std::string name, Location loc, int size, bool isElastic)
    : Module(name, loc, size, isElastic)
{
    adds_synchronous_circuitry = true;
    // Create ports
    addPort("in", PORT_INPUT, "size" ,size, isElastic);
    addPort("out", PORT_OUTPUT_REG, "size",  size, isElastic);
    addPort("enable", PORT_CONFIG, 1, isElastic);


    // Add position for visualization
    node_relative_position.insert({"in", {0, 0}});
    node_relative_position.insert({"m_in", {0, 1.0/6}});
    node_relative_position.insert({"reg", {0, 2.0/6}});
    node_relative_position.insert({"m_out", {0, 3.0/6}});
    node_relative_position.insert({"m_enable", {0, 4.0/6}});
    node_relative_position.insert({"out", {0, 5.0/6}});
}

// Virtual function that overrides Module::GenericName. Returns generic name of the object
std::string Register::GenericName()
{
    std::string elas = isElastic ? "elastic_" : "";
    return elas + "register_" + std::to_string(getSize()) + "b";
}

// Virtual function that overrides Module::GetFunctionality. Generates the functionality for register
void Register::GenFunctionality() // Virtual function override
{
    std::cout << SET_INDENT << "always @(posedge CGRA_Clock, posedge CGRA_Reset)\n";
    std::cout << SET_DOUBLE_INDENT << "if (CGRA_Reset == 1)\n";
    std::cout << SET_TRIPLE_INDENT << "out <= 0;\n";
    std::cout << SET_DOUBLE_INDENT << "else\n";
    std::cout << SET_TRIPLE_INDENT << "out <= in;\n";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json Register::CoreIRGenFunctionality() // Virtual function override
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

    if (!this->isElastic) {
        // Functionality
        moduleDefinition += std::string(SET_INDENT) + "always @(posedge CGRA_Clock, posedge CGRA_Reset)\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset == 1)\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out <= 0;\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + "if (enable & CGRA_Enable)\n";
        moduleDefinition += std::string(SET_QUAD_INDENT) + "out <= in;";
    } else {
        // Functionality
        moduleDefinition += std::string(SET_INDENT) + "always @(posedge CGRA_Clock, posedge CGRA_Reset)\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "if (CGRA_Reset == 1) begin\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out <= 0;\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out_valid_downstream <= 0;\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "else if (enable & CGRA_Enable & ~in_stop_upstream) begin\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out <= in;\n";
        moduleDefinition += std::string(SET_TRIPLE_INDENT) + "out_valid_downstream <= in_valid_upstream;\n";
        moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
        moduleDefinition += std::string(SET_INDENT) + "assign enable_stop_upstream = in_stop_upstream;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign enable_valid_upstream = in_valid_upstream;\n";
        moduleDefinition += std::string(SET_INDENT) + "assign in_stop_upstream = out_stop_downstream;\n";
    }

    vjson["definition"] = moduleDefinition;
    return vjson;
}


Register::~Register()
{
}

BitConfig Register::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
    const Mapping& map,
    const ConfigCell& ccell,
    const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    BitConfig bitConfig(mrrg.initiationInterval());
    // Use the m_in node to check if this register is being used
    const std::regex in_regex("\\.m_in$");

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
            bitConfig.add({BitSetting::DONT_CARE_PREFER_LOW}, cycle);
        } else {
            bool enableRegister = false;
            for (auto & op_val_and_nodes: mrrg_nodes_from_val_by_cycle) {
                for (auto & mrrg_node : op_val_and_nodes.second) {
                    auto name = mrrg_node->getHierarchyQualifiedName();
                    if (regex_search(name, in_regex)) {
                        // Considering doing an error check here to see if "m_in" shows up twice
                        enableRegister = true;
                        break;
                    }
                }
            }
            if (enableRegister) {
                used_cycles++;
                bitConfig.add({BitSetting::HIGH}, cycle);
            }
            else {
                bitConfig.add({BitSetting::LOW}, cycle);
            }
        }
        ++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}

MRRG* Register::createMRRG(unsigned II = 1)
{
    /*
     * Visualized MRRG:
     * Context[N-1]:|Context[N]:                       |Context[N+1]:
     *              |           in ──> m_in ───────┐   |
     *              |                              v   |
     *     ... reg  ─> m_out ─> out ─> m_enable ─> reg ─> m_out ...
     *              |                                  |
     */

    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    // create nodes
    for (int this_cycle = 0; this_cycle < (int)II; ++this_cycle) {
        result.insert(MRRGNode::make_routing(this, data_size, this_cycle, "in",       0));
        result.insert(MRRGNode::make_routing(this, data_size, this_cycle, "m_in",     0));
        result.insert(MRRGNode::make_routing(this, data_size, this_cycle, "reg",      1));
        result.insert(MRRGNode::make_routing(this, data_size, this_cycle, "m_out",    0));
        result.insert(MRRGNode::make_routing(this, data_size, this_cycle, "m_enable",  0));
        result.insert(MRRGNode::make_routing(this, data_size, this_cycle, "out",      0));
    }

    // connect them up
    for (int this_cycle = 0; this_cycle < (int)II; ++this_cycle) {
        const int next_cycle = (this_cycle + 1) % II;
        const auto& nodes_this_cycle = makeNodeGetterForCycle(result, this_cycle);
        const auto& nodes_NEXT_cycle = makeNodeGetterForCycle(result, next_cycle); // emphasis needed

        // stuff within the same cycle
        result.link(nodes_this_cycle("in"),        nodes_this_cycle("m_in"));
        result.link(nodes_this_cycle("m_in"),      nodes_this_cycle("reg"));
        result.link(nodes_this_cycle("m_enable"),  nodes_this_cycle("reg"));
        result.link(nodes_this_cycle("m_out"),     nodes_this_cycle("out"));
        result.link(nodes_this_cycle("out"),       nodes_this_cycle("m_enable"));

        // the special one
        result.link(nodes_this_cycle("reg"),       nodes_NEXT_cycle("m_out"));
    }

    return result_ptr;
}


/************DeMultiplexer**********/
DeMux::DeMux(std::string name, Location loc, unsigned demux_size_, unsigned size, bool isElastic) 
    : Module(name, loc, size, isElastic), demux_size(demux_size_)
{
    if (demux_size <= 1) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "DeMultiplexer doesn't support demux_size <= 1. demux_size = " << demux_size;
    });}

    //Create Ports
    addPort("in", PORT_INPUT, "size", size, isElastic);
    addPort("select", PORT_CONFIG, ceil(log2(demux_size)), isElastic);

    node_relative_position.insert({"in",{0, 0.2}});
    node_relative_position.insert({"demux", {0.5,0.5}});
   
    for(int i = 0; i < demux_size; i++) {
        addPort("out" + std::to_string(i), PORT_OUTPUT_REG, "size", size, isElastic);
        node_relative_position.insert({"out" + std::to_string(i), {static_cast<double>(i)/demux_size, 0.5}});
    }

}

// Virtual function that overrides Module::GenericName. Returns generic name of the object
std::string DeMux::GenericName() // Virtual function override
{
    std::string elas = isElastic ? "elastic_" : "";
    return elas + "demux_" + std::to_string(demux_size) + "to1_" + std::to_string(getSize()) + "b";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json DeMux::CoreIRGenFunctionality() // Virtual function override
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
    if (isElastic) {
	moduleDefinition += std::string(SET_INDENT) + "always @(*)\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + "case (select)\n";
	for (int i = 0; i < demux_size; ++i)
	{
	    std::string stringI = std::to_string(i);
	    moduleDefinition += std::string(SET_TRIPLE_INDENT) + stringI + ":begin\n";
	    moduleDefinition += std::string(SET_QUAD_INDENT) + " out" + stringI + "= in;\n";
	    moduleDefinition += std::string(SET_QUAD_INDENT) + " in_stop_upstream = out" + stringI + "_stop_downstream;\n";
	    for (int j = 0; j < demux_size; ++j){
		std::string stringJ = std::to_string(j);
		if (j == i)
		    moduleDefinition += std::string(SET_QUAD_INDENT) + " out" + stringJ + "_valid_downstream = in_valid_upstream;\n";
		else
		    moduleDefinition += std::string(SET_QUAD_INDENT) + " out" + stringJ + "_valid_downstream = 1'b0;\n";
	    } 
	    moduleDefinition += std::string(SET_TRIPLE_INDENT) + "end\n";
	}
	//moduleDefinition += std::string(SET_TRIPLE_INDENT) + "default: out = {size{1'bx}};\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + "endcase";


    } else { 
	    moduleDefinition += std::string(SET_INDENT) + "always @(*)\n";
    	moduleDefinition += std::string(SET_DOUBLE_INDENT) + "case (select)\n";
    	for (int i = 0; i < demux_size; ++i)
    	{
    	    std::string stringI = std::to_string(i);
    	    moduleDefinition += std::string(SET_TRIPLE_INDENT) + stringI + ": out" + stringI + " = in;\n";
    	}
    	moduleDefinition += std::string(SET_DOUBLE_INDENT) + "endcase";
    }
    vjson["definition"] = moduleDefinition;
    return vjson;
}

BitConfig DeMux::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
    const Mapping& map,
    const ConfigCell& ccell,
    const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_op_node;
    const std::vector<std::regex> required_node_regexes {
        std::regex("\\.demux$"), std::regex("\\.in$"),
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

        if (inputs_used.empty()) {
            bitConfig.add({(size_t)std::lround(ceil(log2(demux_size))), BitSetting::DONT_CARE_PREFER_LOW}, cycle);
        } else {
            if (inputs_used.size() != 1) {
                throw cgrame_error("DeMultiplexer must only have one input");
            } else {
                used_cycles++;
                bitConfig.add(bitsettings_from_int(*begin(inputs_used), (int)std::lround(ceil(log2(demux_size)))), cycle);
	    }
        }
        bitConfig.setUsedCycles(used_cycles);
        ++cycle;
    }

    return bitConfig;
}

MRRG* DeMux::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i++)
    {
        MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in")).first;
        MRRG::NodeDescriptor demux = result.insert(MRRGNode::make_routing(this, data_size, i, "demux", 0)).first;

        result.link(in, demux);

        for(int j = 0; j < demux_size; j++)
        {
            MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out" + std::to_string(j))).first;

            result.link(demux, out);
        }

    }

    return result_ptr;
}

DeMux::~DeMux()
{
}


/************ Multiplexer **********/
Multiplexer::Multiplexer(std::string name, Location loc, unsigned mux_size_, unsigned size, bool isElastic)
    : Module(name, loc, size, isElastic)
    , mux_size(mux_size_)
{

    if (mux_size <= 1) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "Multiplexer doesn't support mux_size <= 1. mux_size = " << mux_size<< " name "<< name;
    });}

    // Create ports
    for(int i = 0; i < mux_size; i++) {
        addPort("in" + std::to_string(i), PORT_INPUT, "size", size, isElastic);
        node_relative_position.insert({"in" + std::to_string(i), {static_cast<double>(i)/mux_size, 0.2}});
    }

    // Add position for visualization
    node_relative_position.insert({"out", {0.25, 0.5}});
    node_relative_position.insert({"mux", {0.5, 0.5}});

    addPort("out", PORT_OUTPUT_REG, "size", size, isElastic);
    // select port should have size that is log base 2 of the mux size
    addPort("select", PORT_CONFIG, ceil(log2(mux_size)), isElastic);
}

// Virtual function that overrides Module::GenericName. Returns generic name of the object
std::string Multiplexer::GenericName() // Virtual function override
{
    std::string elas = isElastic ? "elastic_" : "";
    return elas + "mux_" + std::to_string(mux_size) + "to1_" + std::to_string(getSize()) + "b";
}

// Generates the case statement for the multiplexer
void Multiplexer::GenFunctionality() // Virtual function override
{
    std::cout << SET_INDENT << "always @(*)\n";
    std::cout << SET_DOUBLE_INDENT <<  "case (select)\n";
    for (int i = 0; i < mux_size; ++i)
        std::cout << SET_TRIPLE_INDENT <<  i << ": out = in" << i << ";\n";
    std::cout << SET_TRIPLE_INDENT << "default: out = {size{1'bx}};\n";
    std::cout << SET_DOUBLE_INDENT << "endcase\n";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json Multiplexer::CoreIRGenFunctionality() // Virtual function override
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
    if (isElastic) {
	    moduleDefinition += std::string(SET_INDENT) + "always @(*)\n";
	    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "case (select)\n";
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
	    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "case (select)\n";
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

BitConfig Multiplexer::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
    const Mapping& map,
    const ConfigCell& ccell,
    const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_op_node;
    const std::vector<std::regex> required_node_regexes {
        std::regex("\\.mux$"), std::regex("\\.out$"),
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

        if (inputs_used.empty()) {
            bitConfig.add({(size_t)std::lround(ceil(log2(mux_size))), BitSetting::DONT_CARE_PREFER_LOW}, cycle);
        } else {
            if (inputs_used.size() != 1) {
                throw cgrame_error("Multiplexer must only have one input");
            } else {
                used_cycles++;
                bitConfig.add(bitsettings_from_int(*begin(inputs_used), (int)std::lround(ceil(log2(mux_size)))), cycle);
            }
        }
        ++cycle;
    }
    bitConfig.setUsedCycles(used_cycles);
    return bitConfig;
}

MRRG* Multiplexer::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i++)
    {
        MRRG::NodeDescriptor out = result.insert(MRRGNode::make_routing(this, data_size, i, "out")).first;
        MRRG::NodeDescriptor mux = result.insert(MRRGNode::make_routing(this, data_size, i, "mux", 0)).first;

        result.link(mux, out);

        for(int j = 0; j < mux_size; j++)
        {
            MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in" + std::to_string(j))).first;

            result.link(in, mux);
        }

    }

    return result_ptr;
}

Multiplexer::~Multiplexer()
{
}

/************ Truncate Input **********/
TruncateInput::TruncateInput(std::string name, Location loc, unsigned splitPos, unsigned size, bool isElastic)
    : Module(name, loc, size, isElastic)
    , pos(splitPos)
{
    // Create ports
    addPort("in", PORT_INPUT, "size", size, isElastic);
    addPort("out0", PORT_OUTPUT,  splitPos, isElastic);
}

// Virtual function that overrides Module::GenericName. Returns generic name of the object
std::string TruncateInput::GenericName() // Virtual function override
{
    std::string elas = isElastic ? "elastic_" : "";
    return elas + "split_input_at_" + std::to_string(pos) + "_size_" + std::to_string(getSize()) + "b";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json TruncateInput::CoreIRGenFunctionality() // Virtual function override
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
    moduleDefinition += std::string(SET_INDENT) + "assign out0 = in[" + std::to_string(pos - 1) + ": 0 ];\n" ;
    if (isElastic) {
	    moduleDefinition += std::string(SET_INDENT) + "assign out0_valid_downstream = in_valid_upstream;\n";
	    moduleDefinition += std::string(SET_INDENT) + "assign in_stop_upstream = out0_stop_downstream;\n";
    }
    vjson["definition"] = moduleDefinition;
    return vjson;

}

MRRG* TruncateInput::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i++)
    {
        //MRRG::NodeDescriptor out0 = result.insert(MRRGNode::make_operand_pin(this, i, "out0", {Operands::PREDICATE})).first;
        MRRG::NodeDescriptor out0 = result.insert(MRRGNode::make_routing(this, data_size, i, "out0")).first;
        MRRG::NodeDescriptor truncate = result.insert(MRRGNode::make_routing(this, data_size, i, "truncate")).first;
        MRRG::NodeDescriptor in = result.insert(MRRGNode::make_routing(this, data_size, i, "in")).first;

        result.link(truncate, out0);
        result.link(in, truncate);
    }

    return result_ptr;
}

TruncateInput::~TruncateInput()
{
}

/************ RegisterFile **********/
RegisterFile::RegisterFile(std::string name, Location loc, int numInputPorts_, int numOutputPorts_, int log2Registers_, int size, int II, bool isElastic)
    : Module(name, loc, size, isElastic)
    , numInputPorts(numInputPorts_)
    , numOutputPorts(numOutputPorts_)
    , log2Registers(log2Registers_)
{
    adds_synchronous_circuitry = true;
    if (log2Registers < 1) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "RegisterFile doesn't support less than 2 registers. log2Registers = " << log2Registers;
    });}

    if (II > 1) 
        addPort("Context", PORT_INPUT, ceil(log2(II)));  
    // Generating input ports
    for (int i = 0; i < numInputPorts; i++)
    {
        addPort("in" + std::to_string(i), PORT_INPUT, "size", size, isElastic); // input data
    }

    // Generating output ports
    for (int i = 0; i < numOutputPorts; i++)
    {
        addPort("out" + std::to_string(i), PORT_OUTPUT_REG, "size", size, isElastic); // output ports/registers
        // addPort("address_out" + std::to_string(i), PORT_INPUT, "log2regs", log2Registers); // addresses for output
    }

    // Generating input muxes and registers
    for (int i = 0; i < (1 << log2Registers); ++i) {
        addSubModule(new Register("reg" + std::to_string(i), loc, size, isElastic), 0.1 + static_cast<double>(i)/(1 << log2Registers), 0.3, 1.0/(1 << log2Registers),0.2);
        addConfig(new ConfigCell(name + "Reg" + std::to_string(i) + "Config", II), {"reg" + std::to_string(i) + ".enable"});

        if (numInputPorts > 1) {
            addSubModule(new Multiplexer("in_mux" + std::to_string(i), loc, numInputPorts, size, isElastic), 0.1 + static_cast<double>(i)/numOutputPorts, 0.1, 1.0/numInputPorts, 0.3);
            addConfig(new ConfigCell(name + "InMux" + std::to_string(i) + "Config", II), {"in_mux" + std::to_string(i) + ".select"});

            // Connect input mux to reg
            addConnection("in_mux" + std::to_string(i) + ".out", "reg" + std::to_string(i) + ".in");

            for (int j = 0; j < numInputPorts; ++j) {
                addConnection("this.in" + std::to_string(j), "in_mux" + std::to_string(i) + ".in" + std::to_string(j));
            }
        }
        else {
            addConnection("this.in0", "reg" + std::to_string(i) + ".in");
        }
    }

    // Generating output muxes
    for (int i = 0; i < numOutputPorts; ++i) {
        addSubModule(new Multiplexer("out_mux" + std::to_string(i), loc, (1 << log2Registers), size, isElastic), 0.1 + static_cast<double>(i)/numOutputPorts, 0.7, 1.0/numOutputPorts, 0.3);
        addConfig(new ConfigCell(name + "OutMux" + std::to_string(i) + "Config", II), {"out_mux" + std::to_string(i) + ".select"});

        // Connect every register to this output mux
        for (int j = 0; j < (1 << log2Registers); ++j) {
            addConnection("reg" + std::to_string(j) + ".out", "out_mux" + std::to_string(i) + ".in" + std::to_string(j));
        }
        addConnection("out_mux" + std::to_string(i) + ".out", "this.out" + std::to_string(i));
    }
    if (II > 1) {
        for (int i = 0; i < (1 << log2Registers); ++i) {
            addConnection("this.Context", name + "Reg" + std::to_string(i) + "Config.Context", false);
            if (numInputPorts > 1) {
                addConnection("this.Context", name + "InMux" + std::to_string(i) + "Config.Context", false);
            }
        }
        for (int i = 0; i < numOutputPorts; ++i) {
            addConnection("this.Context", name + "OutMux" + std::to_string(i) + "Config.Context", false);
        }
    }

    // Setting positions for nodes
    for (int i = 0; i < numInputPorts; i++) {
        setNodePosition("in" + std::to_string(i), static_cast<double>(i*log2Registers+1)/(numInputPorts*log2Registers+2), 0.1);
    }

    for (int i = 0; i < numOutputPorts; ++i) {
        setNodePosition("out" + std::to_string(i), static_cast<double>(i+1)/(numOutputPorts+2), 0.95);
    }
}

// Generates a unique name for a register file,based on number of input ports and output ports
std::string RegisterFile::GenericName()
{
    return "registerFile_" + std::to_string(numInputPorts) + "in_" + std::to_string(numOutputPorts) + "out_" + std::to_string(getSize()) + "b";
}

// Prints the register file and always block
// begin block for CGRA Reset MUST be named
// We must use either entirely blocking or nonblocking assignment statements in this always block, no mixes allowed
void RegisterFile::GenFunctionality()
{
    std::cout << "\n" << SET_INDENT << "// Setting the always blocks and inside registers\n";
    std::cout << SET_INDENT << "reg [size-1:0] register[2**log2regs-1:0];\n";
    std::cout << SET_INDENT << "always@(posedge CGRA_Clock, posedge CGRA_Reset)\n";
    std::cout << SET_DOUBLE_INDENT << "if(CGRA_Reset)\n";
    std::cout << SET_TRIPLE_INDENT << "begin : RESET\n";
    std::cout << SET_QUAD_INDENT << "integer i;\n";
    std::cout << SET_QUAD_INDENT << "for (i = 0; i < 2**log2regs; i = i+1)\n";
    std::cout << SET_PENTA_INDENT << "register[i] = 0;\n";
    std::cout << SET_TRIPLE_INDENT << "end\n";
    std::cout << SET_DOUBLE_INDENT << "else\n";
    std::cout << SET_TRIPLE_INDENT << "begin\n";
    for (int i = 0; i < numOutputPorts; i++)
        std::cout << SET_QUAD_INDENT << "out" << i << " = " << "register[address_out" << i << "];\n";
    for (int i = 0; i < numInputPorts; i++)
    {
        std::cout << SET_QUAD_INDENT << "if(WE" << i << ")\n";
        std::cout << SET_PENTA_INDENT << "register[address_in" << i << "] = in" << i << ";\n";
    }
    std::cout << SET_TRIPLE_INDENT << "end\n";
}

// CoreIR Implementation of GenFunctionality
nlohmann::json RegisterFile::CoreIRGenFunctionality() // Virtual function override
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

RegisterFile::~RegisterFile()
{
}


std::string Crossbar::GenericName()
{
    return "crossbar_" + std::to_string(num_inputs) + "in_" + std::to_string(num_outputs) + "out_" + std::to_string(data_size) + "data_size" ;
}

Crossbar::Crossbar(std::string name, Location loc, int num_inputs, int num_outputs, int data_size, bool predExist, int contexts)
    : Module(name, loc, data_size)
    , num_inputs(num_inputs)
    , num_outputs(num_outputs)
{

    // 1. Create num_output muxes (each num_input in sie)
    for(int i = 0; i < num_outputs; i++)
    {
        // Connects to regA for HyCube
        if (predExist){
            addSubModule(new SelMultiplexer("mux_" + std::to_string(i), loc, num_inputs, data_size), 0.2, static_cast<double>(i)/num_outputs, 0.6, 0.8/num_outputs);
        }
        else{
            addSubModule(new Multiplexer("mux_" + std::to_string(i), loc, num_inputs, data_size), 0.2, static_cast<double>(i)/num_outputs, 0.6, 0.8/num_outputs);
        }
    }

    // 2. Create the configuration cells for each mux
    for(int i = 0; i < num_outputs; i++)
    {
        addConfig(new ConfigCell("Mux" + std::to_string(i) + "config", contexts), {"mux_" + std::to_string(i) + ".select"});
    }

    // 3. Wiring and Ports
    // 3A. Create input and output ports
    // Context Port 
    if (contexts > 1) {
        addPort("Context", PORT_INPUT, ceil(log2(contexts)));
    }
    // Input ports:
    for(int i = 0; i < num_inputs; i++)
    {
        addPort("in" + std::to_string(i), PORT_INPUT, data_size);
        node_relative_position.insert({"in" + std::to_string(i), {0, static_cast<double>(i)/num_inputs}});  // Add position for visualization
    }
    // If pred exist a port to get in the select predicate
    if (predExist){ 
        addPort("pred_in", PORT_INPUT, 1);
    }

    // Output ports:
    for(int i = 0; i < num_outputs; i++)
    {
        addPort("out" + std::to_string(i), PORT_OUTPUT, data_size);
        node_relative_position.insert({"out" + std::to_string(i), {1, static_cast<double>(i)/num_outputs}});    // Add position for visualization
    }

    // 3B. Wiring ports to muxes
    // Crossbar input ports to mux inputs:
    for(int j = 0; j < num_outputs; j++)
    {
        if (predExist) {
            addConnection("this.pred_in", "mux_" + std::to_string(j) + ".selectpred");
        }
        for(int i = 0; i < num_inputs; i++)
        {
            addConnection("this.in" + std::to_string(i), "mux_" + std::to_string(j) + ".in" + std::to_string(i));
        }
    }

    // Mux outputs to crossbar output ports:
    for(int i = 0; i < num_outputs; i++)
    {
        addConnection("mux_" + std::to_string(i) + ".out", "this.out" + std::to_string(i));
    }
    if (contexts > 1) {
        for(int i = 0; i < num_outputs; i++)
        {
         addConnection("this.Context" , "Mux" + std::to_string(i) + "config.Context", false);
        }
       
    }
}
