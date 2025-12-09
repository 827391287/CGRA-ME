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
    input [31:0] addr3, input [31:0] data_in3, input w_rq3, output reg [31:0] data_out3,
	// Port 4
    input [31:0] addr4, input [31:0] data_in4, input w_rq4, output reg [31:0] data_out4
);


    reg [31:0] storage [0:1023];


    localparam W16_0 = 32'h7FFF0000; // cos(0)=1, sin(0)=0
    localparam W16_1 = 32'h769ECC84; // cos(-pi/8)=30342(0x769E), sin(-pi/8)=-12724(0xCC84)
    localparam W16_2 = 32'h5A82A57E; // cos(-pi/4)=23170(0x5A82), sin(-pi/4)=-23170(0xA57E)
    localparam W16_3 = 32'h31BCD3C2; // cos(-3pi/8)=12724(0x31BC), sin(-3pi/8)=-30342(0xD3C2)
    localparam W16_4 = 32'h00008001; // cos(-pi/2)=0(0x0000), sin(-pi/2)=-32767(0x8001)
    localparam W16_5 = 32'hCE44D3C2; // cos(-5pi/8)=-12724(0xCE44), sin(-5pi/8)=-30342(0xD3C2)
    localparam W16_6 = 32'hA57EA57E; // cos(-3pi/4)=-23170(0xA57E), sin(-3pi/4)=-23170(0xA57E)
    localparam W16_7 = 32'h8962CC84; // cos(-7pi/8)=-30342(0x8962), sin(-7pi/8)=-12724(0xCC84)

    localparam W8_0 = W16_0;
    localparam W8_1 = W16_2;
    localparam W8_2 = W16_4;
    localparam W8_3 = W16_6;
    localparam W4_0 = W16_0;
    localparam W4_1 = W16_4;

    localparam W2_0 = W16_0;


    initial begin
        integer i;

        $display("[%t] RAM_SIM: Initial block started.", $time);
       
        for (i = 0; i < 1024; i = i+1) begin
            storage[i] = 32'h00000000;
        end
      
        $display("[%t] RAM_SIM: Memory cleared. storage[0] = %h", $time, storage[0]);

        #1;

        storage[0] = 32'd16;


        $display("[%t] RAM_SIM: N written. storage[0] = %d (hex: %h)", $time, storage[0], storage[0]);

        storage[32'h280] = 32'h7FFF0000; 


        $display("[%t] RAM_SIM: Input data written. storage[0x280] = %h", $time, storage[32'h280]);

        

        storage[32'h300 + 1] = W2_0; // m=2, index=1
        
        storage[32'h300 + 2] = W4_0; // m=4, index=2
        storage[32'h300 + 3] = W4_1; // m=4, index=3

        storage[32'h300 + 4] = W8_0; // m=8, index=4
        storage[32'h300 + 5] = W8_1; // m=8, index=5
        storage[32'h300 + 6] = W8_2; // m=8, index=6
        storage[32'h300 + 7] = W8_3; // m=8, index=7
        
        storage[32'h300 + 8]  = W16_0; // m=16, index=8
        storage[32'h300 + 9]  = W16_1; // m=16, index=9
        storage[32'h300 + 10] = W16_2; // m=16, index=10
        storage[32'h300 + 11] = W16_3; // m=16, index=11
        storage[32'h300 + 12] = W16_4; // m=16, index=12
        storage[32'h300 + 13] = W16_5; // m=16, index=13
        storage[32'h300 + 14] = W16_6; // m=16, index=14
        storage[32'h300 + 15] = W16_7; // m=16, index=15
        $display("[%t] RAM_SIM: Twiddle factors written. storage[0x300+1] = %h", $time, storage[32'h300+1]);
	$display("[%t] RAM_SIM: Twiddle factors written. storage[0x300+9] = %h", $time, storage[32'h300+9]);

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

		// Port 4 logic
        if (w_rq4) storage[addr4 >> 2] <= data_in4;
        data_out4 <= storage[addr4 >> 2];
    end

endmodule





module tb_master;

    logic DUT_clock;

    // 配置相关信号
    logic Config_Clock_en;
    logic Config_Clock;
    logic Config_Reset;
    assign Config_Clock = DUT_clock & Config_Clock_en;

    logic ConfigIn;
    logic ConfigOut;
    logic configurator_enable;
    logic configurator_reset;
    logic configurator_done;

    // CGRA 运行相关信号
    logic CGRA_Clock_en;
    logic CGRA_Reset;
    logic CGRA_Enable;

    // --- [修改点 1] 定义输入信号变量 ---
    reg [31:0] i_val;           // 对应 input0 (变量 i)
    reg [31:0] basedist_val;    // 对应 input1/2 (变量 basedist/limit)
    
    // IO 端口定义
    wire [31:0] ext_io_top_0_out, ext_io_top_1_out, ext_io_top_2_out, ext_io_top_3_out, ext_io_top_4_out;
    wire [31:0] ext_io_bottom_0_out, ext_io_bottom_1_out, ext_io_bottom_2_out, ext_io_bottom_3_out, ext_io_bottom_4_out;
    wire [31:0] ext_io_right_0_out, ext_io_right_1_out, ext_io_right_2_out, ext_io_right_3_out, ext_io_right_4_out;

    wire [31:0] ext_io_top_0_in, ext_io_top_1_in, ext_io_top_2_in, ext_io_top_3_in, ext_io_top_4_in;
    wire [31:0] ext_io_bottom_0_in, ext_io_bottom_1_in, ext_io_bottom_2_in, ext_io_bottom_3_in, ext_io_bottom_4_in;
    wire [31:0] ext_io_right_0_in, ext_io_right_1_in, ext_io_right_2_in, ext_io_right_3_in, ext_io_right_4_in;
    
    // --- [修改点 2] 驱动 IO 端口 ---
    // 假设 input0 (i) 映射到了 Top 1
    assign ext_io_top_1_in = i_val;
    
    // 假设 input1 (basedist) 映射到了 Top 2 (根据具体 bitstream 调整)
    // 这里我们将 basedist 同时驱动到 Top 2 和 Top 3 以防万一
    assign ext_io_top_2_in = basedist_val;
    assign ext_io_top_3_in = basedist_val;

    // 其他端口置 0
    assign ext_io_top_0_in = 32'b0;
    assign ext_io_top_4_in = 32'b0;

    // 底部和右侧端口置 0
    assign ext_io_bottom_0_in = 0; assign ext_io_bottom_1_in = 0; assign ext_io_bottom_2_in = 0;
    assign ext_io_bottom_3_in = 0; assign ext_io_bottom_4_in = 0;
    assign ext_io_right_0_in = 0; assign ext_io_right_1_in = 0; assign ext_io_right_2_in = 0;
    assign ext_io_right_3_in = 0; assign ext_io_right_4_in = 0;

    // Memory Wires (保持不变)
    wire [31:0] mem_0_mem_unit_addr_to_ram, mem_0_mem_unit_data_in_to_ram, mem_0_mem_unit_data_out_from_ram; wire mem_0_mem_unit_w_rq_to_ram;
    wire [31:0] mem_1_mem_unit_addr_to_ram, mem_1_mem_unit_data_in_to_ram, mem_1_mem_unit_data_out_from_ram; wire mem_1_mem_unit_w_rq_to_ram;
    wire [31:0] mem_2_mem_unit_addr_to_ram, mem_2_mem_unit_data_in_to_ram, mem_2_mem_unit_data_out_from_ram; wire mem_2_mem_unit_w_rq_to_ram;
    wire [31:0] mem_3_mem_unit_addr_to_ram, mem_3_mem_unit_data_in_to_ram, mem_3_mem_unit_data_out_from_ram; wire mem_3_mem_unit_w_rq_to_ram;
    wire [31:0] mem_4_mem_unit_addr_to_ram, mem_4_mem_unit_data_in_to_ram, mem_4_mem_unit_data_out_from_ram; wire mem_4_mem_unit_w_rq_to_ram;

    // DUT 实例化 (保持不变)
    cgra_U0 DUT(
        .Config_Clock(Config_Clock), .Config_Reset(Config_Reset), .ConfigIn(ConfigIn), .ConfigOut(ConfigOut),
        .CGRA_Clock(DUT_clock & CGRA_Clock_en), .CGRA_Reset(CGRA_Reset), .CGRA_Enable(CGRA_Enable),
        // IOs
        .io_top_0_IOPin_bidir_in(ext_io_top_0_in), .io_top_0_IOPin_bidir_out(ext_io_top_0_out),
        .io_top_1_IOPin_bidir_in(ext_io_top_1_in), .io_top_1_IOPin_bidir_out(ext_io_top_1_out),
        .io_top_2_IOPin_bidir_in(ext_io_top_2_in), .io_top_2_IOPin_bidir_out(ext_io_top_2_out),
        .io_top_3_IOPin_bidir_in(ext_io_top_3_in), .io_top_3_IOPin_bidir_out(ext_io_top_3_out),
        .io_top_4_IOPin_bidir_in(ext_io_top_4_in), .io_top_4_IOPin_bidir_out(ext_io_top_4_out),
        // Bottom IOs
        .io_bottom_0_IOPin_bidir_in(ext_io_bottom_0_in), .io_bottom_0_IOPin_bidir_out(ext_io_bottom_0_out),
        .io_bottom_1_IOPin_bidir_in(ext_io_bottom_1_in), .io_bottom_1_IOPin_bidir_out(ext_io_bottom_1_out),
        .io_bottom_2_IOPin_bidir_in(ext_io_bottom_2_in), .io_bottom_2_IOPin_bidir_out(ext_io_bottom_2_out),
        .io_bottom_3_IOPin_bidir_in(ext_io_bottom_3_in), .io_bottom_3_IOPin_bidir_out(ext_io_bottom_3_out),
        .io_bottom_4_IOPin_bidir_in(ext_io_bottom_4_in), .io_bottom_4_IOPin_bidir_out(ext_io_bottom_4_out),
        // Right IOs
        .io_right_0_IOPin_bidir_in(ext_io_right_0_in), .io_right_0_IOPin_bidir_out(ext_io_right_0_out),
        .io_right_1_IOPin_bidir_in(ext_io_right_1_in), .io_right_1_IOPin_bidir_out(ext_io_right_1_out),
        .io_right_2_IOPin_bidir_in(ext_io_right_2_in), .io_right_2_IOPin_bidir_out(ext_io_right_2_out),
        .io_right_3_IOPin_bidir_in(ext_io_right_3_in), .io_right_3_IOPin_bidir_out(ext_io_right_3_out),
        .io_right_4_IOPin_bidir_in(ext_io_right_4_in), .io_right_4_IOPin_bidir_out(ext_io_right_4_out),
        // Memory Ports
        .mem_0_mem_unit_addr_to_ram(mem_0_mem_unit_addr_to_ram), .mem_0_mem_unit_data_in_to_ram(mem_0_mem_unit_data_in_to_ram), .mem_0_mem_unit_data_out_from_ram(mem_0_mem_unit_data_out_from_ram), .mem_0_mem_unit_w_rq_to_ram(mem_0_mem_unit_w_rq_to_ram),
        .mem_1_mem_unit_addr_to_ram(mem_1_mem_unit_addr_to_ram), .mem_1_mem_unit_data_in_to_ram(mem_1_mem_unit_data_in_to_ram), .mem_1_mem_unit_data_out_from_ram(mem_1_mem_unit_data_out_from_ram), .mem_1_mem_unit_w_rq_to_ram(mem_1_mem_unit_w_rq_to_ram),
        .mem_2_mem_unit_addr_to_ram(mem_2_mem_unit_addr_to_ram), .mem_2_mem_unit_data_in_to_ram(mem_2_mem_unit_data_in_to_ram), .mem_2_mem_unit_data_out_from_ram(mem_2_mem_unit_data_out_from_ram), .mem_2_mem_unit_w_rq_to_ram(mem_2_mem_unit_w_rq_to_ram),
        .mem_3_mem_unit_addr_to_ram(mem_3_mem_unit_addr_to_ram), .mem_3_mem_unit_data_in_to_ram(mem_3_mem_unit_data_in_to_ram), .mem_3_mem_unit_data_out_from_ram(mem_3_mem_unit_data_out_from_ram), .mem_3_mem_unit_w_rq_to_ram(mem_3_mem_unit_w_rq_to_ram),
        .mem_4_mem_unit_addr_to_ram(mem_4_mem_unit_addr_to_ram), .mem_4_mem_unit_data_in_to_ram(mem_4_mem_unit_data_in_to_ram), .mem_4_mem_unit_data_out_from_ram(mem_4_mem_unit_data_out_from_ram), .mem_4_mem_unit_w_rq_to_ram(mem_4_mem_unit_w_rq_to_ram)
    );

    CGRA_configurator configurator(
        .clock(Config_Clock), .enable(configurator_enable), .sync_reset(configurator_reset),
        .bitstream(ConfigIn), .done(configurator_done)
    );

    ram_simulation ram(
        .clock(DUT_clock & CGRA_Clock_en),
        .addr0(mem_0_mem_unit_addr_to_ram), .data_in0(mem_0_mem_unit_data_in_to_ram), .data_out0(mem_0_mem_unit_data_out_from_ram), .w_rq0(mem_0_mem_unit_w_rq_to_ram),
        .addr1(mem_1_mem_unit_addr_to_ram), .data_in1(mem_1_mem_unit_data_in_to_ram), .data_out1(mem_1_mem_unit_data_out_from_ram), .w_rq1(mem_1_mem_unit_w_rq_to_ram),
        .addr2(mem_2_mem_unit_addr_to_ram), .data_in2(mem_2_mem_unit_data_in_to_ram), .data_out2(mem_2_mem_unit_data_out_from_ram), .w_rq2(mem_2_mem_unit_w_rq_to_ram),
        .addr3(mem_3_mem_unit_addr_to_ram), .data_in3(mem_3_mem_unit_data_in_to_ram), .data_out3(mem_3_mem_unit_data_out_from_ram), .w_rq3(mem_3_mem_unit_w_rq_to_ram),
        .addr4(mem_4_mem_unit_addr_to_ram), .data_in4(mem_4_mem_unit_data_in_to_ram), .data_out4(mem_4_mem_unit_data_out_from_ram), .w_rq4(mem_4_mem_unit_w_rq_to_ram)
    );

    initial begin
        // 1. 初始化
        Config_Clock_en = 1; CGRA_Clock_en = 0; configurator_reset = 1; Config_Reset = 1;
        CGRA_Reset = 0; CGRA_Enable = 0; i_val = 0; basedist_val = 0; DUT_clock = 0;

        #1;
        Config_Reset = 0; configurator_reset = 0; configurator_enable = 1;
        $display("[%t] --- Phase 1: Configuring CGRA ---", $time);

        wait(configurator_done);
        
        configurator_enable = 0; Config_Clock_en = 0;
        $display("[%t] --- Configuration Done ---", $time);
        
        CGRA_Reset = 1; CGRA_Clock_en = 1; CGRA_Enable = 1;
        
        #1;
        CGRA_Reset = 0; // 释放 CGRA 复位
        $display("[%t] --- Phase 2: Running CGRA ---", $time);

        // 2. 运行 FFT 控制逻辑
        begin
            integer N = 16;
            integer BlockSize, i, basedist;
            
            // --- [修改点 3] 关键时序参数 ---
            integer II = 3;  // Context 数量，决定了硬件处理一个点需要多少周期
            
            $display("[%t] --- Simulating FFT Loop Control for N=%d ---", $time, N);

            for (BlockSize = 2; BlockSize <= N; BlockSize = BlockSize * 2) begin
                basedist = BlockSize / 2;
                $display("[%t] STAGE: BlockSize = %d, basedist = %d", $time, BlockSize, basedist);

                for (i = 0; i < N; i = i + BlockSize) begin
                    // 等待时钟上升沿，更新输入
                    @(posedge (DUT_clock & CGRA_Clock_en));
                    
                    // --- [修改点 4] 同时提供 i 和 basedist ---
                    i_val <= i;             // Input 0
                    basedist_val <= basedist; // Input 1 / Limit
                    
                    $display("[%t]   Loop Start: i = %d, basedist = %d", $time, i, basedist);

                    if (basedist >= 1) begin
                        // --- [修改点 5] 等待硬件循环结束 ---
                        // 硬件需要 (basedist * II) 个周期来处理完这批数据
                        repeat (basedist * II) @(posedge (DUT_clock & CGRA_Clock_en));
                    end
                    
                    $display("[%t]   Loop Finished for i = %d", $time, i);
                end
            end
            
            $display("[%t] --- FFT Simulation Finished ---", $time);
        end

        #100; 
        $finish;
    end

    always #5 DUT_clock = !DUT_clock;

endmodule