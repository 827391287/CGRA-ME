/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

//HyCUBEPE.cpp
/**
 * Represents a HyCUBE Processing Element as specified in the HyCUBE Paper from NUS
 */

#include <CGRA/user-inc/UserModules.h>

// Macro for calculating number of ones
static const unsigned char ones_in_4b[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
#define get_ones_in_8b(v) (ones_in_4b[v & 0x0f] + ones_in_4b[v >> 4])



std::string HyCUBEPE::GenericName()
{
    std::string name;

    if(predExist)
    {
        name = "hycube_pred_in" + std::to_string(pe_conn_in) + "_out" + std::to_string(pe_conn_out) + "_pred_in" + std::to_string(pred_conn_in) + "_pred_out" + std::to_string(pred_conn_out);
    } else
    {
        name = "hycube_in" + std::to_string(pe_conn_in) + "_out" + std::to_string(pe_conn_out);
    }

    return name;
}


HyCUBEPE::HyCUBEPE(std::string name, bool predExist, int pe_conn_in, int pe_conn_out,
                   int pred_conn_in, int pred_conn_out, int fu_II, int II, int latency, Location loc, std::string pred_scheme)
    : Module(name, loc, 32)
	, predExist(predExist)
    , pe_conn_in(pe_conn_in)
    , pe_conn_out(pe_conn_out)
    , pred_conn_in(pred_conn_in)
    , pred_conn_out(pred_conn_out)
{
 // Figure out Word Length
    int size = getSize();

    // Figure out number of inputs, and outputs using pe_conn
    int num_outPorts = get_ones_in_8b(pe_conn_out);
    int num_inPorts = get_ones_in_8b(pe_conn_in);

    // TODO: Calculate total number of possible inputs/outputs, based on the length of pe_conn
    int num_Possible_Ports = 8;

    // 1. Add all submodules
    // ALU
    addSubModule(new FuncUnit("ALU", loc,
        {
        OpCode::ADD,
        OpCode::SUB,
        OpCode::MUL,
        OpCode::AND,
        OpCode::OR,
        OpCode::XOR,
        OpCode::SHL,
        OpCode::ASHR,
        OpCode::LSHR,
        OpCode::ICMP,
    }, size, fu_II, latency), 0, 0.75, 0.5, 0.25);

    // Crossbar
    // NOTE: Crossbar connection indices may not match with HyCUBEPE's input and output indices (ie this.in1 may be assigned to crossbar.in0)
    int numCrossbarInputs = num_inPorts + 3; // Add 3 for ALU, const and RES register outputs,
    int numCrossbarOutputs = num_outPorts + 2; // Add 2 for ALU in_a and in_b
    bool crossbarSelect = predExist & pred_scheme.find("event") == std::string::npos;
    addSubModule(new Crossbar("crossbar", loc, numCrossbarInputs, numCrossbarOutputs, size, crossbarSelect, II), 0.5, 0.375, 0.4, 0.4);

    // Registers
    // RES register (stores output of ALU)
    addSubModule(new Register("RES", loc, size), 0.5, 0.875, 0.5, 0.125);


    // Inputs to ALU
    addSubModule(new Register("rega", loc, size), 0, 0.05, 0.5, 0.125);
    addSubModule(new Register("regb", loc, size), 0.5, 0.05, 0.5, 0.125);
    addSubModule(new ConstUnit("const_val", loc, size, II), 0.0, 0.15, 1, 0.125);

    // Input registers and 2-to-1 Muxes must follow the pe_conn_in convention (only initialize for enabled connections)
    for (int i = 0; i < num_Possible_Ports; i++) {
        if (pe_conn_in & (0b00000001 << i)) {
            // Input registers (use size to include predication)
            addSubModule(new Register("reg" + std::to_string(i), loc, size), static_cast<double>(i)/num_Possible_Ports, 0.5, 1.0/num_Possible_Ports, 0.125);
            // Input 2-to-1 muxes
            addSubModule(new Multiplexer("mux_" + std::to_string(i), loc, 2, size), static_cast<double>(i)/num_Possible_Ports, 0.625, 1.0/num_Possible_Ports, 0.125);
        }
    }
    
    if (II > 1) 
        addPort("Context", PORT_INPUT, ceil(log2(II)));

    // 2. Add Configuration files
    // ALU
    addConfig(new ConfigCell("ALUconfig", II), {"ALU.select"});
    addConfig(new ConfigCell("RegAConfig", II), {"regb.enable"});
    addConfig(new ConfigCell("RegBConfig", II), {"rega.enable"});
    addConfig(new ConfigCell("RESConfig", II), {"RES.enable"});


    for(int i = 0; i < num_Possible_Ports; i++)
    {
        if (pe_conn_in & (0b00000001<<i))
        {
            // Input registers
            addConfig(new ConfigCell("Reg" + std::to_string(i) + "config", II), {"reg" + std::to_string(i) + ".enable"});
            // Input 2-to-1 Mux
            addConfig(new ConfigCell("Mux" + std::to_string(i) + "config", II), {"mux_" + std::to_string(i) + ".select"});
        }
    }
    
    if (II > 1) {
        addConnection("this.Context" , "ALUconfig.Context", false);
        addConnection("this.Context" , "RegAConfig.Context", false);
        addConnection("this.Context" , "RegBConfig.Context", false);
        addConnection("this.Context" , "RESConfig.Context", false);
        for(int i = 0; i < num_Possible_Ports; i++) {
            if (pe_conn_in & (0b00000001<<i)) {
                addConnection("this.Context" , "Reg" + std::to_string(i) + "config.Context", false);
                addConnection("this.Context" , "Mux" + std::to_string(i) + "config.Context", false);
            }
        }
        addConnection("this.Context" , "crossbar.Context", false);
        addConnection("this.Context" , "const_val.Context", false);
    }

    // 3. Ports and Wiring
    // 3A. Input Ports and Output Ports must follow pe_conn_in and pe_conn_out conventions, respectively
    for(int i = 0; i < num_Possible_Ports; i++)
    {
        if (pe_conn_in & (0b00000001<<i))
        {
            // Add input ports
            addPort("in" + std::to_string(i), PORT_INPUT, size);
            node_relative_position.insert({"in" + std::to_string(i), {0.1, static_cast<double>(i)/num_Possible_Ports}});    // Add position for visualization
        }

        if (pe_conn_out & (0b00000001<<i))
        {
            // Add output ports
            addPort("out" + std::to_string(i), PORT_OUTPUT, size);
            node_relative_position.insert({"out" + std::to_string(i), {0.9, static_cast<double>(i)/num_Possible_Ports}});   // Add position for visualization
        }
    }

    // 3B. Connections
    // InPorts to Registers
    for(int i = 0; i < num_Possible_Ports; i++)
    {
        if (pe_conn_in & (0b00000001<<i))
        {   // InPorts to Registers
            addConnection("this.in" + std::to_string(i), "reg" + std::to_string(i) + ".in");

            // InPorts and Registers to Muxes
            addConnection("this.in" + std::to_string(i), "mux_" + std::to_string(i) + ".in0");
            addConnection("reg" + std::to_string(i) + ".out", "mux_" + std::to_string(i) + ".in1");
        }
    }

    // Muxes to Crossbar
    int crossbarInputCounter = 0; // Needed because the crossbar and mux indices may not be the same

    for(int i = 0; i < num_Possible_Ports; i++)
    {
        if (pe_conn_in & (0b00000001<<i))
        {
            addConnection("mux_" + std::to_string(i) + ".out", "crossbar.in" + std::to_string(crossbarInputCounter));
            crossbarInputCounter += 1; //TODO: Throw an error if this goes out of bounds
        }
    }

    // ALU to Crossbar
    addConnection("ALU.out", "crossbar.in" + std::to_string(num_inPorts));

    // RES to Crossbar
    addConnection("RES.out", "crossbar.in" + std::to_string(num_inPorts + 1));

    // Const to Crossbar
    addConnection("const_val.out", "crossbar.in" + std::to_string(num_inPorts + 2));

    // Crossbar to outPorts
    int crossbarOutputCounter = 0; // Needed because the crossbar and mux indices may not be the same

    for(int i = 0; i < num_Possible_Ports; i++)
    {
        if (pe_conn_out & (0b00000001<<i))
        {
            addConnection("crossbar.out" + std::to_string(crossbarOutputCounter), "this.out" + std::to_string(i));
            crossbarOutputCounter += 1; //TODO: Throw an error if this goes out of bounds
        }
    }

    // Crossbar to rega, regb
    addConnection("crossbar.out" + std::to_string(num_outPorts), "rega.in");
    addConnection("crossbar.out" + std::to_string(num_outPorts + 1), "regb.in");

    // ALU inputs
    addConnection("rega.out", "ALU.in_a");
    addConnection("regb.out", "ALU.in_b");

    // ALU to RES
    addConnection("ALU.out", "RES.in");

    // If predExist add predication network.
    if (!predExist) return;
    /****************** PRED NETWORK BEGINS **************/
    /* The predication network is the same as the normal */
    /* 32-bit architecture however it is a 1-bit wide n- */
    /* etwork.                                           */
    /*****************************************************/

    addSubModule(new TruncateInput("trunc_res", loc, 1, size));
    addSubModule(new TruncateInput("trunc_const", loc, 1, size));
    // set size =1 for pred network
    size = 1;

    // Figure out number of inputs, and outputs using pred_conn
    int num_pred_outPorts = get_ones_in_8b(pred_conn_out);
    int num_pred_inPorts = get_ones_in_8b(pred_conn_in);
    // Add 2 for ALU_pred and RES_pred register outputs,
    // add 1 for const and 1 for connection coming from ALU
    int numPredCrossbarInputs = num_pred_inPorts + 2 + 2;
    // Add 2 for ALU_pred in_a, in_b, add 1 to connect back to phi mux
    int numPredCrossbarOutputs = num_pred_outPorts + 2 + 1;

    // 1. Add all submodules
    // ALU
    addSubModule(new FuncUnit("ALU_pred", loc,
        {
        OpCode::AND,
        OpCode::OR,
        OpCode::XOR,
    }, size, 1, latency), 0, 0.75, 0.5, 0.25);

    // Crossbar
    // NOTE: Crossbar connection indices may not match with HyCUBEPE_Phi's input and output indices (ie this.in1 may be assigned to crossbar.in0)
    addSubModule(new Crossbar("crossbar_pred", loc, numPredCrossbarInputs, numPredCrossbarOutputs, size, false, II), 0.5, 0.375, 0.4, 0.4);

    // Registers
    // RES register (stores output of ALU)
    addSubModule(new Register("RES_pred", loc, size), 0.5, 0.875, 0.5, 0.125);

    // Inputs to ALU
    addSubModule(new Register("rega_pred", loc, size), 0, 0.05, 0.5, 0.125);
    addSubModule(new Register("regb_pred", loc, size), 0.5, 0.05, 0.5, 0.125);
    // addSubModule(new ConstUnit("const_pred", loc, 1), 0.0, 0.15, 1, 0.125);

    // Input registers and 2-to-1 Muxes must follow the pred_conn_in convention (only initialize for enabled connections)
    for (int i = 0; i < num_Possible_Ports; i++) {
        if (pred_conn_in & (0b00000001 << i)) {
            // Input registers (use size to include predication)
            addSubModule(new Register("reg_pred" + std::to_string(i), loc, size), static_cast<double>(i)/num_Possible_Ports, 0.5, 1.0/num_Possible_Ports, 0.125);
            // Input 2-to-1 muxes
            addSubModule(new Multiplexer("mux_pred_" + std::to_string(i), loc, 2, size), static_cast<double>(i)/num_Possible_Ports, 0.625, 1.0/num_Possible_Ports, 0.125);
        }
    }



    for(int i = 0; i < num_Possible_Ports; i++)
    {
        if (pred_conn_in & (0b00000001<<i))
        {
            // Add input ports
            addPort("in_pred" + std::to_string(i), PORT_INPUT, size);
            node_relative_position.insert({"in_pred" + std::to_string(i), {0.1, static_cast<double>(i)/num_Possible_Ports}});    // Add position for visualization
        }

        if (pred_conn_out & (0b00000001<<i))
        {
            // Add output ports
            addPort("out_pred" + std::to_string(i), PORT_OUTPUT, size);
            node_relative_position.insert({"out_pred" + std::to_string(i), {0.9, static_cast<double>(i)/num_Possible_Ports}});   // Add position for visualization
        }
    }

    // 2. Add Configuration files
    // ALU
    addConfig(new ConfigCell("ALUconfig_pred", II), {"ALU_pred.select"});
    addConfig(new ConfigCell("RegAConfig_pred", II), {"regb_pred.enable"});
    addConfig(new ConfigCell("RegBConfig_pred", II), {"rega_pred.enable"});
    addConfig(new ConfigCell("RESConfig_pred", II), {"RES_pred.enable"});

    // addConfig(new ConfigCell{"constValPred"}, {"const_pred.in"});
   

    for(int i = 0; i < num_Possible_Ports; i++) {
        if (pred_conn_in & (0b00000001<<i))
        {
            // Input registers
            addConfig(new ConfigCell("Reg_pred" + std::to_string(i) + "config", II), {"reg_pred" + std::to_string(i) + ".enable"});
            // Input 2-to-1 Mux
            addConfig(new ConfigCell("Mux_pred" + std::to_string(i) + "config", II), {"mux_pred_" + std::to_string(i) + ".select"});
        }
    }
    if (II > 1) {
        addConnection("this.Context" , "ALUconfig_pred.Context", false);
        addConnection("this.Context" , "RegAConfig_pred.Context", false);
        addConnection("this.Context" , "RegBConfig_pred.Context", false);
        addConnection("this.Context" , "RESConfig_pred.Context", false);
        for(int i = 0; i < num_Possible_Ports; i++) {
            if (pred_conn_in & (0b00000001<<i)) {
                addConnection("this.Context" , "Reg_pred" + std::to_string(i) + "config.Context", false);
                addConnection("this.Context" , "Mux_pred" + std::to_string(i) + "config.Context", false);
            }
        }

        addConnection("this.Context" , "crossbar_pred.Context", false);
    }
    
    // 3B. Connections
    // InPorts to Registers
    addConnection("ALU.out", "trunc_res.in");
    addConnection("const_val.out", "trunc_const.in");
    addConnection("trunc_res.out0", "crossbar_pred.in" + std::to_string(numPredCrossbarInputs - 1));

    for(int i = 0; i < num_Possible_Ports; i++)
    {
        if (pred_conn_in & (0b00000001<<i))
        {   // InPorts to Registers
            addConnection("this.in_pred" + std::to_string(i), "reg_pred" + std::to_string(i) + ".in");

            // InPorts and Registers to Muxes
            addConnection("this.in_pred" + std::to_string(i), "mux_pred_" + std::to_string(i) + ".in0");
            addConnection("reg_pred" + std::to_string(i) + ".out", "mux_pred_" + std::to_string(i) + ".in1");
        }
    }

    // Muxes to Crossbar
    crossbarInputCounter = 0; // Needed because the crossbar and mux indices may not be the sameout0

    for(int i = 0; i < num_Possible_Ports; i++)
    {
        if (pred_conn_in & (0b00000001<<i))
        {
            addConnection("mux_pred_" + std::to_string(i) + ".out", "crossbar_pred.in" + std::to_string(crossbarInputCounter));
            crossbarInputCounter += 1; //TODO: Throw an error if this goes out of bounds
        }
    }
 

    // ALU to Crossbar
    addConnection("ALU_pred.out", "crossbar_pred.in" + std::to_string(num_pred_inPorts));

    // RES to Crossbar
    addConnection("RES_pred.out", "crossbar_pred.in" + std::to_string(num_pred_inPorts + 1));

    // Const to Crossbar
    //addConnection("const_pred.out", "crossbar_pred.in" + std::to_string(num_pred_inPorts + 2));
    addConnection("trunc_const.out0", "crossbar_pred.in" + std::to_string(num_pred_inPorts + 2));

    //TODO:: FIX THIS
    crossbarOutputCounter = 0; // Needed because the crossbar and mux indices may not be the same

    for(int i = 0; i < num_Possible_Ports; i++)
    {
        if (pred_conn_out & (0b00000001<<i))
        {
            addConnection("crossbar_pred.out" + std::to_string(crossbarOutputCounter), "this.out_pred" + std::to_string(i));
            crossbarOutputCounter += 1; //TODO: Throw an error if this goes out of bounds
        }
    }

    // Crossbar to rega, regb
    addConnection("crossbar_pred.out" + std::to_string(num_pred_outPorts), "rega_pred.in");
    addConnection("crossbar_pred.out" + std::to_string(num_pred_outPorts + 1), "regb_pred.in");


    // ALU inputs
    addConnection("rega_pred.out", "ALU_pred.in_a");
    addConnection("regb_pred.out", "ALU_pred.in_b");


    // ALU to RES
    addConnection("ALU_pred.out", "RES_pred.in");
    if (pred_scheme.find("event") == std::string::npos) {
        //addConnection("RES_pred.out", "crossbar.pred_in");
        addConnection("crossbar_pred.out" + std::to_string(numPredCrossbarOutputs -1), "crossbar.pred_in");
    }
}
