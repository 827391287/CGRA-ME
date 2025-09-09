/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/user-inc/UserModules.h>
#include <climits>

std::string AdresPE::GenericName()
{
    return "adres_" + std::to_string(num_inputs) + "in_" + fu_type;
}

AdresPE::AdresPE(std::string name, int num_inputs_arg, std::string fu_type, int fu_II, int latency, int II, Location loc, bool use_op_div, bool predExist, bool reg_bypass, bool extra_mem, std::string pred_scheme)
    : Module(name, loc)
    , fu_type(fu_type)
    , num_inputs(num_inputs_arg)
{
    isElastic = false;
    int size = 32;
    // define op lists for types for functional units
    using OpGraphVec = std::vector<OpGraphOpCode>;
    std::map<std::string, OpGraphVec> fu_oplist;
    fu_oplist["vliw"] = {
      OpCode::ADD,
      OpCode::MUL,
      OpCode::SUB,
      OpCode::AND,
      OpCode::OR,
      OpCode::XOR,
      OpCode::SHL,
      OpCode::ASHR,
      OpCode::LSHR,
      OpCode::ICMP};
    fu_oplist["cga"] = {
      OpCode::ADD,
      OpCode::SUB};

    if (use_op_div) fu_oplist["vliw"].push_back(OpCode::DIV);

    // Instantiate submodules
    if (predExist && pred_scheme.find("event") == std::string::npos) {
        addSubModule(new SelMultiplexer("mux_a", loc, num_inputs + 2), 0, 0, 0.5, 0.25);                                     // mux for FU input "A" (+2 bc of const, rf)
        addSubModule(new SelMultiplexer("mux_b", loc, num_inputs + 2), 0.5, 0, 0.5, 0.25);
        addSubModule(new SelMultiplexer("mux_bypass", loc, num_inputs + 1), 0, 0.25, 0.5, 0.25);// bypass mux for all inputs
    } else {
        addSubModule(new Multiplexer("mux_a", loc, num_inputs + 2), 0, 0, 0.5, 0.25);                                     // mux for FU input "A" (+2 bc of const, rf)
        addSubModule(new Multiplexer("mux_b", loc, num_inputs + 2), 0.5, 0, 0.5, 0.25);                                   // mux for FU input "B" (+2 bc of const, mux_out)
        addSubModule(new Multiplexer("mux_bypass", loc, num_inputs), 0, 0.25, 0.5, 0.25);// bypass mux for all inputs
    }
    addSubModule(new ConstUnit("const_val", loc, 32, II), 0.5, 0.25, 0.5, 0.25);               // const unit
    addSubModule(new FuncUnit("func", loc, fu_oplist[fu_type], 32, fu_II, latency), 0, 0.5, 0.5, 0.25);// choose supported ops according to 'fu_type'
    addSubModule(new Multiplexer("mux_out", loc, 2), 0.5, 0.5, 0.5, 0.25);         // mux for output (choose bypass or fu)

    // Instantiate input & output ports
    for(int i = 0; i < num_inputs; i++)
        addPort("in" + std::to_string(i), PORT_INPUT, size);
    addPort("out", PORT_OUTPUT, size);

    // Register file input & output ports
    addPort("rf_to_muxa", PORT_INPUT, size); // input port for external RF into MUXA
    addPort("rf_to_muxout", PORT_INPUT, size); // input port for external RF into MUX_OUT
    addPort("fu_to_rf", PORT_OUTPUT, size); // output port for external RF from FU
    if (reg_bypass) { 
        addPort("bypass_to_rf", PORT_OUTPUT, size); // output port for external RF from FU
    }
    if (II > 1) 
        addPort("Context", PORT_INPUT, ceil(log2(II)));  

    // Instantiate configuration cells
    addConfig(new ConfigCell("MuxAConfig", II), {"mux_a.select"});
    addConfig(new ConfigCell("MuxBConfig", II), {"mux_b.select"});
    addConfig(new ConfigCell("MuxBypassConfig", II), {"mux_bypass.select"});
    addConfig(new ConfigCell("MuxOutConfig", II), {"mux_out.select"});
    addConfig(new ConfigCell("FuncConfig", II), {"func.select"});
    if (II > 1) {
        addConnection("this.Context" , "MuxAConfig.Context", false);
        addConnection("this.Context" , "MuxBConfig.Context", false);
        addConnection("this.Context" , "MuxBypassConfig.Context", false);
        addConnection("this.Context" , "MuxOutConfig.Context", false);
        addConnection("this.Context" , "FuncConfig.Context", false);
        addConnection("this.Context" , "const_val.Context", false);
    }

    // Mux A connections
    for(int i = 0; i < num_inputs; i++)
        addConnection("this.in" + std::to_string(i), "mux_a.in" + std::to_string(i));

    addConnection("const_val.out", "mux_a.in" + std::to_string(num_inputs));
    addConnection("this.rf_to_muxa", "mux_a.in" + std::to_string(num_inputs + 1)); // rf connected to one input
    addConnection("mux_a.out", "func.in_a");

    // Mux B connections
    for(int i = 0; i < num_inputs; i++)
        addConnection("this.in" + std::to_string(i), "mux_b.in" + std::to_string(i));

    addConnection("const_val.out", "mux_b.in" + std::to_string(num_inputs));
    if (predExist) {
        addConnection("const_val.out", "mux_bypass.in" + std::to_string(num_inputs));
    }
    addConnection("mux_out.out", "mux_b.in" + std::to_string(num_inputs + 1));
    addConnection("mux_b.out", "func.in_b");

    // BYPASS MUX CONNECTIONS
    for(int i = 0; i < num_inputs; i++)
        addConnection("this.in" + std::to_string(i), "mux_bypass.in" + std::to_string(i));
    addConnection("mux_bypass.out", "mux_out.in1");

    // output muxing
    addConnection("this.rf_to_muxout", "mux_out.in0");
    addConnection("func.out", "this.fu_to_rf"); // connect registered output to rf
    if (reg_bypass) {
        addConnection("mux_bypass.out", "this.bypass_to_rf"); // connect registered output to rf
    }
    addConnection("mux_out.out", "this.out");

    if (!predExist) return;

    /****************** PRED NETWORK BEGINS **************/
    /* The predication network is the same as the normal */
    /* 32-bit architecture however it is a 1-bit wide n- */
    /* etwork.                                           */
    /*****************************************************/

    bool top_pe = name.find("r0") != std::string::npos;
    bool is_event_pred_scheme = pred_scheme.find("event") != std::string::npos;
    addSubModule(new TruncateInput("trunc_res", loc, 1, size));
    addSubModule(new TruncateInput("trunc_const", loc, 1, size));
    size = 1;

    // define op lists for types for functional units
    using OpGraphVec = std::vector<OpGraphOpCode>;
    std::map<std::string, OpGraphVec> fu_oplist_pred;
    fu_oplist_pred["pred"] = {
      OpCode::AND,
      OpCode::OR,
      OpCode::XOR,
      };
    
    // Instantiate submodules
    addSubModule( new Multiplexer("mux_a_pred", loc, num_inputs + (2 - extra_mem), size), 0, 0, 0.5, 0.25);                                     // mux for FU input "A" (+2 bc of const, rf)
    addSubModule( new Multiplexer("mux_b_pred", loc, num_inputs + (2 - extra_mem), size), 0.5, 0, 0.5, 0.25);                                   // mux for FU input "B" (+2 bc of const, mux_out)
    
    addSubModule(new Multiplexer("mux_bypass_pred", loc, num_inputs - extra_mem, size), 0, 0.25, 0.5, 0.25);// bypass mux for all inputs
    
    addSubModule(new FuncUnit("func_pred", loc, fu_oplist_pred["pred"], size, 1, latency), 0, 0.5, 0.5, 0.25);// choose supported ops according to 'fu_type'
    addSubModule(new Multiplexer("mux_out_pred", loc, 2, size), 0.5, 0.5, 0.5, 0.25);         // mux for output (choose bypass or fu)
    addSubModule(new Multiplexer("mux_pred_out", loc, 4, size), 0.5, 0.5, 0.5, 0.25);         // mux for output (choose bypass or fu)

    // Instantiate configuration cells
    addConfig(new ConfigCell("MuxAConfigPred", II), {"mux_a_pred.select"});
    addConfig(new ConfigCell("MuxBConfigPred", II), {"mux_b_pred.select"});
    addConfig(new ConfigCell("MuxBypassConfigPred", II), {"mux_bypass_pred.select"});
    addConfig(new ConfigCell("MuxPredOutraConfigPred", II), {"mux_pred_out.select"});
    addConfig(new ConfigCell("MuxOutConfigPred", II), {"mux_out_pred.select"});
    addConfig(new ConfigCell("FuncConfigPred", II), {"func_pred.select"});
    if (II > 1) {
        addConnection("this.Context" , "MuxAConfigPred.Context", false);
        addConnection("this.Context" , "MuxBConfigPred.Context", false);
        addConnection("this.Context" , "MuxBypassConfigPred.Context", false);
        addConnection("this.Context" , "MuxOutConfigPred.Context", false);
        addConnection("this.Context" , "FuncConfigPred.Context", false);
    }

    // Instantiate input & output ports
    for(int i = 0; i < num_inputs; i++){
        if (i <= 0 + extra_mem) continue;
        addPort("in_pred" + std::to_string(i), PORT_INPUT, size);
    }
    addPort("out_pred", PORT_OUTPUT, size);

    // Register file input & output ports
    addPort("rf_to_muxa_pred", PORT_INPUT, size); // input port for external RF into MUXA
    addPort("rf_to_muxout_pred", PORT_INPUT, size); // input port for external RF into MUX_OUT
    addPort("fu_to_rf_pred", PORT_OUTPUT, size); // output port for external RF from FU
    if (reg_bypass) { 
        addPort("bypass_to_rf_pred", PORT_OUTPUT, size); // output port for external RF from FU
    }

    addConnection("mux_out.out", "trunc_res.in");
    addConnection("const_val.out", "trunc_const.in");

    // Mux A connections
    for(int i = 0; i < num_inputs; i++) {
        if (i <= 0 + extra_mem) continue;
        addConnection("this.in_pred" + std::to_string(i), "mux_a_pred.in" + std::to_string(i - (1 + extra_mem)));
    }
    addConnection("trunc_const.out0", "mux_a_pred.in" + std::to_string(num_inputs - (1 + extra_mem)));
    addConnection("this.rf_to_muxa_pred", "mux_a_pred.in" + std::to_string(num_inputs - extra_mem)); // rf connected to one input
    addConnection("trunc_res.out0", "mux_a_pred.in" + std::to_string(num_inputs + (1 - extra_mem)));
    addConnection("mux_a_pred.out", "func_pred.in_a");
    addConnection("mux_a_pred.out", "mux_pred_out.in0");

    // Mux B connections
    for(int i = 0; i < num_inputs; i++) {
        if (i <= 0 + extra_mem) continue;
        addConnection("this.in_pred" + std::to_string(i), "mux_b_pred.in" + std::to_string(i- (1 + extra_mem)));
    }

    addConnection("trunc_const.out0", "mux_b_pred.in" + std::to_string(num_inputs - (1+ extra_mem)));
    addConnection("mux_out_pred.out", "mux_b_pred.in" + std::to_string(num_inputs - (extra_mem)));
    addConnection("trunc_res.out0", "mux_b_pred.in" + std::to_string(num_inputs + (1 - extra_mem)));
    addConnection("mux_b_pred.out", "func_pred.in_b");
    addConnection("mux_b_pred.out", "mux_pred_out.in1");

    // BYPASS MUX CONNECTIONS
    for(int i = 0; i < num_inputs; i++){
        if (i <= 0 + extra_mem) continue;
        addConnection("this.in_pred" + std::to_string(i), "mux_bypass_pred.in" + std::to_string(i - (1 + extra_mem)));
    }
    addConnection("trunc_res.out0", "mux_bypass_pred.in" + std::to_string(num_inputs - (1 + extra_mem)));
    addConnection("mux_bypass_pred.out", "mux_out_pred.in1");
    addConnection("mux_bypass_pred.out", "mux_pred_out.in2");

    // output muxing
    addConnection("this.rf_to_muxout_pred", "mux_out_pred.in0");
     addConnection("this.rf_to_muxout_pred", "mux_pred_out.in3");
    addConnection("func_pred.out", "this.fu_to_rf_pred"); // connect registered output to 
    if (reg_bypass) {
        addConnection("mux_bypass_pred.out", "this.bypass_to_rf_pred"); // connect registered output to rf
    }
    if (pred_scheme.find("event") == std::string::npos){
        addConnection("mux_pred_out.out", "mux_b.selectpred"); // connect registered output to rf
        addConnection("mux_pred_out.out", "mux_bypass.selectpred"); // connect registered output to rf
        addConnection("mux_pred_out.out", "mux_a.selectpred"); // connect registered output to rf
    }
    addConnection("mux_out_pred.out", "this.out_pred");
}

