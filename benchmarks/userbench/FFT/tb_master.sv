`timescale 1ns/100ps


module ram_simulation (
    input clock,
    // Port 0
    input [31:0] addr0, input [31:0] data_in0, input w_rq0, output reg [31:0] data_out0,
    // Port 1
    input [31:0] addr1, input [31:0] data_in1, input w_rq1, output reg [31:0] data_out1,
    // Port 2
    input [31:0] addr2, input [31:0] data_in2, input w_rq2, output reg [31:0] data_out2,
    // Port 3
    input [31:0] addr3, input [31:0] data_in3, input w_rq3, output reg [31:0] data_out3
);


    reg [31:0] storage [0:1023];

    // Q15格式下的旋转因子 (cos_val << 16 | (sin_val & 0xFFFF))
    // short cos_val = (short)(cos(angle) * 32767.0);
    // short sin_val = (short)(sin(angle) * 32767.0);
    // N=16
    localparam W16_0 = 32'h7FFF0000; // cos(0)=1, sin(0)=0
    localparam W16_1 = {16'sd30342, -16'sd12724}; // cos(-pi/8)*32767, sin(-pi/8)*32767
    localparam W16_2 = {16'sd23170, -16'sd23170}; // cos(-pi/4)*32767, sin(-pi/4)*32767
    localparam W16_3 = {16'sd12724, -16'sd30342}; // cos(-3pi/8)*32767, sin(-3pi/8)*32767
    localparam W16_4 = {16'sd0,     -16'sd32767}; // cos(-pi/2)*32767, sin(-pi/2)*32767
    localparam W16_5 = {-16'sd12724, -16'sd30342};
    localparam W16_6 = {-16'sd23170, -16'sd23170};
    localparam W16_7 = {-16'sd30342, -16'sd12724};
    
    localparam W8_0 = W16_0;
    localparam W8_1 = W16_2;
    localparam W8_2 = W16_4;
    localparam W8_3 = W16_6;

    localparam W4_0 = W16_0;
    localparam W4_1 = W16_4;

    localparam W2_0 = W16_0;

    // 初始化RAM，模拟CPU预处理完成后的状态
    initial begin
        integer i;
        // --- 诊断信息 #1: 确认 initial 块开始执行 ---
        $display("[%t] RAM_SIM: Initial block started.", $time);
        
        // 1. 将所有内存清零
        for (i = 0; i < 1024; i = i+1) begin
            storage[i] = 32'h00000000;
        end
        // --- 诊断信息 #2: 确认清零后 storage[0] 的值 ---
        $display("[%t] RAM_SIM: Memory cleared. storage[0] = %h", $time, storage[0]);

        // 2. 在地址0x0处写入FFT的点数 N=16
        storage[0] = 32'd16;

        // --- 诊断信息 #3: 确认写入N后 storage[0] 的值 ---
        $display("[%t] RAM_SIM: N written. storage[0] = %d (hex: %h)", $time, storage[0], storage[0]);
        // 3. 准备 data_out 数组的初始内容 (CPU已完成位反转)
        //    字节地址: 0xc00 -> 字索引: 0x300 (768)
        storage[32'h300] = 32'h00010000; // 冲激信号 data_out[0] = 1.0 + 0j
        // --- 诊断信息 #4: 确认写入输入数据后 storage[0x300] 的值 ---
        $display("[%t] RAM_SIM: Input data written. storage[0x300] = %h", $time, storage[32'h300]);
        // 4. 预填充旋转因子表 twiddle_table
        //    基地址: 0xe00 -> 字索引: 0x380 (896)
        storage[32'h380 + 1] = W2_0; // m=2, index=1
        
        storage[32'h380 + 2] = W4_0; // m=4, index=2
        storage[32'h380 + 3] = W4_1; // m=4, index=3

        storage[32'h380 + 4] = W8_0; // m=8, index=4
        storage[32'h380 + 5] = W8_1; // m=8, index=5
        storage[32'h380 + 6] = W8_2; // m=8, index=6
        storage[32'h380 + 7] = W8_3; // m=8, index=7
        
        storage[32'h380 + 8]  = W16_0; // m=16, index=8
        storage[32'h380 + 9]  = W16_1; // m=16, index=9
        storage[32'h380 + 10] = W16_2; // m=16, index=10
        storage[32'h380 + 11] = W16_3; // m=16, index=11
        storage[32'h380 + 12] = W16_4; // m=16, index=12
        storage[32'h380 + 13] = W16_5; // m=16, index=13
        storage[32'h380 + 14] = W16_6; // m=16, index=14
        storage[32'h380 + 15] = W16_7; // m=16, index=15
        $display("[%t] RAM_SIM: Twiddle factors written. storage[0x380+1] = %h", $time, storage[32'h380+1]);
        $display("[%t] RAM_SIM: Initial block finished.", $time);
    end





    always @(posedge clock) begin
        // Port 0 logic
        if (w_rq0) storage[addr0 >> 2] <= data_in0;
        data_out0 <= storage[addr0 >> 2];

        // Port 1 logic
        if (w_rq1) storage[addr1 >> 2] <= data_in1;
        data_out1 <= storage[addr1 >> 2];
        
        // Port 2 logic
        if (w_rq2) storage[addr2 >> 2] <= data_in2;
        data_out2 <= storage[addr2 >> 2];

        // Port 3 logic
        if (w_rq3) storage[addr3 >> 2] <= data_in3;
        data_out3 <= storage[addr3 >> 2];
    end

endmodule





module tb_master;



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

	
	wire [31:0] ext_io_top_0_out, ext_io_top_1_out, ext_io_top_2_out, ext_io_top_3_out;
	wire [31:0] ext_io_bottom_0_out, ext_io_bottom_1_out, ext_io_bottom_2_out, ext_io_bottom_3_out;
	wire [31:0] ext_io_right_0_out, ext_io_right_1_out, ext_io_right_2_out, ext_io_right_3_out;
    
	wire [31:0] ext_io_top_0_in, ext_io_top_1_in, ext_io_top_2_in, ext_io_top_3_in;
	wire [31:0] ext_io_bottom_0_in, ext_io_bottom_1_in, ext_io_bottom_2_in, ext_io_bottom_3_in;
	wire [31:0] ext_io_right_0_in, ext_io_right_1_in, ext_io_right_2_in, ext_io_right_3_in;

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
	wire [31:0] mem_2_mem_unit_addr_to_ram;
	wire [31:0] mem_2_mem_unit_data_in_to_ram;
	wire [31:0] mem_2_mem_unit_data_out_from_ram;
	wire mem_2_mem_unit_w_rq_to_ram;
	wire [31:0] mem_3_mem_unit_addr_to_ram;
	wire [31:0] mem_3_mem_unit_data_in_to_ram;
	wire [31:0] mem_3_mem_unit_data_out_from_ram;
	wire mem_3_mem_unit_w_rq_to_ram;

	cgra_U0 DUT(
		.Config_Clock(Config_Clock),
		.Config_Reset(Config_Reset),
		.ConfigIn(ConfigIn),
		.ConfigOut(ConfigOut),

		.CGRA_Clock(DUT_clock & CGRA_Clock_en),
		.CGRA_Reset(CGRA_Reset),
		.CGRA_Enable(CGRA_Enable),

		// IO Ports
        .io_top_0_IOPin_bidir_in(ext_io_top_0_in), .io_top_0_IOPin_bidir_out(ext_io_top_0_out),
        .io_top_1_IOPin_bidir_in(ext_io_top_1_in), .io_top_1_IOPin_bidir_out(ext_io_top_1_out),
        .io_top_2_IOPin_bidir_in(ext_io_top_2_in), .io_top_2_IOPin_bidir_out(ext_io_top_2_out),
        .io_top_3_IOPin_bidir_in(ext_io_top_3_in), .io_top_3_IOPin_bidir_out(ext_io_top_3_out),

        .io_bottom_0_IOPin_bidir_in(ext_io_bottom_0_in), .io_bottom_0_IOPin_bidir_out(ext_io_bottom_0_out),
        .io_bottom_1_IOPin_bidir_in(ext_io_bottom_1_in), .io_bottom_1_IOPin_bidir_out(ext_io_bottom_1_out),
        .io_bottom_2_IOPin_bidir_in(ext_io_bottom_2_in), .io_bottom_2_IOPin_bidir_out(ext_io_bottom_2_out),
        .io_bottom_3_IOPin_bidir_in(ext_io_bottom_3_in), .io_bottom_3_IOPin_bidir_out(ext_io_bottom_3_out),

        .io_right_0_IOPin_bidir_in(ext_io_right_0_in), .io_right_0_IOPin_bidir_out(ext_io_right_0_out),
        .io_right_1_IOPin_bidir_in(ext_io_right_1_in), .io_right_1_IOPin_bidir_out(ext_io_right_1_out),
        .io_right_2_IOPin_bidir_in(ext_io_right_2_in), .io_right_2_IOPin_bidir_out(ext_io_right_2_out),
        .io_right_3_IOPin_bidir_in(ext_io_right_3_in), .io_right_3_IOPin_bidir_out(ext_io_right_3_out),

		// Memory Ports
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
		.mem_3_mem_unit_w_rq_to_ram(mem_3_mem_unit_w_rq_to_ram)
	);

	CGRA_configurator configurator(
		.clock(Config_Clock),
		.enable(configurator_enable),
		.sync_reset(configurator_reset),

		.bitstream(ConfigIn),
		.done(configurator_done)
	);

	ram_simulation ram(
		.clock(DUT_clock & CGRA_Clock_en),

		.addr0(mem_0_mem_unit_addr_to_ram),
		.data_in0(mem_0_mem_unit_data_in_to_ram),
		.data_out0(mem_0_mem_unit_data_out_from_ram),
		.w_rq0(mem_0_mem_unit_w_rq_to_ram),

		.addr1(mem_1_mem_unit_addr_to_ram),
		.data_in1(mem_1_mem_unit_data_in_to_ram),
		.data_out1(mem_1_mem_unit_data_out_from_ram),
		.w_rq1(mem_1_mem_unit_w_rq_to_ram),

		.addr2(mem_2_mem_unit_addr_to_ram),
		.data_in2(mem_2_mem_unit_data_in_to_ram),
		.data_out2(mem_2_mem_unit_data_out_from_ram),
		.w_rq2(mem_2_mem_unit_w_rq_to_ram),

		.addr3(mem_3_mem_unit_addr_to_ram),
		.data_in3(mem_3_mem_unit_data_in_to_ram),
		.data_out3(mem_3_mem_unit_data_out_from_ram),
		.w_rq3(mem_3_mem_unit_w_rq_to_ram)
	);



	initial begin
		Config_Clock_en = 1;
		CGRA_Clock_en = 0;
		configurator_reset = 1;
		Config_Reset = 1;
		CGRA_Reset = 0; 
		CGRA_Enable = 0;

		DUT_clock = 0;

		#1;
		Config_Reset = 0;
		configurator_reset = 0;
		configurator_enable = 1;
        	$display("[%t] --- Phase 1: Configuring CGRA ---", $time);

		wait(configurator_done);
        
        	// Wait for one clock cycle to ensure stable state transition
        	@(posedge DUT_clock);
		
		configurator_enable = 0;
		Config_Clock_en = 0;
        	$display("[%t] --- Configuration Done ---", $time);
		
        	CGRA_Reset = 1;
		CGRA_Clock_en = 1;
		CGRA_Enable = 1;
        
		#1;
		CGRA_Reset = 0; // Release CGRA reset
        	$display("[%t] --- Phase 2: Running CGRA ---", $time);
		$display("CGRA is now running. Please observe RAM contents in the waveform.");


		
		repeat (500) @(posedge (DUT_clock & CGRA_Clock_en));

		$display("[%t] --- Simulation finished after 500 cycles. ---", $time);
		$display("Please check the contents of ram.storage in your waveform viewer.");
		$finish;
		// --- END OF MODIFIED PART ---
	end

	always
		#5 DUT_clock = !DUT_clock;

endmodule
