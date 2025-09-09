/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

/**
 * RIKEN PE as shown in the Cluster 22 paper with the separate switch block.
 * "Disjoint" SB
 * Boma A. Adhi, Carlos Cortes, Yiyu Tan, Takuya Kojima, Artur Podobas, Kentaro Sano,
 * The Cost of Flexibility: Embedded versus Discrete Routers in CGRAs for HPC. CLUSTER 2022: 347-356
 */

#include <CGRA/user-inc/UserModules.h>

// Macro for calculating number of ones
static const unsigned char ones_in_4b[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
#define get_ones_in_8b(v) (ones_in_4b[v & 0x0f] + ones_in_4b[v >> 4])

std::string RIKEN_PE_Elastic::GenericName()
{
    std::string name;

    name = "elastic_riken_in" + std::to_string(pe_conn_in) + "_out" + std::to_string(pe_conn_out);

    return name;
}

RIKEN_PE_Elastic::RIKEN_PE_Elastic(std::string name, int pe_conn_in, int pe_conn_out,  int II, int latency, Location loc, int type, int buffer_depth, bool eb_enable, int contexts, bool pred, std::string pred_type)
    : Module(name, loc, 32)
    , pe_conn_in(pe_conn_in)
    , pe_conn_out(pe_conn_out)
    , buffer_depth(buffer_depth)
{   
  
  // Figure out Word Length
    int size = getSize();
    isElastic = true; // RIKEN CGRA is dataflow
    if (II != 1)
      throw cgrame_error ("RIKEN CGRA must be single context.\n");
    if (pred == 1)
      throw cgrame_error ("PREDICATE support for Riken Elastic CGRA under development.\n");
    
    int num_ALU_inputs = 2;
    // Figure out number of inputs, and outputs using pe_conn
    int num_outPorts = get_ones_in_8b(pe_conn_out);
    int num_inPorts = get_ones_in_8b(pe_conn_in);

    int num_Possible_Ports = 8; // possible connections from this crossbar to OTHER crossbars in different PEs

    // ****************************************** INSTANTIATE SUBMODULES OF THE PE ************           
    // Add the "guts" of the PE, e.g., the elastic buffers, constant reg, ALU, etc.
    
    addSubModule(new ElasticBufferFifo("buffA", loc, buffer_depth, 32, eb_enable), 0.0, 0.4, 0.5, 0.1);
    addSubModule(new ElasticBufferFifo("buffB", loc, buffer_depth, 32, eb_enable), 0.0, 0.4, 0.5, 0.1);
    addSubModule(new ConstUnit("Imm", loc, 32, 1, isElastic), 0.0, 0.15, 1, 0.125);
    addSubModule(new ElasticEagerFork("Imm_fork", loc, 32, num_ALU_inputs, num_ALU_inputs, isElastic), 0.25, 0, 0.5, 0.1);
    addSubModule(new Multiplexer("muxA", loc, 2, size, isElastic), 0.0, 0.25, 0.25, 0.125);
    addSubModule(new Multiplexer("muxB", loc, 2, size, isElastic), 0.5, 0.25, 0.25, 0.125);
    addSubModule(new ElasticBufferFifo("buffOut", loc, buffer_depth, 32, eb_enable), 0.0, 0.4, 0.5, 0.1);

    addSubModule(new ElasticFPUnit("FuncUnit", loc, {
	  OpCode::FADD,
	//  OpCode::FMUL,
	  OpCode::FDIV,
	  OpCode::SQRT,}, size, II, latency, pred, isElastic), 0.5, 0.4, 0.5, 0.1);

    // ****************************************** CONFIGURATION BITS ************
    
    // Now, add the necessary configuration bits for the above elements
    if (eb_enable){
      addConfig("EBEnable", {"buffOut.enable"}, contexts, isElastic);
      addConfig("buffAEnable", {"buffA.enable"}, contexts, isElastic);
      addConfig("buffBEnable", {"buffB.enable"}, contexts, isElastic);
    }

    // Config bits for the ALU muxes, const fork, immediate value
    addConfig("ALUMuxAconfig", {"muxA.select"}, contexts, isElastic);
    addConfig("ALUMuxBconfig", {"muxB.select"}, contexts, isElastic);
    addConfig("ConstForkConfig", {"Imm_fork.enable_downstream"}, contexts, isElastic);

    // ***************** CONNECT THE PIECES TOGETHER ***********************
    
    // buffA and buffB to muxA and muxB
    connectPorts("buffA.data_out", "muxA.in0", isElastic);                                                         
    connectPorts("buffB.data_out", "muxB.in0", isElastic);                   

    // Imm to Imm_fork
    connectPorts("Imm.out", "Imm_fork.in", isElastic);

    // Imm_fork to muxA and muxB
    connectPorts("Imm_fork.out0", "muxA.in1", isElastic);
    connectPorts("Imm_fork.out1", "muxB.in1", isElastic);

    // muxA and muxB to Func Unit
    connectPorts("muxA.out", "FuncUnit.in_a", isElastic);
    connectPorts("muxB.out", "FuncUnit.in_b", isElastic);

    // and here is the messy part -- connect FuncUnit to ALU or FPALU wrapper
    // Omar no longer messy :) 
    connectPorts("FuncUnit.out", "buffOut.data_in", isElastic);

#if 1 // SECOND ALU
    // ****************************************** INSTANTIATE SUBMODULES OF THE PE ************                                    
    // Add the "guts" of the PE, e.g., the elastic buffers, constant reg, ALU, etc.        
    
    addSubModule(new ElasticBufferFifo("buffA_2", loc, buffer_depth, 32, eb_enable), 0.0, 0.4, 0.5, 0.1);
    addSubModule(new ElasticBufferFifo("buffB_2", loc, buffer_depth, 32, eb_enable), 0.0, 0.4, 0.5, 0.1);
    addSubModule(new ConstUnit("Imm_2", loc, 32, 1, isElastic), 0.0, 0.15, 1, 0.125);
    addSubModule(new ElasticEagerFork("Imm_fork_2", loc, 32, num_ALU_inputs, num_ALU_inputs, isElastic), 0.25, 0, 0.5, 0.1);
    addSubModule(new Multiplexer("muxA_2", loc, 2, size, isElastic), 0.0, 0.25, 0.25, 0.125);
    addSubModule(new Multiplexer("muxB_2", loc, 2, size, isElastic), 0.5, 0.25, 0.25, 0.125);
    addSubModule(new ElasticBufferFifo("buffOut_2", loc, buffer_depth, 32, eb_enable), 0.0, 0.4, 0.5, 0.1);

    addSubModule(new ElasticFPUnit("FuncUnit_2", loc, {
          OpCode::FMUL,
          OpCode::FDIV,
          OpCode::SQRT,}, size, II, latency, pred, isElastic), 0.5, 0.4, 0.5, 0.1);

    // ****************************************** CONFIGURATION BITS ************                                             

    // Now, add the necessary configuration bits for the above elements                                                       
    if (eb_enable){
      addConfig("EBEnable_2", {"buffOut_2.enable"}, contexts, isElastic);
      addConfig("buffAEnable_2", {"buffA_2.enable"}, contexts, isElastic);
      addConfig("buffBEnable_2", {"buffB_2.enable"}, contexts, isElastic);
    }

    // Config bits for the ALU muxes, const fork, immediate value                                                             
    addConfig("ALUMuxAconfig_2", {"muxA_2.select"}, contexts, isElastic);
    addConfig("ALUMuxBconfig_2", {"muxB_2.select"}, contexts, isElastic);
    addConfig("ConstForkConfig_2", {"Imm_fork_2.enable_downstream"}, contexts, isElastic);

     // ***************** CONNECT THE PIECES TOGETHER ***********************                                                  

    // buffA and buffB to muxA and muxB                                                                                       
    connectPorts("buffA_2.data_out", "muxA_2.in0", isElastic);
    connectPorts("buffB_2.data_out", "muxB_2.in0", isElastic);

    // Imm to Imm_fork                                                                                                        
    connectPorts("Imm_2.out", "Imm_fork_2.in", isElastic);
    
    // Imm_fork to muxA and muxB                                                                                              
    connectPorts("Imm_fork_2.out0", "muxA_2.in1", isElastic);
    connectPorts("Imm_fork_2.out1", "muxB_2.in1", isElastic);

    // muxA and muxB to Func Unit                                                                                             
    connectPorts("muxA_2.out", "FuncUnit_2.in_a", isElastic);
    connectPorts("muxB_2.out", "FuncUnit_2.in_b", isElastic);

    // and here is the messy part -- connect FuncUnit to ALU or FPALU wrapper                                                 
    // Omar no longer messy :)                                                                                                
    connectPorts("FuncUnit_2.out", "buffOut_2.data_in", isElastic);
    
#endif    

    // ***************** CREATE THE CROSSBAR (SWITCH BLOCK SB) ***********************                        
    
    // Crossbar (RIKEN paper calls this SB == switch block)
    int numCrossbarInputs = num_inPorts + 1; // Add 1 for ALU output (from FIFO)
    int numCrossbarOutputs = num_outPorts + num_ALU_inputs; // Add 2 for ALU in_a and in_b

#if 1 // SECOND ALU
    numCrossbarInputs += 1; // extra input for second ALU
    numCrossbarOutputs += num_ALU_inputs;  // extra outputs for second ALU
#endif
    
    addSubModule(new ElasticCrossbar("crossbar", loc, numCrossbarInputs, numCrossbarOutputs, size, type, pred, pred_type), 0.5, 0.375, 0.4, 0.4);
    
    // Input elastic registers in front of the crossbar inputs
    for (int i = 0; i < num_Possible_Ports; i++)
    {
      if (pe_conn_in & (0b00000001<<i))
      {
	      addSubModule(new Register("reg" + std::to_string(i), loc, size, isElastic), static_cast<double>(i)/num_Possible_Ports, 0.5, 1.0/num_Possible_Ports, 0.125);
      }
    }
    
    
    // Config bits attached to FF enables (FFs on the inputs to the crossbar)
    for(int i = 0; i < num_Possible_Ports; i++)
    {
	    if (pe_conn_in & (0b00000001<<i))
	    {
	        // Input registers
	        if (eb_enable){
	          addConfig("Reg" + std::to_string(i) + "config", {"reg" + std::to_string(i) + ".enable"}, contexts, isElastic);
	        }
	    }
    }

    // *************************** EXTERNAL PE PORTS AND CONNECTIVITY ************      
    
    // Add inputs ports to the PE tile                                                         
    for(int i = 0; i < num_Possible_Ports; i++)
    {
      if (pe_conn_in & (0b00000001<<i)) {
        addPort("in" + std::to_string(i), PORT_INPUT, size, isElastic);
      }
    }

    // Add output ports to the PE tile
    for (int i = 0; i < num_Possible_Ports; i++)
    {
      if (pe_conn_out & (0b00000001<<i)) {
	      addPort("out" + std::to_string(i), PORT_OUTPUT, size, isElastic);
      } 
    }
    
    // Connect the inputs of the PE to the registers on the crossbar inputs
    for(int i = 0; i < num_Possible_Ports; i++)
    {
	    if (pe_conn_in & (0b00000001<<i))
	    {   // InPorts to Registers
	      connectPorts("this.in" + std::to_string(i), "reg" + std::to_string(i) + ".in", isElastic);
	    }
    }

    // Connect the registers on the crossbar inputs to the crossbar itself
    int crossbarInputCounter = 0; // Needed because the crossbar and mux indices may not be the same
    for(int i = 0; i < num_Possible_Ports; i++)
    {
	    if (pe_conn_in & (0b00000001<<i))
	    {
	      connectPorts("reg" + std::to_string(i) + ".out", "crossbar.in" + std::to_string(crossbarInputCounter), isElastic);
	      crossbarInputCounter += 1; 
	    }
    }

    // Connect the crossbar outputs to the PE tile outputs
    int crossbarOutputCounter = 0; // Needed because the crossbar and mux indices may not be the same
    for(int i = 0; i < num_Possible_Ports; i++)
    {
	if (pe_conn_out & (0b00000001<<i))
	{
	    connectPorts("crossbar.out" + std::to_string(crossbarOutputCounter), "this.out" + std::to_string(i), isElastic);
	    crossbarOutputCounter += 1; 
	}
    }

    // ***************** CONNECT CROSSBAR TO COMPUTE UNIT(S) ***********************                        

    // buffOut to crossbar
    connectPorts("buffOut.data_out", "crossbar.in" + std::to_string(num_inPorts), isElastic);
    // Crossbar to buffA and buffB
    connectPorts("crossbar.out" + std::to_string(num_outPorts), "buffA.data_in", isElastic);
    connectPorts("crossbar.out" + std::to_string(num_outPorts + 1), "buffB.data_in", isElastic);

#if 1 // SECOND ALU
    // buffOut to crossbar                                                                                                         
    connectPorts("buffOut_2.data_out", "crossbar.in" + std::to_string(num_inPorts + 1), isElastic);
    // Crossbar to buffA and buffB                                                                                                 
    connectPorts("crossbar.out" + std::to_string(num_outPorts + 2), "buffA_2.data_in", isElastic);
    connectPorts("crossbar.out" + std::to_string(num_outPorts + 3), "buffB_2.data_in", isElastic);
#endif
 
}
