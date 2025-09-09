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
// Returns a unique name for a funcunit
std::string FracUnit::GenericName()
{
    return "frac_" + std::to_string(getSize()) + "b_frac_mul_frac_add"; // Base name is "frac"
}

FracUnit::FracUnit(std::string name, Location loc, std::vector<OpGraphOpCode> supported_modes_, unsigned size, int II, int latency)
    : Module(name, loc, size)
    , pipeline_mode(II, latency)
    , supported_modes(std::move(supported_modes_))
{
    isElastic = false;
	if (II != 1) {
        std::cout << II << '\n';
        make_and_throw<cgrame_error>([&](auto&& s) { s << "dont support an II other than 1 (given II=" << II << ')'; });
    }

    for (const auto& mode : supported_modes) {
        if (!FracMulUnit::all_modes.count(mode) && !FracAddUnit::all_modes.count(mode)) {
            make_and_throw<cgrame_error>([&](auto&& s) { s << "fracturable unit does not support opcode: " << mode; });
        }
    }

    parameterlist["latency"] = latency;

    // Create the ports
    addPort("in_a", PORT_INPUT, "size", size, isElastic);
    addPort("in_b", PORT_INPUT, "size", size, isElastic);
    //addPort("select", PORT_INPUT, ceil(log2(supported_modes.size())));
    addPort("out", PORT_OUTPUT_REG, "size", size, isElastic);


    addSubModule(new FracMulUnit("fracMulUnit", loc, size, II, latency));
    addSubModule(new FracAddUnit("fracAddUnit", loc, size, II, latency));
    addSubModule(new Multiplexer("fracOutMux", loc, 2, size));

    addConfig(new ConfigCell("FracMulConfig"), {"fracMulUnit.m_sel"});
    addConfig(new ConfigCell("FracAddConfig"), {"fracAddUnit.a_sel"});
    addConfig(new ConfigCell("fracOutMuxConfig"), {"fracOutMux.select"});

    addConnection("this.in_a", "fracMulUnit.in_a");
    addConnection("this.in_b", "fracMulUnit.in_b");

    addConnection("this.in_a", "fracAddUnit.in_a");
    addConnection("this.in_b", "fracAddUnit.in_b");

    addConnection("fracMulUnit.out", "fracOutMux.in0");
    addConnection("fracAddUnit.out", "fracOutMux.in1");
    addConnection("fracOutMux.out", "this.out");
}

/*
// CoreIR Implementation of GenFunctionality
nlohmann::json FracUnit::CoreIRGenFunctionality() // Virtual function override
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

    // MULTIPLY
    //     Declare wires
    moduleDefinition += std::string(SET_INDENT) + "wire [size/2-1:0] m0_op1, m0_op2, m1_op1, m1_op2, m2_op1, m2_op2, m3_op1, m3_op2;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size-1:0] m0_out, m1_out, m2_out, m3_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size-1:0] out_mul_full_hi, out_mul_half_hi, out_mul_quart_hi, out_mul_full_lo, out_mul_half_lo, out_mul_quart_lo;\n\n";

    //     Configure the inputs of the frac multipliers
    moduleDefinition += std::string(SET_INDENT) + "assign m0_op1 = (select%3 == 2) ? {in_a[size-1:size/4*3], 8'b0} : in_a[size-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m0_op2 = (select%3 == 2) ? {in_b[size-1:size/4*3], 8'b0} : in_b[size-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m1_op1 = (select%3 == 2) ? {8'b0, in_a[size/4*3-1:size/2]} : in_a[size-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m1_op2 = (select%3 == 2) ? {8'b0, in_b[size/4*3-1:size/2]} : in_b[size/2-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m2_op1 = (select%3 == 2) ? {in_a[size/2-1:size/4], 8'b0} : in_a[size/2-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m2_op2 = (select%3 == 2) ? {in_b[size/2-1:size/4], 8'b0} : in_b[size-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m3_op1 = (select%3 == 2) ? {8'b0, in_a[size/4-1:0]} : in_a[size/2-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m3_op2 = (select%3 == 2) ? {8'b0, in_b[size/4-1:0]} : in_b[size/2-1:0];\n\n";
	
    moduleDefinition += std::string(SET_INDENT) + "assign m0_out = m0_op1 * m0_op2;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m1_out = m1_op1 * m1_op2;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m2_out = m2_op1 * m2_op2;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m3_out = m3_op1 * m3_op2;\n\n";

    //     Packaging the outputs from frac muls
    moduleDefinition += std::string(SET_INDENT) + "assign {out_mul_full_hi, out_mul_full_lo} = (m0_out << 32) + ((m1_out + m2_out) << 16) + m3_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_half_hi = {m0_out[size-1:size/2], m3_out[size-1:size/2]};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_half_lo = {m0_out[size/2-1:0], m3_out[size/2-1:0]};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_quart_hi = {m0_out[size-1:size/4*3], m1_out[size/2-1:size/4], m2_out[size-1:size/4*3], m3_out[size/2-1:size/4]};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_quart_lo = {m0_out[size/4*3-1:size/2], m1_out[size/4-1:0], m2_out[size/4*3-1:size/2], m3_out[size/4-1:0]};\n\n";

    // ADD
    //     Declaring wires
    moduleDefinition += std::string(SET_INDENT) + "wire a1_carry, a2_carry, a3_carry;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size/4-1:0] a0_out, a1_out, a2_out, a3_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size/4-1:0] q0, q1, q2, q3;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size/2-1:0] h0, h1;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size-1:0] f0;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size-1:0] out_add_full, out_add_half, out_add_quart;\n\n";

    //     Configure the outputs of the frac adders
    moduleDefinition += std::string(SET_INDENT) + "assign a0_out = in_a[size-1:size/4*3] + in_b[size-1:size/4*3];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {a1_carry, a1_out} = in_a[size/4*3-1:size/2] + in_b[size/4*3-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {a2_carry, a2_out} = in_a[size/2-1:size/4] + in_b[size/2-1:size/4];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {a3_carry, a3_out} = in_a[size/4-1:0] + in_b[size/4-1:0];\n\n";

    //     Packaging the outputs from frac adders
    moduleDefinition += std::string(SET_INDENT) + "assign q3 = a3_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign q2 = a2_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign q1 = a1_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign q0 = a0_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign h1 = {(a2_out + a3_carry), a3_out};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign h0 = {(a0_out + a1_carry), a1_out};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign f0 = ((a0_out + a1_carry) << (size/4*3)) + ((a1_out + a2_carry) << (size/2)) + ((a2_out + a3_carry) << (size/4)) + a3_out;\n\n";
    
    moduleDefinition += std::string(SET_INDENT) + "assign out_add_full = f0;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_add_half = {h0, h1};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_add_quart = {q0, q1, q2, q3};\n\n";

    //     Output select
    moduleDefinition += std::string(SET_INDENT) + "always @* begin\n";
    moduleDefinition += std::string(SET_INDENT) + "case (select)\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "4'h0: begin\n";           // Mul High
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "out = out_mul_full_lo;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "4'h1: begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "out = out_mul_half_lo;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "4'h2: begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "out = out_mul_quart_lo;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "4'h3: begin\n";           // Mul Low
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "out = out_mul_full_hi;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "4'h4: begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "out = out_mul_half_hi;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "4'h5: begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "out = out_mul_quart_hi;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "4'h6: begin\n";           // Add
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "out = out_add_full;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "4'h7: begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "out = out_add_half;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "4'h8: begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "out = out_add_quart;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "default: begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "out = 1'bx;\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "end\n";
    moduleDefinition += std::string(SET_INDENT) + "endcase\n";
    moduleDefinition += std::string(SET_INDENT) + "end";
    
    vjson["definition"] = moduleDefinition;
    return vjson;
}

BitConfig FracUnit::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
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
    for (auto & op_and_mrrg_nodes : opNodesByCycle) {
        if (op_and_mrrg_nodes.empty()) {
            bitConfig.add({ (size_t)bits_needed, BitSetting::LOW }, cycle);
        } else if (op_and_mrrg_nodes.size() == 1) {
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

    return bitConfig;
}

MRRG* FracUnit::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i+= getII())
    {
        // create nodes
        MRRG::NodeDescriptor in_a = result.insert(MRRGNode::make_operand_pin(this, i, "in_a", {Operands::BINARY_LHS, Operands::BINARY_ANY})).first;

        MRRG::NodeDescriptor in_b = result.insert(MRRGNode::make_operand_pin(this, i, "in_b", {Operands::BINARY_RHS, Operands::BINARY_ANY})).first;
        MRRG::NodeDescriptor fu  = result.insert(MRRGNode::make_function(this, i, "frac_unit", getLatency(), supported_modes)).first;

        MRRG::NodeDescriptor out  = result.insert(MRRGNode::make_routing(this, i, "out")).first;

        result.link(in_a, fu);
        result.link(in_b, fu);
        (void)out;
    }

    for(unsigned i = 0; i < II; i+= getII())
    {
        MRRG::NodeDescriptor fu           = result.getNode(i, "frac_unit");
        MRRG::NodeDescriptor out_next    = result.getNode(MOD_II(i + getLatency()), "out");

        result.link(fu, out_next);
    }

    return result_ptr;
}
*/

FracUnit::~FracUnit()
{
}


/************ Multiplier **********/
const std::map<OpGraphOpCode, LLVMMode> FracMulUnit::all_modes =
{
    // Adding functionality to the funcunit
    // You can add a mode to the FuncUnit by specifying all_modes.push_back ({"module name", {"functionality"}, "wire name that feeds into the Multiplier"});
    // Functionality can be multiple lines, hence the reason why it is a vector. Each string represents one line, hence the reason it is a vector
    {OpCode::MULU_FULL_LO,     {"op_multiply_unsigned_full_lo",  "multiply_unsigned_full_lo",     {"//mulu_full_lo;"},      "mulu_full_lo_sel"}},
    {OpCode::MULU_HALF_LO,     {"op_multiply_unsigned_half_lo",  "multiply_unsigned_half_lo",     {"//mulu_half_lo;"},      "mulu_half_lo_sel"}},
    {OpCode::MULU_QUART_LO,    {"op_multiply_unsigned_quart_lo", "multiply_unsigned_quart_lo",    {"//mulu_quart_lo;"},     "mulu_quart_lo_sel"}},
    {OpCode::MULU_FULL_HI,     {"op_multiply_unsigned_full_hi",  "multiply_unsigned_full_hi",     {"//mulu_full_hi;"},      "mulu_full_hi_sel"}},
    {OpCode::MULU_HALF_HI,     {"op_multiply_unsigned_half_hi",  "multiply_unsigned_half_hi",     {"//mulu_half_hi;"},      "mulu_half_hi_sel"}},
    {OpCode::MULU_QUART_HI,    {"op_multiply_unsigned_quart_hi", "multiply_unsigned_quart_hi",    {"//mulu_quart_hi;"},     "mulu_quart_hi_sel"}},
    {OpCode::MULS_FULL_LO,     {"op_multiply_signed_full_lo",  "multiply_signed_full_lo",     {"//muls_full_lo;"},      "muls_full_lo_sel"}},
    {OpCode::MULS_HALF_LO,     {"op_multiply_signed_half_lo",  "multiply_signed_half_lo",     {"//muls_half_lo;"},      "muls_half_lo_sel"}},
    {OpCode::MULS_QUART_LO,    {"op_multiply_signed_quart_lo", "multiply_signed_quart_lo",    {"//muls_quart_lo;"},     "muls_quart_lo_sel"}},
    {OpCode::MULS_FULL_HI,     {"op_multiply_signed_full_hi",  "multiply_signed_full_hi",     {"//muls_full_hi;"},      "muls_full_hi_sel"}},
    {OpCode::MULS_HALF_HI,     {"op_multiply_signed_half_hi",  "multiply_signed_half_hi",     {"//muls_half_hi;"},      "muls_half_hi_sel"}},
    {OpCode::MULS_QUART_HI,    {"op_multiply_signed_quart_hi", "multiply_signed_quart_hi",    {"//muls_quart_hi;"},     "muls_quart_hi_sel"}}
};

const std::vector<OpGraphOpCode> FracMulUnit::mul_modes_ordered = {
    OpCode::MULU_FULL_LO,
    OpCode::MULU_HALF_LO,
    OpCode::MULU_QUART_LO,
    OpCode::MULU_FULL_HI,
    OpCode::MULU_HALF_HI,
    OpCode::MULU_QUART_HI,
    OpCode::MULS_FULL_LO,
    OpCode::MULS_HALF_LO,
    OpCode::MULS_QUART_LO,
    OpCode::MULS_FULL_HI,
    OpCode::MULS_HALF_HI,
    OpCode::MULS_QUART_HI
};

FracMulUnit::FracMulUnit(std::string name, Location loc, unsigned size, int II, int latency)
    : Module(name, loc, size)
    , pipeline_mode(II, latency)
{
    isElastic = false; 
    addPort("in_a", PORT_INPUT, "size", size, isElastic);
    addPort("in_b", PORT_INPUT, "size", size, isElastic);
    addPort("m_sel", PORT_INPUT, ceil(log2(mul_modes_ordered.size())), isElastic);
    addPort("out", PORT_OUTPUT_REG, "size", size), isElastic;
}

// Virtual function that overrides Module::GenericName. Returns generic name of the object
std::string FracMulUnit::GenericName() // Virtual function override
{
    return "frac_mul_unit_" + std::to_string(getSize()) + "b";
}

nlohmann::json FracMulUnit::CoreIRGenFunctionality() // Virtual function override
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
    
    // Signs of inputs and outputs
    moduleDefinition += std::string(SET_INDENT) + "wire sign_a0, sign_a1, sign_a2, sign_a3, sign_b0, sign_b1, sign_b2, sign_b3, sign_out0, sign_out1, sign_out2, sign_out3;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_a0 = in_a[size-1];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_a1 = in_a[size/4*3-1];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_a2 = in_a[size/2-1];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_a3 = in_a[size/4-1];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_b0 = in_b[size-1];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_b1 = in_b[size/4*3-1];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_b2 = in_b[size/2-1];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_b3 = in_b[size/4-1];\n\n";

    moduleDefinition += std::string(SET_INDENT) + "assign sign_out0 = sign_a0 ^ sign_b0;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_out1 = sign_a1 ^ sign_b1;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_out2 = sign_a2 ^ sign_b2;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign sign_out3 = sign_a3 ^ sign_b3;\n\n";

    // Changing all operands to unsigned
    moduleDefinition += std::string(SET_INDENT) + "wire b0, b1, b2;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire a_c1, a_c2, a_c3, a_carry1, a_carry2, a_carry3;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire b_c1, b_c2, b_c3, b_carry1, b_carry2, b_carry3;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [(size/4)-1:0] comp_a0, comp_a1, comp_a2, comp_a3;\n";
    moduleDefinition += std::string(SET_INDENT) + " wire [(size/4)-1:0] comp_b0, comp_b1, comp_b2, comp_b3;\n\n";    

	    // adding the 1 when 2's complementing
    moduleDefinition += std::string(SET_INDENT) + "assign b0 =                                (m_sel==5'd8 | m_sel==5'd11) ? 1'b1 : 1'b0;\n";   // quarter
    moduleDefinition += std::string(SET_INDENT) + "assign b1 = (m_sel==5'd7 | m_sel==5'd10 | m_sel==5'd8 | m_sel==5'd11) ? 1'b1 : 1'b0;\n";     // half or quarter
    moduleDefinition += std::string(SET_INDENT) + "assign b2 =                                (m_sel==5'd8 | m_sel==5'd11) ? 1'b1 : 1'b0;\n\n"; // quarter

	    // to carry or not to carry
    moduleDefinition += std::string(SET_INDENT) + "assign a_c1 = (m_sel==5'd6 | m_sel==5'd9 | m_sel==5'd7 | m_sel==5'd10) ? a_carry1 : 1'b0;\n";   // full or half
    moduleDefinition += std::string(SET_INDENT) + "assign a_c2 = (m_sel==5'd6 | m_sel==5'd9)                                ? a_carry2 : 1'b0;\n"; // full
    moduleDefinition += std::string(SET_INDENT) + "assign a_c3 = (m_sel==5'd6 | m_sel==5'd9 | m_sel==5'd7 | m_sel==5'd10) ? a_carry3 : 1'b0;\n\n"; // full or half

    moduleDefinition += std::string(SET_INDENT) + "assign b_c1 = (m_sel==5'd6 | m_sel==5'd9 | m_sel==5'd7 | m_sel==5'd10) ? b_carry1 : 1'b0; // full or half\n";
    moduleDefinition += std::string(SET_INDENT) + "assign b_c2 = (m_sel==5'd6 | m_sel==5'd9)                                ? b_carry2 : 1'b0; // full\n";
    moduleDefinition += std::string(SET_INDENT) + "assign b_c3 = (m_sel==5'd6 | m_sel==5'd9 | m_sel==5'd7 | m_sel==5'd10) ? b_carry3 : 1'b0; // full or half\n\n";
    
	    // 2's complement
    moduleDefinition += std::string(SET_INDENT) + "assign            comp_a0  = ~in_a[size-1       : size/4*3] + b0;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {a_carry1, comp_a1} = ~in_a[(size/4*3)-1 : size/2]   + b1;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {a_carry2, comp_a2} = ~in_a[(size/2)-1   : size/4]   + b2;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {a_carry3, comp_a3} = ~in_a[(size/4)-1   : 0]        + 1'b1;\n\n";
    
    moduleDefinition += std::string(SET_INDENT) + "assign            comp_b0  = ~in_b[size-1       : size/4*3] + b0;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {b_carry1, comp_b1} = ~in_b[(size/4*3)-1 : size/2]   + b1;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {b_carry2, comp_b2} = ~in_b[(size/2)-1   : size/4]   + b2;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {b_carry3, comp_b3} = ~in_b[(size/4)-1   : 0]        + 1'b1;\n\n";
    
	    // packaging
    moduleDefinition += std::string(SET_INDENT) + "wire [size-1:0] unsigned_a, unsigned_b;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size-1:0] comped_a, comped_b;\n\n";
    
    moduleDefinition += std::string(SET_INDENT) + "assign comped_a[size-1       : size/4*3] =  sign_a0                                                                                                                          ? comp_a0 : in_a[size-1       : size/4*3];\n"; // s0
    moduleDefinition += std::string(SET_INDENT) + "assign comped_a[(size/4*3)-1 : size/2]   = ( (sign_a0 & (m_sel==5'd6 | m_sel==5'd9 | m_sel==5'd7 | m_sel==5'd10)) | (sign_a1 & (m_sel==5'd8 | m_sel==5'd11)) )               ? comp_a1 : in_a[(size/4*3)-1 : size/2];\n";   // [s0 & (F|H)] | [s1 & Q]
    moduleDefinition += std::string(SET_INDENT) + "assign comped_a[(size/2)-1   : size/4]   = ( (sign_a0 & (m_sel==5'd6 | m_sel==5'd9)) | (sign_a2 & (m_sel==5'd7 | m_sel==5'd10 | m_sel==5'd8 | m_sel==5'd11)) )               ? comp_a2 : in_a[(size/2)-1   : size/4];\n";   // [s0 & F] | [s2 & (H|Q)]
    moduleDefinition += std::string(SET_INDENT) + "assign comped_a[(size/4)-1   : 0]        = ( (sign_a0 & (m_sel==5'd6 | m_sel==5'd9)) | (sign_a2 & (m_sel==5'd7 | m_sel==5'd10)) | (sign_a3 & (m_sel==5'd8 | m_sel==5'd11)) ) ? comp_a3 : in_a[(size/4)-1   : 0];\n\n";      // [s0 & F] | [s2 & H] | [s3 & Q]
    
    moduleDefinition += std::string(SET_INDENT) + "assign comped_b[size-1       : size/4*3] =  sign_b0                                                                                                                          ? comp_b0 : in_b[size-1       : size/4*3];\n"; // s0
    moduleDefinition += std::string(SET_INDENT) + "assign comped_b[(size/4*3)-1 : size/2]   = ( (sign_b0 & (m_sel==5'd6 | m_sel==5'd9 | m_sel==5'd7 | m_sel==5'd10)) | (sign_b1 & (m_sel==5'd8 | m_sel==5'd11)) )               ? comp_b1 : in_b[(size/4*3)-1 : size/2];\n";   // [s0 & (F|H)] | [s1 & Q]
    moduleDefinition += std::string(SET_INDENT) + "assign comped_b[(size/2)-1   : size/4]   = ( (sign_b0 & (m_sel==5'd6 | m_sel==5'd9)) | (sign_b2 & (m_sel==5'd7 | m_sel==5'd10 | m_sel==5'd8 | m_sel==5'd11)) )               ? comp_b2 : in_b[(size/2)-1   : size/4];\n";   // [s0 & F] | [s2 & (H|Q)]
    moduleDefinition += std::string(SET_INDENT) + "assign comped_b[(size/4)-1   : 0]        = ( (sign_b0 & (m_sel==5'd6 | m_sel==5'd9)) | (sign_b2 & (m_sel==5'd7 | m_sel==5'd10)) | (sign_b3 & (m_sel==5'd8 | m_sel==5'd11)) ) ? comp_b3 : in_b[(size/4)-1   : 0];\n\n";      // [s0 & F] | [s2 & H] | [s3 & Q]
    
    moduleDefinition += std::string(SET_INDENT) + "assign unsigned_a = (m_sel < 5'd6) ? in_a : comped_a;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign unsigned_b = (m_sel < 5'd6) ? in_b : comped_b;\n\n";

    // Changing all operands to unsigned ***DEPRECATED***
    /*
    moduleDefinition += std::string(SET_INDENT) + "always @* begin\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + "case (m_sel)\n";
	moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "5'd6, 5'd9: begin // 2's comp entire\n";
	moduleDefinition += std::string(SET_QUAD_INDENT) +          "unsigned_a = sign_a0 ? ~in_a + 1'b1 : in_a;\n";
	moduleDefinition += std::string(SET_QUAD_INDENT) +          "unsigned_b = sign_b0 ? ~in_b + 1'b1 : in_b;\n";
	moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
	moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "5'd7, 5'd10: begin // 2's comp halves\n";
	moduleDefinition += std::string(SET_QUAD_INDENT) +          "unsigned_a = { {sign_a0 ? ~in_a[size-1:size/2] + 1'b1 : in_a[size-1:size/2]}, {sign_a2 ? ~in_a[size/2-1:0] + 1'b1 : in_a[size/2-1:0]} };\n";
	moduleDefinition += std::string(SET_QUAD_INDENT) +          "unsigned_b = { {sign_b0 ? ~in_b[size-1:size/2] + 1'b1 : in_b[size-1:size/2]}, {sign_b2 ? ~in_b[size/2-1:0] + 1'b1 : in_b[size/2-1:0]} };\n";
	moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
	moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "5'd8, 5'd11: begin // 2's comp quarts\n";
	moduleDefinition += std::string(SET_QUAD_INDENT) +          "unsigned_a = { {sign_a0 ? ~in_a[size-1:size/4*3]+1'b1 : in_a[size-1:size/4*3]}, {sign_a1 ? ~in_a[size/4*3-1:size/2]+1'b1 : in_a[size/4*3-1:size/2]}, {sign_a2 ? ~in_a[size/2-1:size/4]+1'b1 : in_a[size/2-1:size/4]}, {sign_a3 ? ~in_a[size/4-1:0]+1'b1 : in_a[size/4-1:0]} };\n";
	moduleDefinition += std::string(SET_QUAD_INDENT) +          "unsigned_b = { {sign_b0 ? ~in_b[size-1:size/4*3]+1'b1 : in_b[size-1:size/4*3]}, {sign_b1 ? ~in_b[size/4*3-1:size/2]+1'b1 : in_b[size/4*3-1:size/2]}, {sign_b2 ? ~in_b[size/2-1:size/4]+1'b1 : in_b[size/2-1:size/4]}, {sign_b3 ? ~in_b[size/4-1:0]+1'b1 : in_b[size/4-1:0]} };\n";
	moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
	moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "default: begin\n";
	moduleDefinition += std::string(SET_QUAD_INDENT) +          "unsigned_a = in_a;\n";
	moduleDefinition += std::string(SET_QUAD_INDENT) +          "unsigned_b = in_b;\n";
	moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
	moduleDefinition += std::string(SET_DOUBLE_INDENT) + "endcase\n";
	moduleDefinition += std::string(SET_INDENT) + "end\n\n";*/

    // Feed into the four separate multipliers, Zeros done by gating (AND)
    moduleDefinition += std::string(SET_INDENT) + "wire [size/2-1:0] m0_op1, m0_op2, m1_op1, m1_op2, m2_op1, m2_op2, m3_op1, m3_op2;\n";

    moduleDefinition += std::string(SET_INDENT) + "assign m0_op1 = (m_sel%3 == 2) ? {unsigned_a[size-1:size/4*3], 8'b0} : unsigned_a[size-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m0_op2 = (m_sel%3 == 2) ? {unsigned_b[size-1:size/4*3], 8'b0} : unsigned_b[size-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m1_op1 = (m_sel%3 == 2) ? {8'b0, unsigned_a[size/4*3-1:size/2]} : unsigned_a[size-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m1_op2 = (m_sel%3 == 2) ? {8'b0, unsigned_b[size/4*3-1:size/2]} : unsigned_b[size/2-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m2_op1 = (m_sel%3 == 2) ? {unsigned_a[size/2-1:size/4], 8'b0} : unsigned_a[size/2-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m2_op2 = (m_sel%3 == 2) ? {unsigned_b[size/2-1:size/4], 8'b0} : unsigned_b[size-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m3_op1 = (m_sel%3 == 2) ? {8'b0, unsigned_a[size/4-1:0]} : unsigned_a[size/2-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m3_op2 = (m_sel%3 == 2) ? {8'b0, unsigned_b[size/4-1:0]} : unsigned_b[size/2-1:0];\n\n";

    // Multipliers
    moduleDefinition += std::string(SET_INDENT) + "wire [size-1:0] m0_out, m1_out, m2_out, m3_out;\n";

    moduleDefinition += std::string(SET_INDENT) + "assign m0_out = m0_op1 * m0_op2;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m1_out = m1_op1 * m1_op2;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m2_out = m2_op1 * m2_op2;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign m3_out = m3_op1 * m3_op2;\n\n";

    // Concatenating Outputs
    moduleDefinition += std::string(SET_INDENT) + "wire [2*size-1:0] out_mul_full, out_mul_full_0;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size-1:0] out_mul_half_0, out_mul_half_1;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size/2-1:0] out_mul_quart_0, out_mul_quart_1, out_mul_quart_2, out_mul_quart_3;\n";

    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_full = (m0_out << size) + ((m1_out + m2_out) << (size/2)) + m3_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_full_0 = (sign_out0 & (m_sel >= 6)) ? ~out_mul_full + 1'b1 : out_mul_full;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_half_0 = (sign_out0 & (m_sel >= 6)) ? ~m0_out + 1'b1 : m0_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_half_1 = (sign_out2 & (m_sel >= 6)) ? ~m3_out + 1'b1 : m3_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_quart_0 = (sign_out0 & (m_sel >= 6)) ? ~m0_out[size-1:size/2] + 1'b1 : m0_out[size-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_quart_1 = (sign_out1 & (m_sel >= 6)) ? ~m1_out[size/2-1:0] + 1'b1 : m1_out[size/2-1:0];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_quart_2 = (sign_out2 & (m_sel >= 6)) ? ~m2_out[size-1:size/2] + 1'b1 : m2_out[size-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_mul_quart_3 = (sign_out3 & (m_sel >= 6)) ? ~m3_out[size/2-1:0] + 1'b1 : m3_out[size/2-1:0];\n\n";

    moduleDefinition += std::string(SET_INDENT) + "always @(*) begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "case (m_sel % 6)\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "3'd0: begin // lo -----------------\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = out_mul_full_0[size-1:0];\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "3'd1: begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = {out_mul_half_0[size/2-1:0], out_mul_half_1[size/2-1:0]};\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "3'd2: begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = {out_mul_quart_0[size/4-1:0], out_mul_quart_1[size/4-1:0], out_mul_quart_2[size/4-1:0], out_mul_quart_3[size/4-1:0]};\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "3'd3: begin // hi -----------------\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = out_mul_full_0[2*size-1:size];\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "3'd4: begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = {out_mul_half_0[size-1:size/2], out_mul_half_1[size-1:size/2]};\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "3'd5: begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = {out_mul_quart_0[size/2-1:size/4], out_mul_quart_1[size/2-1:size/4], out_mul_quart_2[size/2-1:size/4], out_mul_quart_3[size/2-1:size/4]};\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "default: begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = {size{1'bx}};\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "endcase\n";
    moduleDefinition += std::string(SET_INDENT) + "end";

    vjson["definition"] = moduleDefinition;
    return vjson;
}

BitConfig FracMulUnit::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
    const Mapping& map,
    const ConfigCell& ccell,
    const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_val_node;

    const auto bits_needed = std::lround(ceil(log2(mul_modes_ordered.size())));
    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromOpNode> opNodesByCycle(mrrg.initiationInterval());
    for (const auto& op_and_mrrg_nodes : mrrg_nodes_from_op_node) {
        for (const auto& mrrg_node : op_and_mrrg_nodes.second) {
            opNodesByCycle[mrrg_node->cycle][op_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int cycle = 0;
    for (auto & op_and_mrrg_nodes : opNodesByCycle) {
        if (op_and_mrrg_nodes.empty()) {
            bitConfig.add({ (size_t)bits_needed, BitSetting::LOW }, cycle);
        } else if (op_and_mrrg_nodes.size() == 1) {
            const auto find_result = std::find(begin(mul_modes_ordered), end(mul_modes_ordered), begin(op_and_mrrg_nodes)->first->opcode);
            if (find_result == end(mul_modes_ordered)) {
                throw cgrame_error("couldn't find op in supported modes list");
            } else {
                bitConfig.add( bitsettings_from_int(std::distance(begin(mul_modes_ordered), find_result), bits_needed), cycle);
            }

        } else {
            throw cgrame_error("expect either 0 or 1 op nodes");
        }
        ++cycle;
    }

    return bitConfig;
}

MRRG* FracMulUnit::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i+= getII())
    {
        // create nodes
        MRRG::NodeDescriptor in_a = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "in_a", {Operands::BINARY_LHS, Operands::BINARY_ANY})).first;

        MRRG::NodeDescriptor in_b = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "in_b", {Operands::BINARY_RHS, Operands::BINARY_ANY})).first;
        MRRG::NodeDescriptor fu  = result.insert(MRRGNode::make_function(this, data_size, i, "frac_mul_unit", getLatency(), mul_modes_ordered)).first;

        MRRG::NodeDescriptor out  = result.insert(MRRGNode::make_routing(this, data_size, i, "out")).first;

        result.link(in_a, fu);
        result.link(in_b, fu);
        (void)out;
    }

    for(unsigned i = 0; i < II; i+= getII())
    {
        MRRG::NodeDescriptor fu           = result.getNode(i, "frac_mul_unit");
        MRRG::NodeDescriptor out_next    = result.getNode(MOD_II(i + getLatency()), "out");

        result.link(fu, out_next);
    }

    return result_ptr;
}

FracMulUnit::~FracMulUnit()
{
}

/************ Adder **********/
const std::map<OpGraphOpCode, LLVMMode> FracAddUnit::all_modes =
{
    // Adding functionality to the funcunit
    // You can add a mode to the FuncUnit by specifying all_modes.push_back ({"module name", {"functionality"}, "wire name that feeds into the Multiplier"});
    // Functionality can be multiple lines, hence the reason why it is a vector. Each string represents one line, hence the reason it is a vector
    {OpCode::ADD_FULL,     {"op_add_full",       "add_full",          {"//add_full;"},      "add_full_sel"}},
    {OpCode::ADD_HALF,     {"op_add_half",       "add_half",          {"//add_half;"},      "add_half_sel"}},
    {OpCode::ADD_QUART,    {"op_add_quart",      "add_quart",         {"//add_quart;"},     "add_quart_sel"}}
};

const std::vector<OpGraphOpCode> FracAddUnit::add_modes_ordered = {
    OpCode::ADD_FULL,
    OpCode::ADD_HALF,
    OpCode::ADD_QUART
};

FracAddUnit::FracAddUnit(std::string name, Location loc, unsigned size, int II, int latency)
    : Module(name, loc, size)
    , pipeline_mode(II, latency)
{
    isElastic = false;
    addPort("in_a", PORT_INPUT, "size", size, isElastic);
    addPort("in_b", PORT_INPUT, "size", size, isElastic);
    addPort("a_sel", PORT_INPUT, ceil(log2(add_modes_ordered.size())), isElastic);
    addPort("out", PORT_OUTPUT_REG, "size", size, isElastic);
}

// Virtual function that overrides Module::GenericName. Returns generic name of the object
std::string FracAddUnit::GenericName() // Virtual function override
{
    return "frac_add_unit_" + std::to_string(getSize()) + "b";
}

nlohmann::json FracAddUnit::CoreIRGenFunctionality() // Virtual function override
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

    //     Declaring wires
    moduleDefinition += std::string(SET_INDENT) + "wire a1_carry, a2_carry, a3_carry;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size/4-1:0] a0_out, a1_out, a2_out, a3_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size-1:0] f0;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size/2-1:0] h0, h1;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size/4-1:0] q0, q1, q2, q3;\n";
    moduleDefinition += std::string(SET_INDENT) + "wire [size-1:0] out_add_full, out_add_half, out_add_quart;\n\n";

    //     Configure the outputs of the frac adders
    moduleDefinition += std::string(SET_INDENT) + "assign a0_out = in_a[size-1:size/4*3] + in_b[size-1:size/4*3];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {a1_carry, a1_out} = in_a[size/4*3-1:size/2] + in_b[size/4*3-1:size/2];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {a2_carry, a2_out} = in_a[size/2-1:size/4] + in_b[size/2-1:size/4];\n";
    moduleDefinition += std::string(SET_INDENT) + "assign {a3_carry, a3_out} = in_a[size/4-1:0] + in_b[size/4-1:0];\n\n";

    //     Packaging the outputs from frac adders
    moduleDefinition += std::string(SET_INDENT) + "assign f0 = {(a0_out + a1_carry), (a1_out + a2_carry), (a2_out + a3_carry), a3_out};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign h0 = {(a0_out + a1_carry), a1_out};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign h1 = {(a2_out + a3_carry), a3_out};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign q0 = a0_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign q1 = a1_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign q2 = a2_out;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign q3 = a3_out;\n\n";
    
    moduleDefinition += std::string(SET_INDENT) + "assign out_add_full = f0;\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_add_half = {h0, h1};\n";
    moduleDefinition += std::string(SET_INDENT) + "assign out_add_quart = {q0, q1, q2, q3};\n\n";

    //     Output select
    moduleDefinition += std::string(SET_INDENT) + "always @* begin\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "case (a_sel%3)\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "2'b00: begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = out_add_full;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "2'b01: begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = out_add_half;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "2'b10: begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = out_add_quart;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "default: begin\n";
    moduleDefinition += std::string(SET_QUAD_INDENT) +          "out = 1'bx;\n";
    moduleDefinition += std::string(SET_TRIPLE_INDENT) +    "end\n";
    moduleDefinition += std::string(SET_DOUBLE_INDENT) + "endcase\n";
    moduleDefinition += std::string(SET_INDENT) + "end";

    vjson["definition"] = moduleDefinition;
    return vjson;
}

BitConfig FracAddUnit::getBitConfig(
    const MRRG& mrrg, const OpGraph & og,
    const Mapping& map,
    const ConfigCell& ccell,
    const MRRGNodesFromOpNode& mrrg_nodes_from_op_node,
    const MRRGNodesFromValNode& mrrg_nodes_from_val_node
) const {
    (void)ccell;
    (void)mrrg_nodes_from_val_node;

    const auto bits_needed = std::lround(ceil(log2(add_modes_ordered.size())));
    BitConfig bitConfig(mrrg.initiationInterval());

    // Organize nodes mapping by cycle
    std::vector<MRRGNodesFromOpNode> opNodesByCycle(mrrg.initiationInterval());
    for (const auto& op_and_mrrg_nodes : mrrg_nodes_from_op_node) {
        for (const auto& mrrg_node : op_and_mrrg_nodes.second) {
            opNodesByCycle[mrrg_node->cycle][op_and_mrrg_nodes.first].insert(mrrg_node);
        }
    }

    int cycle = 0;
    for (auto & op_and_mrrg_nodes : opNodesByCycle) {
        if (op_and_mrrg_nodes.empty()) {
            bitConfig.add({ (size_t)bits_needed, BitSetting::LOW }, cycle);
        } else if (op_and_mrrg_nodes.size() == 1) {
            const auto find_result = std::find(begin(add_modes_ordered), end(add_modes_ordered), begin(op_and_mrrg_nodes)->first->opcode);
            if (find_result == end(add_modes_ordered)) {
                throw cgrame_error("couldn't find op in supported modes list");
            } else {
                bitConfig.add( bitsettings_from_int(std::distance(begin(add_modes_ordered), find_result), bits_needed), cycle);
            }

        } else {
            throw cgrame_error("expect either 0 or 1 op nodes");
        }
        ++cycle;
    }

    return bitConfig;
}

MRRG* FracAddUnit::createMRRG(unsigned II = 1)
{
    MRRG* result_ptr = new MRRG(II);
    auto& result = *result_ptr;

    for(unsigned i = 0; i < II; i+= getII())
    {
        // create nodes
        MRRG::NodeDescriptor in_a = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "in_a", {Operands::BINARY_LHS, Operands::BINARY_ANY})).first;

        MRRG::NodeDescriptor in_b = result.insert(MRRGNode::make_operand_pin(this, data_size, i, "in_b", {Operands::BINARY_RHS, Operands::BINARY_ANY})).first;
        MRRG::NodeDescriptor fu  = result.insert(MRRGNode::make_function(this, data_size, i, "frac_add_unit", getLatency(), add_modes_ordered)).first;

        MRRG::NodeDescriptor out  = result.insert(MRRGNode::make_routing(this, data_size, i, "out")).first;

        result.link(in_a, fu);
        result.link(in_b, fu);
        (void)out;
    }

    for(unsigned i = 0; i < II; i+= getII())
    {
        MRRG::NodeDescriptor fu           = result.getNode(i, "frac_add_unit");
        MRRG::NodeDescriptor out_next    = result.getNode(MOD_II(i + getLatency()), "out");

        result.link(fu, out_next);
    }

    return result_ptr;
}

FracAddUnit::~FracAddUnit()
{
}
