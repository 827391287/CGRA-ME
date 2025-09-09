// Copyright 2015 ETH Zurich and University of Bologna.
// Copyright 2017 Embecosm Limited <www.embecosm.com>
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

module instruc_ram
    #(parameter ADDR_WIDTH = 8,
      parameter INSTR_RDATA_WIDTH = 128)
    (input logic                  clk_i,

     input logic                          en_a_i,
     input logic [ADDR_WIDTH-1:0]         addr_a_i,
     output logic [INSTR_RDATA_WIDTH-1:0] rdata_a_o
    );

    localparam bytes = 2**22;

    logic [7:0]                      mem[bytes];
    logic [ADDR_WIDTH-1:0]           addr_a_int;

    always_comb addr_a_int = {addr_a_i[ADDR_WIDTH-1:2], 2'b0};
    
    always @(posedge clk_i) begin
        for (int i = 0; i < INSTR_RDATA_WIDTH/8; i++) begin
            rdata_a_o[(i*8)+: 8] <= mem[addr_a_int +  i];
        end
    end
endmodule
