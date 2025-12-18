module hybrid_wrapper
	#(parameter INSTR_RDATA_WIDTH = 128,
	parameter BOOT_ADDR = 'h180,
	parameter PULP_SECURE = 1,
	parameter A_EXTENSION = 1)
	(input logic clk_i,
	input logic rst_ni,
	input logic fetch_enable_i,
	input logic Config_Clock_en,
	input logic Config_Reset,
	input logic CGRA_Clock_en,
	input logic CGRA_Reset,
	input logic riscv_enable,
	input logic configurator_enable,
	input logic configurator_reset,
	output logic configurator_done
	);

	logic riscv_rst_n;
	logic [31:0] counter;
	logic instr_req;
	logic instr_gnt;
	logic instr_rvalid;
	logic [31:0] instr_addr;
	logic [INSTR_RDATA_WIDTH-1:0] instr_rdata;
	logic data_req;
	logic data_gnt;
	logic data_rvalid;
	logic [31:0] data_addr;
	logic data_we;
	logic [3:0] data_be;
	logic [31:0] data_rdata;
	logic [31:0] data_wdata;
	logic [31:0] data_rdata_port;
	logic [31:0] data_rdata_ram;
	logic [5:0] data_atop;
	logic debug_req_i;
	logic sec_lvl_o;
	logic irq_ack;
	logic [0:4] irq_id;
	logic irq_sec;
	logic irq_software;
	logic irq_timer;
	logic irq_external;
	logic [14:0] irq_fast;
	logic irq_nmi;
	logic [31:0] irq_fastx;
	logic ConfigIn;
	logic ConfigOut;
	logic CGRA_Enable;
	logic core_busy_o;
	logic Config_Clock;
	logic CGRA_Reset_Control;
	logic [31:0] io_bottom_0_IOPin_bidir_in;
	logic [31:0] io_bottom_0_IOPin_bidir_out;
	logic [31:0] io_bottom_1_IOPin_bidir_in;
	logic [31:0] io_bottom_1_IOPin_bidir_out;
	logic [31:0] io_bottom_2_IOPin_bidir_in;
	logic [31:0] io_bottom_2_IOPin_bidir_out;
	logic [31:0] io_bottom_3_IOPin_bidir_in;
	logic [31:0] io_bottom_3_IOPin_bidir_out;
	logic [31:0] io_bottom_4_IOPin_bidir_in;
	logic [31:0] io_bottom_4_IOPin_bidir_out;
	logic [31:0] io_bottom_5_IOPin_bidir_in;
	logic [31:0] io_bottom_5_IOPin_bidir_out;
	logic [31:0] io_bottom_6_IOPin_bidir_in;
	logic [31:0] io_bottom_6_IOPin_bidir_out;
	logic [31:0] io_bottom_7_IOPin_bidir_in;
	logic [31:0] io_bottom_7_IOPin_bidir_out;
	logic [31:0] io_right_0_IOPin_bidir_in;
	logic [31:0] io_right_0_IOPin_bidir_out;
	logic [31:0] io_right_1_IOPin_bidir_in;
	logic [31:0] io_right_1_IOPin_bidir_out;
	logic [31:0] io_right_2_IOPin_bidir_in;
	logic [31:0] io_right_2_IOPin_bidir_out;
	logic [31:0] io_right_3_IOPin_bidir_in;
	logic [31:0] io_right_3_IOPin_bidir_out;
	logic [31:0] io_right_4_IOPin_bidir_in;
	logic [31:0] io_right_4_IOPin_bidir_out;
	logic [31:0] io_right_5_IOPin_bidir_in;
	logic [31:0] io_right_5_IOPin_bidir_out;
	logic [31:0] io_right_6_IOPin_bidir_in;
	logic [31:0] io_right_6_IOPin_bidir_out;
	logic [31:0] io_right_7_IOPin_bidir_in;
	logic [31:0] io_right_7_IOPin_bidir_out;
	logic [31:0] io_top_0_IOPin_bidir_in;
	logic [31:0] io_top_0_IOPin_bidir_out;
	logic [31:0] io_top_1_IOPin_bidir_in;
	logic [31:0] io_top_1_IOPin_bidir_out;
	logic [31:0] io_top_2_IOPin_bidir_in;
	logic [31:0] io_top_2_IOPin_bidir_out;
	logic [31:0] io_top_3_IOPin_bidir_in;
	logic [31:0] io_top_3_IOPin_bidir_out;
	logic [31:0] io_top_4_IOPin_bidir_in;
	logic [31:0] io_top_4_IOPin_bidir_out;
	logic [31:0] io_top_5_IOPin_bidir_in;
	logic [31:0] io_top_5_IOPin_bidir_out;
	logic [31:0] io_top_6_IOPin_bidir_in;
	logic [31:0] io_top_6_IOPin_bidir_out;
	logic [31:0] io_top_7_IOPin_bidir_in;
	logic [31:0] io_top_7_IOPin_bidir_out;
	logic [31:0] mem_0_mem_unit_addr_to_ram;
	logic [31:0] mem_0_mem_unit_data_in_to_ram;
	logic [31:0] mem_0_mem_unit_data_out_from_ram;
	logic mem_0_mem_unit_w_rq_to_ram;
	logic [31:0] mem_1_mem_unit_addr_to_ram;
	logic [31:0] mem_1_mem_unit_data_in_to_ram;
	logic [31:0] mem_1_mem_unit_data_out_from_ram;
	logic mem_1_mem_unit_w_rq_to_ram;
	logic [31:0] mem_2_mem_unit_addr_to_ram;
	logic [31:0] mem_2_mem_unit_data_in_to_ram;
	logic [31:0] mem_2_mem_unit_data_out_from_ram;
	logic mem_2_mem_unit_w_rq_to_ram;
	logic [31:0] mem_3_mem_unit_addr_to_ram;
	logic [31:0] mem_3_mem_unit_data_in_to_ram;
	logic [31:0] mem_3_mem_unit_data_out_from_ram;
	logic mem_3_mem_unit_w_rq_to_ram;
	logic [31:0] mem_4_mem_unit_addr_to_ram;
	logic [31:0] mem_4_mem_unit_data_in_to_ram;
	logic [31:0] mem_4_mem_unit_data_out_from_ram;
	logic mem_4_mem_unit_w_rq_to_ram;
	logic [31:0] mem_5_mem_unit_addr_to_ram;
	logic [31:0] mem_5_mem_unit_data_in_to_ram;
	logic [31:0] mem_5_mem_unit_data_out_from_ram;
	logic mem_5_mem_unit_w_rq_to_ram;
	logic [31:0] mem_6_mem_unit_addr_to_ram;
	logic [31:0] mem_6_mem_unit_data_in_to_ram;
	logic [31:0] mem_6_mem_unit_data_out_from_ram;
	logic mem_6_mem_unit_w_rq_to_ram;
	logic [31:0] mem_7_mem_unit_addr_to_ram;
	logic [31:0] mem_7_mem_unit_data_in_to_ram;
	logic [31:0] mem_7_mem_unit_data_out_from_ram;
	logic mem_7_mem_unit_w_rq_to_ram;
	logic [31:0] cgra_data_addr[7:0];
	logic [255:0] cgra_data_rdata;
	logic [31:0] cgra_data_wdata[7:0];
	logic [7:0] cgra_we_i;
	logic [31:0] cgra_io[23:0];
	logic [5:0] select_port;
	logic [31:0] end_port;
	logic use_counter;
	logic end_cgra;
	assign Config_Clock = clk_i & Config_Clock_en;
	assign riscv_rst_n = rst_ni & riscv_enable;
	assign cgra_we_i = {
		mem_7_mem_unit_w_rq_to_ram,
		mem_6_mem_unit_w_rq_to_ram,
		mem_5_mem_unit_w_rq_to_ram,
		mem_4_mem_unit_w_rq_to_ram,
		mem_3_mem_unit_w_rq_to_ram,
		mem_2_mem_unit_w_rq_to_ram,
		mem_1_mem_unit_w_rq_to_ram,
		mem_0_mem_unit_w_rq_to_ram};

	assign end_port = cgra_io[select_port];
	assign cgra_io = {
		io_top_7_IOPin_bidir_out,
		io_top_6_IOPin_bidir_out,
		io_top_5_IOPin_bidir_out,
		io_top_4_IOPin_bidir_out,
		io_top_3_IOPin_bidir_out,
		io_top_2_IOPin_bidir_out,
		io_top_1_IOPin_bidir_out,
		io_top_0_IOPin_bidir_out,
		io_right_7_IOPin_bidir_out,
		io_right_6_IOPin_bidir_out,
		io_right_5_IOPin_bidir_out,
		io_right_4_IOPin_bidir_out,
		io_right_3_IOPin_bidir_out,
		io_right_2_IOPin_bidir_out,
		io_right_1_IOPin_bidir_out,
		io_right_0_IOPin_bidir_out,
		io_bottom_7_IOPin_bidir_out,
		io_bottom_6_IOPin_bidir_out,
		io_bottom_5_IOPin_bidir_out,
		io_bottom_4_IOPin_bidir_out,
		io_bottom_3_IOPin_bidir_out,
		io_bottom_2_IOPin_bidir_out,
		io_bottom_1_IOPin_bidir_out,
		io_bottom_0_IOPin_bidir_out};

	always_comb begin
		cgra_data_addr = {
		mem_7_mem_unit_addr_to_ram,
		mem_6_mem_unit_addr_to_ram,
		mem_5_mem_unit_addr_to_ram,
		mem_4_mem_unit_addr_to_ram,
		mem_3_mem_unit_addr_to_ram,
		mem_2_mem_unit_addr_to_ram,
		mem_1_mem_unit_addr_to_ram,
		mem_0_mem_unit_addr_to_ram};
		{
		mem_7_mem_unit_data_out_from_ram,
		mem_6_mem_unit_data_out_from_ram,
		mem_5_mem_unit_data_out_from_ram,
		mem_4_mem_unit_data_out_from_ram,
		mem_3_mem_unit_data_out_from_ram,
		mem_2_mem_unit_data_out_from_ram,
		mem_1_mem_unit_data_out_from_ram,
		mem_0_mem_unit_data_out_from_ram} = cgra_data_rdata;
		cgra_data_wdata = {
		mem_7_mem_unit_data_in_to_ram,
		mem_6_mem_unit_data_in_to_ram,
		mem_5_mem_unit_data_in_to_ram,
		mem_4_mem_unit_data_in_to_ram,
		mem_3_mem_unit_data_in_to_ram,
		mem_2_mem_unit_data_in_to_ram,
		mem_1_mem_unit_data_in_to_ram,
		mem_0_mem_unit_data_in_to_ram};
	end
	assign debug_req_i = 1'b0;
	assign irq_sec = 1'b0;

	riscv_core
		#(.INSTR_RDATA_WIDTH (INSTR_RDATA_WIDTH),
		.PULP_SECURE(PULP_SECURE),
		.A_EXTENSION(A_EXTENSION),
		.FPU(0))
	riscv_core_i (
		.clk_i(clk_i),
		.rst_ni(riscv_rst_n),
		.clock_en_i('1),
		.test_en_i('0),
		.boot_addr_i(BOOT_ADDR),
		.core_id_i(4'h0),
		.cluster_id_i(6'h0),
		.instr_addr_o(instr_addr),
		.instr_req_o(instr_req),
		.instr_rdata_i(instr_rdata),
		.instr_gnt_i(instr_gnt),
		.instr_rvalid_i(instr_rvalid),
		.data_addr_o(data_addr),
		.data_wdata_o( data_wdata),
		.data_we_o( data_we),
		.data_req_o(data_req),
		.data_be_o(data_be),
		.data_rdata_i(data_rdata),
		.data_gnt_i( data_gnt),
		.data_rvalid_i( data_rvalid),
		.data_atop_o( data_atop),
		.apu_master_req_o(),
		.apu_master_ready_o(),
		.apu_master_gnt_i(),
		.apu_master_operands_o(),
		.apu_master_op_o(),
		.apu_master_type_o(),
		.apu_master_flags_o(),
		.apu_master_valid_i(),
		.apu_master_result_i (),
		.apu_master_flags_i(),
		.irq_software_i(irq_software),
		.irq_timer_i(irq_timer),
		.irq_external_i(irq_external),
		.irq_fast_i(irq_fast),
		.irq_nmi_i(irq_nmi),
		.irq_fastx_i(irq_fastx),
		.irq_ack_o(irq_ack),
		.irq_id_o(irq_id),
		.irq_sec_i(irq_sec),
		.sec_lvl_o(sec_lvl_o),
		.debug_req_i(debug_req_i),
		.fetch_enable_i(fetch_enable_i),
		.core_busy_o(core_busy_o),
		.ext_perf_counters_i(),
		.fregfile_disable_i(1'b0));

	always @(posedge clk_i or negedge rst_ni) begin
		if (~rst_ni) begin
			io_bottom_0_IOPin_bidir_in <= '0;
			io_bottom_1_IOPin_bidir_in <= '0;
			io_bottom_2_IOPin_bidir_in <= '0;
			io_bottom_3_IOPin_bidir_in <= '0;
			io_bottom_4_IOPin_bidir_in <= '0;
			io_bottom_5_IOPin_bidir_in <= '0;
			io_bottom_6_IOPin_bidir_in <= '0;
			io_bottom_7_IOPin_bidir_in <= '0;
			io_right_0_IOPin_bidir_in <= '0;
			io_right_1_IOPin_bidir_in <= '0;
			io_right_2_IOPin_bidir_in <= '0;
			io_right_3_IOPin_bidir_in <= '0;
			io_right_4_IOPin_bidir_in <= '0;
			io_right_5_IOPin_bidir_in <= '0;
			io_right_6_IOPin_bidir_in <= '0;
			io_right_7_IOPin_bidir_in <= '0;
			io_top_0_IOPin_bidir_in <= '0;
			io_top_1_IOPin_bidir_in <= '0;
			io_top_2_IOPin_bidir_in <= '0;
			io_top_3_IOPin_bidir_in <= '0;
			io_top_4_IOPin_bidir_in <= '0;
			io_top_5_IOPin_bidir_in <= '0;
			io_top_6_IOPin_bidir_in <= '0;
			io_top_7_IOPin_bidir_in <= '0;
			counter <= '0;
			CGRA_Enable <= 1'b0;
			data_rdata_port <= '0;
			CGRA_Reset_Control <= '0;
			select_port <= '0;
			use_counter <= '1;
			end_cgra <= '0;
		end else begin
			if (data_req) begin
				if (data_addr[22]) begin
					if (data_we) begin
						if (data_addr[9:2] == 0) begin
							io_bottom_0_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 1) begin
							io_bottom_1_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 2) begin
							io_bottom_2_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 3) begin
							io_bottom_3_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 4) begin
							io_bottom_4_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 5) begin
							io_bottom_5_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 6) begin
							io_bottom_6_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 7) begin
							io_bottom_7_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 8) begin
							io_right_0_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 9) begin
							io_right_1_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 10) begin
							io_right_2_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 11) begin
							io_right_3_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 12) begin
							io_right_4_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 13) begin
							io_right_5_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 14) begin
							io_right_6_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 15) begin
							io_right_7_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 16) begin
							io_top_0_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 17) begin
							io_top_1_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 18) begin
							io_top_2_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 19) begin
							io_top_3_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 20) begin
							io_top_4_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 21) begin
							io_top_5_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 22) begin
							io_top_6_IOPin_bidir_in <= data_wdata;
						end else if (data_addr[9:2] == 23) begin
							io_top_7_IOPin_bidir_in <= data_wdata;
						end else if(data_addr[9:2] == 24) begin
							counter <= data_wdata;
							CGRA_Enable <= 1'b1;
							use_counter <= 1'b1;
						end else if(data_addr[9:2] == 25) begin
							CGRA_Enable <= 1'b1;
							use_counter <= 1'b0;
						end else if(data_addr[9:2] == 26) begin
							CGRA_Reset_Control <= data_wdata;
						end else if(data_addr[9:2] == 27) begin
							select_port <= data_wdata;
						end
					end else begin
						if (data_addr[9:2] == 0) begin
							data_rdata_port <= io_bottom_0_IOPin_bidir_out;
						end else if (data_addr[9:2] == 1) begin
							data_rdata_port <= io_bottom_1_IOPin_bidir_out;
						end else if (data_addr[9:2] == 2) begin
							data_rdata_port <= io_bottom_2_IOPin_bidir_out;
						end else if (data_addr[9:2] == 3) begin
							data_rdata_port <= io_bottom_3_IOPin_bidir_out;
						end else if (data_addr[9:2] == 4) begin
							data_rdata_port <= io_bottom_4_IOPin_bidir_out;
						end else if (data_addr[9:2] == 5) begin
							data_rdata_port <= io_bottom_5_IOPin_bidir_out;
						end else if (data_addr[9:2] == 6) begin
							data_rdata_port <= io_bottom_6_IOPin_bidir_out;
						end else if (data_addr[9:2] == 7) begin
							data_rdata_port <= io_bottom_7_IOPin_bidir_out;
						end else if (data_addr[9:2] == 8) begin
							data_rdata_port <= io_right_0_IOPin_bidir_out;
						end else if (data_addr[9:2] == 9) begin
							data_rdata_port <= io_right_1_IOPin_bidir_out;
						end else if (data_addr[9:2] == 10) begin
							data_rdata_port <= io_right_2_IOPin_bidir_out;
						end else if (data_addr[9:2] == 11) begin
							data_rdata_port <= io_right_3_IOPin_bidir_out;
						end else if (data_addr[9:2] == 12) begin
							data_rdata_port <= io_right_4_IOPin_bidir_out;
						end else if (data_addr[9:2] == 13) begin
							data_rdata_port <= io_right_5_IOPin_bidir_out;
						end else if (data_addr[9:2] == 14) begin
							data_rdata_port <= io_right_6_IOPin_bidir_out;
						end else if (data_addr[9:2] == 15) begin
							data_rdata_port <= io_right_7_IOPin_bidir_out;
						end else if (data_addr[9:2] == 16) begin
							data_rdata_port <= io_top_0_IOPin_bidir_out;
						end else if (data_addr[9:2] == 17) begin
							data_rdata_port <= io_top_1_IOPin_bidir_out;
						end else if (data_addr[9:2] == 18) begin
							data_rdata_port <= io_top_2_IOPin_bidir_out;
						end else if (data_addr[9:2] == 19) begin
							data_rdata_port <= io_top_3_IOPin_bidir_out;
						end else if (data_addr[9:2] == 20) begin
							data_rdata_port <= io_top_4_IOPin_bidir_out;
						end else if (data_addr[9:2] == 21) begin
							data_rdata_port <= io_top_5_IOPin_bidir_out;
						end else if (data_addr[9:2] == 22) begin
							data_rdata_port <= io_top_6_IOPin_bidir_out;
						end else if (data_addr[9:2] == 23) begin
							data_rdata_port <= io_top_7_IOPin_bidir_out;
						end else if (data_addr[9:2] == 24) begin
							if (counter <= 3) begin
								data_rdata_port <= 1;
							end else begin
								data_rdata_port <= 0;
							end
						end else if (data_addr[9:2] == 25) begin
							if (end_cgra == 1) begin
								data_rdata_port <= 1;
							end else begin
								data_rdata_port <= 0;
							end
						end
					end
				end
			end
			if (use_counter) begin
				if (CGRA_Enable) begin
					if (counter == 0) begin
						CGRA_Enable <= 1'b0;
					end else begin
						counter <= counter - 1;
					end
				end
			end else begin
				if (CGRA_Enable) begin
					if (end_port == 1) begin
						CGRA_Enable <= 1'b0;
						end_cgra <= 1'b1;
					end
				end
			end
		end
	end

	always_comb begin
		if(data_addr < 2 ** 22) begin
			data_rdata = data_rdata_ram;
		end else begin
			data_rdata = data_rdata_port;
		end
	end

	cgra_U0 cgra_i(
		.Config_Clock(Config_Clock),
		.Config_Reset(Config_Reset),
		.ConfigIn(ConfigIn),
		.ConfigOut(ConfigOut),
		.CGRA_Clock(clk_i & CGRA_Clock_en),
		.CGRA_Enable(CGRA_Enable),
		.CGRA_Reset(CGRA_Reset | CGRA_Reset_Control),
		.io_bottom_0_IOPin_bidir_in(io_bottom_0_IOPin_bidir_in),
		.io_bottom_0_IOPin_bidir_out(io_bottom_0_IOPin_bidir_out),
		.io_bottom_1_IOPin_bidir_in(io_bottom_1_IOPin_bidir_in),
		.io_bottom_1_IOPin_bidir_out(io_bottom_1_IOPin_bidir_out),
		.io_bottom_2_IOPin_bidir_in(io_bottom_2_IOPin_bidir_in),
		.io_bottom_2_IOPin_bidir_out(io_bottom_2_IOPin_bidir_out),
		.io_bottom_3_IOPin_bidir_in(io_bottom_3_IOPin_bidir_in),
		.io_bottom_3_IOPin_bidir_out(io_bottom_3_IOPin_bidir_out),
		.io_bottom_4_IOPin_bidir_in(io_bottom_4_IOPin_bidir_in),
		.io_bottom_4_IOPin_bidir_out(io_bottom_4_IOPin_bidir_out),
		.io_bottom_5_IOPin_bidir_in(io_bottom_5_IOPin_bidir_in),
		.io_bottom_5_IOPin_bidir_out(io_bottom_5_IOPin_bidir_out),
		.io_bottom_6_IOPin_bidir_in(io_bottom_6_IOPin_bidir_in),
		.io_bottom_6_IOPin_bidir_out(io_bottom_6_IOPin_bidir_out),
		.io_bottom_7_IOPin_bidir_in(io_bottom_7_IOPin_bidir_in),
		.io_bottom_7_IOPin_bidir_out(io_bottom_7_IOPin_bidir_out),
		.io_right_0_IOPin_bidir_in(io_right_0_IOPin_bidir_in),
		.io_right_0_IOPin_bidir_out(io_right_0_IOPin_bidir_out),
		.io_right_1_IOPin_bidir_in(io_right_1_IOPin_bidir_in),
		.io_right_1_IOPin_bidir_out(io_right_1_IOPin_bidir_out),
		.io_right_2_IOPin_bidir_in(io_right_2_IOPin_bidir_in),
		.io_right_2_IOPin_bidir_out(io_right_2_IOPin_bidir_out),
		.io_right_3_IOPin_bidir_in(io_right_3_IOPin_bidir_in),
		.io_right_3_IOPin_bidir_out(io_right_3_IOPin_bidir_out),
		.io_right_4_IOPin_bidir_in(io_right_4_IOPin_bidir_in),
		.io_right_4_IOPin_bidir_out(io_right_4_IOPin_bidir_out),
		.io_right_5_IOPin_bidir_in(io_right_5_IOPin_bidir_in),
		.io_right_5_IOPin_bidir_out(io_right_5_IOPin_bidir_out),
		.io_right_6_IOPin_bidir_in(io_right_6_IOPin_bidir_in),
		.io_right_6_IOPin_bidir_out(io_right_6_IOPin_bidir_out),
		.io_right_7_IOPin_bidir_in(io_right_7_IOPin_bidir_in),
		.io_right_7_IOPin_bidir_out(io_right_7_IOPin_bidir_out),
		.io_top_0_IOPin_bidir_in(io_top_0_IOPin_bidir_in),
		.io_top_0_IOPin_bidir_out(io_top_0_IOPin_bidir_out),
		.io_top_1_IOPin_bidir_in(io_top_1_IOPin_bidir_in),
		.io_top_1_IOPin_bidir_out(io_top_1_IOPin_bidir_out),
		.io_top_2_IOPin_bidir_in(io_top_2_IOPin_bidir_in),
		.io_top_2_IOPin_bidir_out(io_top_2_IOPin_bidir_out),
		.io_top_3_IOPin_bidir_in(io_top_3_IOPin_bidir_in),
		.io_top_3_IOPin_bidir_out(io_top_3_IOPin_bidir_out),
		.io_top_4_IOPin_bidir_in(io_top_4_IOPin_bidir_in),
		.io_top_4_IOPin_bidir_out(io_top_4_IOPin_bidir_out),
		.io_top_5_IOPin_bidir_in(io_top_5_IOPin_bidir_in),
		.io_top_5_IOPin_bidir_out(io_top_5_IOPin_bidir_out),
		.io_top_6_IOPin_bidir_in(io_top_6_IOPin_bidir_in),
		.io_top_6_IOPin_bidir_out(io_top_6_IOPin_bidir_out),
		.io_top_7_IOPin_bidir_in(io_top_7_IOPin_bidir_in),
		.io_top_7_IOPin_bidir_out(io_top_7_IOPin_bidir_out),
		.mem_0_mem_unit_addr_to_ram(mem_0_mem_unit_addr_to_ram),
		.mem_0_mem_unit_data_in_to_ram(mem_0_mem_unit_data_in_to_ram),
		.mem_0_mem_unit_data_out_from_ram(mem_0_mem_unit_data_out_from_ram),
		.mem_0_mem_unit_w_rq_to_ram(mem_0_mem_unit_w_rq_to_ram),
		.mem_1_mem_unit_addr_to_ram(mem_1_mem_unit_addr_to_ram),
		.mem_1_mem_unit_data_in_to_ram(mem_1_mem_unit_data_in_to_ram),
		.mem_1_mem_unit_data_out_from_ram(mem_1_mem_unit_data_out_from_ram),
		.mem_1_mem_unit_w_rq_to_ram(mem_1_mem_unit_w_rq_to_ram),
		.mem_2_mem_unit_addr_to_ram(mem_2_mem_unit_addr_to_ram),
		.mem_2_mem_unit_data_in_to_ram(mem_2_mem_unit_data_in_to_ram),
		.mem_2_mem_unit_data_out_from_ram(mem_2_mem_unit_data_out_from_ram),
		.mem_2_mem_unit_w_rq_to_ram(mem_2_mem_unit_w_rq_to_ram),
		.mem_3_mem_unit_addr_to_ram(mem_3_mem_unit_addr_to_ram),
		.mem_3_mem_unit_data_in_to_ram(mem_3_mem_unit_data_in_to_ram),
		.mem_3_mem_unit_data_out_from_ram(mem_3_mem_unit_data_out_from_ram),
		.mem_3_mem_unit_w_rq_to_ram(mem_3_mem_unit_w_rq_to_ram),
		.mem_4_mem_unit_addr_to_ram(mem_4_mem_unit_addr_to_ram),
		.mem_4_mem_unit_data_in_to_ram(mem_4_mem_unit_data_in_to_ram),
		.mem_4_mem_unit_data_out_from_ram(mem_4_mem_unit_data_out_from_ram),
		.mem_4_mem_unit_w_rq_to_ram(mem_4_mem_unit_w_rq_to_ram),
		.mem_5_mem_unit_addr_to_ram(mem_5_mem_unit_addr_to_ram),
		.mem_5_mem_unit_data_in_to_ram(mem_5_mem_unit_data_in_to_ram),
		.mem_5_mem_unit_data_out_from_ram(mem_5_mem_unit_data_out_from_ram),
		.mem_5_mem_unit_w_rq_to_ram(mem_5_mem_unit_w_rq_to_ram),
		.mem_6_mem_unit_addr_to_ram(mem_6_mem_unit_addr_to_ram),
		.mem_6_mem_unit_data_in_to_ram(mem_6_mem_unit_data_in_to_ram),
		.mem_6_mem_unit_data_out_from_ram(mem_6_mem_unit_data_out_from_ram),
		.mem_6_mem_unit_w_rq_to_ram(mem_6_mem_unit_w_rq_to_ram),
		.mem_7_mem_unit_addr_to_ram(mem_7_mem_unit_addr_to_ram),
		.mem_7_mem_unit_data_in_to_ram(mem_7_mem_unit_data_in_to_ram),
		.mem_7_mem_unit_data_out_from_ram(mem_7_mem_unit_data_out_from_ram),
		.mem_7_mem_unit_w_rq_to_ram(mem_7_mem_unit_w_rq_to_ram)
		);

	CGRA_configurator configurator(
		.clock(Config_Clock),
		.enable(configurator_enable),
		.sync_reset(configurator_reset),
		.bitstream(ConfigIn),
		.done(configurator_done));

	mm_ram
		#(.RAM_ADDR_WIDTH (22),
		.INSTR_RDATA_WIDTH (INSTR_RDATA_WIDTH))
		ram_i(
		.clk_i(clk_i),
		.rst_ni(riscv_rst_n),
		.CGRA_Reset(CGRA_Reset | CGRA_Reset_Control),
		.instr_req_i(instr_req),
		.instr_addr_i   (instr_addr),
		.instr_rdata_o(instr_rdata),
		.instr_rvalid_o(instr_rvalid),
		.instr_gnt_o(instr_gnt),
		.riscv_req_i(data_req),
		.riscv_addr_i(data_addr),
		.riscv_we_i(data_we),
		.riscv_be_i(data_be ),
		.riscv_wdata_i(data_wdata),
		.riscv_rdata_o( data_rdata_ram),
		.riscv_rvalid_o  ( data_rvalid),
		.riscv_gnt_o( data_gnt),
		.cgra_enable( CGRA_Enable),
		.cgra_data_addr ( cgra_data_addr),
		.cgra_data_wdata( cgra_data_wdata),
		.cgra_data_rdata( cgra_data_rdata),
		.cgra_we_i(cgra_we_i),
		.irq_id_i( irq_id),
		.irq_ack_i( irq_ack  ),
		.irq_software_o ( irq_software),
		.irq_timer_o ( irq_timer),
		.irq_external_o ( irq_external),
		.irq_fast_o( irq_fast ),
		.irq_nmi_o( irq_nmi  ),
		.irq_fastx_o( irq_fastx));
endmodule
module mm_ram
	#(parameter RAM_ADDR_WIDTH = 16,
	parameter INSTR_RDATA_WIDTH = 128)
	(input logic clk_i,
	input logic rst_ni,
	input logic	CGRA_Reset,
	input logic instr_req_i,
	input logic [31:0] instr_addr_i,
	output logic [INSTR_RDATA_WIDTH-1:0] instr_rdata_o,
	output logic instr_rvalid_o,
	output logic instr_gnt_o,
	input logic riscv_req_i,
	input logic [31:0] riscv_addr_i,
	input logic riscv_we_i,
	input logic [3:0]  riscv_be_i,
	input logic [31:0] riscv_wdata_i,
	output logic [31:0] riscv_rdata_o,
	output logic riscv_rvalid_o,
	output logic riscv_gnt_o,
	input logic cgra_enable,
	input logic [31:0] cgra_data_addr[7:0],
	input logic [31:0] cgra_data_wdata[7:0],
	output logic [255:0] cgra_data_rdata,
	input logic [7:0] cgra_we_i,
	input logic [4:0] irq_id_i,
	input logic irq_ack_i,
	output logic irq_software_o,
	output logic irq_timer_o,
	output logic irq_external_o,
	output logic [14:0] irq_fast_o,
	output logic irq_nmi_o,
	output logic [31:0] irq_fastx_o);
	localparam int TIMER_IRQ_ID = 7;
	localparam int INSTRUC_ADDR_WIDTH = 22;
	localparam int BANK_ADDR_WIDTH = RAM_ADDR_WIDTH - 3;
	enum logic [1:0]{RAM, MM, RND_STALL, ERR} select_rdata_d, select_rdata_q;
	enum logic {T_RAM, T_PER} transaction;
	enum logic [1:0] {IDLE, PERIPHEARL_VALID, WAIT_RAM_GNT, WAIT_RAM_VALID} state_valid_n, state_valid_q;
	logic [31:0] data_addr_aligned;
	logic data_rvalid_q;
	logic instr_rvalid_q;
	logic [INSTR_RDATA_WIDTH-1:0] core_instr_rdata;
	logic [31:0] core_data_rdata;
	logic [7:0] ram_bank_enable;
	logic [RAM_ADDR_WIDTH-1:0] ram_data_addr;
	logic [31:0] ram_data_wdata;
	logic [31:0] ram_data_rdata;
	logic ram_data_we;
	logic [3:0]  ram_data_be;
	logic ram_data_gnt;
	logic ram_data_valid;
	logic data_req_dec;
	logic [31:0] data_wdata_dec;
	logic [RAM_ADDR_WIDTH-1:0] data_addr_dec;
	logic data_we_dec;
	logic [3:0]  data_be_dec;
	logic [RAM_ADDR_WIDTH-1:0] data_addr_dec_q;
	logic [INSTR_RDATA_WIDTH-1:0] ram_instr_rdata;
	logic ram_instr_req;
	logic [INSTRUC_ADDR_WIDTH-1:0] ram_instr_addr;
	logic ram_instr_gnt;
	logic ram_instr_valid;
	logic [31:0] timer_irq_mask_q;
	logic [31:0] timer_cnt_q;
	logic irq_timer_q;
	logic timer_reg_valid;
	logic timer_val_valid;
	logic [31:0] timer_wdata;
	logic [31:0] ram_data_rdata_tmp[0:3];
	logic [7:0] cgra_bank_enable;
	typedef struct packed {
		logic irq_software;
		logic irq_timer;
		logic irq_external;
		logic [14:0] irq_fast;
		logic irq_nmi;
		logic [31:0] irq_fastx;
	} Interrupts_tb_t;
	Interrupts_tb_t irq_rnd_lines;
	always_comb data_addr_aligned = {riscv_addr_i[31:2], 2'b0} ;
	always_comb begin
		data_req_dec = '0;
		data_addr_dec = '0;
		data_wdata_dec = '0;
		data_we_dec = '0;
		data_be_dec = '0;
		timer_wdata = '0;
		timer_reg_valid = '0;
		timer_val_valid = '0;
		select_rdata_d = RAM;
		transaction  = T_PER;
		if (riscv_req_i)  begin
			if (riscv_we_i)  begin
				if (riscv_addr_i < 2 ** RAM_ADDR_WIDTH)  begin
					data_req_dec = riscv_req_i;
					data_addr_dec = riscv_addr_i[RAM_ADDR_WIDTH-1:0];
					data_wdata_dec = riscv_wdata_i;
					data_we_dec = riscv_we_i;
					data_be_dec = riscv_be_i;
					transaction = T_RAM;
				end else  begin
					data_req_dec = 0;
					if (riscv_addr_i == 32'h1500_0000)  begin
						timer_wdata = riscv_wdata_i;
						timer_reg_valid = '1;
					end else if (riscv_addr_i == 32'h1500_0004)  begin
						timer_wdata = riscv_wdata_i;
						timer_val_valid = '1;
					end
				end
			end else  begin
				if (riscv_addr_i < 2 ** RAM_ADDR_WIDTH)  begin
					select_rdata_d = RAM;
					data_req_dec = riscv_req_i;
					data_addr_dec = riscv_addr_i[RAM_ADDR_WIDTH-1:0];
					data_wdata_dec = riscv_wdata_i;
					data_we_dec = riscv_we_i;
					data_be_dec = riscv_be_i;
					transaction = T_RAM;
				end else
					select_rdata_d = ERR;
			end
		end
	end
	always_comb begin: read_mux
		riscv_rdata_o = '0;
		if(select_rdata_q == RAM) begin
			riscv_rdata_o = core_data_rdata;
		end
	end
	always_ff @(posedge clk_i, negedge rst_ni) begin: tb_timer
		if(~rst_ni) begin
			timer_irq_mask_q <= '0;
			timer_cnt_q <= '0;
			irq_timer_q <= '0;
		end else begin
			if(timer_reg_valid) begin
				timer_irq_mask_q <= timer_wdata;
			end else if(timer_val_valid) begin
				timer_cnt_q <= timer_wdata;
			end else begin
				if(timer_cnt_q > 0)
					timer_cnt_q <= timer_cnt_q - 1;
				if(timer_cnt_q == 1)
					irq_timer_q <= 1'b1 && timer_irq_mask_q[TIMER_IRQ_ID];
				if(irq_ack_i == 1'b1 && irq_id_i == TIMER_IRQ_ID)
					irq_timer_q <= '0;
			end
		end
	end
	dp_ram
		#(.ADDR_WIDTH (RAM_ADDR_WIDTH), .BANK_ADDR_WIDTH(BANK_ADDR_WIDTH))
	dp_ram_0
		(
		.clk_i ( clk_i ),
		.rst_ni ( rst_ni ),
		.rst_CGRA  ( CGRA_Reset ),
		.en_a_i ( cgra_bank_enable[0] ),
		.addr_a_i  ( cgra_data_addr[0][RAM_ADDR_WIDTH-1:0] ),
		.wdata_a_i ( cgra_data_wdata[0] ),
		.rdata_a_o ( cgra_data_rdata[31:0] ),
		.we_a_i ( cgra_we_i[0] ),
		.be_a_i ( 4'b1111 ),
		.en_b_i ( ram_bank_enable[0] ),
		.addr_b_i  ( ram_data_addr ),
		.wdata_b_i ( ram_data_wdata ),
		.rdata_b_o ( ram_data_rdata_tmp[0] ),
		.we_b_i ( ram_data_we ),
		.be_b_i ( ram_data_be ));
	dp_ram
		#(.ADDR_WIDTH (RAM_ADDR_WIDTH), .BANK_ADDR_WIDTH(BANK_ADDR_WIDTH))
	dp_ram_1
		(
		.clk_i ( clk_i ),
		.rst_ni ( rst_ni ),
		.rst_CGRA  ( CGRA_Reset ),
		.en_a_i ( cgra_bank_enable[1] ),
		.addr_a_i  ( cgra_data_addr[1][RAM_ADDR_WIDTH-1:0] ),
		.wdata_a_i ( cgra_data_wdata[1] ),
		.rdata_a_o ( cgra_data_rdata[63:32] ),
		.we_a_i ( cgra_we_i[1] ),
		.be_a_i ( 4'b1111 ),
		.en_b_i ( ram_bank_enable[1] ),
		.addr_b_i  ( ram_data_addr ),
		.wdata_b_i ( ram_data_wdata ),
		.rdata_b_o ( ram_data_rdata_tmp[1] ),
		.we_b_i ( ram_data_we ),
		.be_b_i ( ram_data_be ));
	dp_ram
		#(.ADDR_WIDTH (RAM_ADDR_WIDTH), .BANK_ADDR_WIDTH(BANK_ADDR_WIDTH))
	dp_ram_2
		(
		.clk_i ( clk_i ),
		.rst_ni ( rst_ni ),
		.rst_CGRA  ( CGRA_Reset ),
		.en_a_i ( cgra_bank_enable[2] ),
		.addr_a_i  ( cgra_data_addr[2][RAM_ADDR_WIDTH-1:0] ),
		.wdata_a_i ( cgra_data_wdata[2] ),
		.rdata_a_o ( cgra_data_rdata[95:64] ),
		.we_a_i ( cgra_we_i[2] ),
		.be_a_i ( 4'b1111 ),
		.en_b_i ( ram_bank_enable[2] ),
		.addr_b_i  ( ram_data_addr ),
		.wdata_b_i ( ram_data_wdata ),
		.rdata_b_o ( ram_data_rdata_tmp[2] ),
		.we_b_i ( ram_data_we ),
		.be_b_i ( ram_data_be ));
	dp_ram
		#(.ADDR_WIDTH (RAM_ADDR_WIDTH), .BANK_ADDR_WIDTH(BANK_ADDR_WIDTH))
	dp_ram_3
		(
		.clk_i ( clk_i ),
		.rst_ni ( rst_ni ),
		.rst_CGRA  ( CGRA_Reset ),
		.en_a_i ( cgra_bank_enable[3] ),
		.addr_a_i  ( cgra_data_addr[3][RAM_ADDR_WIDTH-1:0] ),
		.wdata_a_i ( cgra_data_wdata[3] ),
		.rdata_a_o ( cgra_data_rdata[127:96] ),
		.we_a_i ( cgra_we_i[3] ),
		.be_a_i ( 4'b1111 ),
		.en_b_i ( ram_bank_enable[3] ),
		.addr_b_i  ( ram_data_addr ),
		.wdata_b_i ( ram_data_wdata ),
		.rdata_b_o ( ram_data_rdata_tmp[3] ),
		.we_b_i ( ram_data_we ),
		.be_b_i ( ram_data_be ));
	dp_ram
		#(.ADDR_WIDTH (RAM_ADDR_WIDTH), .BANK_ADDR_WIDTH(BANK_ADDR_WIDTH))
	dp_ram_4
		(
		.clk_i ( clk_i ),
		.rst_ni ( rst_ni ),
		.rst_CGRA  ( CGRA_Reset ),
		.en_a_i ( cgra_bank_enable[4] ),
		.addr_a_i  ( cgra_data_addr[4][RAM_ADDR_WIDTH-1:0] ),
		.wdata_a_i ( cgra_data_wdata[4] ),
		.rdata_a_o ( cgra_data_rdata[159:128] ),
		.we_a_i ( cgra_we_i[4] ),
		.be_a_i ( 4'b1111 ),
		.en_b_i ( ram_bank_enable[4] ),
		.addr_b_i  ( ram_data_addr ),
		.wdata_b_i ( ram_data_wdata ),
		.rdata_b_o ( ram_data_rdata_tmp[4] ),
		.we_b_i ( ram_data_we ),
		.be_b_i ( ram_data_be ));
	dp_ram
		#(.ADDR_WIDTH (RAM_ADDR_WIDTH), .BANK_ADDR_WIDTH(BANK_ADDR_WIDTH))
	dp_ram_5
		(
		.clk_i ( clk_i ),
		.rst_ni ( rst_ni ),
		.rst_CGRA  ( CGRA_Reset ),
		.en_a_i ( cgra_bank_enable[5] ),
		.addr_a_i  ( cgra_data_addr[5][RAM_ADDR_WIDTH-1:0] ),
		.wdata_a_i ( cgra_data_wdata[5] ),
		.rdata_a_o ( cgra_data_rdata[191:160] ),
		.we_a_i ( cgra_we_i[5] ),
		.be_a_i ( 4'b1111 ),
		.en_b_i ( ram_bank_enable[5] ),
		.addr_b_i  ( ram_data_addr ),
		.wdata_b_i ( ram_data_wdata ),
		.rdata_b_o ( ram_data_rdata_tmp[5] ),
		.we_b_i ( ram_data_we ),
		.be_b_i ( ram_data_be ));
	dp_ram
		#(.ADDR_WIDTH (RAM_ADDR_WIDTH), .BANK_ADDR_WIDTH(BANK_ADDR_WIDTH))
	dp_ram_6
		(
		.clk_i ( clk_i ),
		.rst_ni ( rst_ni ),
		.rst_CGRA  ( CGRA_Reset ),
		.en_a_i ( cgra_bank_enable[6] ),
		.addr_a_i  ( cgra_data_addr[6][RAM_ADDR_WIDTH-1:0] ),
		.wdata_a_i ( cgra_data_wdata[6] ),
		.rdata_a_o ( cgra_data_rdata[223:192] ),
		.we_a_i ( cgra_we_i[6] ),
		.be_a_i ( 4'b1111 ),
		.en_b_i ( ram_bank_enable[6] ),
		.addr_b_i  ( ram_data_addr ),
		.wdata_b_i ( ram_data_wdata ),
		.rdata_b_o ( ram_data_rdata_tmp[6] ),
		.we_b_i ( ram_data_we ),
		.be_b_i ( ram_data_be ));
	dp_ram
		#(.ADDR_WIDTH (RAM_ADDR_WIDTH), .BANK_ADDR_WIDTH(BANK_ADDR_WIDTH))
	dp_ram_7
		(
		.clk_i ( clk_i ),
		.rst_ni ( rst_ni ),
		.rst_CGRA  ( CGRA_Reset ),
		.en_a_i ( cgra_bank_enable[7] ),
		.addr_a_i  ( cgra_data_addr[7][RAM_ADDR_WIDTH-1:0] ),
		.wdata_a_i ( cgra_data_wdata[7] ),
		.rdata_a_o ( cgra_data_rdata[255:224] ),
		.we_a_i ( cgra_we_i[7] ),
		.be_a_i ( 4'b1111 ),
		.en_b_i ( ram_bank_enable[7] ),
		.addr_b_i  ( ram_data_addr ),
		.wdata_b_i ( ram_data_wdata ),
		.rdata_b_o ( ram_data_rdata_tmp[7] ),
		.we_b_i ( ram_data_we ),
		.be_b_i ( ram_data_be ));
	instruc_ram
		#(.ADDR_WIDTH (INSTRUC_ADDR_WIDTH),
		.INSTR_RDATA_WIDTH(INSTR_RDATA_WIDTH))
	instruc_ram_i
		(.clk_i( clk_i ),
		.en_a_i( ram_instr_req ),
		.addr_a_i( ram_instr_addr ),
		.rdata_a_o( ram_instr_rdata )
		);
	always_ff @(posedge clk_i, negedge rst_ni) begin
		if (~rst_ni) begin
			select_rdata_q <= RAM;
			data_rvalid_q  <= '0;
			instr_rvalid_q <= '0;
			state_valid_q  <= IDLE;
			data_addr_dec_q <= '0;
		end else begin
			select_rdata_q <= select_rdata_d;
			data_rvalid_q  <= data_req_dec;
			instr_rvalid_q <= ram_instr_req;
			state_valid_q  <= state_valid_n;
			data_addr_dec_q <= data_addr_dec;
		end
	end
	always_comb begin
		riscv_gnt_o    = 1'b0;
		riscv_rvalid_o = 1'b0;
		state_valid_n = state_valid_q;
		unique case(state_valid_q)
			IDLE:
			begin
				if(riscv_req_i) begin
					if(transaction == T_RAM) begin
						riscv_gnt_o = ram_data_gnt;
						if(ram_data_gnt) begin
							state_valid_n = WAIT_RAM_VALID;
						end else begin
							state_valid_n = WAIT_RAM_GNT;
						end
					end else begin
						state_valid_n = PERIPHEARL_VALID;
						riscv_gnt_o    = 1'b1;
					end
				end
			end
			PERIPHEARL_VALID:
			begin
				riscv_rvalid_o  = 1'b1;
				if(riscv_req_i) begin
					if(transaction == T_RAM) begin
						riscv_gnt_o = ram_data_gnt;
						if(ram_data_gnt) begin
							state_valid_n = WAIT_RAM_VALID;
						end else begin
							state_valid_n = WAIT_RAM_GNT;
						end
					end else begin
						state_valid_n = PERIPHEARL_VALID;
						riscv_gnt_o    = 1'b1;
					end
				end else state_valid_n = IDLE;
			end
			WAIT_RAM_GNT:
			begin
				riscv_rvalid_o  = 1'b0;
				if(riscv_req_i) begin
					riscv_gnt_o = ram_data_gnt;
					if(ram_data_gnt) begin
						state_valid_n = WAIT_RAM_VALID;
					end else begin
						state_valid_n = WAIT_RAM_GNT;
					end
				end else state_valid_n = IDLE;
			end
			WAIT_RAM_VALID:
			begin
				riscv_rvalid_o  = ram_data_valid;
				if(ram_data_valid) begin
					if(riscv_req_i) begin
						if(transaction == RAM) begin
							riscv_gnt_o    = ram_data_gnt;
							if(ram_data_gnt) begin
								state_valid_n = WAIT_RAM_VALID;
							end else begin
								state_valid_n = WAIT_RAM_GNT;
							end
						end else begin
							state_valid_n = PERIPHEARL_VALID;
							riscv_gnt_o    = 1'b1;
						end
					end else state_valid_n = IDLE;
				end
			end
			default: begin
			end
		endcase
	end
	assign instr_gnt_o    = ram_instr_gnt;
	assign instr_rvalid_o = ram_instr_valid;
	assign instr_rdata_o  = core_instr_rdata;
	always_comb begin
		ram_instr_req    = instr_req_i;
		ram_instr_addr   = instr_addr_i;
		ram_instr_gnt    = instr_req_i;
		ram_instr_valid  = instr_rvalid_q;
		core_instr_rdata = ram_instr_rdata;
		ram_data_valid   = data_rvalid_q;
		ram_data_gnt     = data_req_dec;
		core_data_rdata  = ram_data_rdata;
		ram_data_wdata   = data_wdata_dec;
		ram_data_we      = data_we_dec;
		ram_data_be      = data_be_dec;
		ram_data_addr	 = data_addr_dec[BANK_ADDR_WIDTH - 1:0];
		cgra_bank_enable = {8{cgra_enable}};
	end
	always_comb begin
		ram_bank_enable = '0;
		if (data_addr_dec < 2**BANK_ADDR_WIDTH) begin			ram_bank_enable[0]  = data_req_dec;
		end else if (data_addr_dec > 2**BANK_ADDR_WIDTH * 1 - 1 && data_addr_dec < 2**BANK_ADDR_WIDTH * 2) begin;
			ram_bank_enable[1]  = data_req_dec;
		end else if (data_addr_dec > 2**BANK_ADDR_WIDTH * 2 - 1 && data_addr_dec < 2**BANK_ADDR_WIDTH * 3) begin;
			ram_bank_enable[2]  = data_req_dec;
		end else if (data_addr_dec > 2**BANK_ADDR_WIDTH * 3 - 1 && data_addr_dec < 2**BANK_ADDR_WIDTH * 4) begin;
			ram_bank_enable[3]  = data_req_dec;
		end else if (data_addr_dec > 2**BANK_ADDR_WIDTH * 4 - 1 && data_addr_dec < 2**BANK_ADDR_WIDTH * 5) begin;
			ram_bank_enable[4]  = data_req_dec;
		end else if (data_addr_dec > 2**BANK_ADDR_WIDTH * 5 - 1 && data_addr_dec < 2**BANK_ADDR_WIDTH * 6) begin;
			ram_bank_enable[5]  = data_req_dec;
		end else if (data_addr_dec > 2**BANK_ADDR_WIDTH * 6 - 1 && data_addr_dec < 2**BANK_ADDR_WIDTH * 7) begin;
			ram_bank_enable[6]  = data_req_dec;
		end else if (data_addr_dec > 2**BANK_ADDR_WIDTH * 7 - 1 && data_addr_dec < 2**BANK_ADDR_WIDTH * 8) begin;
			ram_bank_enable[7]  = data_req_dec;
		end
	end
	always_comb begin
		ram_data_rdata = '0;
		if (data_addr_dec_q < 2**BANK_ADDR_WIDTH) begin			ram_data_rdata = ram_data_rdata_tmp[0];
		end else if (data_addr_dec_q > 2**BANK_ADDR_WIDTH * 1 - 1 && data_addr_dec_q < 2**BANK_ADDR_WIDTH * 2) begin;
			ram_data_rdata = ram_data_rdata_tmp[1];
		end else if (data_addr_dec_q > 2**BANK_ADDR_WIDTH * 2 - 1 && data_addr_dec_q < 2**BANK_ADDR_WIDTH * 3) begin;
			ram_data_rdata = ram_data_rdata_tmp[2];
		end else if (data_addr_dec_q > 2**BANK_ADDR_WIDTH * 3 - 1 && data_addr_dec_q < 2**BANK_ADDR_WIDTH * 4) begin;
			ram_data_rdata = ram_data_rdata_tmp[3];
		end else if (data_addr_dec_q > 2**BANK_ADDR_WIDTH * 4 - 1 && data_addr_dec_q < 2**BANK_ADDR_WIDTH * 5) begin;
			ram_data_rdata = ram_data_rdata_tmp[4];
		end else if (data_addr_dec_q > 2**BANK_ADDR_WIDTH * 5 - 1 && data_addr_dec_q < 2**BANK_ADDR_WIDTH * 6) begin;
			ram_data_rdata = ram_data_rdata_tmp[5];
		end else if (data_addr_dec_q > 2**BANK_ADDR_WIDTH * 6 - 1 && data_addr_dec_q < 2**BANK_ADDR_WIDTH * 7) begin;
			ram_data_rdata = ram_data_rdata_tmp[6];
		end else if (data_addr_dec_q > 2**BANK_ADDR_WIDTH * 7 - 1 && data_addr_dec_q < 2**BANK_ADDR_WIDTH * 8) begin;
			ram_data_rdata = ram_data_rdata_tmp[7];
		end
	end
	assign irq_software_o = irq_rnd_lines.irq_software;
	assign irq_timer_o    = irq_rnd_lines.irq_timer | irq_timer_q;
	assign irq_external_o = irq_rnd_lines.irq_external;
	assign irq_fast_o     = irq_rnd_lines.irq_fast;
	assign irq_nmi_o      = irq_rnd_lines.irq_nmi;
	assign irq_fastx_o    = irq_rnd_lines.irq_fastx;
endmodule