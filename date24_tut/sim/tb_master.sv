`timescale 1ns/100ps

module ram_simulation (
    input clock,
    input [31:0] addr0,
    input [31:0] data_in0,
    input w_rq0,
    output reg [31:0] data_out0,
    input [31:0] addr1,
    input [31:0] data_in1,
    input w_rq1,
    output reg [31:0] data_out1
);

    reg [31:0] storage [0:100];

    integer i;
    initial begin
        for (i = 0; i <= 100; i = i+1)
            storage[i] = i;
    end

    always @(posedge clock) begin
        if (w_rq0 == 1'b1)
            storage[addr0 >> 2] <= data_in0;
        if (addr0 >> 2 <= 100)
            data_out0 <= storage[addr0 >> 2];
        else
            data_out0 <= 0;
        if (w_rq1 == 1'b1)
            storage[addr1 >> 2] <= data_in1;
        if (addr1 >> 2 <= 100)
            data_out1 <= storage[addr1 >> 2];
        else
            data_out1 <= 0;
    end

endmodule

module tb_master #(
) (
);

	bit test_pass;

	logic DUT_clock;

	logic Config_Clock_en;
	logic Config_Clock;
	logic Config_Reset;
	assign Config_Clock = DUT_clock & Config_Clock_en;

	logic ConfigIn;
	logic ConfigOut;
	logic CGRA_Clock_en;
	logic CGRA_Reset;
	logic CGRA_Enable;

	logic configurator_enable;
	logic configurator_reset;
	logic configurator_done;

	wire [31:0] ext_io_top_0_in;
	wire [31:0] ext_io_top_0_out;
	wire [31:0] ext_io_top_1_in;
	wire [31:0] ext_io_top_1_out;

	wire [31:0] ext_io_right_0_in;
	wire [31:0] ext_io_right_0_out;
	wire [31:0] ext_io_right_1_in;
	wire [31:0] ext_io_right_1_out;

	wire [31:0] ext_io_bottom_0_in;
	wire [31:0] ext_io_bottom_0_out;
	wire [31:0] ext_io_bottom_1_in;
	wire [31:0] ext_io_bottom_1_out;

	wire [31:0] ram_data_in;
	wire [31:0] ram_data_out;
	wire [31:0] ram_address;
	wire ram_w_rq;

	wire [31:0] mem_0_mem_unit_addr_to_ram;
	wire [31:0] mem_0_mem_unit_data_in_to_ram;
	wire [31:0] mem_0_mem_unit_data_out_from_ram;
	wire mem_0_mem_unit_w_rq_to_ram;
	wire [31:0] mem_1_mem_unit_addr_to_ram;
	wire [31:0] mem_1_mem_unit_data_in_to_ram;
	wire [31:0] mem_1_mem_unit_data_out_from_ram;
	wire mem_1_mem_unit_w_rq_to_ram;

	cgra_U0 DUT(
		.Config_Clock(Config_Clock),
		.Config_Reset(Config_Reset),
		.ConfigIn(ConfigIn),
		.ConfigOut(ConfigOut),

		.CGRA_Clock(DUT_clock & CGRA_Clock_en),
		.CGRA_Reset(CGRA_Reset),
		.CGRA_Enable(CGRA_Enable),

		.io_top_0_IOPin_bidir_in(ext_io_top_0_in),
		.io_top_0_IOPin_bidir_out(ext_io_top_0_out),
		.io_top_1_IOPin_bidir_in(ext_io_top_1_in),
		.io_top_1_IOPin_bidir_out(ext_io_top_1_out),
		.io_bottom_0_IOPin_bidir_in(ext_io_bottom_0_in),
		.io_bottom_0_IOPin_bidir_out(ext_io_bottom_0_out),
		.io_bottom_1_IOPin_bidir_in(ext_io_bottom_1_in),
		.io_bottom_1_IOPin_bidir_out(ext_io_bottom_1_out),
		.io_right_0_IOPin_bidir_in(ext_io_right_0_in),
		.io_right_0_IOPin_bidir_out(ext_io_right_0_out),
		.io_right_1_IOPin_bidir_in(ext_io_right_1_in),
		.io_right_1_IOPin_bidir_out(ext_io_right_1_out),
		.mem_0_mem_unit_addr_to_ram(mem_0_mem_unit_addr_to_ram),
		.mem_0_mem_unit_data_in_to_ram(mem_0_mem_unit_data_in_to_ram),
		.mem_0_mem_unit_data_out_from_ram(mem_0_mem_unit_data_out_from_ram),
		.mem_0_mem_unit_w_rq_to_ram(mem_0_mem_unit_w_rq_to_ram),
		.mem_1_mem_unit_addr_to_ram(mem_1_mem_unit_addr_to_ram),
		.mem_1_mem_unit_data_in_to_ram(mem_1_mem_unit_data_in_to_ram),
		.mem_1_mem_unit_data_out_from_ram(mem_1_mem_unit_data_out_from_ram),
		.mem_1_mem_unit_w_rq_to_ram(mem_1_mem_unit_w_rq_to_ram)
	);

	CGRA_configurator configurator(
		.clock(Config_Clock),
		.enable(configurator_enable),
		.sync_reset(configurator_reset),

		.bitstream(ConfigIn),
		.done(configurator_done)
	);

	ram_simulation ram(
		.clock(CGRA_Clock_en & DUT_clock),

		.addr0(mem_0_mem_unit_addr_to_ram),
		.data_in0(mem_0_mem_unit_data_in_to_ram),
		.data_out0(mem_0_mem_unit_data_out_from_ram),
		.w_rq0(mem_0_mem_unit_w_rq_to_ram),

		.addr1(mem_1_mem_unit_addr_to_ram),
		.data_in1(mem_1_mem_unit_data_in_to_ram),
		.data_out1(mem_1_mem_unit_data_out_from_ram),
		.w_rq1(mem_1_mem_unit_w_rq_to_ram)
	);

	// if any bits are x or z, use the other one
	// goal is to not care what the mapping is
	logic [31:0] out_pin;
	always @(*) begin
		if (^ext_io_top_0_out === 1'bx)
			out_pin = ext_io_top_1_out;
		else
			out_pin = ext_io_top_0_out;
	end
	`define CHECK_OUT_PIN(expected) \
		if (!(out_pin === expected)) begin \
			test_pass = 0; \
			$error("expected out_pin to be %d, but it was %d",expected,out_pin); \
		end

	initial begin
		test_pass = 1;
		Config_Clock_en = 1;
		CGRA_Clock_en = 0;
		configurator_reset = 1;
		Config_Reset = 1;
		CGRA_Reset = 0;
		CGRA_Enable = 0;

		#0.8;
		DUT_clock = 0;

		#1;
		Config_Reset = 0;
		configurator_reset = 0;
		configurator_enable = 1;

		while (1) if (configurator_done) break; else #1;

		configurator_enable = 0;
		Config_Clock_en = 0;
		CGRA_Reset = 1;
		CGRA_Enable = 1;
		CGRA_Clock_en = 1;

		#1;
		CGRA_Reset = 0;

		#0.5;
		while (1) if (^out_pin === 1'bx) #1; else break;
		while (1) if (out_pin === 'd0) #1; else break;

		`CHECK_OUT_PIN(0)
		#1;
		`CHECK_OUT_PIN(1)
		#1;
		`CHECK_OUT_PIN(3)
		#1;
		`CHECK_OUT_PIN(6)

		#50;
		`CHECK_OUT_PIN(1431)

		if (test_pass)
			$finish;
		else begin
			$error("test failure");
			$stop;
		end
	end

	always
		#0.5 DUT_clock = !DUT_clock;

	always begin
		#0.9;
		if (!DUT_clock) begin
		end
		#0.1;
	end
endmodule
