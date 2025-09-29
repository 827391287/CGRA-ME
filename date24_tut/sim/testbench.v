module CGRA_configurator(
    input      clock,
    input      enable,
    input      sync_reset,

    output reg bitstream,
    output reg done
);

    localparam TOTAL_NUM_BITS = 285;
	reg [0:TOTAL_NUM_BITS-1] storage = {
		1'b0,1'b0,1'b1, // crossbar::Mux5config
		1'b1,1'b1,1'b0, // crossbar::Mux4config
		1'bx,1'bx,1'bx, // crossbar::Mux3config
		1'bx,1'bx,1'bx, // crossbar::Mux2config
		1'b0,1'b0,1'b1, // crossbar::Mux1config
		1'bx,1'bx,1'bx, // crossbar::Mux0config
		1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0, // pe_c1_r1::ConstVal
		1'b1, // pe_c1_r1::RegBConfig
		1'b1, // pe_c1_r1::RegAConfig
		1'bx, // pe_c1_r1::Reg3config
		1'bx, // pe_c1_r1::Reg2config
		1'bx, // pe_c1_r1::Reg1config
		1'bx, // pe_c1_r1::Reg0config
		1'bx, // pe_c1_r1::RESConfig
		1'b0, // pe_c1_r1::Mux3config
		1'bx, // pe_c1_r1::Mux2config
		1'bx, // pe_c1_r1::Mux1config
		1'bx, // pe_c1_r1::Mux0config
		1'b0,1'b0,1'b0,1'b0, // pe_c1_r1::ALUconfig
		1'bx,1'bx,1'bx, // crossbar::Mux5config
		1'bx,1'bx,1'bx, // crossbar::Mux4config
		1'bx,1'bx,1'bx, // crossbar::Mux3config
		1'bx,1'bx,1'bx, // crossbar::Mux2config
		1'bx,1'bx,1'bx, // crossbar::Mux1config
		1'bx,1'bx,1'bx, // crossbar::Mux0config
		1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0, // pe_c1_r0::ConstVal
		1'bx, // pe_c1_r0::RegBConfig
		1'bx, // pe_c1_r0::RegAConfig
		1'bx, // pe_c1_r0::Reg3config
		1'bx, // pe_c1_r0::Reg2config
		1'bx, // pe_c1_r0::Reg1config
		1'bx, // pe_c1_r0::Reg0config
		1'bx, // pe_c1_r0::RESConfig
		1'bx, // pe_c1_r0::Mux3config
		1'bx, // pe_c1_r0::Mux2config
		1'bx, // pe_c1_r0::Mux1config
		1'bx, // pe_c1_r0::Mux0config
		1'bx,1'bx,1'bx,1'bx, // pe_c1_r0::ALUconfig
		1'b0,1'b1,1'b1, // crossbar::Mux5config
		1'b0,1'b0,1'b0, // crossbar::Mux4config
		1'b0,1'b0,1'b1, // crossbar::Mux3config
		1'bx,1'bx,1'bx, // crossbar::Mux2config
		1'b1,1'b1,1'b0, // crossbar::Mux1config
		1'bx,1'bx,1'bx, // crossbar::Mux0config
		1'b0,1'b0,1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0, // pe_c0_r1::ConstVal
		1'b1, // pe_c0_r1::RegBConfig
		1'b1, // pe_c0_r1::RegAConfig
		1'bx, // pe_c0_r1::Reg3config
		1'bx, // pe_c0_r1::Reg2config
		1'bx, // pe_c0_r1::Reg1config
		1'bx, // pe_c0_r1::Reg0config
		1'bx, // pe_c0_r1::RESConfig
		1'b0, // pe_c0_r1::Mux3config
		1'bx, // pe_c0_r1::Mux2config
		1'bx, // pe_c0_r1::Mux1config
		1'b0, // pe_c0_r1::Mux0config
		1'b0,1'b1,1'b0,1'b0, // pe_c0_r1::ALUconfig
		1'b0,1'b1,1'b1, // crossbar::Mux5config
		1'b0,1'b0,1'b1, // crossbar::Mux4config
		1'bx,1'bx,1'bx, // crossbar::Mux3config
		1'b0,1'b0,1'b1, // crossbar::Mux2config
		1'bx,1'bx,1'bx, // crossbar::Mux1config
		1'bx,1'bx,1'bx, // crossbar::Mux0config
		1'b1,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0, // pe_c0_r0::ConstVal
		1'b1, // pe_c0_r0::RegBConfig
		1'b1, // pe_c0_r0::RegAConfig
		1'bx, // pe_c0_r0::Reg3config
		1'bx, // pe_c0_r0::Reg2config
		1'bx, // pe_c0_r0::Reg1config
		1'bx, // pe_c0_r0::Reg0config
		1'bx, // pe_c0_r0::RESConfig
		1'bx, // pe_c0_r0::Mux3config
		1'bx, // pe_c0_r0::Mux2config
		1'bx, // pe_c0_r0::Mux1config
		1'bx, // pe_c0_r0::Mux0config
		1'b0,1'b0,1'b0,1'b0, // pe_c0_r0::ALUconfig
		1'b0, // mem_1::WriteRq
		1'bx, // mem_1::MuxData
		1'b1, // mem_1::MuxAddr
		1'b0, // mem_0::WriteRq
		1'bx, // mem_0::MuxData
		1'bx, // mem_0::MuxAddr
		1'bx, // io_top_1::RegOutConfig
		1'bx, // io_top_1::RegInConfig
		1'b0, // io_top_1::IOPinConfig
		1'bx, // io_top_0::RegOutConfig
		1'bx, // io_top_0::RegInConfig
		1'b0, // io_top_0::IOPinConfig
		1'bx, // io_right_1::RegOutConfig
		1'b1, // io_right_1::RegInConfig
		1'b1, // io_right_1::IOPinConfig
		1'bx, // io_right_0::RegOutConfig
		1'bx, // io_right_0::RegInConfig
		1'b0, // io_right_0::IOPinConfig
		1'bx, // io_bottom_1::RegOutConfig
		1'bx, // io_bottom_1::RegInConfig
		1'b0, // io_bottom_1::IOPinConfig
		1'bx, // io_bottom_0::RegOutConfig
		1'bx, // io_bottom_0::RegInConfig
		1'b0 // io_bottom_0::IOPinConfig
	};

	reg [31:0] next_pos;
	always @(posedge clock) begin
		if (sync_reset) begin
			next_pos <= 0;
			bitstream <= 1'b0;
			done <= 0;
		end else if (next_pos >= TOTAL_NUM_BITS) begin
			done <= 1;
			bitstream <= 1'b0;
		end else if (enable) begin
			bitstream <= storage[next_pos];
			next_pos <= next_pos + 1;
		end
	end
endmodule
