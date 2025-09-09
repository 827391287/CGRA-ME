// Copyright 2018 Robert Balas <balasr@student.ethz.ch>
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

// Wrapper for a RI5CY testbench, containing RI5CY, Memory and stdout peripheral
// Contributor: Robert Balas <balasr@student.ethz.ch>

module tb_top
    #(parameter INSTR_RDATA_WIDTH = 128,
      parameter BOOT_ADDR = 'h180,
      parameter PULP_SECURE = 1,
      parameter A_EXTENSION = 1);

     const time CLK_PHASE_HI       = 0.5;
     const time CLK_PHASE_LO       = 0.5;
     const time CLK_PERIOD         = CLK_PHASE_HI + CLK_PHASE_LO;
     const time STIM_APPLICATION_DEL = CLK_PERIOD * 0.1;
     const time RESP_ACQUISITION_DEL = CLK_PERIOD * 0.9;
     const time RESET_DEL = STIM_APPLICATION_DEL;
     const int  RESET_WAIT_CYCLES  = 4;

    // clock and reset for tb
    logic                   clk_i   = 'b1;
    logic                   rst_ni = 'b0;

    // cycle counter
    int unsigned            cycle_cnt_q;

     // signals for ri5cy
    logic                   fetch_enable;

    // make the core start fetching instruction immediately
    assign fetch_enable = '1;
 
     logic Config_Clock_en;
     logic Config_Reset;
     logic CGRA_Clock_en;
     logic CGRA_Reset = 'b1;

     logic riscv_enable;
     
     logic configurator_enable;
     logic configurator_reset;
     logic configurator_done;

    initial begin
        if ($test$plusargs("vcd")) begin
            $dumpfile("riscy_tb.vcd");
            $dumpvars(0, tb_top);
        end
    end

    initial begin: load_prog
        automatic string firmware;
        automatic int prog_size = 6;

        if($value$plusargs("firmware=%s", firmware)) begin
            if($test$plusargs("verbose"))
                $display("[TESTBENCH] %t: loading firmware %0s ...",
                         $time, firmware);
            $readmemh(firmware, riscv_wrapper_i.ram_i.instruc_ram_i.mem);
	    //$readmemh(firmware, riscv_wrapper_i.ram_i.dp_ram_i.mem);

        end else begin
            $display("No firmware specified");
            $finish;
        end
    end
    
    // clock generation
    initial begin: clock_gen
        forever begin
            #CLK_PHASE_HI clk_i = 1'b0;
            #CLK_PHASE_LO clk_i = 1'b1;
        end
    end: clock_gen

    // set timing format
    initial begin: timing_format
        $timeformat(-9, 0, "ns", 9);
    end: timing_format

    // abort after n cycles, if we want to
    always_ff @(posedge clk_i, negedge rst_ni) begin
        automatic int maxcycles;
        if($value$plusargs("maxcycles=%d", maxcycles)) begin
            if (~rst_ni) begin
                cycle_cnt_q <= 0;
            end else begin
                cycle_cnt_q     <= cycle_cnt_q + 1;
                if (cycle_cnt_q >= maxcycles) begin
                    $fatal(2, "Simulation aborted due to maximum cycle limit");
                end
            end
        end
    end

	hybrid_wrapper
    	#(.INSTR_RDATA_WIDTH (INSTR_RDATA_WIDTH),
          .BOOT_ADDR (BOOT_ADDR),
          .PULP_SECURE (1),
          .A_EXTENSION (1))
	riscv_wrapper_i
        (.clk_i          ( clk_i          ),
        .rst_ni         ( rst_ni          ),
        .fetch_enable_i ( fetch_enable   ),
        .Config_Clock_en( Config_Clock_en),
        .Config_Reset   ( Config_Reset   ),
        .CGRA_Clock_en  ( CGRA_Clock_en  ),
        .CGRA_Reset     ( CGRA_Reset     ),
        .riscv_enable	 ( riscv_enable         ),
        .configurator_enable  ( configurator_enable),
        .configurator_reset  ( configurator_reset),
        .configurator_done  ( configurator_done)
        );

    
	initial begin
		Config_Clock_en = 1;
		CGRA_Clock_en = 0;
		configurator_reset = 1;
		Config_Reset = 1;
		CGRA_Reset = 0;
		riscv_enable = 0;
		rst_ni = 1'b1;
		CGRA_Reset = 1'b0;
		
		#CLK_PERIOD;
		Config_Reset = 0;
		configurator_reset = 0;
		configurator_enable = 1;
		
		while (1) if (configurator_done) break; else #CLK_PERIOD;

		configurator_enable = 0;
		Config_Clock_en = 0;
		CGRA_Reset = 1;
		rst_ni = 1'b0;
		CGRA_Clock_en = 1;

		#CLK_PERIOD;
		CGRA_Reset = 0;
		rst_ni = 1'b1;
		riscv_enable = 1;
	end  

endmodule
