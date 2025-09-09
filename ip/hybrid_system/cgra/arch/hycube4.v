

module cgrame_caseStatement_10in_0latency_32b #(parameter contexts=1, parameter latency=1, parameter size=1) (
  CGRA_Clock,
  CGRA_Reset,
  CGRA_Enable,
  in0,
  in1,
  in2,
  in3,
  in4,
  in5,
  in6,
  in7,
  in8,
  in9,
  out,
  select
);
    input CGRA_Clock;
    input CGRA_Reset;
    input CGRA_Enable;
    input [size-1:0] in0;
    input [size-1:0] in1;
    input [size-1:0] in2;
    input [size-1:0] in3;
    input [size-1:0] in4;
    input [size-1:0] in5;
    input [size-1:0] in6;
    input [size-1:0] in7;
    input [size-1:0] in8;
    input [size-1:0] in9;
    output [size-1:0] out;
    input [3:0] select;
    reg [size-1:0] computation;
    reg [size-1:0] delay_ppln [latency-1:0];
    always @*
        case (select)
            0: computation = in0;
            1: computation = in1;
            2: computation = in2;
            3: computation = in3;
            4: computation = in4;
            5: computation = in5;
            6: computation = in6;
            7: computation = in7;
            8: computation = in8;
            9: computation = in9;
            default: computation = {size{1'bx}};
        endcase
    integer i;
    always @(posedge CGRA_Clock, posedge CGRA_Reset) begin
        if (CGRA_Reset) begin
            for (i=0; i < latency; i=i+1)
                delay_ppln[i] <= {size{1'b0}};
        end else if (CGRA_Enable) begin
            for (i=1; i < latency; i=i+1)
                delay_ppln[i] <= delay_ppln[i-1];
            delay_ppln[0] <= computation;
        end
    end

    generate
        if (latency > 0)
            assign out = delay_ppln[latency-1];
        else
            assign out = computation;
    endgenerate

endmodule //cgrame_caseStatement_10in_0latency_32b

module cgrame_op_or_32b #(parameter contexts=1, parameter size=1) (
  a,
  b,
  c
);
    input [size-1:0] a;
    input [size-1:0] b;
    output [size-1:0] c;
    assign c = a | b;


endmodule //cgrame_op_or_32b

module cgrame_configcell_context1_size1 #(parameter contexts=1, parameter size=1) (
  ConfigIn,
  ConfigOut,
  Config_Clock,
  Config_Reset,
  CGRA_Clock,
  CGRA_Reset,
  CGRA_Enable,
  select
);
    input CGRA_Clock;
    input CGRA_Reset;
    input CGRA_Enable;
    input ConfigIn;
    input Config_Clock;
    input Config_Reset;
    output ConfigOut;
    output [size-1:0] select;
    reg [size-1:0] config_reg [contexts-1:0];

    reg [contexts-1:0] context_counter = {size{1'b0}};

    integer i;

    always @(posedge Config_Clock, posedge Config_Reset)
        if (Config_Reset)
            begin
                for (i = 0; i < contexts; i = i+1) begin
                    config_reg[i] <= 'd0;
                end
            end
        else
            begin
                for (i = 0; i < contexts; i = i+1) begin
                    if(i == 0) begin
                        config_reg[i] <= ConfigIn;
                    end
                    else begin
                        config_reg[i] <= config_reg[i-1];
                    end
                end
            end

    always @(posedge CGRA_Clock, posedge CGRA_Reset)
        if(CGRA_Reset || context_counter == contexts)
            begin
                context_counter <= 'd0;
            end
        else if(CGRA_Enable)
            begin
                if(context_counter + 'b1 >= contexts)
                    begin
                        context_counter <= 'd0;
                    end
                else
                    context_counter <= context_counter + 'b1;
            end
    assign select = config_reg[context_counter];
    assign ConfigOut = config_reg[contexts-1][0];

endmodule //cgrame_configcell_context1_size1

module cgrame_configcell_context1_size2 #(parameter contexts=1, parameter size=1) (
  ConfigIn,
  ConfigOut,
  Config_Clock,
  Config_Reset,
  CGRA_Clock,
  CGRA_Reset,
  CGRA_Enable,
  select
);
    input CGRA_Clock;
    input CGRA_Reset;
    input CGRA_Enable;
    input ConfigIn;
    input Config_Clock;
    input Config_Reset;
    output ConfigOut;
    output [size-1:0] select;
    reg [size-1:0] config_reg [contexts-1:0];

    reg [contexts-1:0] context_counter = {size{1'b0}};

    integer i;

    always @(posedge Config_Clock, posedge Config_Reset)
        if (Config_Reset)
            begin
                for (i = 0; i < contexts; i = i+1) begin
                    config_reg[i] <= 'd0;
                end
            end
        else
            begin
                for (i = 0; i < contexts; i = i+1) begin
                    if(i == 0) begin
                        config_reg[i] <= {ConfigIn,config_reg[i][size-1:1]};
                    end
                    else begin
                        config_reg[i] <= {config_reg[i-1][0],config_reg[i][size-1:1]};
                    end
                end
            end

    always @(posedge CGRA_Clock, posedge CGRA_Reset)
        if(CGRA_Reset || context_counter == contexts)
            begin
                context_counter <= 'd0;
            end
        else if(CGRA_Enable)
            begin
                if(context_counter + 'b1 >= contexts)
                    begin
                        context_counter <= 'd0;
                    end
                else
                    context_counter <= context_counter + 'b1;
            end
    assign select = config_reg[context_counter];
    assign ConfigOut = config_reg[contexts-1][0];

endmodule //cgrame_configcell_context1_size2

module cgrame_configcell_context1_size32 #(parameter contexts=1, parameter size=1) (
  ConfigIn,
  ConfigOut,
  Config_Clock,
  Config_Reset,
  CGRA_Clock,
  CGRA_Reset,
  CGRA_Enable,
  select
);
    input CGRA_Clock;
    input CGRA_Reset;
    input CGRA_Enable;
    input ConfigIn;
    input Config_Clock;
    input Config_Reset;
    output ConfigOut;
    output [size-1:0] select;
    reg [size-1:0] config_reg [contexts-1:0];

    reg [contexts-1:0] context_counter = {size{1'b0}};

    integer i;

    always @(posedge Config_Clock, posedge Config_Reset)
        if (Config_Reset)
            begin
                for (i = 0; i < contexts; i = i+1) begin
                    config_reg[i] <= 'd0;
                end
            end
        else
            begin
                for (i = 0; i < contexts; i = i+1) begin
                    if(i == 0) begin
                        config_reg[i] <= {ConfigIn,config_reg[i][size-1:1]};
                    end
                    else begin
                        config_reg[i] <= {config_reg[i-1][0],config_reg[i][size-1:1]};
                    end
                end
            end

    always @(posedge CGRA_Clock, posedge CGRA_Reset)
        if(CGRA_Reset || context_counter == contexts)
            begin
                context_counter <= 'd0;
            end
        else if(CGRA_Enable)
            begin
                if(context_counter + 'b1 >= contexts)
                    begin
                        context_counter <= 'd0;
                    end
                else
                    context_counter <= context_counter + 'b1;
            end
    assign select = config_reg[context_counter];
    assign ConfigOut = config_reg[contexts-1][0];

endmodule //cgrame_configcell_context1_size32

module cgrame_configcell_context1_size4 #(parameter contexts=1, parameter size=1) (
  ConfigIn,
  ConfigOut,
  Config_Clock,
  Config_Reset,
  CGRA_Clock,
  CGRA_Reset,
  CGRA_Enable,
  select
);
    input CGRA_Clock;
    input CGRA_Reset;
    input CGRA_Enable;
    input ConfigIn;
    input Config_Clock;
    input Config_Reset;
    output ConfigOut;
    output [size-1:0] select;
    reg [size-1:0] config_reg [contexts-1:0];

    reg [contexts-1:0] context_counter = {size{1'b0}};

    integer i;

    always @(posedge Config_Clock, posedge Config_Reset)
        if (Config_Reset)
            begin
                for (i = 0; i < contexts; i = i+1) begin
                    config_reg[i] <= 'd0;
                end
            end
        else
            begin
                for (i = 0; i < contexts; i = i+1) begin
                    if(i == 0) begin
                        config_reg[i] <= {ConfigIn,config_reg[i][size-1:1]};
                    end
                    else begin
                        config_reg[i] <= {config_reg[i-1][0],config_reg[i][size-1:1]};
                    end
                end
            end

    always @(posedge CGRA_Clock, posedge CGRA_Reset)
        if(CGRA_Reset || context_counter == contexts)
            begin
                context_counter <= 'd0;
            end
        else if(CGRA_Enable)
            begin
                if(context_counter + 'b1 >= contexts)
                    begin
                        context_counter <= 'd0;
                    end
                else
                    context_counter <= context_counter + 'b1;
            end
    assign select = config_reg[context_counter];
    assign ConfigOut = config_reg[contexts-1][0];

endmodule //cgrame_configcell_context1_size4

module cgrame_configcell_context1_size3 #(parameter contexts=1, parameter size=1) (
  ConfigIn,
  ConfigOut,
  Config_Clock,
  Config_Reset,
  CGRA_Clock,
  CGRA_Reset,
  CGRA_Enable,
  select
);
    input CGRA_Clock;
    input CGRA_Reset;
    input CGRA_Enable;
    input ConfigIn;
    input Config_Clock;
    input Config_Reset;
    output ConfigOut;
    output [size-1:0] select;
    reg [size-1:0] config_reg [contexts-1:0];

    reg [contexts-1:0] context_counter = {size{1'b0}};

    integer i;

    always @(posedge Config_Clock, posedge Config_Reset)
        if (Config_Reset)
            begin
                for (i = 0; i < contexts; i = i+1) begin
                    config_reg[i] <= 'd0;
                end
            end
        else
            begin
                for (i = 0; i < contexts; i = i+1) begin
                    if(i == 0) begin
                        config_reg[i] <= {ConfigIn,config_reg[i][size-1:1]};
                    end
                    else begin
                        config_reg[i] <= {config_reg[i-1][0],config_reg[i][size-1:1]};
                    end
                end
            end

    always @(posedge CGRA_Clock, posedge CGRA_Reset)
        if(CGRA_Reset || context_counter == contexts)
            begin
                context_counter <= 'd0;
            end
        else if(CGRA_Enable)
            begin
                if(context_counter + 'b1 >= contexts)
                    begin
                        context_counter <= 'd0;
                    end
                else
                    context_counter <= context_counter + 'b1;
            end
    assign select = config_reg[context_counter];
    assign ConfigOut = config_reg[contexts-1][0];

endmodule //cgrame_configcell_context1_size3

module const_32b_U15 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  output [31:0] out
);
  //Wire declarations for instance 'ConstVal' (Module cgrame_configcell_context1_size32)
  wire  ConstVal__CGRA_Clock;
  wire  ConstVal__CGRA_Enable;
  wire  ConstVal__CGRA_Reset;
  wire  ConstVal__ConfigIn;
  wire  ConstVal__ConfigOut;
  wire  ConstVal__Config_Clock;
  wire  ConstVal__Config_Reset;
  wire [31:0] ConstVal__select;
  cgrame_configcell_context1_size32 #(.contexts(1),.size(32)) ConstVal(
    .CGRA_Clock(ConstVal__CGRA_Clock),
    .CGRA_Enable(ConstVal__CGRA_Enable),
    .CGRA_Reset(ConstVal__CGRA_Reset),
    .ConfigIn(ConstVal__ConfigIn),
    .ConfigOut(ConstVal__ConfigOut),
    .Config_Clock(ConstVal__Config_Clock),
    .Config_Reset(ConstVal__Config_Reset),
    .select(ConstVal__select)
  );

  //All the connections
  assign ConstVal__CGRA_Clock = CGRA_Clock;
  assign ConstVal__CGRA_Reset = CGRA_Reset;
  assign ConstVal__CGRA_Enable = CGRA_Enable;
  assign ConstVal__Config_Clock = Config_Clock;
  assign ConstVal__Config_Reset = Config_Reset;
  assign out[31:0] = ConstVal__select[31:0];
  assign ConstVal__ConfigIn = ConfigIn;
  assign ConfigOut = ConstVal__ConfigOut;

endmodule //const_32b_U15

module cgrame_memUnit_32b #(parameter contexts=1, parameter size=1) (
  addr,
  data_in,
  data_out,
  w_rq,
  addr_to_ram,
  data_in_to_ram,
  data_out_from_ram,
  w_rq_to_ram
);
    input [size-1:0] addr;
    input [size-1:0] data_in;
    output [size-1:0] data_out;
    input [0:0] w_rq;
    output [size-1:0] addr_to_ram;
    output [size-1:0] data_in_to_ram;
    input [size-1:0] data_out_from_ram;
    output [0:0] w_rq_to_ram;
    assign addr_to_ram = addr;
    assign data_in_to_ram = data_in;
    assign w_rq_to_ram = w_rq;
    assign data_out = data_out_from_ram;

endmodule //cgrame_memUnit_32b

module cgrame_mux_10to1_32b #(parameter contexts=1, parameter size=1) (
  in0,
  in1,
  in2,
  in3,
  in4,
  in5,
  in6,
  in7,
  in8,
  in9,
  out,
  select
);
    input [size-1:0] in0;
    input [size-1:0] in1;
    input [size-1:0] in2;
    input [size-1:0] in3;
    input [size-1:0] in4;
    input [size-1:0] in5;
    input [size-1:0] in6;
    input [size-1:0] in7;
    input [size-1:0] in8;
    input [size-1:0] in9;
    output reg [size-1:0] out;
    input [3:0] select;
    always @(*)
        case (select)
            0: out = in0;
            1: out = in1;
            2: out = in2;
            3: out = in3;
            4: out = in4;
            5: out = in5;
            6: out = in6;
            7: out = in7;
            8: out = in8;
            9: out = in9;
            default: out = {size{1'bx}};
        endcase

endmodule //cgrame_mux_10to1_32b

module crossbar_10in_10out_U20 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in4,
  input [31:0] in5,
  input [31:0] in6,
  input [31:0] in7,
  input [31:0] in8,
  input [31:0] in9,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out4,
  output [31:0] out5,
  output [31:0] out6,
  output [31:0] out7,
  output [31:0] out8,
  output [31:0] out9
);
  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size4)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [3:0] Mux0config__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size4)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [3:0] Mux1config__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size4)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [3:0] Mux2config__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size4)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [3:0] Mux3config__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux4config' (Module cgrame_configcell_context1_size4)
  wire  Mux4config__CGRA_Clock;
  wire  Mux4config__CGRA_Enable;
  wire  Mux4config__CGRA_Reset;
  wire  Mux4config__ConfigIn;
  wire  Mux4config__ConfigOut;
  wire  Mux4config__Config_Clock;
  wire  Mux4config__Config_Reset;
  wire [3:0] Mux4config__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) Mux4config(
    .CGRA_Clock(Mux4config__CGRA_Clock),
    .CGRA_Enable(Mux4config__CGRA_Enable),
    .CGRA_Reset(Mux4config__CGRA_Reset),
    .ConfigIn(Mux4config__ConfigIn),
    .ConfigOut(Mux4config__ConfigOut),
    .Config_Clock(Mux4config__Config_Clock),
    .Config_Reset(Mux4config__Config_Reset),
    .select(Mux4config__select)
  );

  //Wire declarations for instance 'Mux5config' (Module cgrame_configcell_context1_size4)
  wire  Mux5config__CGRA_Clock;
  wire  Mux5config__CGRA_Enable;
  wire  Mux5config__CGRA_Reset;
  wire  Mux5config__ConfigIn;
  wire  Mux5config__ConfigOut;
  wire  Mux5config__Config_Clock;
  wire  Mux5config__Config_Reset;
  wire [3:0] Mux5config__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) Mux5config(
    .CGRA_Clock(Mux5config__CGRA_Clock),
    .CGRA_Enable(Mux5config__CGRA_Enable),
    .CGRA_Reset(Mux5config__CGRA_Reset),
    .ConfigIn(Mux5config__ConfigIn),
    .ConfigOut(Mux5config__ConfigOut),
    .Config_Clock(Mux5config__Config_Clock),
    .Config_Reset(Mux5config__Config_Reset),
    .select(Mux5config__select)
  );

  //Wire declarations for instance 'Mux6config' (Module cgrame_configcell_context1_size4)
  wire  Mux6config__CGRA_Clock;
  wire  Mux6config__CGRA_Enable;
  wire  Mux6config__CGRA_Reset;
  wire  Mux6config__ConfigIn;
  wire  Mux6config__ConfigOut;
  wire  Mux6config__Config_Clock;
  wire  Mux6config__Config_Reset;
  wire [3:0] Mux6config__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) Mux6config(
    .CGRA_Clock(Mux6config__CGRA_Clock),
    .CGRA_Enable(Mux6config__CGRA_Enable),
    .CGRA_Reset(Mux6config__CGRA_Reset),
    .ConfigIn(Mux6config__ConfigIn),
    .ConfigOut(Mux6config__ConfigOut),
    .Config_Clock(Mux6config__Config_Clock),
    .Config_Reset(Mux6config__Config_Reset),
    .select(Mux6config__select)
  );

  //Wire declarations for instance 'Mux7config' (Module cgrame_configcell_context1_size4)
  wire  Mux7config__CGRA_Clock;
  wire  Mux7config__CGRA_Enable;
  wire  Mux7config__CGRA_Reset;
  wire  Mux7config__ConfigIn;
  wire  Mux7config__ConfigOut;
  wire  Mux7config__Config_Clock;
  wire  Mux7config__Config_Reset;
  wire [3:0] Mux7config__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) Mux7config(
    .CGRA_Clock(Mux7config__CGRA_Clock),
    .CGRA_Enable(Mux7config__CGRA_Enable),
    .CGRA_Reset(Mux7config__CGRA_Reset),
    .ConfigIn(Mux7config__ConfigIn),
    .ConfigOut(Mux7config__ConfigOut),
    .Config_Clock(Mux7config__Config_Clock),
    .Config_Reset(Mux7config__Config_Reset),
    .select(Mux7config__select)
  );

  //Wire declarations for instance 'Mux8config' (Module cgrame_configcell_context1_size4)
  wire  Mux8config__CGRA_Clock;
  wire  Mux8config__CGRA_Enable;
  wire  Mux8config__CGRA_Reset;
  wire  Mux8config__ConfigIn;
  wire  Mux8config__ConfigOut;
  wire  Mux8config__Config_Clock;
  wire  Mux8config__Config_Reset;
  wire [3:0] Mux8config__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) Mux8config(
    .CGRA_Clock(Mux8config__CGRA_Clock),
    .CGRA_Enable(Mux8config__CGRA_Enable),
    .CGRA_Reset(Mux8config__CGRA_Reset),
    .ConfigIn(Mux8config__ConfigIn),
    .ConfigOut(Mux8config__ConfigOut),
    .Config_Clock(Mux8config__Config_Clock),
    .Config_Reset(Mux8config__Config_Reset),
    .select(Mux8config__select)
  );

  //Wire declarations for instance 'Mux9config' (Module cgrame_configcell_context1_size4)
  wire  Mux9config__CGRA_Clock;
  wire  Mux9config__CGRA_Enable;
  wire  Mux9config__CGRA_Reset;
  wire  Mux9config__ConfigIn;
  wire  Mux9config__ConfigOut;
  wire  Mux9config__Config_Clock;
  wire  Mux9config__Config_Reset;
  wire [3:0] Mux9config__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) Mux9config(
    .CGRA_Clock(Mux9config__CGRA_Clock),
    .CGRA_Enable(Mux9config__CGRA_Enable),
    .CGRA_Reset(Mux9config__CGRA_Reset),
    .ConfigIn(Mux9config__ConfigIn),
    .ConfigOut(Mux9config__ConfigOut),
    .Config_Clock(Mux9config__Config_Clock),
    .Config_Reset(Mux9config__Config_Reset),
    .select(Mux9config__select)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_10to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__in2;
  wire [31:0] mux_0__in3;
  wire [31:0] mux_0__in4;
  wire [31:0] mux_0__in5;
  wire [31:0] mux_0__in6;
  wire [31:0] mux_0__in7;
  wire [31:0] mux_0__in8;
  wire [31:0] mux_0__in9;
  wire [31:0] mux_0__out;
  wire [3:0] mux_0__select;
  cgrame_mux_10to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .in2(mux_0__in2),
    .in3(mux_0__in3),
    .in4(mux_0__in4),
    .in5(mux_0__in5),
    .in6(mux_0__in6),
    .in7(mux_0__in7),
    .in8(mux_0__in8),
    .in9(mux_0__in9),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_10to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__in2;
  wire [31:0] mux_1__in3;
  wire [31:0] mux_1__in4;
  wire [31:0] mux_1__in5;
  wire [31:0] mux_1__in6;
  wire [31:0] mux_1__in7;
  wire [31:0] mux_1__in8;
  wire [31:0] mux_1__in9;
  wire [31:0] mux_1__out;
  wire [3:0] mux_1__select;
  cgrame_mux_10to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .in2(mux_1__in2),
    .in3(mux_1__in3),
    .in4(mux_1__in4),
    .in5(mux_1__in5),
    .in6(mux_1__in6),
    .in7(mux_1__in7),
    .in8(mux_1__in8),
    .in9(mux_1__in9),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_10to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__in2;
  wire [31:0] mux_2__in3;
  wire [31:0] mux_2__in4;
  wire [31:0] mux_2__in5;
  wire [31:0] mux_2__in6;
  wire [31:0] mux_2__in7;
  wire [31:0] mux_2__in8;
  wire [31:0] mux_2__in9;
  wire [31:0] mux_2__out;
  wire [3:0] mux_2__select;
  cgrame_mux_10to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .in2(mux_2__in2),
    .in3(mux_2__in3),
    .in4(mux_2__in4),
    .in5(mux_2__in5),
    .in6(mux_2__in6),
    .in7(mux_2__in7),
    .in8(mux_2__in8),
    .in9(mux_2__in9),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_10to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__in2;
  wire [31:0] mux_3__in3;
  wire [31:0] mux_3__in4;
  wire [31:0] mux_3__in5;
  wire [31:0] mux_3__in6;
  wire [31:0] mux_3__in7;
  wire [31:0] mux_3__in8;
  wire [31:0] mux_3__in9;
  wire [31:0] mux_3__out;
  wire [3:0] mux_3__select;
  cgrame_mux_10to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .in2(mux_3__in2),
    .in3(mux_3__in3),
    .in4(mux_3__in4),
    .in5(mux_3__in5),
    .in6(mux_3__in6),
    .in7(mux_3__in7),
    .in8(mux_3__in8),
    .in9(mux_3__in9),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_4' (Module cgrame_mux_10to1_32b)
  wire [31:0] mux_4__in0;
  wire [31:0] mux_4__in1;
  wire [31:0] mux_4__in2;
  wire [31:0] mux_4__in3;
  wire [31:0] mux_4__in4;
  wire [31:0] mux_4__in5;
  wire [31:0] mux_4__in6;
  wire [31:0] mux_4__in7;
  wire [31:0] mux_4__in8;
  wire [31:0] mux_4__in9;
  wire [31:0] mux_4__out;
  wire [3:0] mux_4__select;
  cgrame_mux_10to1_32b #(.contexts(1),.size(32)) mux_4(
    .in0(mux_4__in0),
    .in1(mux_4__in1),
    .in2(mux_4__in2),
    .in3(mux_4__in3),
    .in4(mux_4__in4),
    .in5(mux_4__in5),
    .in6(mux_4__in6),
    .in7(mux_4__in7),
    .in8(mux_4__in8),
    .in9(mux_4__in9),
    .out(mux_4__out),
    .select(mux_4__select)
  );

  //Wire declarations for instance 'mux_5' (Module cgrame_mux_10to1_32b)
  wire [31:0] mux_5__in0;
  wire [31:0] mux_5__in1;
  wire [31:0] mux_5__in2;
  wire [31:0] mux_5__in3;
  wire [31:0] mux_5__in4;
  wire [31:0] mux_5__in5;
  wire [31:0] mux_5__in6;
  wire [31:0] mux_5__in7;
  wire [31:0] mux_5__in8;
  wire [31:0] mux_5__in9;
  wire [31:0] mux_5__out;
  wire [3:0] mux_5__select;
  cgrame_mux_10to1_32b #(.contexts(1),.size(32)) mux_5(
    .in0(mux_5__in0),
    .in1(mux_5__in1),
    .in2(mux_5__in2),
    .in3(mux_5__in3),
    .in4(mux_5__in4),
    .in5(mux_5__in5),
    .in6(mux_5__in6),
    .in7(mux_5__in7),
    .in8(mux_5__in8),
    .in9(mux_5__in9),
    .out(mux_5__out),
    .select(mux_5__select)
  );

  //Wire declarations for instance 'mux_6' (Module cgrame_mux_10to1_32b)
  wire [31:0] mux_6__in0;
  wire [31:0] mux_6__in1;
  wire [31:0] mux_6__in2;
  wire [31:0] mux_6__in3;
  wire [31:0] mux_6__in4;
  wire [31:0] mux_6__in5;
  wire [31:0] mux_6__in6;
  wire [31:0] mux_6__in7;
  wire [31:0] mux_6__in8;
  wire [31:0] mux_6__in9;
  wire [31:0] mux_6__out;
  wire [3:0] mux_6__select;
  cgrame_mux_10to1_32b #(.contexts(1),.size(32)) mux_6(
    .in0(mux_6__in0),
    .in1(mux_6__in1),
    .in2(mux_6__in2),
    .in3(mux_6__in3),
    .in4(mux_6__in4),
    .in5(mux_6__in5),
    .in6(mux_6__in6),
    .in7(mux_6__in7),
    .in8(mux_6__in8),
    .in9(mux_6__in9),
    .out(mux_6__out),
    .select(mux_6__select)
  );

  //Wire declarations for instance 'mux_7' (Module cgrame_mux_10to1_32b)
  wire [31:0] mux_7__in0;
  wire [31:0] mux_7__in1;
  wire [31:0] mux_7__in2;
  wire [31:0] mux_7__in3;
  wire [31:0] mux_7__in4;
  wire [31:0] mux_7__in5;
  wire [31:0] mux_7__in6;
  wire [31:0] mux_7__in7;
  wire [31:0] mux_7__in8;
  wire [31:0] mux_7__in9;
  wire [31:0] mux_7__out;
  wire [3:0] mux_7__select;
  cgrame_mux_10to1_32b #(.contexts(1),.size(32)) mux_7(
    .in0(mux_7__in0),
    .in1(mux_7__in1),
    .in2(mux_7__in2),
    .in3(mux_7__in3),
    .in4(mux_7__in4),
    .in5(mux_7__in5),
    .in6(mux_7__in6),
    .in7(mux_7__in7),
    .in8(mux_7__in8),
    .in9(mux_7__in9),
    .out(mux_7__out),
    .select(mux_7__select)
  );

  //Wire declarations for instance 'mux_8' (Module cgrame_mux_10to1_32b)
  wire [31:0] mux_8__in0;
  wire [31:0] mux_8__in1;
  wire [31:0] mux_8__in2;
  wire [31:0] mux_8__in3;
  wire [31:0] mux_8__in4;
  wire [31:0] mux_8__in5;
  wire [31:0] mux_8__in6;
  wire [31:0] mux_8__in7;
  wire [31:0] mux_8__in8;
  wire [31:0] mux_8__in9;
  wire [31:0] mux_8__out;
  wire [3:0] mux_8__select;
  cgrame_mux_10to1_32b #(.contexts(1),.size(32)) mux_8(
    .in0(mux_8__in0),
    .in1(mux_8__in1),
    .in2(mux_8__in2),
    .in3(mux_8__in3),
    .in4(mux_8__in4),
    .in5(mux_8__in5),
    .in6(mux_8__in6),
    .in7(mux_8__in7),
    .in8(mux_8__in8),
    .in9(mux_8__in9),
    .out(mux_8__out),
    .select(mux_8__select)
  );

  //Wire declarations for instance 'mux_9' (Module cgrame_mux_10to1_32b)
  wire [31:0] mux_9__in0;
  wire [31:0] mux_9__in1;
  wire [31:0] mux_9__in2;
  wire [31:0] mux_9__in3;
  wire [31:0] mux_9__in4;
  wire [31:0] mux_9__in5;
  wire [31:0] mux_9__in6;
  wire [31:0] mux_9__in7;
  wire [31:0] mux_9__in8;
  wire [31:0] mux_9__in9;
  wire [31:0] mux_9__out;
  wire [3:0] mux_9__select;
  cgrame_mux_10to1_32b #(.contexts(1),.size(32)) mux_9(
    .in0(mux_9__in0),
    .in1(mux_9__in1),
    .in2(mux_9__in2),
    .in3(mux_9__in3),
    .in4(mux_9__in4),
    .in5(mux_9__in5),
    .in6(mux_9__in6),
    .in7(mux_9__in7),
    .in8(mux_9__in8),
    .in9(mux_9__in9),
    .out(mux_9__out),
    .select(mux_9__select)
  );

  //All the connections
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux4config__CGRA_Clock = CGRA_Clock;
  assign Mux5config__CGRA_Clock = CGRA_Clock;
  assign Mux6config__CGRA_Clock = CGRA_Clock;
  assign Mux7config__CGRA_Clock = CGRA_Clock;
  assign Mux8config__CGRA_Clock = CGRA_Clock;
  assign Mux9config__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux4config__CGRA_Reset = CGRA_Reset;
  assign Mux5config__CGRA_Reset = CGRA_Reset;
  assign Mux6config__CGRA_Reset = CGRA_Reset;
  assign Mux7config__CGRA_Reset = CGRA_Reset;
  assign Mux8config__CGRA_Reset = CGRA_Reset;
  assign Mux9config__CGRA_Reset = CGRA_Reset;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux4config__CGRA_Enable = CGRA_Enable;
  assign Mux5config__CGRA_Enable = CGRA_Enable;
  assign Mux6config__CGRA_Enable = CGRA_Enable;
  assign Mux7config__CGRA_Enable = CGRA_Enable;
  assign Mux8config__CGRA_Enable = CGRA_Enable;
  assign Mux9config__CGRA_Enable = CGRA_Enable;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux4config__Config_Clock = Config_Clock;
  assign Mux5config__Config_Clock = Config_Clock;
  assign Mux6config__Config_Clock = Config_Clock;
  assign Mux7config__Config_Clock = Config_Clock;
  assign Mux8config__Config_Clock = Config_Clock;
  assign Mux9config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux4config__Config_Reset = Config_Reset;
  assign Mux5config__Config_Reset = Config_Reset;
  assign Mux6config__Config_Reset = Config_Reset;
  assign Mux7config__Config_Reset = Config_Reset;
  assign Mux8config__Config_Reset = Config_Reset;
  assign Mux9config__Config_Reset = Config_Reset;
  assign out0[31:0] = mux_0__out[31:0];
  assign out1[31:0] = mux_1__out[31:0];
  assign out2[31:0] = mux_2__out[31:0];
  assign out3[31:0] = mux_3__out[31:0];
  assign out4[31:0] = mux_4__out[31:0];
  assign out5[31:0] = mux_5__out[31:0];
  assign out6[31:0] = mux_6__out[31:0];
  assign out7[31:0] = mux_7__out[31:0];
  assign out8[31:0] = mux_8__out[31:0];
  assign out9[31:0] = mux_9__out[31:0];
  assign mux_1__in0[31:0] = in0[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign mux_2__in0[31:0] = in0[31:0];
  assign mux_3__in0[31:0] = in0[31:0];
  assign mux_4__in0[31:0] = in0[31:0];
  assign mux_5__in0[31:0] = in0[31:0];
  assign mux_6__in0[31:0] = in0[31:0];
  assign mux_7__in0[31:0] = in0[31:0];
  assign mux_8__in0[31:0] = in0[31:0];
  assign mux_9__in0[31:0] = in0[31:0];
  assign mux_1__in1[31:0] = in1[31:0];
  assign mux_0__in1[31:0] = in1[31:0];
  assign mux_2__in1[31:0] = in1[31:0];
  assign mux_3__in1[31:0] = in1[31:0];
  assign mux_4__in1[31:0] = in1[31:0];
  assign mux_5__in1[31:0] = in1[31:0];
  assign mux_6__in1[31:0] = in1[31:0];
  assign mux_7__in1[31:0] = in1[31:0];
  assign mux_8__in1[31:0] = in1[31:0];
  assign mux_9__in1[31:0] = in1[31:0];
  assign mux_1__in2[31:0] = in2[31:0];
  assign mux_0__in2[31:0] = in2[31:0];
  assign mux_2__in2[31:0] = in2[31:0];
  assign mux_3__in2[31:0] = in2[31:0];
  assign mux_4__in2[31:0] = in2[31:0];
  assign mux_5__in2[31:0] = in2[31:0];
  assign mux_6__in2[31:0] = in2[31:0];
  assign mux_7__in2[31:0] = in2[31:0];
  assign mux_8__in2[31:0] = in2[31:0];
  assign mux_9__in2[31:0] = in2[31:0];
  assign mux_1__in3[31:0] = in3[31:0];
  assign mux_0__in3[31:0] = in3[31:0];
  assign mux_2__in3[31:0] = in3[31:0];
  assign mux_3__in3[31:0] = in3[31:0];
  assign mux_4__in3[31:0] = in3[31:0];
  assign mux_5__in3[31:0] = in3[31:0];
  assign mux_6__in3[31:0] = in3[31:0];
  assign mux_7__in3[31:0] = in3[31:0];
  assign mux_8__in3[31:0] = in3[31:0];
  assign mux_9__in3[31:0] = in3[31:0];
  assign mux_1__in4[31:0] = in4[31:0];
  assign mux_0__in4[31:0] = in4[31:0];
  assign mux_2__in4[31:0] = in4[31:0];
  assign mux_3__in4[31:0] = in4[31:0];
  assign mux_4__in4[31:0] = in4[31:0];
  assign mux_5__in4[31:0] = in4[31:0];
  assign mux_6__in4[31:0] = in4[31:0];
  assign mux_7__in4[31:0] = in4[31:0];
  assign mux_8__in4[31:0] = in4[31:0];
  assign mux_9__in4[31:0] = in4[31:0];
  assign mux_1__in5[31:0] = in5[31:0];
  assign mux_0__in5[31:0] = in5[31:0];
  assign mux_2__in5[31:0] = in5[31:0];
  assign mux_3__in5[31:0] = in5[31:0];
  assign mux_4__in5[31:0] = in5[31:0];
  assign mux_5__in5[31:0] = in5[31:0];
  assign mux_6__in5[31:0] = in5[31:0];
  assign mux_7__in5[31:0] = in5[31:0];
  assign mux_8__in5[31:0] = in5[31:0];
  assign mux_9__in5[31:0] = in5[31:0];
  assign mux_1__in6[31:0] = in6[31:0];
  assign mux_0__in6[31:0] = in6[31:0];
  assign mux_2__in6[31:0] = in6[31:0];
  assign mux_3__in6[31:0] = in6[31:0];
  assign mux_4__in6[31:0] = in6[31:0];
  assign mux_5__in6[31:0] = in6[31:0];
  assign mux_6__in6[31:0] = in6[31:0];
  assign mux_7__in6[31:0] = in6[31:0];
  assign mux_8__in6[31:0] = in6[31:0];
  assign mux_9__in6[31:0] = in6[31:0];
  assign mux_1__in7[31:0] = in7[31:0];
  assign mux_0__in7[31:0] = in7[31:0];
  assign mux_2__in7[31:0] = in7[31:0];
  assign mux_3__in7[31:0] = in7[31:0];
  assign mux_4__in7[31:0] = in7[31:0];
  assign mux_5__in7[31:0] = in7[31:0];
  assign mux_6__in7[31:0] = in7[31:0];
  assign mux_7__in7[31:0] = in7[31:0];
  assign mux_8__in7[31:0] = in7[31:0];
  assign mux_9__in7[31:0] = in7[31:0];
  assign mux_1__in8[31:0] = in8[31:0];
  assign mux_0__in8[31:0] = in8[31:0];
  assign mux_2__in8[31:0] = in8[31:0];
  assign mux_3__in8[31:0] = in8[31:0];
  assign mux_4__in8[31:0] = in8[31:0];
  assign mux_5__in8[31:0] = in8[31:0];
  assign mux_6__in8[31:0] = in8[31:0];
  assign mux_7__in8[31:0] = in8[31:0];
  assign mux_8__in8[31:0] = in8[31:0];
  assign mux_9__in8[31:0] = in8[31:0];
  assign mux_1__in9[31:0] = in9[31:0];
  assign mux_0__in9[31:0] = in9[31:0];
  assign mux_2__in9[31:0] = in9[31:0];
  assign mux_3__in9[31:0] = in9[31:0];
  assign mux_4__in9[31:0] = in9[31:0];
  assign mux_5__in9[31:0] = in9[31:0];
  assign mux_6__in9[31:0] = in9[31:0];
  assign mux_7__in9[31:0] = in9[31:0];
  assign mux_8__in9[31:0] = in9[31:0];
  assign mux_9__in9[31:0] = in9[31:0];
  assign mux_0__select[3:0] = Mux0config__select[3:0];
  assign mux_1__select[3:0] = Mux1config__select[3:0];
  assign mux_2__select[3:0] = Mux2config__select[3:0];
  assign mux_3__select[3:0] = Mux3config__select[3:0];
  assign mux_4__select[3:0] = Mux4config__select[3:0];
  assign mux_5__select[3:0] = Mux5config__select[3:0];
  assign mux_6__select[3:0] = Mux6config__select[3:0];
  assign mux_7__select[3:0] = Mux7config__select[3:0];
  assign mux_8__select[3:0] = Mux8config__select[3:0];
  assign mux_9__select[3:0] = Mux9config__select[3:0];
  assign Mux0config__ConfigIn = ConfigIn;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux4config__ConfigIn = Mux3config__ConfigOut;
  assign Mux5config__ConfigIn = Mux4config__ConfigOut;
  assign Mux6config__ConfigIn = Mux5config__ConfigOut;
  assign Mux7config__ConfigIn = Mux6config__ConfigOut;
  assign Mux8config__ConfigIn = Mux7config__ConfigOut;
  assign Mux9config__ConfigIn = Mux8config__ConfigOut;
  assign ConfigOut = Mux9config__ConfigOut;

endmodule //crossbar_10in_10out_U20

module cgrame_mux_2to1_32b #(parameter contexts=1, parameter size=1) (
  in0,
  in1,
  out,
  select
);
    input [size-1:0] in0;
    input [size-1:0] in1;
    output reg [size-1:0] out;
    input [0:0] select;
    always @(*)
        case (select)
            0: out = in0;
            1: out = in1;
            default: out = {size{1'bx}};
        endcase

endmodule //cgrame_mux_2to1_32b

module cgrame_mux_4to1_32b #(parameter contexts=1, parameter size=1) (
  in0,
  in1,
  in2,
  in3,
  out,
  select
);
    input [size-1:0] in0;
    input [size-1:0] in1;
    input [size-1:0] in2;
    input [size-1:0] in3;
    output reg [size-1:0] out;
    input [1:0] select;
    always @(*)
        case (select)
            0: out = in0;
            1: out = in1;
            2: out = in2;
            3: out = in3;
            default: out = {size{1'bx}};
        endcase

endmodule //cgrame_mux_4to1_32b

module memoryPort_4connect_32b_U2 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  output [31:0] mem_unit_addr_to_ram,
  output [31:0] mem_unit_data_in_to_ram,
  input [31:0] mem_unit_data_out_from_ram,
  output [0:0] mem_unit_w_rq_to_ram,
  output [31:0] out
);
  //Wire declarations for instance 'MuxAddr' (Module cgrame_configcell_context1_size2)
  wire  MuxAddr__CGRA_Clock;
  wire  MuxAddr__CGRA_Enable;
  wire  MuxAddr__CGRA_Reset;
  wire  MuxAddr__ConfigIn;
  wire  MuxAddr__ConfigOut;
  wire  MuxAddr__Config_Clock;
  wire  MuxAddr__Config_Reset;
  wire [1:0] MuxAddr__select;
  cgrame_configcell_context1_size2 #(.contexts(1),.size(2)) MuxAddr(
    .CGRA_Clock(MuxAddr__CGRA_Clock),
    .CGRA_Enable(MuxAddr__CGRA_Enable),
    .CGRA_Reset(MuxAddr__CGRA_Reset),
    .ConfigIn(MuxAddr__ConfigIn),
    .ConfigOut(MuxAddr__ConfigOut),
    .Config_Clock(MuxAddr__Config_Clock),
    .Config_Reset(MuxAddr__Config_Reset),
    .select(MuxAddr__select)
  );

  //Wire declarations for instance 'MuxData' (Module cgrame_configcell_context1_size2)
  wire  MuxData__CGRA_Clock;
  wire  MuxData__CGRA_Enable;
  wire  MuxData__CGRA_Reset;
  wire  MuxData__ConfigIn;
  wire  MuxData__ConfigOut;
  wire  MuxData__Config_Clock;
  wire  MuxData__Config_Reset;
  wire [1:0] MuxData__select;
  cgrame_configcell_context1_size2 #(.contexts(1),.size(2)) MuxData(
    .CGRA_Clock(MuxData__CGRA_Clock),
    .CGRA_Enable(MuxData__CGRA_Enable),
    .CGRA_Reset(MuxData__CGRA_Reset),
    .ConfigIn(MuxData__ConfigIn),
    .ConfigOut(MuxData__ConfigOut),
    .Config_Clock(MuxData__Config_Clock),
    .Config_Reset(MuxData__Config_Reset),
    .select(MuxData__select)
  );

  //Wire declarations for instance 'WriteRq' (Module cgrame_configcell_context1_size1)
  wire  WriteRq__CGRA_Clock;
  wire  WriteRq__CGRA_Enable;
  wire  WriteRq__CGRA_Reset;
  wire  WriteRq__ConfigIn;
  wire  WriteRq__ConfigOut;
  wire  WriteRq__Config_Clock;
  wire  WriteRq__Config_Reset;
  wire [0:0] WriteRq__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) WriteRq(
    .CGRA_Clock(WriteRq__CGRA_Clock),
    .CGRA_Enable(WriteRq__CGRA_Enable),
    .CGRA_Reset(WriteRq__CGRA_Reset),
    .ConfigIn(WriteRq__ConfigIn),
    .ConfigOut(WriteRq__ConfigOut),
    .Config_Clock(WriteRq__Config_Clock),
    .Config_Reset(WriteRq__Config_Reset),
    .select(WriteRq__select)
  );

  //Wire declarations for instance 'mem_unit' (Module cgrame_memUnit_32b)
  wire [31:0] mem_unit__addr;
  wire [31:0] mem_unit__addr_to_ram;
  wire [31:0] mem_unit__data_in;
  wire [31:0] mem_unit__data_in_to_ram;
  wire [31:0] mem_unit__data_out;
  wire [31:0] mem_unit__data_out_from_ram;
  wire [0:0] mem_unit__w_rq;
  wire [0:0] mem_unit__w_rq_to_ram;
  cgrame_memUnit_32b #(.contexts(1),.size(32)) mem_unit(
    .addr(mem_unit__addr),
    .addr_to_ram(mem_unit__addr_to_ram),
    .data_in(mem_unit__data_in),
    .data_in_to_ram(mem_unit__data_in_to_ram),
    .data_out(mem_unit__data_out),
    .data_out_from_ram(mem_unit__data_out_from_ram),
    .w_rq(mem_unit__w_rq),
    .w_rq_to_ram(mem_unit__w_rq_to_ram)
  );

  //Wire declarations for instance 'mux_addr' (Module cgrame_mux_4to1_32b)
  wire [31:0] mux_addr__in0;
  wire [31:0] mux_addr__in1;
  wire [31:0] mux_addr__in2;
  wire [31:0] mux_addr__in3;
  wire [31:0] mux_addr__out;
  wire [1:0] mux_addr__select;
  cgrame_mux_4to1_32b #(.contexts(1),.size(32)) mux_addr(
    .in0(mux_addr__in0),
    .in1(mux_addr__in1),
    .in2(mux_addr__in2),
    .in3(mux_addr__in3),
    .out(mux_addr__out),
    .select(mux_addr__select)
  );

  //Wire declarations for instance 'mux_data' (Module cgrame_mux_4to1_32b)
  wire [31:0] mux_data__in0;
  wire [31:0] mux_data__in1;
  wire [31:0] mux_data__in2;
  wire [31:0] mux_data__in3;
  wire [31:0] mux_data__out;
  wire [1:0] mux_data__select;
  cgrame_mux_4to1_32b #(.contexts(1),.size(32)) mux_data(
    .in0(mux_data__in0),
    .in1(mux_data__in1),
    .in2(mux_data__in2),
    .in3(mux_data__in3),
    .out(mux_data__out),
    .select(mux_data__select)
  );

  //All the connections
  assign MuxAddr__CGRA_Reset = CGRA_Reset;
  assign MuxAddr__CGRA_Clock = CGRA_Clock;
  assign MuxData__CGRA_Clock = CGRA_Clock;
  assign WriteRq__CGRA_Clock = CGRA_Clock;
  assign MuxAddr__CGRA_Enable = CGRA_Enable;
  assign MuxData__CGRA_Reset = CGRA_Reset;
  assign WriteRq__CGRA_Reset = CGRA_Reset;
  assign MuxAddr__Config_Clock = Config_Clock;
  assign MuxData__CGRA_Enable = CGRA_Enable;
  assign WriteRq__CGRA_Enable = CGRA_Enable;
  assign MuxAddr__Config_Reset = Config_Reset;
  assign MuxData__Config_Clock = Config_Clock;
  assign WriteRq__Config_Clock = Config_Clock;
  assign MuxData__Config_Reset = Config_Reset;
  assign WriteRq__Config_Reset = Config_Reset;
  assign mux_addr__in0[31:0] = in0[31:0];
  assign mux_data__in0[31:0] = in0[31:0];
  assign mux_addr__in1[31:0] = in1[31:0];
  assign mux_data__in1[31:0] = in1[31:0];
  assign mux_addr__in2[31:0] = in2[31:0];
  assign mux_data__in2[31:0] = in2[31:0];
  assign mux_addr__in3[31:0] = in3[31:0];
  assign mux_data__in3[31:0] = in3[31:0];
  assign out[31:0] = mem_unit__data_out[31:0];
  assign mem_unit__addr[31:0] = mux_addr__out[31:0];
  assign mem_unit__data_in[31:0] = mux_data__out[31:0];
  assign mux_addr__select[1:0] = MuxAddr__select[1:0];
  assign mux_data__select[1:0] = MuxData__select[1:0];
  assign mem_unit__w_rq[0:0] = WriteRq__select[0:0];
  assign MuxAddr__ConfigIn = ConfigIn;
  assign MuxData__ConfigIn = MuxAddr__ConfigOut;
  assign WriteRq__ConfigIn = MuxData__ConfigOut;
  assign ConfigOut = WriteRq__ConfigOut;
  assign mem_unit_data_in_to_ram[31:0] = mem_unit__data_in_to_ram[31:0];
  assign mem_unit_addr_to_ram[31:0] = mem_unit__addr_to_ram[31:0];
  assign mem_unit__data_out_from_ram[31:0] = mem_unit_data_out_from_ram[31:0];
  assign mem_unit_w_rq_to_ram[0:0] = mem_unit__w_rq_to_ram[0:0];

endmodule //memoryPort_4connect_32b_U2

module cgrame_mux_7to1_32b #(parameter contexts=1, parameter size=1) (
  in0,
  in1,
  in2,
  in3,
  in4,
  in5,
  in6,
  out,
  select
);
    input [size-1:0] in0;
    input [size-1:0] in1;
    input [size-1:0] in2;
    input [size-1:0] in3;
    input [size-1:0] in4;
    input [size-1:0] in5;
    input [size-1:0] in6;
    output reg [size-1:0] out;
    input [2:0] select;
    always @(*)
        case (select)
            0: out = in0;
            1: out = in1;
            2: out = in2;
            3: out = in3;
            4: out = in4;
            5: out = in5;
            6: out = in6;
            default: out = {size{1'bx}};
        endcase

endmodule //cgrame_mux_7to1_32b

module crossbar_7in_7out_U19 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in4,
  input [31:0] in5,
  input [31:0] in6,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out4,
  output [31:0] out5,
  output [31:0] out6
);
  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size3)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [2:0] Mux0config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size3)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [2:0] Mux1config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size3)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [2:0] Mux2config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size3)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [2:0] Mux3config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux4config' (Module cgrame_configcell_context1_size3)
  wire  Mux4config__CGRA_Clock;
  wire  Mux4config__CGRA_Enable;
  wire  Mux4config__CGRA_Reset;
  wire  Mux4config__ConfigIn;
  wire  Mux4config__ConfigOut;
  wire  Mux4config__Config_Clock;
  wire  Mux4config__Config_Reset;
  wire [2:0] Mux4config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux4config(
    .CGRA_Clock(Mux4config__CGRA_Clock),
    .CGRA_Enable(Mux4config__CGRA_Enable),
    .CGRA_Reset(Mux4config__CGRA_Reset),
    .ConfigIn(Mux4config__ConfigIn),
    .ConfigOut(Mux4config__ConfigOut),
    .Config_Clock(Mux4config__Config_Clock),
    .Config_Reset(Mux4config__Config_Reset),
    .select(Mux4config__select)
  );

  //Wire declarations for instance 'Mux5config' (Module cgrame_configcell_context1_size3)
  wire  Mux5config__CGRA_Clock;
  wire  Mux5config__CGRA_Enable;
  wire  Mux5config__CGRA_Reset;
  wire  Mux5config__ConfigIn;
  wire  Mux5config__ConfigOut;
  wire  Mux5config__Config_Clock;
  wire  Mux5config__Config_Reset;
  wire [2:0] Mux5config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux5config(
    .CGRA_Clock(Mux5config__CGRA_Clock),
    .CGRA_Enable(Mux5config__CGRA_Enable),
    .CGRA_Reset(Mux5config__CGRA_Reset),
    .ConfigIn(Mux5config__ConfigIn),
    .ConfigOut(Mux5config__ConfigOut),
    .Config_Clock(Mux5config__Config_Clock),
    .Config_Reset(Mux5config__Config_Reset),
    .select(Mux5config__select)
  );

  //Wire declarations for instance 'Mux6config' (Module cgrame_configcell_context1_size3)
  wire  Mux6config__CGRA_Clock;
  wire  Mux6config__CGRA_Enable;
  wire  Mux6config__CGRA_Reset;
  wire  Mux6config__ConfigIn;
  wire  Mux6config__ConfigOut;
  wire  Mux6config__Config_Clock;
  wire  Mux6config__Config_Reset;
  wire [2:0] Mux6config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux6config(
    .CGRA_Clock(Mux6config__CGRA_Clock),
    .CGRA_Enable(Mux6config__CGRA_Enable),
    .CGRA_Reset(Mux6config__CGRA_Reset),
    .ConfigIn(Mux6config__ConfigIn),
    .ConfigOut(Mux6config__ConfigOut),
    .Config_Clock(Mux6config__Config_Clock),
    .Config_Reset(Mux6config__Config_Reset),
    .select(Mux6config__select)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_7to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__in2;
  wire [31:0] mux_0__in3;
  wire [31:0] mux_0__in4;
  wire [31:0] mux_0__in5;
  wire [31:0] mux_0__in6;
  wire [31:0] mux_0__out;
  wire [2:0] mux_0__select;
  cgrame_mux_7to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .in2(mux_0__in2),
    .in3(mux_0__in3),
    .in4(mux_0__in4),
    .in5(mux_0__in5),
    .in6(mux_0__in6),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_7to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__in2;
  wire [31:0] mux_1__in3;
  wire [31:0] mux_1__in4;
  wire [31:0] mux_1__in5;
  wire [31:0] mux_1__in6;
  wire [31:0] mux_1__out;
  wire [2:0] mux_1__select;
  cgrame_mux_7to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .in2(mux_1__in2),
    .in3(mux_1__in3),
    .in4(mux_1__in4),
    .in5(mux_1__in5),
    .in6(mux_1__in6),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_7to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__in2;
  wire [31:0] mux_2__in3;
  wire [31:0] mux_2__in4;
  wire [31:0] mux_2__in5;
  wire [31:0] mux_2__in6;
  wire [31:0] mux_2__out;
  wire [2:0] mux_2__select;
  cgrame_mux_7to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .in2(mux_2__in2),
    .in3(mux_2__in3),
    .in4(mux_2__in4),
    .in5(mux_2__in5),
    .in6(mux_2__in6),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_7to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__in2;
  wire [31:0] mux_3__in3;
  wire [31:0] mux_3__in4;
  wire [31:0] mux_3__in5;
  wire [31:0] mux_3__in6;
  wire [31:0] mux_3__out;
  wire [2:0] mux_3__select;
  cgrame_mux_7to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .in2(mux_3__in2),
    .in3(mux_3__in3),
    .in4(mux_3__in4),
    .in5(mux_3__in5),
    .in6(mux_3__in6),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_4' (Module cgrame_mux_7to1_32b)
  wire [31:0] mux_4__in0;
  wire [31:0] mux_4__in1;
  wire [31:0] mux_4__in2;
  wire [31:0] mux_4__in3;
  wire [31:0] mux_4__in4;
  wire [31:0] mux_4__in5;
  wire [31:0] mux_4__in6;
  wire [31:0] mux_4__out;
  wire [2:0] mux_4__select;
  cgrame_mux_7to1_32b #(.contexts(1),.size(32)) mux_4(
    .in0(mux_4__in0),
    .in1(mux_4__in1),
    .in2(mux_4__in2),
    .in3(mux_4__in3),
    .in4(mux_4__in4),
    .in5(mux_4__in5),
    .in6(mux_4__in6),
    .out(mux_4__out),
    .select(mux_4__select)
  );

  //Wire declarations for instance 'mux_5' (Module cgrame_mux_7to1_32b)
  wire [31:0] mux_5__in0;
  wire [31:0] mux_5__in1;
  wire [31:0] mux_5__in2;
  wire [31:0] mux_5__in3;
  wire [31:0] mux_5__in4;
  wire [31:0] mux_5__in5;
  wire [31:0] mux_5__in6;
  wire [31:0] mux_5__out;
  wire [2:0] mux_5__select;
  cgrame_mux_7to1_32b #(.contexts(1),.size(32)) mux_5(
    .in0(mux_5__in0),
    .in1(mux_5__in1),
    .in2(mux_5__in2),
    .in3(mux_5__in3),
    .in4(mux_5__in4),
    .in5(mux_5__in5),
    .in6(mux_5__in6),
    .out(mux_5__out),
    .select(mux_5__select)
  );

  //Wire declarations for instance 'mux_6' (Module cgrame_mux_7to1_32b)
  wire [31:0] mux_6__in0;
  wire [31:0] mux_6__in1;
  wire [31:0] mux_6__in2;
  wire [31:0] mux_6__in3;
  wire [31:0] mux_6__in4;
  wire [31:0] mux_6__in5;
  wire [31:0] mux_6__in6;
  wire [31:0] mux_6__out;
  wire [2:0] mux_6__select;
  cgrame_mux_7to1_32b #(.contexts(1),.size(32)) mux_6(
    .in0(mux_6__in0),
    .in1(mux_6__in1),
    .in2(mux_6__in2),
    .in3(mux_6__in3),
    .in4(mux_6__in4),
    .in5(mux_6__in5),
    .in6(mux_6__in6),
    .out(mux_6__out),
    .select(mux_6__select)
  );

  //All the connections
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux4config__CGRA_Clock = CGRA_Clock;
  assign Mux5config__CGRA_Clock = CGRA_Clock;
  assign Mux6config__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux4config__CGRA_Reset = CGRA_Reset;
  assign Mux5config__CGRA_Reset = CGRA_Reset;
  assign Mux6config__CGRA_Reset = CGRA_Reset;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux4config__CGRA_Enable = CGRA_Enable;
  assign Mux5config__CGRA_Enable = CGRA_Enable;
  assign Mux6config__CGRA_Enable = CGRA_Enable;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux4config__Config_Clock = Config_Clock;
  assign Mux5config__Config_Clock = Config_Clock;
  assign Mux6config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux4config__Config_Reset = Config_Reset;
  assign Mux5config__Config_Reset = Config_Reset;
  assign Mux6config__Config_Reset = Config_Reset;
  assign out0[31:0] = mux_0__out[31:0];
  assign out1[31:0] = mux_1__out[31:0];
  assign out2[31:0] = mux_2__out[31:0];
  assign out3[31:0] = mux_3__out[31:0];
  assign out4[31:0] = mux_4__out[31:0];
  assign out5[31:0] = mux_5__out[31:0];
  assign out6[31:0] = mux_6__out[31:0];
  assign mux_1__in0[31:0] = in0[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign mux_2__in0[31:0] = in0[31:0];
  assign mux_3__in0[31:0] = in0[31:0];
  assign mux_4__in0[31:0] = in0[31:0];
  assign mux_5__in0[31:0] = in0[31:0];
  assign mux_6__in0[31:0] = in0[31:0];
  assign mux_1__in1[31:0] = in1[31:0];
  assign mux_0__in1[31:0] = in1[31:0];
  assign mux_2__in1[31:0] = in1[31:0];
  assign mux_3__in1[31:0] = in1[31:0];
  assign mux_4__in1[31:0] = in1[31:0];
  assign mux_5__in1[31:0] = in1[31:0];
  assign mux_6__in1[31:0] = in1[31:0];
  assign mux_1__in2[31:0] = in2[31:0];
  assign mux_0__in2[31:0] = in2[31:0];
  assign mux_2__in2[31:0] = in2[31:0];
  assign mux_3__in2[31:0] = in2[31:0];
  assign mux_4__in2[31:0] = in2[31:0];
  assign mux_5__in2[31:0] = in2[31:0];
  assign mux_6__in2[31:0] = in2[31:0];
  assign mux_1__in3[31:0] = in3[31:0];
  assign mux_0__in3[31:0] = in3[31:0];
  assign mux_2__in3[31:0] = in3[31:0];
  assign mux_3__in3[31:0] = in3[31:0];
  assign mux_4__in3[31:0] = in3[31:0];
  assign mux_5__in3[31:0] = in3[31:0];
  assign mux_6__in3[31:0] = in3[31:0];
  assign mux_1__in4[31:0] = in4[31:0];
  assign mux_0__in4[31:0] = in4[31:0];
  assign mux_2__in4[31:0] = in4[31:0];
  assign mux_3__in4[31:0] = in4[31:0];
  assign mux_4__in4[31:0] = in4[31:0];
  assign mux_5__in4[31:0] = in4[31:0];
  assign mux_6__in4[31:0] = in4[31:0];
  assign mux_1__in5[31:0] = in5[31:0];
  assign mux_0__in5[31:0] = in5[31:0];
  assign mux_2__in5[31:0] = in5[31:0];
  assign mux_3__in5[31:0] = in5[31:0];
  assign mux_4__in5[31:0] = in5[31:0];
  assign mux_5__in5[31:0] = in5[31:0];
  assign mux_6__in5[31:0] = in5[31:0];
  assign mux_1__in6[31:0] = in6[31:0];
  assign mux_0__in6[31:0] = in6[31:0];
  assign mux_2__in6[31:0] = in6[31:0];
  assign mux_3__in6[31:0] = in6[31:0];
  assign mux_4__in6[31:0] = in6[31:0];
  assign mux_5__in6[31:0] = in6[31:0];
  assign mux_6__in6[31:0] = in6[31:0];
  assign mux_0__select[2:0] = Mux0config__select[2:0];
  assign mux_1__select[2:0] = Mux1config__select[2:0];
  assign mux_2__select[2:0] = Mux2config__select[2:0];
  assign mux_3__select[2:0] = Mux3config__select[2:0];
  assign mux_4__select[2:0] = Mux4config__select[2:0];
  assign mux_5__select[2:0] = Mux5config__select[2:0];
  assign mux_6__select[2:0] = Mux6config__select[2:0];
  assign Mux0config__ConfigIn = ConfigIn;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux4config__ConfigIn = Mux3config__ConfigOut;
  assign Mux5config__ConfigIn = Mux4config__ConfigOut;
  assign Mux6config__ConfigIn = Mux5config__ConfigOut;
  assign ConfigOut = Mux6config__ConfigOut;

endmodule //crossbar_7in_7out_U19

module cgrame_mux_8to1_32b #(parameter contexts=1, parameter size=1) (
  in0,
  in1,
  in2,
  in3,
  in4,
  in5,
  in6,
  in7,
  out,
  select
);
    input [size-1:0] in0;
    input [size-1:0] in1;
    input [size-1:0] in2;
    input [size-1:0] in3;
    input [size-1:0] in4;
    input [size-1:0] in5;
    input [size-1:0] in6;
    input [size-1:0] in7;
    output reg [size-1:0] out;
    input [2:0] select;
    always @(*)
        case (select)
            0: out = in0;
            1: out = in1;
            2: out = in2;
            3: out = in3;
            4: out = in4;
            5: out = in5;
            6: out = in6;
            7: out = in7;
            default: out = {size{1'bx}};
        endcase

endmodule //cgrame_mux_8to1_32b

module crossbar_8in_8out_U16 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in4,
  input [31:0] in5,
  input [31:0] in6,
  input [31:0] in7,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out4,
  output [31:0] out5,
  output [31:0] out6,
  output [31:0] out7
);
  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size3)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [2:0] Mux0config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size3)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [2:0] Mux1config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size3)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [2:0] Mux2config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size3)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [2:0] Mux3config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux4config' (Module cgrame_configcell_context1_size3)
  wire  Mux4config__CGRA_Clock;
  wire  Mux4config__CGRA_Enable;
  wire  Mux4config__CGRA_Reset;
  wire  Mux4config__ConfigIn;
  wire  Mux4config__ConfigOut;
  wire  Mux4config__Config_Clock;
  wire  Mux4config__Config_Reset;
  wire [2:0] Mux4config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux4config(
    .CGRA_Clock(Mux4config__CGRA_Clock),
    .CGRA_Enable(Mux4config__CGRA_Enable),
    .CGRA_Reset(Mux4config__CGRA_Reset),
    .ConfigIn(Mux4config__ConfigIn),
    .ConfigOut(Mux4config__ConfigOut),
    .Config_Clock(Mux4config__Config_Clock),
    .Config_Reset(Mux4config__Config_Reset),
    .select(Mux4config__select)
  );

  //Wire declarations for instance 'Mux5config' (Module cgrame_configcell_context1_size3)
  wire  Mux5config__CGRA_Clock;
  wire  Mux5config__CGRA_Enable;
  wire  Mux5config__CGRA_Reset;
  wire  Mux5config__ConfigIn;
  wire  Mux5config__ConfigOut;
  wire  Mux5config__Config_Clock;
  wire  Mux5config__Config_Reset;
  wire [2:0] Mux5config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux5config(
    .CGRA_Clock(Mux5config__CGRA_Clock),
    .CGRA_Enable(Mux5config__CGRA_Enable),
    .CGRA_Reset(Mux5config__CGRA_Reset),
    .ConfigIn(Mux5config__ConfigIn),
    .ConfigOut(Mux5config__ConfigOut),
    .Config_Clock(Mux5config__Config_Clock),
    .Config_Reset(Mux5config__Config_Reset),
    .select(Mux5config__select)
  );

  //Wire declarations for instance 'Mux6config' (Module cgrame_configcell_context1_size3)
  wire  Mux6config__CGRA_Clock;
  wire  Mux6config__CGRA_Enable;
  wire  Mux6config__CGRA_Reset;
  wire  Mux6config__ConfigIn;
  wire  Mux6config__ConfigOut;
  wire  Mux6config__Config_Clock;
  wire  Mux6config__Config_Reset;
  wire [2:0] Mux6config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux6config(
    .CGRA_Clock(Mux6config__CGRA_Clock),
    .CGRA_Enable(Mux6config__CGRA_Enable),
    .CGRA_Reset(Mux6config__CGRA_Reset),
    .ConfigIn(Mux6config__ConfigIn),
    .ConfigOut(Mux6config__ConfigOut),
    .Config_Clock(Mux6config__Config_Clock),
    .Config_Reset(Mux6config__Config_Reset),
    .select(Mux6config__select)
  );

  //Wire declarations for instance 'Mux7config' (Module cgrame_configcell_context1_size3)
  wire  Mux7config__CGRA_Clock;
  wire  Mux7config__CGRA_Enable;
  wire  Mux7config__CGRA_Reset;
  wire  Mux7config__ConfigIn;
  wire  Mux7config__ConfigOut;
  wire  Mux7config__Config_Clock;
  wire  Mux7config__Config_Reset;
  wire [2:0] Mux7config__select;
  cgrame_configcell_context1_size3 #(.contexts(1),.size(3)) Mux7config(
    .CGRA_Clock(Mux7config__CGRA_Clock),
    .CGRA_Enable(Mux7config__CGRA_Enable),
    .CGRA_Reset(Mux7config__CGRA_Reset),
    .ConfigIn(Mux7config__ConfigIn),
    .ConfigOut(Mux7config__ConfigOut),
    .Config_Clock(Mux7config__Config_Clock),
    .Config_Reset(Mux7config__Config_Reset),
    .select(Mux7config__select)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_8to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__in2;
  wire [31:0] mux_0__in3;
  wire [31:0] mux_0__in4;
  wire [31:0] mux_0__in5;
  wire [31:0] mux_0__in6;
  wire [31:0] mux_0__in7;
  wire [31:0] mux_0__out;
  wire [2:0] mux_0__select;
  cgrame_mux_8to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .in2(mux_0__in2),
    .in3(mux_0__in3),
    .in4(mux_0__in4),
    .in5(mux_0__in5),
    .in6(mux_0__in6),
    .in7(mux_0__in7),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_8to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__in2;
  wire [31:0] mux_1__in3;
  wire [31:0] mux_1__in4;
  wire [31:0] mux_1__in5;
  wire [31:0] mux_1__in6;
  wire [31:0] mux_1__in7;
  wire [31:0] mux_1__out;
  wire [2:0] mux_1__select;
  cgrame_mux_8to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .in2(mux_1__in2),
    .in3(mux_1__in3),
    .in4(mux_1__in4),
    .in5(mux_1__in5),
    .in6(mux_1__in6),
    .in7(mux_1__in7),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_8to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__in2;
  wire [31:0] mux_2__in3;
  wire [31:0] mux_2__in4;
  wire [31:0] mux_2__in5;
  wire [31:0] mux_2__in6;
  wire [31:0] mux_2__in7;
  wire [31:0] mux_2__out;
  wire [2:0] mux_2__select;
  cgrame_mux_8to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .in2(mux_2__in2),
    .in3(mux_2__in3),
    .in4(mux_2__in4),
    .in5(mux_2__in5),
    .in6(mux_2__in6),
    .in7(mux_2__in7),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_8to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__in2;
  wire [31:0] mux_3__in3;
  wire [31:0] mux_3__in4;
  wire [31:0] mux_3__in5;
  wire [31:0] mux_3__in6;
  wire [31:0] mux_3__in7;
  wire [31:0] mux_3__out;
  wire [2:0] mux_3__select;
  cgrame_mux_8to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .in2(mux_3__in2),
    .in3(mux_3__in3),
    .in4(mux_3__in4),
    .in5(mux_3__in5),
    .in6(mux_3__in6),
    .in7(mux_3__in7),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_4' (Module cgrame_mux_8to1_32b)
  wire [31:0] mux_4__in0;
  wire [31:0] mux_4__in1;
  wire [31:0] mux_4__in2;
  wire [31:0] mux_4__in3;
  wire [31:0] mux_4__in4;
  wire [31:0] mux_4__in5;
  wire [31:0] mux_4__in6;
  wire [31:0] mux_4__in7;
  wire [31:0] mux_4__out;
  wire [2:0] mux_4__select;
  cgrame_mux_8to1_32b #(.contexts(1),.size(32)) mux_4(
    .in0(mux_4__in0),
    .in1(mux_4__in1),
    .in2(mux_4__in2),
    .in3(mux_4__in3),
    .in4(mux_4__in4),
    .in5(mux_4__in5),
    .in6(mux_4__in6),
    .in7(mux_4__in7),
    .out(mux_4__out),
    .select(mux_4__select)
  );

  //Wire declarations for instance 'mux_5' (Module cgrame_mux_8to1_32b)
  wire [31:0] mux_5__in0;
  wire [31:0] mux_5__in1;
  wire [31:0] mux_5__in2;
  wire [31:0] mux_5__in3;
  wire [31:0] mux_5__in4;
  wire [31:0] mux_5__in5;
  wire [31:0] mux_5__in6;
  wire [31:0] mux_5__in7;
  wire [31:0] mux_5__out;
  wire [2:0] mux_5__select;
  cgrame_mux_8to1_32b #(.contexts(1),.size(32)) mux_5(
    .in0(mux_5__in0),
    .in1(mux_5__in1),
    .in2(mux_5__in2),
    .in3(mux_5__in3),
    .in4(mux_5__in4),
    .in5(mux_5__in5),
    .in6(mux_5__in6),
    .in7(mux_5__in7),
    .out(mux_5__out),
    .select(mux_5__select)
  );

  //Wire declarations for instance 'mux_6' (Module cgrame_mux_8to1_32b)
  wire [31:0] mux_6__in0;
  wire [31:0] mux_6__in1;
  wire [31:0] mux_6__in2;
  wire [31:0] mux_6__in3;
  wire [31:0] mux_6__in4;
  wire [31:0] mux_6__in5;
  wire [31:0] mux_6__in6;
  wire [31:0] mux_6__in7;
  wire [31:0] mux_6__out;
  wire [2:0] mux_6__select;
  cgrame_mux_8to1_32b #(.contexts(1),.size(32)) mux_6(
    .in0(mux_6__in0),
    .in1(mux_6__in1),
    .in2(mux_6__in2),
    .in3(mux_6__in3),
    .in4(mux_6__in4),
    .in5(mux_6__in5),
    .in6(mux_6__in6),
    .in7(mux_6__in7),
    .out(mux_6__out),
    .select(mux_6__select)
  );

  //Wire declarations for instance 'mux_7' (Module cgrame_mux_8to1_32b)
  wire [31:0] mux_7__in0;
  wire [31:0] mux_7__in1;
  wire [31:0] mux_7__in2;
  wire [31:0] mux_7__in3;
  wire [31:0] mux_7__in4;
  wire [31:0] mux_7__in5;
  wire [31:0] mux_7__in6;
  wire [31:0] mux_7__in7;
  wire [31:0] mux_7__out;
  wire [2:0] mux_7__select;
  cgrame_mux_8to1_32b #(.contexts(1),.size(32)) mux_7(
    .in0(mux_7__in0),
    .in1(mux_7__in1),
    .in2(mux_7__in2),
    .in3(mux_7__in3),
    .in4(mux_7__in4),
    .in5(mux_7__in5),
    .in6(mux_7__in6),
    .in7(mux_7__in7),
    .out(mux_7__out),
    .select(mux_7__select)
  );

  //All the connections
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux4config__CGRA_Clock = CGRA_Clock;
  assign Mux5config__CGRA_Clock = CGRA_Clock;
  assign Mux6config__CGRA_Clock = CGRA_Clock;
  assign Mux7config__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux4config__CGRA_Reset = CGRA_Reset;
  assign Mux5config__CGRA_Reset = CGRA_Reset;
  assign Mux6config__CGRA_Reset = CGRA_Reset;
  assign Mux7config__CGRA_Reset = CGRA_Reset;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux4config__CGRA_Enable = CGRA_Enable;
  assign Mux5config__CGRA_Enable = CGRA_Enable;
  assign Mux6config__CGRA_Enable = CGRA_Enable;
  assign Mux7config__CGRA_Enable = CGRA_Enable;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux4config__Config_Clock = Config_Clock;
  assign Mux5config__Config_Clock = Config_Clock;
  assign Mux6config__Config_Clock = Config_Clock;
  assign Mux7config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux4config__Config_Reset = Config_Reset;
  assign Mux5config__Config_Reset = Config_Reset;
  assign Mux6config__Config_Reset = Config_Reset;
  assign Mux7config__Config_Reset = Config_Reset;
  assign out0[31:0] = mux_0__out[31:0];
  assign out1[31:0] = mux_1__out[31:0];
  assign out2[31:0] = mux_2__out[31:0];
  assign out3[31:0] = mux_3__out[31:0];
  assign out4[31:0] = mux_4__out[31:0];
  assign out5[31:0] = mux_5__out[31:0];
  assign out6[31:0] = mux_6__out[31:0];
  assign out7[31:0] = mux_7__out[31:0];
  assign mux_1__in0[31:0] = in0[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign mux_2__in0[31:0] = in0[31:0];
  assign mux_3__in0[31:0] = in0[31:0];
  assign mux_4__in0[31:0] = in0[31:0];
  assign mux_5__in0[31:0] = in0[31:0];
  assign mux_6__in0[31:0] = in0[31:0];
  assign mux_7__in0[31:0] = in0[31:0];
  assign mux_1__in1[31:0] = in1[31:0];
  assign mux_0__in1[31:0] = in1[31:0];
  assign mux_2__in1[31:0] = in1[31:0];
  assign mux_3__in1[31:0] = in1[31:0];
  assign mux_4__in1[31:0] = in1[31:0];
  assign mux_5__in1[31:0] = in1[31:0];
  assign mux_6__in1[31:0] = in1[31:0];
  assign mux_7__in1[31:0] = in1[31:0];
  assign mux_1__in2[31:0] = in2[31:0];
  assign mux_0__in2[31:0] = in2[31:0];
  assign mux_2__in2[31:0] = in2[31:0];
  assign mux_3__in2[31:0] = in2[31:0];
  assign mux_4__in2[31:0] = in2[31:0];
  assign mux_5__in2[31:0] = in2[31:0];
  assign mux_6__in2[31:0] = in2[31:0];
  assign mux_7__in2[31:0] = in2[31:0];
  assign mux_1__in3[31:0] = in3[31:0];
  assign mux_0__in3[31:0] = in3[31:0];
  assign mux_2__in3[31:0] = in3[31:0];
  assign mux_3__in3[31:0] = in3[31:0];
  assign mux_4__in3[31:0] = in3[31:0];
  assign mux_5__in3[31:0] = in3[31:0];
  assign mux_6__in3[31:0] = in3[31:0];
  assign mux_7__in3[31:0] = in3[31:0];
  assign mux_1__in4[31:0] = in4[31:0];
  assign mux_0__in4[31:0] = in4[31:0];
  assign mux_2__in4[31:0] = in4[31:0];
  assign mux_3__in4[31:0] = in4[31:0];
  assign mux_4__in4[31:0] = in4[31:0];
  assign mux_5__in4[31:0] = in4[31:0];
  assign mux_6__in4[31:0] = in4[31:0];
  assign mux_7__in4[31:0] = in4[31:0];
  assign mux_1__in5[31:0] = in5[31:0];
  assign mux_0__in5[31:0] = in5[31:0];
  assign mux_2__in5[31:0] = in5[31:0];
  assign mux_3__in5[31:0] = in5[31:0];
  assign mux_4__in5[31:0] = in5[31:0];
  assign mux_5__in5[31:0] = in5[31:0];
  assign mux_6__in5[31:0] = in5[31:0];
  assign mux_7__in5[31:0] = in5[31:0];
  assign mux_1__in6[31:0] = in6[31:0];
  assign mux_0__in6[31:0] = in6[31:0];
  assign mux_2__in6[31:0] = in6[31:0];
  assign mux_3__in6[31:0] = in6[31:0];
  assign mux_4__in6[31:0] = in6[31:0];
  assign mux_5__in6[31:0] = in6[31:0];
  assign mux_6__in6[31:0] = in6[31:0];
  assign mux_7__in6[31:0] = in6[31:0];
  assign mux_1__in7[31:0] = in7[31:0];
  assign mux_0__in7[31:0] = in7[31:0];
  assign mux_2__in7[31:0] = in7[31:0];
  assign mux_3__in7[31:0] = in7[31:0];
  assign mux_4__in7[31:0] = in7[31:0];
  assign mux_5__in7[31:0] = in7[31:0];
  assign mux_6__in7[31:0] = in7[31:0];
  assign mux_7__in7[31:0] = in7[31:0];
  assign mux_0__select[2:0] = Mux0config__select[2:0];
  assign mux_1__select[2:0] = Mux1config__select[2:0];
  assign mux_2__select[2:0] = Mux2config__select[2:0];
  assign mux_3__select[2:0] = Mux3config__select[2:0];
  assign mux_4__select[2:0] = Mux4config__select[2:0];
  assign mux_5__select[2:0] = Mux5config__select[2:0];
  assign mux_6__select[2:0] = Mux6config__select[2:0];
  assign mux_7__select[2:0] = Mux7config__select[2:0];
  assign Mux0config__ConfigIn = ConfigIn;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux4config__ConfigIn = Mux3config__ConfigOut;
  assign Mux5config__ConfigIn = Mux4config__ConfigOut;
  assign Mux6config__ConfigIn = Mux5config__ConfigOut;
  assign Mux7config__ConfigIn = Mux6config__ConfigOut;
  assign ConfigOut = Mux7config__ConfigOut;

endmodule //crossbar_8in_8out_U16

module cgrame_op_add_32b #(parameter contexts=1, parameter size=1) (
  a,
  b,
  c
);
    input [size-1:0] a;
    input [size-1:0] b;
    output [size-1:0] c;
    assign c = a + b;


endmodule //cgrame_op_add_32b

module cgrame_op_cmp_32b #(parameter contexts=1, parameter size=1) (
  a,
  b,
  c
);
    input [size-1:0] a;
    input [size-1:0] b;
    output [size-1:0] c;
    assign c = a == b? 1: 0;


endmodule //cgrame_op_cmp_32b

module cgrame_op_and_32b #(parameter contexts=1, parameter size=1) (
  a,
  b,
  c
);
    input [size-1:0] a;
    input [size-1:0] b;
    output [size-1:0] c;
    assign c = a & b;


endmodule //cgrame_op_and_32b

module cgrame_op_ashr_32b #(parameter contexts=1, parameter size=1) (
  a,
  b,
  c
);
    input [size-1:0] a;
    input [size-1:0] b;
    output [size-1:0] c;
    assign c = a >>> b;


endmodule //cgrame_op_ashr_32b

module cgrame_op_lshr_32b #(parameter contexts=1, parameter size=1) (
  a,
  b,
  c
);
    input [size-1:0] a;
    input [size-1:0] b;
    output [size-1:0] c;
    assign c = a >> b;


endmodule //cgrame_op_lshr_32b

module cgrame_op_multiply_32b #(parameter contexts=1, parameter size=1) (
  a,
  b,
  c
);
    input [size-1:0] a;
    input [size-1:0] b;
    output [size-1:0] c;
    assign c = a * b;


endmodule //cgrame_op_multiply_32b

module cgrame_op_shl_32b #(parameter contexts=1, parameter size=1) (
  a,
  b,
  c
);
    input [size-1:0] a;
    input [size-1:0] b;
    output [size-1:0] c;
    assign c = a << b;


endmodule //cgrame_op_shl_32b

module cgrame_op_sub_32b #(parameter contexts=1, parameter size=1) (
  a,
  b,
  c
);
    input [size-1:0] a;
    input [size-1:0] b;
    output [size-1:0] c;
    assign c = a - b;


endmodule //cgrame_op_sub_32b

module cgrame_op_xor_32b #(parameter contexts=1, parameter size=1) (
  a,
  b,
  c
);
    input [size-1:0] a;
    input [size-1:0] b;
    output [size-1:0] c;
    assign c = a ^ b;


endmodule //cgrame_op_xor_32b

module func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input [31:0] in_a,
  input [31:0] in_b,
  output [31:0] out,
  input [3:0] select
);
  //Wire declarations for instance 'caseStatement' (Module cgrame_caseStatement_10in_0latency_32b)
  wire  caseStatement__CGRA_Clock;
  wire  caseStatement__CGRA_Enable;
  wire  caseStatement__CGRA_Reset;
  wire [31:0] caseStatement__in0;
  wire [31:0] caseStatement__in1;
  wire [31:0] caseStatement__in2;
  wire [31:0] caseStatement__in3;
  wire [31:0] caseStatement__in4;
  wire [31:0] caseStatement__in5;
  wire [31:0] caseStatement__in6;
  wire [31:0] caseStatement__in7;
  wire [31:0] caseStatement__in8;
  wire [31:0] caseStatement__in9;
  wire [31:0] caseStatement__out;
  wire [3:0] caseStatement__select;
  cgrame_caseStatement_10in_0latency_32b #(.contexts(1),.latency(0),.size(32)) caseStatement(
    .CGRA_Clock(caseStatement__CGRA_Clock),
    .CGRA_Enable(caseStatement__CGRA_Enable),
    .CGRA_Reset(caseStatement__CGRA_Reset),
    .in0(caseStatement__in0),
    .in1(caseStatement__in1),
    .in2(caseStatement__in2),
    .in3(caseStatement__in3),
    .in4(caseStatement__in4),
    .in5(caseStatement__in5),
    .in6(caseStatement__in6),
    .in7(caseStatement__in7),
    .in8(caseStatement__in8),
    .in9(caseStatement__in9),
    .out(caseStatement__out),
    .select(caseStatement__select)
  );

  //Wire declarations for instance 'op_add_32b' (Module cgrame_op_add_32b)
  wire [31:0] op_add_32b__a;
  wire [31:0] op_add_32b__b;
  wire [31:0] op_add_32b__c;
  cgrame_op_add_32b #(.contexts(1),.size(32)) op_add_32b(
    .a(op_add_32b__a),
    .b(op_add_32b__b),
    .c(op_add_32b__c)
  );

  //Wire declarations for instance 'op_and_32b' (Module cgrame_op_and_32b)
  wire [31:0] op_and_32b__a;
  wire [31:0] op_and_32b__b;
  wire [31:0] op_and_32b__c;
  cgrame_op_and_32b #(.contexts(1),.size(32)) op_and_32b(
    .a(op_and_32b__a),
    .b(op_and_32b__b),
    .c(op_and_32b__c)
  );

  //Wire declarations for instance 'op_ashr_32b' (Module cgrame_op_ashr_32b)
  wire [31:0] op_ashr_32b__a;
  wire [31:0] op_ashr_32b__b;
  wire [31:0] op_ashr_32b__c;
  cgrame_op_ashr_32b #(.contexts(1),.size(32)) op_ashr_32b(
    .a(op_ashr_32b__a),
    .b(op_ashr_32b__b),
    .c(op_ashr_32b__c)
  );

  //Wire declarations for instance 'op_cmp_32b' (Module cgrame_op_cmp_32b)
  wire [31:0] op_cmp_32b__a;
  wire [31:0] op_cmp_32b__b;
  wire [31:0] op_cmp_32b__c;
  cgrame_op_cmp_32b #(.contexts(1),.size(32)) op_cmp_32b(
    .a(op_cmp_32b__a),
    .b(op_cmp_32b__b),
    .c(op_cmp_32b__c)
  );

  //Wire declarations for instance 'op_lshr_32b' (Module cgrame_op_lshr_32b)
  wire [31:0] op_lshr_32b__a;
  wire [31:0] op_lshr_32b__b;
  wire [31:0] op_lshr_32b__c;
  cgrame_op_lshr_32b #(.contexts(1),.size(32)) op_lshr_32b(
    .a(op_lshr_32b__a),
    .b(op_lshr_32b__b),
    .c(op_lshr_32b__c)
  );

  //Wire declarations for instance 'op_multiply_32b' (Module cgrame_op_multiply_32b)
  wire [31:0] op_multiply_32b__a;
  wire [31:0] op_multiply_32b__b;
  wire [31:0] op_multiply_32b__c;
  cgrame_op_multiply_32b #(.contexts(1),.size(32)) op_multiply_32b(
    .a(op_multiply_32b__a),
    .b(op_multiply_32b__b),
    .c(op_multiply_32b__c)
  );

  //Wire declarations for instance 'op_or_32b' (Module cgrame_op_or_32b)
  wire [31:0] op_or_32b__a;
  wire [31:0] op_or_32b__b;
  wire [31:0] op_or_32b__c;
  cgrame_op_or_32b #(.contexts(1),.size(32)) op_or_32b(
    .a(op_or_32b__a),
    .b(op_or_32b__b),
    .c(op_or_32b__c)
  );

  //Wire declarations for instance 'op_shl_32b' (Module cgrame_op_shl_32b)
  wire [31:0] op_shl_32b__a;
  wire [31:0] op_shl_32b__b;
  wire [31:0] op_shl_32b__c;
  cgrame_op_shl_32b #(.contexts(1),.size(32)) op_shl_32b(
    .a(op_shl_32b__a),
    .b(op_shl_32b__b),
    .c(op_shl_32b__c)
  );

  //Wire declarations for instance 'op_sub_32b' (Module cgrame_op_sub_32b)
  wire [31:0] op_sub_32b__a;
  wire [31:0] op_sub_32b__b;
  wire [31:0] op_sub_32b__c;
  cgrame_op_sub_32b #(.contexts(1),.size(32)) op_sub_32b(
    .a(op_sub_32b__a),
    .b(op_sub_32b__b),
    .c(op_sub_32b__c)
  );

  //Wire declarations for instance 'op_xor_32b' (Module cgrame_op_xor_32b)
  wire [31:0] op_xor_32b__a;
  wire [31:0] op_xor_32b__b;
  wire [31:0] op_xor_32b__c;
  cgrame_op_xor_32b #(.contexts(1),.size(32)) op_xor_32b(
    .a(op_xor_32b__a),
    .b(op_xor_32b__b),
    .c(op_xor_32b__c)
  );

  //All the connections
  assign caseStatement__CGRA_Reset = CGRA_Reset;
  assign caseStatement__CGRA_Clock = CGRA_Clock;
  assign caseStatement__CGRA_Enable = CGRA_Enable;
  assign op_sub_32b__a[31:0] = in_a[31:0];
  assign op_add_32b__a[31:0] = in_a[31:0];
  assign op_multiply_32b__a[31:0] = in_a[31:0];
  assign op_and_32b__a[31:0] = in_a[31:0];
  assign op_or_32b__a[31:0] = in_a[31:0];
  assign op_xor_32b__a[31:0] = in_a[31:0];
  assign op_shl_32b__a[31:0] = in_a[31:0];
  assign op_ashr_32b__a[31:0] = in_a[31:0];
  assign op_lshr_32b__a[31:0] = in_a[31:0];
  assign op_cmp_32b__a[31:0] = in_a[31:0];
  assign caseStatement__in0[31:0] = op_add_32b__c[31:0];
  assign caseStatement__in2[31:0] = op_multiply_32b__c[31:0];
  assign op_sub_32b__b[31:0] = in_b[31:0];
  assign op_add_32b__b[31:0] = in_b[31:0];
  assign op_multiply_32b__b[31:0] = in_b[31:0];
  assign op_and_32b__b[31:0] = in_b[31:0];
  assign op_or_32b__b[31:0] = in_b[31:0];
  assign op_xor_32b__b[31:0] = in_b[31:0];
  assign op_shl_32b__b[31:0] = in_b[31:0];
  assign op_ashr_32b__b[31:0] = in_b[31:0];
  assign op_lshr_32b__b[31:0] = in_b[31:0];
  assign op_cmp_32b__b[31:0] = in_b[31:0];
  assign caseStatement__in5[31:0] = op_xor_32b__c[31:0];
  assign caseStatement__in1[31:0] = op_sub_32b__c[31:0];
  assign caseStatement__in3[31:0] = op_and_32b__c[31:0];
  assign caseStatement__in4[31:0] = op_or_32b__c[31:0];
  assign caseStatement__in6[31:0] = op_shl_32b__c[31:0];
  assign caseStatement__in7[31:0] = op_ashr_32b__c[31:0];
  assign caseStatement__in8[31:0] = op_lshr_32b__c[31:0];
  assign caseStatement__in9[31:0] = op_cmp_32b__c[31:0];
  assign caseStatement__select[3:0] = select[3:0];
  assign out[31:0] = caseStatement__out[31:0];

endmodule //func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12

module cgrame_register_32b #(parameter contexts=1, parameter size=1) (
  CGRA_Clock,
  CGRA_Reset,
  CGRA_Enable,
  enable,
  in,
  out
);
    input CGRA_Clock;
    input CGRA_Reset;
    input CGRA_Enable;
    input [0:0] enable;
    input [size-1:0] in;
    output reg [size-1:0] out;
    always @(posedge CGRA_Clock, posedge CGRA_Reset)
        if (CGRA_Reset == 1)
            out <= 0;
        else
            if (enable & CGRA_Enable)
                out <= in;

endmodule //cgrame_register_32b

module hycube_no_pred_in159_out159_U10 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in4,
  input [31:0] in7,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out4,
  output [31:0] out7
);
  //Wire declarations for instance 'ALU' (Module func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12)
  wire  ALU__CGRA_Clock;
  wire  ALU__CGRA_Enable;
  wire  ALU__CGRA_Reset;
  wire [31:0] ALU__in_a;
  wire [31:0] ALU__in_b;
  wire [31:0] ALU__out;
  wire [3:0] ALU__select;
  func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12 ALU(
    .CGRA_Clock(ALU__CGRA_Clock),
    .CGRA_Enable(ALU__CGRA_Enable),
    .CGRA_Reset(ALU__CGRA_Reset),
    .in_a(ALU__in_a),
    .in_b(ALU__in_b),
    .out(ALU__out),
    .select(ALU__select)
  );

  //Wire declarations for instance 'ALUMuxAconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxAconfig__CGRA_Clock;
  wire  ALUMuxAconfig__CGRA_Enable;
  wire  ALUMuxAconfig__CGRA_Reset;
  wire  ALUMuxAconfig__ConfigIn;
  wire  ALUMuxAconfig__ConfigOut;
  wire  ALUMuxAconfig__Config_Clock;
  wire  ALUMuxAconfig__Config_Reset;
  wire [0:0] ALUMuxAconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxAconfig(
    .CGRA_Clock(ALUMuxAconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxAconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxAconfig__CGRA_Reset),
    .ConfigIn(ALUMuxAconfig__ConfigIn),
    .ConfigOut(ALUMuxAconfig__ConfigOut),
    .Config_Clock(ALUMuxAconfig__Config_Clock),
    .Config_Reset(ALUMuxAconfig__Config_Reset),
    .select(ALUMuxAconfig__select)
  );

  //Wire declarations for instance 'ALUMuxBconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxBconfig__CGRA_Clock;
  wire  ALUMuxBconfig__CGRA_Enable;
  wire  ALUMuxBconfig__CGRA_Reset;
  wire  ALUMuxBconfig__ConfigIn;
  wire  ALUMuxBconfig__ConfigOut;
  wire  ALUMuxBconfig__Config_Clock;
  wire  ALUMuxBconfig__Config_Reset;
  wire [0:0] ALUMuxBconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxBconfig(
    .CGRA_Clock(ALUMuxBconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxBconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxBconfig__CGRA_Reset),
    .ConfigIn(ALUMuxBconfig__ConfigIn),
    .ConfigOut(ALUMuxBconfig__ConfigOut),
    .Config_Clock(ALUMuxBconfig__Config_Clock),
    .Config_Reset(ALUMuxBconfig__Config_Reset),
    .select(ALUMuxBconfig__select)
  );

  //Wire declarations for instance 'ALUconfig' (Module cgrame_configcell_context1_size4)
  wire  ALUconfig__CGRA_Clock;
  wire  ALUconfig__CGRA_Enable;
  wire  ALUconfig__CGRA_Reset;
  wire  ALUconfig__ConfigIn;
  wire  ALUconfig__ConfigOut;
  wire  ALUconfig__Config_Clock;
  wire  ALUconfig__Config_Reset;
  wire [3:0] ALUconfig__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) ALUconfig(
    .CGRA_Clock(ALUconfig__CGRA_Clock),
    .CGRA_Enable(ALUconfig__CGRA_Enable),
    .CGRA_Reset(ALUconfig__CGRA_Reset),
    .ConfigIn(ALUconfig__ConfigIn),
    .ConfigOut(ALUconfig__ConfigOut),
    .Config_Clock(ALUconfig__Config_Clock),
    .Config_Reset(ALUconfig__Config_Reset),
    .select(ALUconfig__select)
  );

  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size1)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [0:0] Mux0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size1)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [0:0] Mux1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size1)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [0:0] Mux2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size1)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [0:0] Mux3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux4config' (Module cgrame_configcell_context1_size1)
  wire  Mux4config__CGRA_Clock;
  wire  Mux4config__CGRA_Enable;
  wire  Mux4config__CGRA_Reset;
  wire  Mux4config__ConfigIn;
  wire  Mux4config__ConfigOut;
  wire  Mux4config__Config_Clock;
  wire  Mux4config__Config_Reset;
  wire [0:0] Mux4config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux4config(
    .CGRA_Clock(Mux4config__CGRA_Clock),
    .CGRA_Enable(Mux4config__CGRA_Enable),
    .CGRA_Reset(Mux4config__CGRA_Reset),
    .ConfigIn(Mux4config__ConfigIn),
    .ConfigOut(Mux4config__ConfigOut),
    .Config_Clock(Mux4config__Config_Clock),
    .Config_Reset(Mux4config__Config_Reset),
    .select(Mux4config__select)
  );

  //Wire declarations for instance 'Mux7config' (Module cgrame_configcell_context1_size1)
  wire  Mux7config__CGRA_Clock;
  wire  Mux7config__CGRA_Enable;
  wire  Mux7config__CGRA_Reset;
  wire  Mux7config__ConfigIn;
  wire  Mux7config__ConfigOut;
  wire  Mux7config__Config_Clock;
  wire  Mux7config__Config_Reset;
  wire [0:0] Mux7config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux7config(
    .CGRA_Clock(Mux7config__CGRA_Clock),
    .CGRA_Enable(Mux7config__CGRA_Enable),
    .CGRA_Reset(Mux7config__CGRA_Reset),
    .ConfigIn(Mux7config__ConfigIn),
    .ConfigOut(Mux7config__ConfigOut),
    .Config_Clock(Mux7config__Config_Clock),
    .Config_Reset(Mux7config__Config_Reset),
    .select(Mux7config__select)
  );

  //Wire declarations for instance 'RES' (Module cgrame_register_32b)
  wire  RES__CGRA_Clock;
  wire  RES__CGRA_Enable;
  wire  RES__CGRA_Reset;
  wire [0:0] RES__enable;
  wire [31:0] RES__in;
  wire [31:0] RES__out;
  cgrame_register_32b #(.contexts(1),.size(32)) RES(
    .CGRA_Clock(RES__CGRA_Clock),
    .CGRA_Enable(RES__CGRA_Enable),
    .CGRA_Reset(RES__CGRA_Reset),
    .enable(RES__enable),
    .in(RES__in),
    .out(RES__out)
  );

  //Wire declarations for instance 'RESConfig' (Module cgrame_configcell_context1_size1)
  wire  RESConfig__CGRA_Clock;
  wire  RESConfig__CGRA_Enable;
  wire  RESConfig__CGRA_Reset;
  wire  RESConfig__ConfigIn;
  wire  RESConfig__ConfigOut;
  wire  RESConfig__Config_Clock;
  wire  RESConfig__Config_Reset;
  wire [0:0] RESConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RESConfig(
    .CGRA_Clock(RESConfig__CGRA_Clock),
    .CGRA_Enable(RESConfig__CGRA_Enable),
    .CGRA_Reset(RESConfig__CGRA_Reset),
    .ConfigIn(RESConfig__ConfigIn),
    .ConfigOut(RESConfig__ConfigOut),
    .Config_Clock(RESConfig__Config_Clock),
    .Config_Reset(RESConfig__Config_Reset),
    .select(RESConfig__select)
  );

  //Wire declarations for instance 'Reg0config' (Module cgrame_configcell_context1_size1)
  wire  Reg0config__CGRA_Clock;
  wire  Reg0config__CGRA_Enable;
  wire  Reg0config__CGRA_Reset;
  wire  Reg0config__ConfigIn;
  wire  Reg0config__ConfigOut;
  wire  Reg0config__Config_Clock;
  wire  Reg0config__Config_Reset;
  wire [0:0] Reg0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg0config(
    .CGRA_Clock(Reg0config__CGRA_Clock),
    .CGRA_Enable(Reg0config__CGRA_Enable),
    .CGRA_Reset(Reg0config__CGRA_Reset),
    .ConfigIn(Reg0config__ConfigIn),
    .ConfigOut(Reg0config__ConfigOut),
    .Config_Clock(Reg0config__Config_Clock),
    .Config_Reset(Reg0config__Config_Reset),
    .select(Reg0config__select)
  );

  //Wire declarations for instance 'Reg1config' (Module cgrame_configcell_context1_size1)
  wire  Reg1config__CGRA_Clock;
  wire  Reg1config__CGRA_Enable;
  wire  Reg1config__CGRA_Reset;
  wire  Reg1config__ConfigIn;
  wire  Reg1config__ConfigOut;
  wire  Reg1config__Config_Clock;
  wire  Reg1config__Config_Reset;
  wire [0:0] Reg1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg1config(
    .CGRA_Clock(Reg1config__CGRA_Clock),
    .CGRA_Enable(Reg1config__CGRA_Enable),
    .CGRA_Reset(Reg1config__CGRA_Reset),
    .ConfigIn(Reg1config__ConfigIn),
    .ConfigOut(Reg1config__ConfigOut),
    .Config_Clock(Reg1config__Config_Clock),
    .Config_Reset(Reg1config__Config_Reset),
    .select(Reg1config__select)
  );

  //Wire declarations for instance 'Reg2config' (Module cgrame_configcell_context1_size1)
  wire  Reg2config__CGRA_Clock;
  wire  Reg2config__CGRA_Enable;
  wire  Reg2config__CGRA_Reset;
  wire  Reg2config__ConfigIn;
  wire  Reg2config__ConfigOut;
  wire  Reg2config__Config_Clock;
  wire  Reg2config__Config_Reset;
  wire [0:0] Reg2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg2config(
    .CGRA_Clock(Reg2config__CGRA_Clock),
    .CGRA_Enable(Reg2config__CGRA_Enable),
    .CGRA_Reset(Reg2config__CGRA_Reset),
    .ConfigIn(Reg2config__ConfigIn),
    .ConfigOut(Reg2config__ConfigOut),
    .Config_Clock(Reg2config__Config_Clock),
    .Config_Reset(Reg2config__Config_Reset),
    .select(Reg2config__select)
  );

  //Wire declarations for instance 'Reg3config' (Module cgrame_configcell_context1_size1)
  wire  Reg3config__CGRA_Clock;
  wire  Reg3config__CGRA_Enable;
  wire  Reg3config__CGRA_Reset;
  wire  Reg3config__ConfigIn;
  wire  Reg3config__ConfigOut;
  wire  Reg3config__Config_Clock;
  wire  Reg3config__Config_Reset;
  wire [0:0] Reg3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg3config(
    .CGRA_Clock(Reg3config__CGRA_Clock),
    .CGRA_Enable(Reg3config__CGRA_Enable),
    .CGRA_Reset(Reg3config__CGRA_Reset),
    .ConfigIn(Reg3config__ConfigIn),
    .ConfigOut(Reg3config__ConfigOut),
    .Config_Clock(Reg3config__Config_Clock),
    .Config_Reset(Reg3config__Config_Reset),
    .select(Reg3config__select)
  );

  //Wire declarations for instance 'Reg4config' (Module cgrame_configcell_context1_size1)
  wire  Reg4config__CGRA_Clock;
  wire  Reg4config__CGRA_Enable;
  wire  Reg4config__CGRA_Reset;
  wire  Reg4config__ConfigIn;
  wire  Reg4config__ConfigOut;
  wire  Reg4config__Config_Clock;
  wire  Reg4config__Config_Reset;
  wire [0:0] Reg4config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg4config(
    .CGRA_Clock(Reg4config__CGRA_Clock),
    .CGRA_Enable(Reg4config__CGRA_Enable),
    .CGRA_Reset(Reg4config__CGRA_Reset),
    .ConfigIn(Reg4config__ConfigIn),
    .ConfigOut(Reg4config__ConfigOut),
    .Config_Clock(Reg4config__Config_Clock),
    .Config_Reset(Reg4config__Config_Reset),
    .select(Reg4config__select)
  );

  //Wire declarations for instance 'Reg7config' (Module cgrame_configcell_context1_size1)
  wire  Reg7config__CGRA_Clock;
  wire  Reg7config__CGRA_Enable;
  wire  Reg7config__CGRA_Reset;
  wire  Reg7config__ConfigIn;
  wire  Reg7config__ConfigOut;
  wire  Reg7config__Config_Clock;
  wire  Reg7config__Config_Reset;
  wire [0:0] Reg7config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg7config(
    .CGRA_Clock(Reg7config__CGRA_Clock),
    .CGRA_Enable(Reg7config__CGRA_Enable),
    .CGRA_Reset(Reg7config__CGRA_Reset),
    .ConfigIn(Reg7config__ConfigIn),
    .ConfigOut(Reg7config__ConfigOut),
    .Config_Clock(Reg7config__Config_Clock),
    .Config_Reset(Reg7config__Config_Reset),
    .select(Reg7config__select)
  );

  //Wire declarations for instance 'RegAConfig' (Module cgrame_configcell_context1_size1)
  wire  RegAConfig__CGRA_Clock;
  wire  RegAConfig__CGRA_Enable;
  wire  RegAConfig__CGRA_Reset;
  wire  RegAConfig__ConfigIn;
  wire  RegAConfig__ConfigOut;
  wire  RegAConfig__Config_Clock;
  wire  RegAConfig__Config_Reset;
  wire [0:0] RegAConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegAConfig(
    .CGRA_Clock(RegAConfig__CGRA_Clock),
    .CGRA_Enable(RegAConfig__CGRA_Enable),
    .CGRA_Reset(RegAConfig__CGRA_Reset),
    .ConfigIn(RegAConfig__ConfigIn),
    .ConfigOut(RegAConfig__ConfigOut),
    .Config_Clock(RegAConfig__Config_Clock),
    .Config_Reset(RegAConfig__Config_Reset),
    .select(RegAConfig__select)
  );

  //Wire declarations for instance 'RegBConfig' (Module cgrame_configcell_context1_size1)
  wire  RegBConfig__CGRA_Clock;
  wire  RegBConfig__CGRA_Enable;
  wire  RegBConfig__CGRA_Reset;
  wire  RegBConfig__ConfigIn;
  wire  RegBConfig__ConfigOut;
  wire  RegBConfig__Config_Clock;
  wire  RegBConfig__Config_Reset;
  wire [0:0] RegBConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegBConfig(
    .CGRA_Clock(RegBConfig__CGRA_Clock),
    .CGRA_Enable(RegBConfig__CGRA_Enable),
    .CGRA_Reset(RegBConfig__CGRA_Reset),
    .ConfigIn(RegBConfig__ConfigIn),
    .ConfigOut(RegBConfig__ConfigOut),
    .Config_Clock(RegBConfig__Config_Clock),
    .Config_Reset(RegBConfig__Config_Reset),
    .select(RegBConfig__select)
  );

  //Wire declarations for instance 'alu_muxa' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxa__in0;
  wire [31:0] alu_muxa__in1;
  wire [31:0] alu_muxa__out;
  wire [0:0] alu_muxa__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxa(
    .in0(alu_muxa__in0),
    .in1(alu_muxa__in1),
    .out(alu_muxa__out),
    .select(alu_muxa__select)
  );

  //Wire declarations for instance 'alu_muxb' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxb__in0;
  wire [31:0] alu_muxb__in1;
  wire [31:0] alu_muxb__out;
  wire [0:0] alu_muxb__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxb(
    .in0(alu_muxb__in0),
    .in1(alu_muxb__in1),
    .out(alu_muxb__out),
    .select(alu_muxb__select)
  );

  //Wire declarations for instance 'const' (Module const_32b_U15)
  wire  const__CGRA_Clock;
  wire  const__CGRA_Enable;
  wire  const__CGRA_Reset;
  wire  const__ConfigIn;
  wire  const__ConfigOut;
  wire  const__Config_Clock;
  wire  const__Config_Reset;
  wire [31:0] const__out;
  const_32b_U15 const(
    .CGRA_Clock(const__CGRA_Clock),
    .CGRA_Enable(const__CGRA_Enable),
    .CGRA_Reset(const__CGRA_Reset),
    .ConfigIn(const__ConfigIn),
    .ConfigOut(const__ConfigOut),
    .Config_Clock(const__Config_Clock),
    .Config_Reset(const__Config_Reset),
    .out(const__out)
  );

  //Wire declarations for instance 'crossbar' (Module crossbar_8in_8out_U16)
  wire  crossbar__CGRA_Clock;
  wire  crossbar__CGRA_Enable;
  wire  crossbar__CGRA_Reset;
  wire  crossbar__ConfigIn;
  wire  crossbar__ConfigOut;
  wire  crossbar__Config_Clock;
  wire  crossbar__Config_Reset;
  wire [31:0] crossbar__in0;
  wire [31:0] crossbar__in1;
  wire [31:0] crossbar__in2;
  wire [31:0] crossbar__in3;
  wire [31:0] crossbar__in4;
  wire [31:0] crossbar__in5;
  wire [31:0] crossbar__in6;
  wire [31:0] crossbar__in7;
  wire [31:0] crossbar__out0;
  wire [31:0] crossbar__out1;
  wire [31:0] crossbar__out2;
  wire [31:0] crossbar__out3;
  wire [31:0] crossbar__out4;
  wire [31:0] crossbar__out5;
  wire [31:0] crossbar__out6;
  wire [31:0] crossbar__out7;
  crossbar_8in_8out_U16 crossbar(
    .CGRA_Clock(crossbar__CGRA_Clock),
    .CGRA_Enable(crossbar__CGRA_Enable),
    .CGRA_Reset(crossbar__CGRA_Reset),
    .ConfigIn(crossbar__ConfigIn),
    .ConfigOut(crossbar__ConfigOut),
    .Config_Clock(crossbar__Config_Clock),
    .Config_Reset(crossbar__Config_Reset),
    .in0(crossbar__in0),
    .in1(crossbar__in1),
    .in2(crossbar__in2),
    .in3(crossbar__in3),
    .in4(crossbar__in4),
    .in5(crossbar__in5),
    .in6(crossbar__in6),
    .in7(crossbar__in7),
    .out0(crossbar__out0),
    .out1(crossbar__out1),
    .out2(crossbar__out2),
    .out3(crossbar__out3),
    .out4(crossbar__out4),
    .out5(crossbar__out5),
    .out6(crossbar__out6),
    .out7(crossbar__out7)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__out;
  wire [0:0] mux_0__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__out;
  wire [0:0] mux_1__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__out;
  wire [0:0] mux_2__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__out;
  wire [0:0] mux_3__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_4' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_4__in0;
  wire [31:0] mux_4__in1;
  wire [31:0] mux_4__out;
  wire [0:0] mux_4__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_4(
    .in0(mux_4__in0),
    .in1(mux_4__in1),
    .out(mux_4__out),
    .select(mux_4__select)
  );

  //Wire declarations for instance 'mux_7' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_7__in0;
  wire [31:0] mux_7__in1;
  wire [31:0] mux_7__out;
  wire [0:0] mux_7__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_7(
    .in0(mux_7__in0),
    .in1(mux_7__in1),
    .out(mux_7__out),
    .select(mux_7__select)
  );

  //Wire declarations for instance 'reg0' (Module cgrame_register_32b)
  wire  reg0__CGRA_Clock;
  wire  reg0__CGRA_Enable;
  wire  reg0__CGRA_Reset;
  wire [0:0] reg0__enable;
  wire [31:0] reg0__in;
  wire [31:0] reg0__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg0(
    .CGRA_Clock(reg0__CGRA_Clock),
    .CGRA_Enable(reg0__CGRA_Enable),
    .CGRA_Reset(reg0__CGRA_Reset),
    .enable(reg0__enable),
    .in(reg0__in),
    .out(reg0__out)
  );

  //Wire declarations for instance 'reg1' (Module cgrame_register_32b)
  wire  reg1__CGRA_Clock;
  wire  reg1__CGRA_Enable;
  wire  reg1__CGRA_Reset;
  wire [0:0] reg1__enable;
  wire [31:0] reg1__in;
  wire [31:0] reg1__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg1(
    .CGRA_Clock(reg1__CGRA_Clock),
    .CGRA_Enable(reg1__CGRA_Enable),
    .CGRA_Reset(reg1__CGRA_Reset),
    .enable(reg1__enable),
    .in(reg1__in),
    .out(reg1__out)
  );

  //Wire declarations for instance 'reg2' (Module cgrame_register_32b)
  wire  reg2__CGRA_Clock;
  wire  reg2__CGRA_Enable;
  wire  reg2__CGRA_Reset;
  wire [0:0] reg2__enable;
  wire [31:0] reg2__in;
  wire [31:0] reg2__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg2(
    .CGRA_Clock(reg2__CGRA_Clock),
    .CGRA_Enable(reg2__CGRA_Enable),
    .CGRA_Reset(reg2__CGRA_Reset),
    .enable(reg2__enable),
    .in(reg2__in),
    .out(reg2__out)
  );

  //Wire declarations for instance 'reg3' (Module cgrame_register_32b)
  wire  reg3__CGRA_Clock;
  wire  reg3__CGRA_Enable;
  wire  reg3__CGRA_Reset;
  wire [0:0] reg3__enable;
  wire [31:0] reg3__in;
  wire [31:0] reg3__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg3(
    .CGRA_Clock(reg3__CGRA_Clock),
    .CGRA_Enable(reg3__CGRA_Enable),
    .CGRA_Reset(reg3__CGRA_Reset),
    .enable(reg3__enable),
    .in(reg3__in),
    .out(reg3__out)
  );

  //Wire declarations for instance 'reg4' (Module cgrame_register_32b)
  wire  reg4__CGRA_Clock;
  wire  reg4__CGRA_Enable;
  wire  reg4__CGRA_Reset;
  wire [0:0] reg4__enable;
  wire [31:0] reg4__in;
  wire [31:0] reg4__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg4(
    .CGRA_Clock(reg4__CGRA_Clock),
    .CGRA_Enable(reg4__CGRA_Enable),
    .CGRA_Reset(reg4__CGRA_Reset),
    .enable(reg4__enable),
    .in(reg4__in),
    .out(reg4__out)
  );

  //Wire declarations for instance 'reg7' (Module cgrame_register_32b)
  wire  reg7__CGRA_Clock;
  wire  reg7__CGRA_Enable;
  wire  reg7__CGRA_Reset;
  wire [0:0] reg7__enable;
  wire [31:0] reg7__in;
  wire [31:0] reg7__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg7(
    .CGRA_Clock(reg7__CGRA_Clock),
    .CGRA_Enable(reg7__CGRA_Enable),
    .CGRA_Reset(reg7__CGRA_Reset),
    .enable(reg7__enable),
    .in(reg7__in),
    .out(reg7__out)
  );

  //Wire declarations for instance 'rega' (Module cgrame_register_32b)
  wire  rega__CGRA_Clock;
  wire  rega__CGRA_Enable;
  wire  rega__CGRA_Reset;
  wire [0:0] rega__enable;
  wire [31:0] rega__in;
  wire [31:0] rega__out;
  cgrame_register_32b #(.contexts(1),.size(32)) rega(
    .CGRA_Clock(rega__CGRA_Clock),
    .CGRA_Enable(rega__CGRA_Enable),
    .CGRA_Reset(rega__CGRA_Reset),
    .enable(rega__enable),
    .in(rega__in),
    .out(rega__out)
  );

  //Wire declarations for instance 'regb' (Module cgrame_register_32b)
  wire  regb__CGRA_Clock;
  wire  regb__CGRA_Enable;
  wire  regb__CGRA_Reset;
  wire [0:0] regb__enable;
  wire [31:0] regb__in;
  wire [31:0] regb__out;
  cgrame_register_32b #(.contexts(1),.size(32)) regb(
    .CGRA_Clock(regb__CGRA_Clock),
    .CGRA_Enable(regb__CGRA_Enable),
    .CGRA_Reset(regb__CGRA_Reset),
    .enable(regb__enable),
    .in(regb__in),
    .out(regb__out)
  );

  //All the connections
  assign ALU__CGRA_Reset = CGRA_Reset;
  assign ALU__CGRA_Clock = CGRA_Clock;
  assign RES__CGRA_Clock = CGRA_Clock;
  assign const__CGRA_Clock = CGRA_Clock;
  assign crossbar__CGRA_Clock = CGRA_Clock;
  assign reg0__CGRA_Clock = CGRA_Clock;
  assign reg1__CGRA_Clock = CGRA_Clock;
  assign reg2__CGRA_Clock = CGRA_Clock;
  assign reg3__CGRA_Clock = CGRA_Clock;
  assign reg4__CGRA_Clock = CGRA_Clock;
  assign reg7__CGRA_Clock = CGRA_Clock;
  assign rega__CGRA_Clock = CGRA_Clock;
  assign regb__CGRA_Clock = CGRA_Clock;
  assign ALUMuxAconfig__CGRA_Clock = CGRA_Clock;
  assign ALUMuxBconfig__CGRA_Clock = CGRA_Clock;
  assign ALUconfig__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux4config__CGRA_Clock = CGRA_Clock;
  assign Mux7config__CGRA_Clock = CGRA_Clock;
  assign RESConfig__CGRA_Clock = CGRA_Clock;
  assign Reg0config__CGRA_Clock = CGRA_Clock;
  assign Reg1config__CGRA_Clock = CGRA_Clock;
  assign Reg2config__CGRA_Clock = CGRA_Clock;
  assign Reg3config__CGRA_Clock = CGRA_Clock;
  assign Reg4config__CGRA_Clock = CGRA_Clock;
  assign Reg7config__CGRA_Clock = CGRA_Clock;
  assign RegAConfig__CGRA_Clock = CGRA_Clock;
  assign RegBConfig__CGRA_Clock = CGRA_Clock;
  assign ALU__CGRA_Enable = CGRA_Enable;
  assign RES__CGRA_Reset = CGRA_Reset;
  assign const__CGRA_Reset = CGRA_Reset;
  assign crossbar__CGRA_Reset = CGRA_Reset;
  assign reg0__CGRA_Reset = CGRA_Reset;
  assign reg1__CGRA_Reset = CGRA_Reset;
  assign reg2__CGRA_Reset = CGRA_Reset;
  assign reg3__CGRA_Reset = CGRA_Reset;
  assign reg4__CGRA_Reset = CGRA_Reset;
  assign reg7__CGRA_Reset = CGRA_Reset;
  assign rega__CGRA_Reset = CGRA_Reset;
  assign regb__CGRA_Reset = CGRA_Reset;
  assign ALUMuxAconfig__CGRA_Reset = CGRA_Reset;
  assign ALUMuxBconfig__CGRA_Reset = CGRA_Reset;
  assign ALUconfig__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux4config__CGRA_Reset = CGRA_Reset;
  assign Mux7config__CGRA_Reset = CGRA_Reset;
  assign RESConfig__CGRA_Reset = CGRA_Reset;
  assign Reg0config__CGRA_Reset = CGRA_Reset;
  assign Reg1config__CGRA_Reset = CGRA_Reset;
  assign Reg2config__CGRA_Reset = CGRA_Reset;
  assign Reg3config__CGRA_Reset = CGRA_Reset;
  assign Reg4config__CGRA_Reset = CGRA_Reset;
  assign Reg7config__CGRA_Reset = CGRA_Reset;
  assign RegAConfig__CGRA_Reset = CGRA_Reset;
  assign RegBConfig__CGRA_Reset = CGRA_Reset;
  assign RES__CGRA_Enable = CGRA_Enable;
  assign const__CGRA_Enable = CGRA_Enable;
  assign crossbar__CGRA_Enable = CGRA_Enable;
  assign reg0__CGRA_Enable = CGRA_Enable;
  assign reg1__CGRA_Enable = CGRA_Enable;
  assign reg2__CGRA_Enable = CGRA_Enable;
  assign reg3__CGRA_Enable = CGRA_Enable;
  assign reg4__CGRA_Enable = CGRA_Enable;
  assign reg7__CGRA_Enable = CGRA_Enable;
  assign rega__CGRA_Enable = CGRA_Enable;
  assign regb__CGRA_Enable = CGRA_Enable;
  assign ALUMuxAconfig__CGRA_Enable = CGRA_Enable;
  assign ALUMuxBconfig__CGRA_Enable = CGRA_Enable;
  assign ALUconfig__CGRA_Enable = CGRA_Enable;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux4config__CGRA_Enable = CGRA_Enable;
  assign Mux7config__CGRA_Enable = CGRA_Enable;
  assign RESConfig__CGRA_Enable = CGRA_Enable;
  assign Reg0config__CGRA_Enable = CGRA_Enable;
  assign Reg1config__CGRA_Enable = CGRA_Enable;
  assign Reg2config__CGRA_Enable = CGRA_Enable;
  assign Reg3config__CGRA_Enable = CGRA_Enable;
  assign Reg4config__CGRA_Enable = CGRA_Enable;
  assign Reg7config__CGRA_Enable = CGRA_Enable;
  assign RegAConfig__CGRA_Enable = CGRA_Enable;
  assign RegBConfig__CGRA_Enable = CGRA_Enable;
  assign const__Config_Clock = Config_Clock;
  assign const__Config_Reset = Config_Reset;
  assign crossbar__Config_Clock = Config_Clock;
  assign ALUMuxAconfig__Config_Clock = Config_Clock;
  assign ALUMuxBconfig__Config_Clock = Config_Clock;
  assign ALUconfig__Config_Clock = Config_Clock;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux4config__Config_Clock = Config_Clock;
  assign Mux7config__Config_Clock = Config_Clock;
  assign RESConfig__Config_Clock = Config_Clock;
  assign Reg0config__Config_Clock = Config_Clock;
  assign Reg1config__Config_Clock = Config_Clock;
  assign Reg2config__Config_Clock = Config_Clock;
  assign Reg3config__Config_Clock = Config_Clock;
  assign Reg4config__Config_Clock = Config_Clock;
  assign Reg7config__Config_Clock = Config_Clock;
  assign RegAConfig__Config_Clock = Config_Clock;
  assign RegBConfig__Config_Clock = Config_Clock;
  assign crossbar__Config_Reset = Config_Reset;
  assign ALUMuxAconfig__Config_Reset = Config_Reset;
  assign ALUMuxBconfig__Config_Reset = Config_Reset;
  assign ALUconfig__Config_Reset = Config_Reset;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux4config__Config_Reset = Config_Reset;
  assign Mux7config__Config_Reset = Config_Reset;
  assign RESConfig__Config_Reset = Config_Reset;
  assign Reg0config__Config_Reset = Config_Reset;
  assign Reg1config__Config_Reset = Config_Reset;
  assign Reg2config__Config_Reset = Config_Reset;
  assign Reg3config__Config_Reset = Config_Reset;
  assign Reg4config__Config_Reset = Config_Reset;
  assign Reg7config__Config_Reset = Config_Reset;
  assign RegAConfig__Config_Reset = Config_Reset;
  assign RegBConfig__Config_Reset = Config_Reset;
  assign crossbar__in6[31:0] = ALU__out[31:0];
  assign RES__in[31:0] = ALU__out[31:0];
  assign out0[31:0] = crossbar__out0[31:0];
  assign out1[31:0] = crossbar__out1[31:0];
  assign out2[31:0] = crossbar__out2[31:0];
  assign out3[31:0] = crossbar__out3[31:0];
  assign out4[31:0] = crossbar__out4[31:0];
  assign out7[31:0] = crossbar__out5[31:0];
  assign rega__in[31:0] = crossbar__out6[31:0];
  assign regb__in[31:0] = crossbar__out7[31:0];
  assign crossbar__in7[31:0] = RES__out[31:0];
  assign alu_muxa__in0[31:0] = rega__out[31:0];
  assign alu_muxb__in0[31:0] = regb__out[31:0];
  assign alu_muxa__in1[31:0] = const__out[31:0];
  assign alu_muxb__in1[31:0] = const__out[31:0];
  assign ALU__in_a[31:0] = alu_muxa__out[31:0];
  assign ALU__in_b[31:0] = alu_muxb__out[31:0];
  assign mux_0__in1[31:0] = reg0__out[31:0];
  assign crossbar__in0[31:0] = mux_0__out[31:0];
  assign mux_1__in1[31:0] = reg1__out[31:0];
  assign crossbar__in1[31:0] = mux_1__out[31:0];
  assign mux_2__in1[31:0] = reg2__out[31:0];
  assign crossbar__in2[31:0] = mux_2__out[31:0];
  assign mux_3__in1[31:0] = reg3__out[31:0];
  assign crossbar__in3[31:0] = mux_3__out[31:0];
  assign mux_4__in1[31:0] = reg4__out[31:0];
  assign crossbar__in4[31:0] = mux_4__out[31:0];
  assign mux_7__in1[31:0] = reg7__out[31:0];
  assign crossbar__in5[31:0] = mux_7__out[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign reg0__in[31:0] = in0[31:0];
  assign mux_1__in0[31:0] = in1[31:0];
  assign reg1__in[31:0] = in1[31:0];
  assign mux_2__in0[31:0] = in2[31:0];
  assign reg2__in[31:0] = in2[31:0];
  assign mux_3__in0[31:0] = in3[31:0];
  assign reg3__in[31:0] = in3[31:0];
  assign mux_4__in0[31:0] = in4[31:0];
  assign reg4__in[31:0] = in4[31:0];
  assign mux_7__in0[31:0] = in7[31:0];
  assign reg7__in[31:0] = in7[31:0];
  assign alu_muxa__select[0:0] = ALUMuxAconfig__select[0:0];
  assign alu_muxb__select[0:0] = ALUMuxBconfig__select[0:0];
  assign ALU__select[3:0] = ALUconfig__select[3:0];
  assign mux_0__select[0:0] = Mux0config__select[0:0];
  assign mux_1__select[0:0] = Mux1config__select[0:0];
  assign mux_2__select[0:0] = Mux2config__select[0:0];
  assign mux_3__select[0:0] = Mux3config__select[0:0];
  assign mux_4__select[0:0] = Mux4config__select[0:0];
  assign mux_7__select[0:0] = Mux7config__select[0:0];
  assign RES__enable[0:0] = RESConfig__select[0:0];
  assign reg0__enable[0:0] = Reg0config__select[0:0];
  assign reg1__enable[0:0] = Reg1config__select[0:0];
  assign reg2__enable[0:0] = Reg2config__select[0:0];
  assign reg3__enable[0:0] = Reg3config__select[0:0];
  assign reg4__enable[0:0] = Reg4config__select[0:0];
  assign reg7__enable[0:0] = Reg7config__select[0:0];
  assign regb__enable[0:0] = RegAConfig__select[0:0];
  assign rega__enable[0:0] = RegBConfig__select[0:0];
  assign ALUMuxAconfig__ConfigIn = ConfigIn;
  assign ALUMuxBconfig__ConfigIn = ALUMuxAconfig__ConfigOut;
  assign ALUconfig__ConfigIn = ALUMuxBconfig__ConfigOut;
  assign Mux0config__ConfigIn = ALUconfig__ConfigOut;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux4config__ConfigIn = Mux3config__ConfigOut;
  assign Mux7config__ConfigIn = Mux4config__ConfigOut;
  assign RESConfig__ConfigIn = Mux7config__ConfigOut;
  assign Reg0config__ConfigIn = RESConfig__ConfigOut;
  assign Reg1config__ConfigIn = Reg0config__ConfigOut;
  assign Reg2config__ConfigIn = Reg1config__ConfigOut;
  assign Reg3config__ConfigIn = Reg2config__ConfigOut;
  assign Reg4config__ConfigIn = Reg3config__ConfigOut;
  assign Reg7config__ConfigIn = Reg4config__ConfigOut;
  assign RegAConfig__ConfigIn = Reg7config__ConfigOut;
  assign RegBConfig__ConfigIn = RegAConfig__ConfigOut;
  assign const__ConfigIn = RegBConfig__ConfigOut;
  assign crossbar__ConfigIn = const__ConfigOut;
  assign ConfigOut = crossbar__ConfigOut;

endmodule //hycube_no_pred_in159_out159_U10

module hycube_no_pred_in31_out31_U11 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in4,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out4
);
  //Wire declarations for instance 'ALU' (Module func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12)
  wire  ALU__CGRA_Clock;
  wire  ALU__CGRA_Enable;
  wire  ALU__CGRA_Reset;
  wire [31:0] ALU__in_a;
  wire [31:0] ALU__in_b;
  wire [31:0] ALU__out;
  wire [3:0] ALU__select;
  func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12 ALU(
    .CGRA_Clock(ALU__CGRA_Clock),
    .CGRA_Enable(ALU__CGRA_Enable),
    .CGRA_Reset(ALU__CGRA_Reset),
    .in_a(ALU__in_a),
    .in_b(ALU__in_b),
    .out(ALU__out),
    .select(ALU__select)
  );

  //Wire declarations for instance 'ALUMuxAconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxAconfig__CGRA_Clock;
  wire  ALUMuxAconfig__CGRA_Enable;
  wire  ALUMuxAconfig__CGRA_Reset;
  wire  ALUMuxAconfig__ConfigIn;
  wire  ALUMuxAconfig__ConfigOut;
  wire  ALUMuxAconfig__Config_Clock;
  wire  ALUMuxAconfig__Config_Reset;
  wire [0:0] ALUMuxAconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxAconfig(
    .CGRA_Clock(ALUMuxAconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxAconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxAconfig__CGRA_Reset),
    .ConfigIn(ALUMuxAconfig__ConfigIn),
    .ConfigOut(ALUMuxAconfig__ConfigOut),
    .Config_Clock(ALUMuxAconfig__Config_Clock),
    .Config_Reset(ALUMuxAconfig__Config_Reset),
    .select(ALUMuxAconfig__select)
  );

  //Wire declarations for instance 'ALUMuxBconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxBconfig__CGRA_Clock;
  wire  ALUMuxBconfig__CGRA_Enable;
  wire  ALUMuxBconfig__CGRA_Reset;
  wire  ALUMuxBconfig__ConfigIn;
  wire  ALUMuxBconfig__ConfigOut;
  wire  ALUMuxBconfig__Config_Clock;
  wire  ALUMuxBconfig__Config_Reset;
  wire [0:0] ALUMuxBconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxBconfig(
    .CGRA_Clock(ALUMuxBconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxBconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxBconfig__CGRA_Reset),
    .ConfigIn(ALUMuxBconfig__ConfigIn),
    .ConfigOut(ALUMuxBconfig__ConfigOut),
    .Config_Clock(ALUMuxBconfig__Config_Clock),
    .Config_Reset(ALUMuxBconfig__Config_Reset),
    .select(ALUMuxBconfig__select)
  );

  //Wire declarations for instance 'ALUconfig' (Module cgrame_configcell_context1_size4)
  wire  ALUconfig__CGRA_Clock;
  wire  ALUconfig__CGRA_Enable;
  wire  ALUconfig__CGRA_Reset;
  wire  ALUconfig__ConfigIn;
  wire  ALUconfig__ConfigOut;
  wire  ALUconfig__Config_Clock;
  wire  ALUconfig__Config_Reset;
  wire [3:0] ALUconfig__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) ALUconfig(
    .CGRA_Clock(ALUconfig__CGRA_Clock),
    .CGRA_Enable(ALUconfig__CGRA_Enable),
    .CGRA_Reset(ALUconfig__CGRA_Reset),
    .ConfigIn(ALUconfig__ConfigIn),
    .ConfigOut(ALUconfig__ConfigOut),
    .Config_Clock(ALUconfig__Config_Clock),
    .Config_Reset(ALUconfig__Config_Reset),
    .select(ALUconfig__select)
  );

  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size1)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [0:0] Mux0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size1)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [0:0] Mux1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size1)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [0:0] Mux2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size1)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [0:0] Mux3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux4config' (Module cgrame_configcell_context1_size1)
  wire  Mux4config__CGRA_Clock;
  wire  Mux4config__CGRA_Enable;
  wire  Mux4config__CGRA_Reset;
  wire  Mux4config__ConfigIn;
  wire  Mux4config__ConfigOut;
  wire  Mux4config__Config_Clock;
  wire  Mux4config__Config_Reset;
  wire [0:0] Mux4config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux4config(
    .CGRA_Clock(Mux4config__CGRA_Clock),
    .CGRA_Enable(Mux4config__CGRA_Enable),
    .CGRA_Reset(Mux4config__CGRA_Reset),
    .ConfigIn(Mux4config__ConfigIn),
    .ConfigOut(Mux4config__ConfigOut),
    .Config_Clock(Mux4config__Config_Clock),
    .Config_Reset(Mux4config__Config_Reset),
    .select(Mux4config__select)
  );

  //Wire declarations for instance 'RES' (Module cgrame_register_32b)
  wire  RES__CGRA_Clock;
  wire  RES__CGRA_Enable;
  wire  RES__CGRA_Reset;
  wire [0:0] RES__enable;
  wire [31:0] RES__in;
  wire [31:0] RES__out;
  cgrame_register_32b #(.contexts(1),.size(32)) RES(
    .CGRA_Clock(RES__CGRA_Clock),
    .CGRA_Enable(RES__CGRA_Enable),
    .CGRA_Reset(RES__CGRA_Reset),
    .enable(RES__enable),
    .in(RES__in),
    .out(RES__out)
  );

  //Wire declarations for instance 'RESConfig' (Module cgrame_configcell_context1_size1)
  wire  RESConfig__CGRA_Clock;
  wire  RESConfig__CGRA_Enable;
  wire  RESConfig__CGRA_Reset;
  wire  RESConfig__ConfigIn;
  wire  RESConfig__ConfigOut;
  wire  RESConfig__Config_Clock;
  wire  RESConfig__Config_Reset;
  wire [0:0] RESConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RESConfig(
    .CGRA_Clock(RESConfig__CGRA_Clock),
    .CGRA_Enable(RESConfig__CGRA_Enable),
    .CGRA_Reset(RESConfig__CGRA_Reset),
    .ConfigIn(RESConfig__ConfigIn),
    .ConfigOut(RESConfig__ConfigOut),
    .Config_Clock(RESConfig__Config_Clock),
    .Config_Reset(RESConfig__Config_Reset),
    .select(RESConfig__select)
  );

  //Wire declarations for instance 'Reg0config' (Module cgrame_configcell_context1_size1)
  wire  Reg0config__CGRA_Clock;
  wire  Reg0config__CGRA_Enable;
  wire  Reg0config__CGRA_Reset;
  wire  Reg0config__ConfigIn;
  wire  Reg0config__ConfigOut;
  wire  Reg0config__Config_Clock;
  wire  Reg0config__Config_Reset;
  wire [0:0] Reg0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg0config(
    .CGRA_Clock(Reg0config__CGRA_Clock),
    .CGRA_Enable(Reg0config__CGRA_Enable),
    .CGRA_Reset(Reg0config__CGRA_Reset),
    .ConfigIn(Reg0config__ConfigIn),
    .ConfigOut(Reg0config__ConfigOut),
    .Config_Clock(Reg0config__Config_Clock),
    .Config_Reset(Reg0config__Config_Reset),
    .select(Reg0config__select)
  );

  //Wire declarations for instance 'Reg1config' (Module cgrame_configcell_context1_size1)
  wire  Reg1config__CGRA_Clock;
  wire  Reg1config__CGRA_Enable;
  wire  Reg1config__CGRA_Reset;
  wire  Reg1config__ConfigIn;
  wire  Reg1config__ConfigOut;
  wire  Reg1config__Config_Clock;
  wire  Reg1config__Config_Reset;
  wire [0:0] Reg1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg1config(
    .CGRA_Clock(Reg1config__CGRA_Clock),
    .CGRA_Enable(Reg1config__CGRA_Enable),
    .CGRA_Reset(Reg1config__CGRA_Reset),
    .ConfigIn(Reg1config__ConfigIn),
    .ConfigOut(Reg1config__ConfigOut),
    .Config_Clock(Reg1config__Config_Clock),
    .Config_Reset(Reg1config__Config_Reset),
    .select(Reg1config__select)
  );

  //Wire declarations for instance 'Reg2config' (Module cgrame_configcell_context1_size1)
  wire  Reg2config__CGRA_Clock;
  wire  Reg2config__CGRA_Enable;
  wire  Reg2config__CGRA_Reset;
  wire  Reg2config__ConfigIn;
  wire  Reg2config__ConfigOut;
  wire  Reg2config__Config_Clock;
  wire  Reg2config__Config_Reset;
  wire [0:0] Reg2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg2config(
    .CGRA_Clock(Reg2config__CGRA_Clock),
    .CGRA_Enable(Reg2config__CGRA_Enable),
    .CGRA_Reset(Reg2config__CGRA_Reset),
    .ConfigIn(Reg2config__ConfigIn),
    .ConfigOut(Reg2config__ConfigOut),
    .Config_Clock(Reg2config__Config_Clock),
    .Config_Reset(Reg2config__Config_Reset),
    .select(Reg2config__select)
  );

  //Wire declarations for instance 'Reg3config' (Module cgrame_configcell_context1_size1)
  wire  Reg3config__CGRA_Clock;
  wire  Reg3config__CGRA_Enable;
  wire  Reg3config__CGRA_Reset;
  wire  Reg3config__ConfigIn;
  wire  Reg3config__ConfigOut;
  wire  Reg3config__Config_Clock;
  wire  Reg3config__Config_Reset;
  wire [0:0] Reg3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg3config(
    .CGRA_Clock(Reg3config__CGRA_Clock),
    .CGRA_Enable(Reg3config__CGRA_Enable),
    .CGRA_Reset(Reg3config__CGRA_Reset),
    .ConfigIn(Reg3config__ConfigIn),
    .ConfigOut(Reg3config__ConfigOut),
    .Config_Clock(Reg3config__Config_Clock),
    .Config_Reset(Reg3config__Config_Reset),
    .select(Reg3config__select)
  );

  //Wire declarations for instance 'Reg4config' (Module cgrame_configcell_context1_size1)
  wire  Reg4config__CGRA_Clock;
  wire  Reg4config__CGRA_Enable;
  wire  Reg4config__CGRA_Reset;
  wire  Reg4config__ConfigIn;
  wire  Reg4config__ConfigOut;
  wire  Reg4config__Config_Clock;
  wire  Reg4config__Config_Reset;
  wire [0:0] Reg4config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg4config(
    .CGRA_Clock(Reg4config__CGRA_Clock),
    .CGRA_Enable(Reg4config__CGRA_Enable),
    .CGRA_Reset(Reg4config__CGRA_Reset),
    .ConfigIn(Reg4config__ConfigIn),
    .ConfigOut(Reg4config__ConfigOut),
    .Config_Clock(Reg4config__Config_Clock),
    .Config_Reset(Reg4config__Config_Reset),
    .select(Reg4config__select)
  );

  //Wire declarations for instance 'RegAConfig' (Module cgrame_configcell_context1_size1)
  wire  RegAConfig__CGRA_Clock;
  wire  RegAConfig__CGRA_Enable;
  wire  RegAConfig__CGRA_Reset;
  wire  RegAConfig__ConfigIn;
  wire  RegAConfig__ConfigOut;
  wire  RegAConfig__Config_Clock;
  wire  RegAConfig__Config_Reset;
  wire [0:0] RegAConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegAConfig(
    .CGRA_Clock(RegAConfig__CGRA_Clock),
    .CGRA_Enable(RegAConfig__CGRA_Enable),
    .CGRA_Reset(RegAConfig__CGRA_Reset),
    .ConfigIn(RegAConfig__ConfigIn),
    .ConfigOut(RegAConfig__ConfigOut),
    .Config_Clock(RegAConfig__Config_Clock),
    .Config_Reset(RegAConfig__Config_Reset),
    .select(RegAConfig__select)
  );

  //Wire declarations for instance 'RegBConfig' (Module cgrame_configcell_context1_size1)
  wire  RegBConfig__CGRA_Clock;
  wire  RegBConfig__CGRA_Enable;
  wire  RegBConfig__CGRA_Reset;
  wire  RegBConfig__ConfigIn;
  wire  RegBConfig__ConfigOut;
  wire  RegBConfig__Config_Clock;
  wire  RegBConfig__Config_Reset;
  wire [0:0] RegBConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegBConfig(
    .CGRA_Clock(RegBConfig__CGRA_Clock),
    .CGRA_Enable(RegBConfig__CGRA_Enable),
    .CGRA_Reset(RegBConfig__CGRA_Reset),
    .ConfigIn(RegBConfig__ConfigIn),
    .ConfigOut(RegBConfig__ConfigOut),
    .Config_Clock(RegBConfig__Config_Clock),
    .Config_Reset(RegBConfig__Config_Reset),
    .select(RegBConfig__select)
  );

  //Wire declarations for instance 'alu_muxa' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxa__in0;
  wire [31:0] alu_muxa__in1;
  wire [31:0] alu_muxa__out;
  wire [0:0] alu_muxa__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxa(
    .in0(alu_muxa__in0),
    .in1(alu_muxa__in1),
    .out(alu_muxa__out),
    .select(alu_muxa__select)
  );

  //Wire declarations for instance 'alu_muxb' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxb__in0;
  wire [31:0] alu_muxb__in1;
  wire [31:0] alu_muxb__out;
  wire [0:0] alu_muxb__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxb(
    .in0(alu_muxb__in0),
    .in1(alu_muxb__in1),
    .out(alu_muxb__out),
    .select(alu_muxb__select)
  );

  //Wire declarations for instance 'const' (Module const_32b_U15)
  wire  const__CGRA_Clock;
  wire  const__CGRA_Enable;
  wire  const__CGRA_Reset;
  wire  const__ConfigIn;
  wire  const__ConfigOut;
  wire  const__Config_Clock;
  wire  const__Config_Reset;
  wire [31:0] const__out;
  const_32b_U15 const(
    .CGRA_Clock(const__CGRA_Clock),
    .CGRA_Enable(const__CGRA_Enable),
    .CGRA_Reset(const__CGRA_Reset),
    .ConfigIn(const__ConfigIn),
    .ConfigOut(const__ConfigOut),
    .Config_Clock(const__Config_Clock),
    .Config_Reset(const__Config_Reset),
    .out(const__out)
  );

  //Wire declarations for instance 'crossbar' (Module crossbar_7in_7out_U19)
  wire  crossbar__CGRA_Clock;
  wire  crossbar__CGRA_Enable;
  wire  crossbar__CGRA_Reset;
  wire  crossbar__ConfigIn;
  wire  crossbar__ConfigOut;
  wire  crossbar__Config_Clock;
  wire  crossbar__Config_Reset;
  wire [31:0] crossbar__in0;
  wire [31:0] crossbar__in1;
  wire [31:0] crossbar__in2;
  wire [31:0] crossbar__in3;
  wire [31:0] crossbar__in4;
  wire [31:0] crossbar__in5;
  wire [31:0] crossbar__in6;
  wire [31:0] crossbar__out0;
  wire [31:0] crossbar__out1;
  wire [31:0] crossbar__out2;
  wire [31:0] crossbar__out3;
  wire [31:0] crossbar__out4;
  wire [31:0] crossbar__out5;
  wire [31:0] crossbar__out6;
  crossbar_7in_7out_U19 crossbar(
    .CGRA_Clock(crossbar__CGRA_Clock),
    .CGRA_Enable(crossbar__CGRA_Enable),
    .CGRA_Reset(crossbar__CGRA_Reset),
    .ConfigIn(crossbar__ConfigIn),
    .ConfigOut(crossbar__ConfigOut),
    .Config_Clock(crossbar__Config_Clock),
    .Config_Reset(crossbar__Config_Reset),
    .in0(crossbar__in0),
    .in1(crossbar__in1),
    .in2(crossbar__in2),
    .in3(crossbar__in3),
    .in4(crossbar__in4),
    .in5(crossbar__in5),
    .in6(crossbar__in6),
    .out0(crossbar__out0),
    .out1(crossbar__out1),
    .out2(crossbar__out2),
    .out3(crossbar__out3),
    .out4(crossbar__out4),
    .out5(crossbar__out5),
    .out6(crossbar__out6)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__out;
  wire [0:0] mux_0__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__out;
  wire [0:0] mux_1__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__out;
  wire [0:0] mux_2__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__out;
  wire [0:0] mux_3__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_4' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_4__in0;
  wire [31:0] mux_4__in1;
  wire [31:0] mux_4__out;
  wire [0:0] mux_4__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_4(
    .in0(mux_4__in0),
    .in1(mux_4__in1),
    .out(mux_4__out),
    .select(mux_4__select)
  );

  //Wire declarations for instance 'reg0' (Module cgrame_register_32b)
  wire  reg0__CGRA_Clock;
  wire  reg0__CGRA_Enable;
  wire  reg0__CGRA_Reset;
  wire [0:0] reg0__enable;
  wire [31:0] reg0__in;
  wire [31:0] reg0__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg0(
    .CGRA_Clock(reg0__CGRA_Clock),
    .CGRA_Enable(reg0__CGRA_Enable),
    .CGRA_Reset(reg0__CGRA_Reset),
    .enable(reg0__enable),
    .in(reg0__in),
    .out(reg0__out)
  );

  //Wire declarations for instance 'reg1' (Module cgrame_register_32b)
  wire  reg1__CGRA_Clock;
  wire  reg1__CGRA_Enable;
  wire  reg1__CGRA_Reset;
  wire [0:0] reg1__enable;
  wire [31:0] reg1__in;
  wire [31:0] reg1__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg1(
    .CGRA_Clock(reg1__CGRA_Clock),
    .CGRA_Enable(reg1__CGRA_Enable),
    .CGRA_Reset(reg1__CGRA_Reset),
    .enable(reg1__enable),
    .in(reg1__in),
    .out(reg1__out)
  );

  //Wire declarations for instance 'reg2' (Module cgrame_register_32b)
  wire  reg2__CGRA_Clock;
  wire  reg2__CGRA_Enable;
  wire  reg2__CGRA_Reset;
  wire [0:0] reg2__enable;
  wire [31:0] reg2__in;
  wire [31:0] reg2__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg2(
    .CGRA_Clock(reg2__CGRA_Clock),
    .CGRA_Enable(reg2__CGRA_Enable),
    .CGRA_Reset(reg2__CGRA_Reset),
    .enable(reg2__enable),
    .in(reg2__in),
    .out(reg2__out)
  );

  //Wire declarations for instance 'reg3' (Module cgrame_register_32b)
  wire  reg3__CGRA_Clock;
  wire  reg3__CGRA_Enable;
  wire  reg3__CGRA_Reset;
  wire [0:0] reg3__enable;
  wire [31:0] reg3__in;
  wire [31:0] reg3__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg3(
    .CGRA_Clock(reg3__CGRA_Clock),
    .CGRA_Enable(reg3__CGRA_Enable),
    .CGRA_Reset(reg3__CGRA_Reset),
    .enable(reg3__enable),
    .in(reg3__in),
    .out(reg3__out)
  );

  //Wire declarations for instance 'reg4' (Module cgrame_register_32b)
  wire  reg4__CGRA_Clock;
  wire  reg4__CGRA_Enable;
  wire  reg4__CGRA_Reset;
  wire [0:0] reg4__enable;
  wire [31:0] reg4__in;
  wire [31:0] reg4__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg4(
    .CGRA_Clock(reg4__CGRA_Clock),
    .CGRA_Enable(reg4__CGRA_Enable),
    .CGRA_Reset(reg4__CGRA_Reset),
    .enable(reg4__enable),
    .in(reg4__in),
    .out(reg4__out)
  );

  //Wire declarations for instance 'rega' (Module cgrame_register_32b)
  wire  rega__CGRA_Clock;
  wire  rega__CGRA_Enable;
  wire  rega__CGRA_Reset;
  wire [0:0] rega__enable;
  wire [31:0] rega__in;
  wire [31:0] rega__out;
  cgrame_register_32b #(.contexts(1),.size(32)) rega(
    .CGRA_Clock(rega__CGRA_Clock),
    .CGRA_Enable(rega__CGRA_Enable),
    .CGRA_Reset(rega__CGRA_Reset),
    .enable(rega__enable),
    .in(rega__in),
    .out(rega__out)
  );

  //Wire declarations for instance 'regb' (Module cgrame_register_32b)
  wire  regb__CGRA_Clock;
  wire  regb__CGRA_Enable;
  wire  regb__CGRA_Reset;
  wire [0:0] regb__enable;
  wire [31:0] regb__in;
  wire [31:0] regb__out;
  cgrame_register_32b #(.contexts(1),.size(32)) regb(
    .CGRA_Clock(regb__CGRA_Clock),
    .CGRA_Enable(regb__CGRA_Enable),
    .CGRA_Reset(regb__CGRA_Reset),
    .enable(regb__enable),
    .in(regb__in),
    .out(regb__out)
  );

  //All the connections
  assign ALU__CGRA_Reset = CGRA_Reset;
  assign ALU__CGRA_Clock = CGRA_Clock;
  assign RES__CGRA_Clock = CGRA_Clock;
  assign const__CGRA_Clock = CGRA_Clock;
  assign crossbar__CGRA_Clock = CGRA_Clock;
  assign reg0__CGRA_Clock = CGRA_Clock;
  assign reg1__CGRA_Clock = CGRA_Clock;
  assign reg2__CGRA_Clock = CGRA_Clock;
  assign reg3__CGRA_Clock = CGRA_Clock;
  assign reg4__CGRA_Clock = CGRA_Clock;
  assign rega__CGRA_Clock = CGRA_Clock;
  assign regb__CGRA_Clock = CGRA_Clock;
  assign ALUMuxAconfig__CGRA_Clock = CGRA_Clock;
  assign ALUMuxBconfig__CGRA_Clock = CGRA_Clock;
  assign ALUconfig__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux4config__CGRA_Clock = CGRA_Clock;
  assign RESConfig__CGRA_Clock = CGRA_Clock;
  assign Reg0config__CGRA_Clock = CGRA_Clock;
  assign Reg1config__CGRA_Clock = CGRA_Clock;
  assign Reg2config__CGRA_Clock = CGRA_Clock;
  assign Reg3config__CGRA_Clock = CGRA_Clock;
  assign Reg4config__CGRA_Clock = CGRA_Clock;
  assign RegAConfig__CGRA_Clock = CGRA_Clock;
  assign RegBConfig__CGRA_Clock = CGRA_Clock;
  assign ALU__CGRA_Enable = CGRA_Enable;
  assign RES__CGRA_Reset = CGRA_Reset;
  assign const__CGRA_Reset = CGRA_Reset;
  assign crossbar__CGRA_Reset = CGRA_Reset;
  assign reg0__CGRA_Reset = CGRA_Reset;
  assign reg1__CGRA_Reset = CGRA_Reset;
  assign reg2__CGRA_Reset = CGRA_Reset;
  assign reg3__CGRA_Reset = CGRA_Reset;
  assign reg4__CGRA_Reset = CGRA_Reset;
  assign rega__CGRA_Reset = CGRA_Reset;
  assign regb__CGRA_Reset = CGRA_Reset;
  assign ALUMuxAconfig__CGRA_Reset = CGRA_Reset;
  assign ALUMuxBconfig__CGRA_Reset = CGRA_Reset;
  assign ALUconfig__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux4config__CGRA_Reset = CGRA_Reset;
  assign RESConfig__CGRA_Reset = CGRA_Reset;
  assign Reg0config__CGRA_Reset = CGRA_Reset;
  assign Reg1config__CGRA_Reset = CGRA_Reset;
  assign Reg2config__CGRA_Reset = CGRA_Reset;
  assign Reg3config__CGRA_Reset = CGRA_Reset;
  assign Reg4config__CGRA_Reset = CGRA_Reset;
  assign RegAConfig__CGRA_Reset = CGRA_Reset;
  assign RegBConfig__CGRA_Reset = CGRA_Reset;
  assign RES__CGRA_Enable = CGRA_Enable;
  assign const__CGRA_Enable = CGRA_Enable;
  assign crossbar__CGRA_Enable = CGRA_Enable;
  assign reg0__CGRA_Enable = CGRA_Enable;
  assign reg1__CGRA_Enable = CGRA_Enable;
  assign reg2__CGRA_Enable = CGRA_Enable;
  assign reg3__CGRA_Enable = CGRA_Enable;
  assign reg4__CGRA_Enable = CGRA_Enable;
  assign rega__CGRA_Enable = CGRA_Enable;
  assign regb__CGRA_Enable = CGRA_Enable;
  assign ALUMuxAconfig__CGRA_Enable = CGRA_Enable;
  assign ALUMuxBconfig__CGRA_Enable = CGRA_Enable;
  assign ALUconfig__CGRA_Enable = CGRA_Enable;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux4config__CGRA_Enable = CGRA_Enable;
  assign RESConfig__CGRA_Enable = CGRA_Enable;
  assign Reg0config__CGRA_Enable = CGRA_Enable;
  assign Reg1config__CGRA_Enable = CGRA_Enable;
  assign Reg2config__CGRA_Enable = CGRA_Enable;
  assign Reg3config__CGRA_Enable = CGRA_Enable;
  assign Reg4config__CGRA_Enable = CGRA_Enable;
  assign RegAConfig__CGRA_Enable = CGRA_Enable;
  assign RegBConfig__CGRA_Enable = CGRA_Enable;
  assign const__Config_Clock = Config_Clock;
  assign const__Config_Reset = Config_Reset;
  assign crossbar__Config_Clock = Config_Clock;
  assign ALUMuxAconfig__Config_Clock = Config_Clock;
  assign ALUMuxBconfig__Config_Clock = Config_Clock;
  assign ALUconfig__Config_Clock = Config_Clock;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux4config__Config_Clock = Config_Clock;
  assign RESConfig__Config_Clock = Config_Clock;
  assign Reg0config__Config_Clock = Config_Clock;
  assign Reg1config__Config_Clock = Config_Clock;
  assign Reg2config__Config_Clock = Config_Clock;
  assign Reg3config__Config_Clock = Config_Clock;
  assign Reg4config__Config_Clock = Config_Clock;
  assign RegAConfig__Config_Clock = Config_Clock;
  assign RegBConfig__Config_Clock = Config_Clock;
  assign crossbar__Config_Reset = Config_Reset;
  assign ALUMuxAconfig__Config_Reset = Config_Reset;
  assign ALUMuxBconfig__Config_Reset = Config_Reset;
  assign ALUconfig__Config_Reset = Config_Reset;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux4config__Config_Reset = Config_Reset;
  assign RESConfig__Config_Reset = Config_Reset;
  assign Reg0config__Config_Reset = Config_Reset;
  assign Reg1config__Config_Reset = Config_Reset;
  assign Reg2config__Config_Reset = Config_Reset;
  assign Reg3config__Config_Reset = Config_Reset;
  assign Reg4config__Config_Reset = Config_Reset;
  assign RegAConfig__Config_Reset = Config_Reset;
  assign RegBConfig__Config_Reset = Config_Reset;
  assign crossbar__in5[31:0] = ALU__out[31:0];
  assign RES__in[31:0] = ALU__out[31:0];
  assign out0[31:0] = crossbar__out0[31:0];
  assign out1[31:0] = crossbar__out1[31:0];
  assign out2[31:0] = crossbar__out2[31:0];
  assign out3[31:0] = crossbar__out3[31:0];
  assign out4[31:0] = crossbar__out4[31:0];
  assign rega__in[31:0] = crossbar__out5[31:0];
  assign regb__in[31:0] = crossbar__out6[31:0];
  assign crossbar__in6[31:0] = RES__out[31:0];
  assign alu_muxa__in0[31:0] = rega__out[31:0];
  assign alu_muxb__in0[31:0] = regb__out[31:0];
  assign alu_muxa__in1[31:0] = const__out[31:0];
  assign alu_muxb__in1[31:0] = const__out[31:0];
  assign ALU__in_a[31:0] = alu_muxa__out[31:0];
  assign ALU__in_b[31:0] = alu_muxb__out[31:0];
  assign mux_0__in1[31:0] = reg0__out[31:0];
  assign crossbar__in0[31:0] = mux_0__out[31:0];
  assign mux_1__in1[31:0] = reg1__out[31:0];
  assign crossbar__in1[31:0] = mux_1__out[31:0];
  assign mux_2__in1[31:0] = reg2__out[31:0];
  assign crossbar__in2[31:0] = mux_2__out[31:0];
  assign mux_3__in1[31:0] = reg3__out[31:0];
  assign crossbar__in3[31:0] = mux_3__out[31:0];
  assign mux_4__in1[31:0] = reg4__out[31:0];
  assign crossbar__in4[31:0] = mux_4__out[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign reg0__in[31:0] = in0[31:0];
  assign mux_1__in0[31:0] = in1[31:0];
  assign reg1__in[31:0] = in1[31:0];
  assign mux_2__in0[31:0] = in2[31:0];
  assign reg2__in[31:0] = in2[31:0];
  assign mux_3__in0[31:0] = in3[31:0];
  assign reg3__in[31:0] = in3[31:0];
  assign mux_4__in0[31:0] = in4[31:0];
  assign reg4__in[31:0] = in4[31:0];
  assign alu_muxa__select[0:0] = ALUMuxAconfig__select[0:0];
  assign alu_muxb__select[0:0] = ALUMuxBconfig__select[0:0];
  assign ALU__select[3:0] = ALUconfig__select[3:0];
  assign mux_0__select[0:0] = Mux0config__select[0:0];
  assign mux_1__select[0:0] = Mux1config__select[0:0];
  assign mux_2__select[0:0] = Mux2config__select[0:0];
  assign mux_3__select[0:0] = Mux3config__select[0:0];
  assign mux_4__select[0:0] = Mux4config__select[0:0];
  assign RES__enable[0:0] = RESConfig__select[0:0];
  assign reg0__enable[0:0] = Reg0config__select[0:0];
  assign reg1__enable[0:0] = Reg1config__select[0:0];
  assign reg2__enable[0:0] = Reg2config__select[0:0];
  assign reg3__enable[0:0] = Reg3config__select[0:0];
  assign reg4__enable[0:0] = Reg4config__select[0:0];
  assign regb__enable[0:0] = RegAConfig__select[0:0];
  assign rega__enable[0:0] = RegBConfig__select[0:0];
  assign ALUMuxAconfig__ConfigIn = ConfigIn;
  assign ALUMuxBconfig__ConfigIn = ALUMuxAconfig__ConfigOut;
  assign ALUconfig__ConfigIn = ALUMuxBconfig__ConfigOut;
  assign Mux0config__ConfigIn = ALUconfig__ConfigOut;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux4config__ConfigIn = Mux3config__ConfigOut;
  assign RESConfig__ConfigIn = Mux4config__ConfigOut;
  assign Reg0config__ConfigIn = RESConfig__ConfigOut;
  assign Reg1config__ConfigIn = Reg0config__ConfigOut;
  assign Reg2config__ConfigIn = Reg1config__ConfigOut;
  assign Reg3config__ConfigIn = Reg2config__ConfigOut;
  assign Reg4config__ConfigIn = Reg3config__ConfigOut;
  assign RegAConfig__ConfigIn = Reg4config__ConfigOut;
  assign RegBConfig__ConfigIn = RegAConfig__ConfigOut;
  assign const__ConfigIn = RegBConfig__ConfigOut;
  assign crossbar__ConfigIn = const__ConfigOut;
  assign ConfigOut = crossbar__ConfigOut;

endmodule //hycube_no_pred_in31_out31_U11

module hycube_no_pred_in47_out47_U5 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in5,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out5
);
  //Wire declarations for instance 'ALU' (Module func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12)
  wire  ALU__CGRA_Clock;
  wire  ALU__CGRA_Enable;
  wire  ALU__CGRA_Reset;
  wire [31:0] ALU__in_a;
  wire [31:0] ALU__in_b;
  wire [31:0] ALU__out;
  wire [3:0] ALU__select;
  func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12 ALU(
    .CGRA_Clock(ALU__CGRA_Clock),
    .CGRA_Enable(ALU__CGRA_Enable),
    .CGRA_Reset(ALU__CGRA_Reset),
    .in_a(ALU__in_a),
    .in_b(ALU__in_b),
    .out(ALU__out),
    .select(ALU__select)
  );

  //Wire declarations for instance 'ALUMuxAconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxAconfig__CGRA_Clock;
  wire  ALUMuxAconfig__CGRA_Enable;
  wire  ALUMuxAconfig__CGRA_Reset;
  wire  ALUMuxAconfig__ConfigIn;
  wire  ALUMuxAconfig__ConfigOut;
  wire  ALUMuxAconfig__Config_Clock;
  wire  ALUMuxAconfig__Config_Reset;
  wire [0:0] ALUMuxAconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxAconfig(
    .CGRA_Clock(ALUMuxAconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxAconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxAconfig__CGRA_Reset),
    .ConfigIn(ALUMuxAconfig__ConfigIn),
    .ConfigOut(ALUMuxAconfig__ConfigOut),
    .Config_Clock(ALUMuxAconfig__Config_Clock),
    .Config_Reset(ALUMuxAconfig__Config_Reset),
    .select(ALUMuxAconfig__select)
  );

  //Wire declarations for instance 'ALUMuxBconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxBconfig__CGRA_Clock;
  wire  ALUMuxBconfig__CGRA_Enable;
  wire  ALUMuxBconfig__CGRA_Reset;
  wire  ALUMuxBconfig__ConfigIn;
  wire  ALUMuxBconfig__ConfigOut;
  wire  ALUMuxBconfig__Config_Clock;
  wire  ALUMuxBconfig__Config_Reset;
  wire [0:0] ALUMuxBconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxBconfig(
    .CGRA_Clock(ALUMuxBconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxBconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxBconfig__CGRA_Reset),
    .ConfigIn(ALUMuxBconfig__ConfigIn),
    .ConfigOut(ALUMuxBconfig__ConfigOut),
    .Config_Clock(ALUMuxBconfig__Config_Clock),
    .Config_Reset(ALUMuxBconfig__Config_Reset),
    .select(ALUMuxBconfig__select)
  );

  //Wire declarations for instance 'ALUconfig' (Module cgrame_configcell_context1_size4)
  wire  ALUconfig__CGRA_Clock;
  wire  ALUconfig__CGRA_Enable;
  wire  ALUconfig__CGRA_Reset;
  wire  ALUconfig__ConfigIn;
  wire  ALUconfig__ConfigOut;
  wire  ALUconfig__Config_Clock;
  wire  ALUconfig__Config_Reset;
  wire [3:0] ALUconfig__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) ALUconfig(
    .CGRA_Clock(ALUconfig__CGRA_Clock),
    .CGRA_Enable(ALUconfig__CGRA_Enable),
    .CGRA_Reset(ALUconfig__CGRA_Reset),
    .ConfigIn(ALUconfig__ConfigIn),
    .ConfigOut(ALUconfig__ConfigOut),
    .Config_Clock(ALUconfig__Config_Clock),
    .Config_Reset(ALUconfig__Config_Reset),
    .select(ALUconfig__select)
  );

  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size1)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [0:0] Mux0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size1)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [0:0] Mux1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size1)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [0:0] Mux2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size1)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [0:0] Mux3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux5config' (Module cgrame_configcell_context1_size1)
  wire  Mux5config__CGRA_Clock;
  wire  Mux5config__CGRA_Enable;
  wire  Mux5config__CGRA_Reset;
  wire  Mux5config__ConfigIn;
  wire  Mux5config__ConfigOut;
  wire  Mux5config__Config_Clock;
  wire  Mux5config__Config_Reset;
  wire [0:0] Mux5config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux5config(
    .CGRA_Clock(Mux5config__CGRA_Clock),
    .CGRA_Enable(Mux5config__CGRA_Enable),
    .CGRA_Reset(Mux5config__CGRA_Reset),
    .ConfigIn(Mux5config__ConfigIn),
    .ConfigOut(Mux5config__ConfigOut),
    .Config_Clock(Mux5config__Config_Clock),
    .Config_Reset(Mux5config__Config_Reset),
    .select(Mux5config__select)
  );

  //Wire declarations for instance 'RES' (Module cgrame_register_32b)
  wire  RES__CGRA_Clock;
  wire  RES__CGRA_Enable;
  wire  RES__CGRA_Reset;
  wire [0:0] RES__enable;
  wire [31:0] RES__in;
  wire [31:0] RES__out;
  cgrame_register_32b #(.contexts(1),.size(32)) RES(
    .CGRA_Clock(RES__CGRA_Clock),
    .CGRA_Enable(RES__CGRA_Enable),
    .CGRA_Reset(RES__CGRA_Reset),
    .enable(RES__enable),
    .in(RES__in),
    .out(RES__out)
  );

  //Wire declarations for instance 'RESConfig' (Module cgrame_configcell_context1_size1)
  wire  RESConfig__CGRA_Clock;
  wire  RESConfig__CGRA_Enable;
  wire  RESConfig__CGRA_Reset;
  wire  RESConfig__ConfigIn;
  wire  RESConfig__ConfigOut;
  wire  RESConfig__Config_Clock;
  wire  RESConfig__Config_Reset;
  wire [0:0] RESConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RESConfig(
    .CGRA_Clock(RESConfig__CGRA_Clock),
    .CGRA_Enable(RESConfig__CGRA_Enable),
    .CGRA_Reset(RESConfig__CGRA_Reset),
    .ConfigIn(RESConfig__ConfigIn),
    .ConfigOut(RESConfig__ConfigOut),
    .Config_Clock(RESConfig__Config_Clock),
    .Config_Reset(RESConfig__Config_Reset),
    .select(RESConfig__select)
  );

  //Wire declarations for instance 'Reg0config' (Module cgrame_configcell_context1_size1)
  wire  Reg0config__CGRA_Clock;
  wire  Reg0config__CGRA_Enable;
  wire  Reg0config__CGRA_Reset;
  wire  Reg0config__ConfigIn;
  wire  Reg0config__ConfigOut;
  wire  Reg0config__Config_Clock;
  wire  Reg0config__Config_Reset;
  wire [0:0] Reg0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg0config(
    .CGRA_Clock(Reg0config__CGRA_Clock),
    .CGRA_Enable(Reg0config__CGRA_Enable),
    .CGRA_Reset(Reg0config__CGRA_Reset),
    .ConfigIn(Reg0config__ConfigIn),
    .ConfigOut(Reg0config__ConfigOut),
    .Config_Clock(Reg0config__Config_Clock),
    .Config_Reset(Reg0config__Config_Reset),
    .select(Reg0config__select)
  );

  //Wire declarations for instance 'Reg1config' (Module cgrame_configcell_context1_size1)
  wire  Reg1config__CGRA_Clock;
  wire  Reg1config__CGRA_Enable;
  wire  Reg1config__CGRA_Reset;
  wire  Reg1config__ConfigIn;
  wire  Reg1config__ConfigOut;
  wire  Reg1config__Config_Clock;
  wire  Reg1config__Config_Reset;
  wire [0:0] Reg1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg1config(
    .CGRA_Clock(Reg1config__CGRA_Clock),
    .CGRA_Enable(Reg1config__CGRA_Enable),
    .CGRA_Reset(Reg1config__CGRA_Reset),
    .ConfigIn(Reg1config__ConfigIn),
    .ConfigOut(Reg1config__ConfigOut),
    .Config_Clock(Reg1config__Config_Clock),
    .Config_Reset(Reg1config__Config_Reset),
    .select(Reg1config__select)
  );

  //Wire declarations for instance 'Reg2config' (Module cgrame_configcell_context1_size1)
  wire  Reg2config__CGRA_Clock;
  wire  Reg2config__CGRA_Enable;
  wire  Reg2config__CGRA_Reset;
  wire  Reg2config__ConfigIn;
  wire  Reg2config__ConfigOut;
  wire  Reg2config__Config_Clock;
  wire  Reg2config__Config_Reset;
  wire [0:0] Reg2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg2config(
    .CGRA_Clock(Reg2config__CGRA_Clock),
    .CGRA_Enable(Reg2config__CGRA_Enable),
    .CGRA_Reset(Reg2config__CGRA_Reset),
    .ConfigIn(Reg2config__ConfigIn),
    .ConfigOut(Reg2config__ConfigOut),
    .Config_Clock(Reg2config__Config_Clock),
    .Config_Reset(Reg2config__Config_Reset),
    .select(Reg2config__select)
  );

  //Wire declarations for instance 'Reg3config' (Module cgrame_configcell_context1_size1)
  wire  Reg3config__CGRA_Clock;
  wire  Reg3config__CGRA_Enable;
  wire  Reg3config__CGRA_Reset;
  wire  Reg3config__ConfigIn;
  wire  Reg3config__ConfigOut;
  wire  Reg3config__Config_Clock;
  wire  Reg3config__Config_Reset;
  wire [0:0] Reg3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg3config(
    .CGRA_Clock(Reg3config__CGRA_Clock),
    .CGRA_Enable(Reg3config__CGRA_Enable),
    .CGRA_Reset(Reg3config__CGRA_Reset),
    .ConfigIn(Reg3config__ConfigIn),
    .ConfigOut(Reg3config__ConfigOut),
    .Config_Clock(Reg3config__Config_Clock),
    .Config_Reset(Reg3config__Config_Reset),
    .select(Reg3config__select)
  );

  //Wire declarations for instance 'Reg5config' (Module cgrame_configcell_context1_size1)
  wire  Reg5config__CGRA_Clock;
  wire  Reg5config__CGRA_Enable;
  wire  Reg5config__CGRA_Reset;
  wire  Reg5config__ConfigIn;
  wire  Reg5config__ConfigOut;
  wire  Reg5config__Config_Clock;
  wire  Reg5config__Config_Reset;
  wire [0:0] Reg5config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg5config(
    .CGRA_Clock(Reg5config__CGRA_Clock),
    .CGRA_Enable(Reg5config__CGRA_Enable),
    .CGRA_Reset(Reg5config__CGRA_Reset),
    .ConfigIn(Reg5config__ConfigIn),
    .ConfigOut(Reg5config__ConfigOut),
    .Config_Clock(Reg5config__Config_Clock),
    .Config_Reset(Reg5config__Config_Reset),
    .select(Reg5config__select)
  );

  //Wire declarations for instance 'RegAConfig' (Module cgrame_configcell_context1_size1)
  wire  RegAConfig__CGRA_Clock;
  wire  RegAConfig__CGRA_Enable;
  wire  RegAConfig__CGRA_Reset;
  wire  RegAConfig__ConfigIn;
  wire  RegAConfig__ConfigOut;
  wire  RegAConfig__Config_Clock;
  wire  RegAConfig__Config_Reset;
  wire [0:0] RegAConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegAConfig(
    .CGRA_Clock(RegAConfig__CGRA_Clock),
    .CGRA_Enable(RegAConfig__CGRA_Enable),
    .CGRA_Reset(RegAConfig__CGRA_Reset),
    .ConfigIn(RegAConfig__ConfigIn),
    .ConfigOut(RegAConfig__ConfigOut),
    .Config_Clock(RegAConfig__Config_Clock),
    .Config_Reset(RegAConfig__Config_Reset),
    .select(RegAConfig__select)
  );

  //Wire declarations for instance 'RegBConfig' (Module cgrame_configcell_context1_size1)
  wire  RegBConfig__CGRA_Clock;
  wire  RegBConfig__CGRA_Enable;
  wire  RegBConfig__CGRA_Reset;
  wire  RegBConfig__ConfigIn;
  wire  RegBConfig__ConfigOut;
  wire  RegBConfig__Config_Clock;
  wire  RegBConfig__Config_Reset;
  wire [0:0] RegBConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegBConfig(
    .CGRA_Clock(RegBConfig__CGRA_Clock),
    .CGRA_Enable(RegBConfig__CGRA_Enable),
    .CGRA_Reset(RegBConfig__CGRA_Reset),
    .ConfigIn(RegBConfig__ConfigIn),
    .ConfigOut(RegBConfig__ConfigOut),
    .Config_Clock(RegBConfig__Config_Clock),
    .Config_Reset(RegBConfig__Config_Reset),
    .select(RegBConfig__select)
  );

  //Wire declarations for instance 'alu_muxa' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxa__in0;
  wire [31:0] alu_muxa__in1;
  wire [31:0] alu_muxa__out;
  wire [0:0] alu_muxa__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxa(
    .in0(alu_muxa__in0),
    .in1(alu_muxa__in1),
    .out(alu_muxa__out),
    .select(alu_muxa__select)
  );

  //Wire declarations for instance 'alu_muxb' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxb__in0;
  wire [31:0] alu_muxb__in1;
  wire [31:0] alu_muxb__out;
  wire [0:0] alu_muxb__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxb(
    .in0(alu_muxb__in0),
    .in1(alu_muxb__in1),
    .out(alu_muxb__out),
    .select(alu_muxb__select)
  );

  //Wire declarations for instance 'const' (Module const_32b_U15)
  wire  const__CGRA_Clock;
  wire  const__CGRA_Enable;
  wire  const__CGRA_Reset;
  wire  const__ConfigIn;
  wire  const__ConfigOut;
  wire  const__Config_Clock;
  wire  const__Config_Reset;
  wire [31:0] const__out;
  const_32b_U15 const(
    .CGRA_Clock(const__CGRA_Clock),
    .CGRA_Enable(const__CGRA_Enable),
    .CGRA_Reset(const__CGRA_Reset),
    .ConfigIn(const__ConfigIn),
    .ConfigOut(const__ConfigOut),
    .Config_Clock(const__Config_Clock),
    .Config_Reset(const__Config_Reset),
    .out(const__out)
  );

  //Wire declarations for instance 'crossbar' (Module crossbar_7in_7out_U19)
  wire  crossbar__CGRA_Clock;
  wire  crossbar__CGRA_Enable;
  wire  crossbar__CGRA_Reset;
  wire  crossbar__ConfigIn;
  wire  crossbar__ConfigOut;
  wire  crossbar__Config_Clock;
  wire  crossbar__Config_Reset;
  wire [31:0] crossbar__in0;
  wire [31:0] crossbar__in1;
  wire [31:0] crossbar__in2;
  wire [31:0] crossbar__in3;
  wire [31:0] crossbar__in4;
  wire [31:0] crossbar__in5;
  wire [31:0] crossbar__in6;
  wire [31:0] crossbar__out0;
  wire [31:0] crossbar__out1;
  wire [31:0] crossbar__out2;
  wire [31:0] crossbar__out3;
  wire [31:0] crossbar__out4;
  wire [31:0] crossbar__out5;
  wire [31:0] crossbar__out6;
  crossbar_7in_7out_U19 crossbar(
    .CGRA_Clock(crossbar__CGRA_Clock),
    .CGRA_Enable(crossbar__CGRA_Enable),
    .CGRA_Reset(crossbar__CGRA_Reset),
    .ConfigIn(crossbar__ConfigIn),
    .ConfigOut(crossbar__ConfigOut),
    .Config_Clock(crossbar__Config_Clock),
    .Config_Reset(crossbar__Config_Reset),
    .in0(crossbar__in0),
    .in1(crossbar__in1),
    .in2(crossbar__in2),
    .in3(crossbar__in3),
    .in4(crossbar__in4),
    .in5(crossbar__in5),
    .in6(crossbar__in6),
    .out0(crossbar__out0),
    .out1(crossbar__out1),
    .out2(crossbar__out2),
    .out3(crossbar__out3),
    .out4(crossbar__out4),
    .out5(crossbar__out5),
    .out6(crossbar__out6)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__out;
  wire [0:0] mux_0__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__out;
  wire [0:0] mux_1__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__out;
  wire [0:0] mux_2__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__out;
  wire [0:0] mux_3__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_5' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_5__in0;
  wire [31:0] mux_5__in1;
  wire [31:0] mux_5__out;
  wire [0:0] mux_5__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_5(
    .in0(mux_5__in0),
    .in1(mux_5__in1),
    .out(mux_5__out),
    .select(mux_5__select)
  );

  //Wire declarations for instance 'reg0' (Module cgrame_register_32b)
  wire  reg0__CGRA_Clock;
  wire  reg0__CGRA_Enable;
  wire  reg0__CGRA_Reset;
  wire [0:0] reg0__enable;
  wire [31:0] reg0__in;
  wire [31:0] reg0__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg0(
    .CGRA_Clock(reg0__CGRA_Clock),
    .CGRA_Enable(reg0__CGRA_Enable),
    .CGRA_Reset(reg0__CGRA_Reset),
    .enable(reg0__enable),
    .in(reg0__in),
    .out(reg0__out)
  );

  //Wire declarations for instance 'reg1' (Module cgrame_register_32b)
  wire  reg1__CGRA_Clock;
  wire  reg1__CGRA_Enable;
  wire  reg1__CGRA_Reset;
  wire [0:0] reg1__enable;
  wire [31:0] reg1__in;
  wire [31:0] reg1__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg1(
    .CGRA_Clock(reg1__CGRA_Clock),
    .CGRA_Enable(reg1__CGRA_Enable),
    .CGRA_Reset(reg1__CGRA_Reset),
    .enable(reg1__enable),
    .in(reg1__in),
    .out(reg1__out)
  );

  //Wire declarations for instance 'reg2' (Module cgrame_register_32b)
  wire  reg2__CGRA_Clock;
  wire  reg2__CGRA_Enable;
  wire  reg2__CGRA_Reset;
  wire [0:0] reg2__enable;
  wire [31:0] reg2__in;
  wire [31:0] reg2__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg2(
    .CGRA_Clock(reg2__CGRA_Clock),
    .CGRA_Enable(reg2__CGRA_Enable),
    .CGRA_Reset(reg2__CGRA_Reset),
    .enable(reg2__enable),
    .in(reg2__in),
    .out(reg2__out)
  );

  //Wire declarations for instance 'reg3' (Module cgrame_register_32b)
  wire  reg3__CGRA_Clock;
  wire  reg3__CGRA_Enable;
  wire  reg3__CGRA_Reset;
  wire [0:0] reg3__enable;
  wire [31:0] reg3__in;
  wire [31:0] reg3__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg3(
    .CGRA_Clock(reg3__CGRA_Clock),
    .CGRA_Enable(reg3__CGRA_Enable),
    .CGRA_Reset(reg3__CGRA_Reset),
    .enable(reg3__enable),
    .in(reg3__in),
    .out(reg3__out)
  );

  //Wire declarations for instance 'reg5' (Module cgrame_register_32b)
  wire  reg5__CGRA_Clock;
  wire  reg5__CGRA_Enable;
  wire  reg5__CGRA_Reset;
  wire [0:0] reg5__enable;
  wire [31:0] reg5__in;
  wire [31:0] reg5__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg5(
    .CGRA_Clock(reg5__CGRA_Clock),
    .CGRA_Enable(reg5__CGRA_Enable),
    .CGRA_Reset(reg5__CGRA_Reset),
    .enable(reg5__enable),
    .in(reg5__in),
    .out(reg5__out)
  );

  //Wire declarations for instance 'rega' (Module cgrame_register_32b)
  wire  rega__CGRA_Clock;
  wire  rega__CGRA_Enable;
  wire  rega__CGRA_Reset;
  wire [0:0] rega__enable;
  wire [31:0] rega__in;
  wire [31:0] rega__out;
  cgrame_register_32b #(.contexts(1),.size(32)) rega(
    .CGRA_Clock(rega__CGRA_Clock),
    .CGRA_Enable(rega__CGRA_Enable),
    .CGRA_Reset(rega__CGRA_Reset),
    .enable(rega__enable),
    .in(rega__in),
    .out(rega__out)
  );

  //Wire declarations for instance 'regb' (Module cgrame_register_32b)
  wire  regb__CGRA_Clock;
  wire  regb__CGRA_Enable;
  wire  regb__CGRA_Reset;
  wire [0:0] regb__enable;
  wire [31:0] regb__in;
  wire [31:0] regb__out;
  cgrame_register_32b #(.contexts(1),.size(32)) regb(
    .CGRA_Clock(regb__CGRA_Clock),
    .CGRA_Enable(regb__CGRA_Enable),
    .CGRA_Reset(regb__CGRA_Reset),
    .enable(regb__enable),
    .in(regb__in),
    .out(regb__out)
  );

  //All the connections
  assign ALU__CGRA_Reset = CGRA_Reset;
  assign ALU__CGRA_Clock = CGRA_Clock;
  assign RES__CGRA_Clock = CGRA_Clock;
  assign const__CGRA_Clock = CGRA_Clock;
  assign crossbar__CGRA_Clock = CGRA_Clock;
  assign reg0__CGRA_Clock = CGRA_Clock;
  assign reg1__CGRA_Clock = CGRA_Clock;
  assign reg2__CGRA_Clock = CGRA_Clock;
  assign reg3__CGRA_Clock = CGRA_Clock;
  assign reg5__CGRA_Clock = CGRA_Clock;
  assign rega__CGRA_Clock = CGRA_Clock;
  assign regb__CGRA_Clock = CGRA_Clock;
  assign ALUMuxAconfig__CGRA_Clock = CGRA_Clock;
  assign ALUMuxBconfig__CGRA_Clock = CGRA_Clock;
  assign ALUconfig__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux5config__CGRA_Clock = CGRA_Clock;
  assign RESConfig__CGRA_Clock = CGRA_Clock;
  assign Reg0config__CGRA_Clock = CGRA_Clock;
  assign Reg1config__CGRA_Clock = CGRA_Clock;
  assign Reg2config__CGRA_Clock = CGRA_Clock;
  assign Reg3config__CGRA_Clock = CGRA_Clock;
  assign Reg5config__CGRA_Clock = CGRA_Clock;
  assign RegAConfig__CGRA_Clock = CGRA_Clock;
  assign RegBConfig__CGRA_Clock = CGRA_Clock;
  assign ALU__CGRA_Enable = CGRA_Enable;
  assign RES__CGRA_Reset = CGRA_Reset;
  assign const__CGRA_Reset = CGRA_Reset;
  assign crossbar__CGRA_Reset = CGRA_Reset;
  assign reg0__CGRA_Reset = CGRA_Reset;
  assign reg1__CGRA_Reset = CGRA_Reset;
  assign reg2__CGRA_Reset = CGRA_Reset;
  assign reg3__CGRA_Reset = CGRA_Reset;
  assign reg5__CGRA_Reset = CGRA_Reset;
  assign rega__CGRA_Reset = CGRA_Reset;
  assign regb__CGRA_Reset = CGRA_Reset;
  assign ALUMuxAconfig__CGRA_Reset = CGRA_Reset;
  assign ALUMuxBconfig__CGRA_Reset = CGRA_Reset;
  assign ALUconfig__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux5config__CGRA_Reset = CGRA_Reset;
  assign RESConfig__CGRA_Reset = CGRA_Reset;
  assign Reg0config__CGRA_Reset = CGRA_Reset;
  assign Reg1config__CGRA_Reset = CGRA_Reset;
  assign Reg2config__CGRA_Reset = CGRA_Reset;
  assign Reg3config__CGRA_Reset = CGRA_Reset;
  assign Reg5config__CGRA_Reset = CGRA_Reset;
  assign RegAConfig__CGRA_Reset = CGRA_Reset;
  assign RegBConfig__CGRA_Reset = CGRA_Reset;
  assign RES__CGRA_Enable = CGRA_Enable;
  assign const__CGRA_Enable = CGRA_Enable;
  assign crossbar__CGRA_Enable = CGRA_Enable;
  assign reg0__CGRA_Enable = CGRA_Enable;
  assign reg1__CGRA_Enable = CGRA_Enable;
  assign reg2__CGRA_Enable = CGRA_Enable;
  assign reg3__CGRA_Enable = CGRA_Enable;
  assign reg5__CGRA_Enable = CGRA_Enable;
  assign rega__CGRA_Enable = CGRA_Enable;
  assign regb__CGRA_Enable = CGRA_Enable;
  assign ALUMuxAconfig__CGRA_Enable = CGRA_Enable;
  assign ALUMuxBconfig__CGRA_Enable = CGRA_Enable;
  assign ALUconfig__CGRA_Enable = CGRA_Enable;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux5config__CGRA_Enable = CGRA_Enable;
  assign RESConfig__CGRA_Enable = CGRA_Enable;
  assign Reg0config__CGRA_Enable = CGRA_Enable;
  assign Reg1config__CGRA_Enable = CGRA_Enable;
  assign Reg2config__CGRA_Enable = CGRA_Enable;
  assign Reg3config__CGRA_Enable = CGRA_Enable;
  assign Reg5config__CGRA_Enable = CGRA_Enable;
  assign RegAConfig__CGRA_Enable = CGRA_Enable;
  assign RegBConfig__CGRA_Enable = CGRA_Enable;
  assign const__Config_Clock = Config_Clock;
  assign const__Config_Reset = Config_Reset;
  assign crossbar__Config_Clock = Config_Clock;
  assign ALUMuxAconfig__Config_Clock = Config_Clock;
  assign ALUMuxBconfig__Config_Clock = Config_Clock;
  assign ALUconfig__Config_Clock = Config_Clock;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux5config__Config_Clock = Config_Clock;
  assign RESConfig__Config_Clock = Config_Clock;
  assign Reg0config__Config_Clock = Config_Clock;
  assign Reg1config__Config_Clock = Config_Clock;
  assign Reg2config__Config_Clock = Config_Clock;
  assign Reg3config__Config_Clock = Config_Clock;
  assign Reg5config__Config_Clock = Config_Clock;
  assign RegAConfig__Config_Clock = Config_Clock;
  assign RegBConfig__Config_Clock = Config_Clock;
  assign crossbar__Config_Reset = Config_Reset;
  assign ALUMuxAconfig__Config_Reset = Config_Reset;
  assign ALUMuxBconfig__Config_Reset = Config_Reset;
  assign ALUconfig__Config_Reset = Config_Reset;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux5config__Config_Reset = Config_Reset;
  assign RESConfig__Config_Reset = Config_Reset;
  assign Reg0config__Config_Reset = Config_Reset;
  assign Reg1config__Config_Reset = Config_Reset;
  assign Reg2config__Config_Reset = Config_Reset;
  assign Reg3config__Config_Reset = Config_Reset;
  assign Reg5config__Config_Reset = Config_Reset;
  assign RegAConfig__Config_Reset = Config_Reset;
  assign RegBConfig__Config_Reset = Config_Reset;
  assign crossbar__in5[31:0] = ALU__out[31:0];
  assign RES__in[31:0] = ALU__out[31:0];
  assign out0[31:0] = crossbar__out0[31:0];
  assign out1[31:0] = crossbar__out1[31:0];
  assign out2[31:0] = crossbar__out2[31:0];
  assign out3[31:0] = crossbar__out3[31:0];
  assign out5[31:0] = crossbar__out4[31:0];
  assign rega__in[31:0] = crossbar__out5[31:0];
  assign regb__in[31:0] = crossbar__out6[31:0];
  assign crossbar__in6[31:0] = RES__out[31:0];
  assign alu_muxa__in0[31:0] = rega__out[31:0];
  assign alu_muxb__in0[31:0] = regb__out[31:0];
  assign alu_muxa__in1[31:0] = const__out[31:0];
  assign alu_muxb__in1[31:0] = const__out[31:0];
  assign ALU__in_a[31:0] = alu_muxa__out[31:0];
  assign ALU__in_b[31:0] = alu_muxb__out[31:0];
  assign mux_0__in1[31:0] = reg0__out[31:0];
  assign crossbar__in0[31:0] = mux_0__out[31:0];
  assign mux_1__in1[31:0] = reg1__out[31:0];
  assign crossbar__in1[31:0] = mux_1__out[31:0];
  assign mux_2__in1[31:0] = reg2__out[31:0];
  assign crossbar__in2[31:0] = mux_2__out[31:0];
  assign mux_3__in1[31:0] = reg3__out[31:0];
  assign crossbar__in3[31:0] = mux_3__out[31:0];
  assign mux_5__in1[31:0] = reg5__out[31:0];
  assign crossbar__in4[31:0] = mux_5__out[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign reg0__in[31:0] = in0[31:0];
  assign mux_1__in0[31:0] = in1[31:0];
  assign reg1__in[31:0] = in1[31:0];
  assign mux_2__in0[31:0] = in2[31:0];
  assign reg2__in[31:0] = in2[31:0];
  assign mux_3__in0[31:0] = in3[31:0];
  assign reg3__in[31:0] = in3[31:0];
  assign mux_5__in0[31:0] = in5[31:0];
  assign reg5__in[31:0] = in5[31:0];
  assign alu_muxa__select[0:0] = ALUMuxAconfig__select[0:0];
  assign alu_muxb__select[0:0] = ALUMuxBconfig__select[0:0];
  assign ALU__select[3:0] = ALUconfig__select[3:0];
  assign mux_0__select[0:0] = Mux0config__select[0:0];
  assign mux_1__select[0:0] = Mux1config__select[0:0];
  assign mux_2__select[0:0] = Mux2config__select[0:0];
  assign mux_3__select[0:0] = Mux3config__select[0:0];
  assign mux_5__select[0:0] = Mux5config__select[0:0];
  assign RES__enable[0:0] = RESConfig__select[0:0];
  assign reg0__enable[0:0] = Reg0config__select[0:0];
  assign reg1__enable[0:0] = Reg1config__select[0:0];
  assign reg2__enable[0:0] = Reg2config__select[0:0];
  assign reg3__enable[0:0] = Reg3config__select[0:0];
  assign reg5__enable[0:0] = Reg5config__select[0:0];
  assign regb__enable[0:0] = RegAConfig__select[0:0];
  assign rega__enable[0:0] = RegBConfig__select[0:0];
  assign ALUMuxAconfig__ConfigIn = ConfigIn;
  assign ALUMuxBconfig__ConfigIn = ALUMuxAconfig__ConfigOut;
  assign ALUconfig__ConfigIn = ALUMuxBconfig__ConfigOut;
  assign Mux0config__ConfigIn = ALUconfig__ConfigOut;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux5config__ConfigIn = Mux3config__ConfigOut;
  assign RESConfig__ConfigIn = Mux5config__ConfigOut;
  assign Reg0config__ConfigIn = RESConfig__ConfigOut;
  assign Reg1config__ConfigIn = Reg0config__ConfigOut;
  assign Reg2config__ConfigIn = Reg1config__ConfigOut;
  assign Reg3config__ConfigIn = Reg2config__ConfigOut;
  assign Reg5config__ConfigIn = Reg3config__ConfigOut;
  assign RegAConfig__ConfigIn = Reg5config__ConfigOut;
  assign RegBConfig__ConfigIn = RegAConfig__ConfigOut;
  assign const__ConfigIn = RegBConfig__ConfigOut;
  assign crossbar__ConfigIn = const__ConfigOut;
  assign ConfigOut = crossbar__ConfigOut;

endmodule //hycube_no_pred_in47_out47_U5

module hycube_no_pred_in63_out63_U8 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in4,
  input [31:0] in5,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out4,
  output [31:0] out5
);
  //Wire declarations for instance 'ALU' (Module func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12)
  wire  ALU__CGRA_Clock;
  wire  ALU__CGRA_Enable;
  wire  ALU__CGRA_Reset;
  wire [31:0] ALU__in_a;
  wire [31:0] ALU__in_b;
  wire [31:0] ALU__out;
  wire [3:0] ALU__select;
  func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12 ALU(
    .CGRA_Clock(ALU__CGRA_Clock),
    .CGRA_Enable(ALU__CGRA_Enable),
    .CGRA_Reset(ALU__CGRA_Reset),
    .in_a(ALU__in_a),
    .in_b(ALU__in_b),
    .out(ALU__out),
    .select(ALU__select)
  );

  //Wire declarations for instance 'ALUMuxAconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxAconfig__CGRA_Clock;
  wire  ALUMuxAconfig__CGRA_Enable;
  wire  ALUMuxAconfig__CGRA_Reset;
  wire  ALUMuxAconfig__ConfigIn;
  wire  ALUMuxAconfig__ConfigOut;
  wire  ALUMuxAconfig__Config_Clock;
  wire  ALUMuxAconfig__Config_Reset;
  wire [0:0] ALUMuxAconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxAconfig(
    .CGRA_Clock(ALUMuxAconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxAconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxAconfig__CGRA_Reset),
    .ConfigIn(ALUMuxAconfig__ConfigIn),
    .ConfigOut(ALUMuxAconfig__ConfigOut),
    .Config_Clock(ALUMuxAconfig__Config_Clock),
    .Config_Reset(ALUMuxAconfig__Config_Reset),
    .select(ALUMuxAconfig__select)
  );

  //Wire declarations for instance 'ALUMuxBconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxBconfig__CGRA_Clock;
  wire  ALUMuxBconfig__CGRA_Enable;
  wire  ALUMuxBconfig__CGRA_Reset;
  wire  ALUMuxBconfig__ConfigIn;
  wire  ALUMuxBconfig__ConfigOut;
  wire  ALUMuxBconfig__Config_Clock;
  wire  ALUMuxBconfig__Config_Reset;
  wire [0:0] ALUMuxBconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxBconfig(
    .CGRA_Clock(ALUMuxBconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxBconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxBconfig__CGRA_Reset),
    .ConfigIn(ALUMuxBconfig__ConfigIn),
    .ConfigOut(ALUMuxBconfig__ConfigOut),
    .Config_Clock(ALUMuxBconfig__Config_Clock),
    .Config_Reset(ALUMuxBconfig__Config_Reset),
    .select(ALUMuxBconfig__select)
  );

  //Wire declarations for instance 'ALUconfig' (Module cgrame_configcell_context1_size4)
  wire  ALUconfig__CGRA_Clock;
  wire  ALUconfig__CGRA_Enable;
  wire  ALUconfig__CGRA_Reset;
  wire  ALUconfig__ConfigIn;
  wire  ALUconfig__ConfigOut;
  wire  ALUconfig__Config_Clock;
  wire  ALUconfig__Config_Reset;
  wire [3:0] ALUconfig__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) ALUconfig(
    .CGRA_Clock(ALUconfig__CGRA_Clock),
    .CGRA_Enable(ALUconfig__CGRA_Enable),
    .CGRA_Reset(ALUconfig__CGRA_Reset),
    .ConfigIn(ALUconfig__ConfigIn),
    .ConfigOut(ALUconfig__ConfigOut),
    .Config_Clock(ALUconfig__Config_Clock),
    .Config_Reset(ALUconfig__Config_Reset),
    .select(ALUconfig__select)
  );

  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size1)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [0:0] Mux0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size1)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [0:0] Mux1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size1)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [0:0] Mux2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size1)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [0:0] Mux3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux4config' (Module cgrame_configcell_context1_size1)
  wire  Mux4config__CGRA_Clock;
  wire  Mux4config__CGRA_Enable;
  wire  Mux4config__CGRA_Reset;
  wire  Mux4config__ConfigIn;
  wire  Mux4config__ConfigOut;
  wire  Mux4config__Config_Clock;
  wire  Mux4config__Config_Reset;
  wire [0:0] Mux4config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux4config(
    .CGRA_Clock(Mux4config__CGRA_Clock),
    .CGRA_Enable(Mux4config__CGRA_Enable),
    .CGRA_Reset(Mux4config__CGRA_Reset),
    .ConfigIn(Mux4config__ConfigIn),
    .ConfigOut(Mux4config__ConfigOut),
    .Config_Clock(Mux4config__Config_Clock),
    .Config_Reset(Mux4config__Config_Reset),
    .select(Mux4config__select)
  );

  //Wire declarations for instance 'Mux5config' (Module cgrame_configcell_context1_size1)
  wire  Mux5config__CGRA_Clock;
  wire  Mux5config__CGRA_Enable;
  wire  Mux5config__CGRA_Reset;
  wire  Mux5config__ConfigIn;
  wire  Mux5config__ConfigOut;
  wire  Mux5config__Config_Clock;
  wire  Mux5config__Config_Reset;
  wire [0:0] Mux5config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux5config(
    .CGRA_Clock(Mux5config__CGRA_Clock),
    .CGRA_Enable(Mux5config__CGRA_Enable),
    .CGRA_Reset(Mux5config__CGRA_Reset),
    .ConfigIn(Mux5config__ConfigIn),
    .ConfigOut(Mux5config__ConfigOut),
    .Config_Clock(Mux5config__Config_Clock),
    .Config_Reset(Mux5config__Config_Reset),
    .select(Mux5config__select)
  );

  //Wire declarations for instance 'RES' (Module cgrame_register_32b)
  wire  RES__CGRA_Clock;
  wire  RES__CGRA_Enable;
  wire  RES__CGRA_Reset;
  wire [0:0] RES__enable;
  wire [31:0] RES__in;
  wire [31:0] RES__out;
  cgrame_register_32b #(.contexts(1),.size(32)) RES(
    .CGRA_Clock(RES__CGRA_Clock),
    .CGRA_Enable(RES__CGRA_Enable),
    .CGRA_Reset(RES__CGRA_Reset),
    .enable(RES__enable),
    .in(RES__in),
    .out(RES__out)
  );

  //Wire declarations for instance 'RESConfig' (Module cgrame_configcell_context1_size1)
  wire  RESConfig__CGRA_Clock;
  wire  RESConfig__CGRA_Enable;
  wire  RESConfig__CGRA_Reset;
  wire  RESConfig__ConfigIn;
  wire  RESConfig__ConfigOut;
  wire  RESConfig__Config_Clock;
  wire  RESConfig__Config_Reset;
  wire [0:0] RESConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RESConfig(
    .CGRA_Clock(RESConfig__CGRA_Clock),
    .CGRA_Enable(RESConfig__CGRA_Enable),
    .CGRA_Reset(RESConfig__CGRA_Reset),
    .ConfigIn(RESConfig__ConfigIn),
    .ConfigOut(RESConfig__ConfigOut),
    .Config_Clock(RESConfig__Config_Clock),
    .Config_Reset(RESConfig__Config_Reset),
    .select(RESConfig__select)
  );

  //Wire declarations for instance 'Reg0config' (Module cgrame_configcell_context1_size1)
  wire  Reg0config__CGRA_Clock;
  wire  Reg0config__CGRA_Enable;
  wire  Reg0config__CGRA_Reset;
  wire  Reg0config__ConfigIn;
  wire  Reg0config__ConfigOut;
  wire  Reg0config__Config_Clock;
  wire  Reg0config__Config_Reset;
  wire [0:0] Reg0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg0config(
    .CGRA_Clock(Reg0config__CGRA_Clock),
    .CGRA_Enable(Reg0config__CGRA_Enable),
    .CGRA_Reset(Reg0config__CGRA_Reset),
    .ConfigIn(Reg0config__ConfigIn),
    .ConfigOut(Reg0config__ConfigOut),
    .Config_Clock(Reg0config__Config_Clock),
    .Config_Reset(Reg0config__Config_Reset),
    .select(Reg0config__select)
  );

  //Wire declarations for instance 'Reg1config' (Module cgrame_configcell_context1_size1)
  wire  Reg1config__CGRA_Clock;
  wire  Reg1config__CGRA_Enable;
  wire  Reg1config__CGRA_Reset;
  wire  Reg1config__ConfigIn;
  wire  Reg1config__ConfigOut;
  wire  Reg1config__Config_Clock;
  wire  Reg1config__Config_Reset;
  wire [0:0] Reg1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg1config(
    .CGRA_Clock(Reg1config__CGRA_Clock),
    .CGRA_Enable(Reg1config__CGRA_Enable),
    .CGRA_Reset(Reg1config__CGRA_Reset),
    .ConfigIn(Reg1config__ConfigIn),
    .ConfigOut(Reg1config__ConfigOut),
    .Config_Clock(Reg1config__Config_Clock),
    .Config_Reset(Reg1config__Config_Reset),
    .select(Reg1config__select)
  );

  //Wire declarations for instance 'Reg2config' (Module cgrame_configcell_context1_size1)
  wire  Reg2config__CGRA_Clock;
  wire  Reg2config__CGRA_Enable;
  wire  Reg2config__CGRA_Reset;
  wire  Reg2config__ConfigIn;
  wire  Reg2config__ConfigOut;
  wire  Reg2config__Config_Clock;
  wire  Reg2config__Config_Reset;
  wire [0:0] Reg2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg2config(
    .CGRA_Clock(Reg2config__CGRA_Clock),
    .CGRA_Enable(Reg2config__CGRA_Enable),
    .CGRA_Reset(Reg2config__CGRA_Reset),
    .ConfigIn(Reg2config__ConfigIn),
    .ConfigOut(Reg2config__ConfigOut),
    .Config_Clock(Reg2config__Config_Clock),
    .Config_Reset(Reg2config__Config_Reset),
    .select(Reg2config__select)
  );

  //Wire declarations for instance 'Reg3config' (Module cgrame_configcell_context1_size1)
  wire  Reg3config__CGRA_Clock;
  wire  Reg3config__CGRA_Enable;
  wire  Reg3config__CGRA_Reset;
  wire  Reg3config__ConfigIn;
  wire  Reg3config__ConfigOut;
  wire  Reg3config__Config_Clock;
  wire  Reg3config__Config_Reset;
  wire [0:0] Reg3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg3config(
    .CGRA_Clock(Reg3config__CGRA_Clock),
    .CGRA_Enable(Reg3config__CGRA_Enable),
    .CGRA_Reset(Reg3config__CGRA_Reset),
    .ConfigIn(Reg3config__ConfigIn),
    .ConfigOut(Reg3config__ConfigOut),
    .Config_Clock(Reg3config__Config_Clock),
    .Config_Reset(Reg3config__Config_Reset),
    .select(Reg3config__select)
  );

  //Wire declarations for instance 'Reg4config' (Module cgrame_configcell_context1_size1)
  wire  Reg4config__CGRA_Clock;
  wire  Reg4config__CGRA_Enable;
  wire  Reg4config__CGRA_Reset;
  wire  Reg4config__ConfigIn;
  wire  Reg4config__ConfigOut;
  wire  Reg4config__Config_Clock;
  wire  Reg4config__Config_Reset;
  wire [0:0] Reg4config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg4config(
    .CGRA_Clock(Reg4config__CGRA_Clock),
    .CGRA_Enable(Reg4config__CGRA_Enable),
    .CGRA_Reset(Reg4config__CGRA_Reset),
    .ConfigIn(Reg4config__ConfigIn),
    .ConfigOut(Reg4config__ConfigOut),
    .Config_Clock(Reg4config__Config_Clock),
    .Config_Reset(Reg4config__Config_Reset),
    .select(Reg4config__select)
  );

  //Wire declarations for instance 'Reg5config' (Module cgrame_configcell_context1_size1)
  wire  Reg5config__CGRA_Clock;
  wire  Reg5config__CGRA_Enable;
  wire  Reg5config__CGRA_Reset;
  wire  Reg5config__ConfigIn;
  wire  Reg5config__ConfigOut;
  wire  Reg5config__Config_Clock;
  wire  Reg5config__Config_Reset;
  wire [0:0] Reg5config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg5config(
    .CGRA_Clock(Reg5config__CGRA_Clock),
    .CGRA_Enable(Reg5config__CGRA_Enable),
    .CGRA_Reset(Reg5config__CGRA_Reset),
    .ConfigIn(Reg5config__ConfigIn),
    .ConfigOut(Reg5config__ConfigOut),
    .Config_Clock(Reg5config__Config_Clock),
    .Config_Reset(Reg5config__Config_Reset),
    .select(Reg5config__select)
  );

  //Wire declarations for instance 'RegAConfig' (Module cgrame_configcell_context1_size1)
  wire  RegAConfig__CGRA_Clock;
  wire  RegAConfig__CGRA_Enable;
  wire  RegAConfig__CGRA_Reset;
  wire  RegAConfig__ConfigIn;
  wire  RegAConfig__ConfigOut;
  wire  RegAConfig__Config_Clock;
  wire  RegAConfig__Config_Reset;
  wire [0:0] RegAConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegAConfig(
    .CGRA_Clock(RegAConfig__CGRA_Clock),
    .CGRA_Enable(RegAConfig__CGRA_Enable),
    .CGRA_Reset(RegAConfig__CGRA_Reset),
    .ConfigIn(RegAConfig__ConfigIn),
    .ConfigOut(RegAConfig__ConfigOut),
    .Config_Clock(RegAConfig__Config_Clock),
    .Config_Reset(RegAConfig__Config_Reset),
    .select(RegAConfig__select)
  );

  //Wire declarations for instance 'RegBConfig' (Module cgrame_configcell_context1_size1)
  wire  RegBConfig__CGRA_Clock;
  wire  RegBConfig__CGRA_Enable;
  wire  RegBConfig__CGRA_Reset;
  wire  RegBConfig__ConfigIn;
  wire  RegBConfig__ConfigOut;
  wire  RegBConfig__Config_Clock;
  wire  RegBConfig__Config_Reset;
  wire [0:0] RegBConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegBConfig(
    .CGRA_Clock(RegBConfig__CGRA_Clock),
    .CGRA_Enable(RegBConfig__CGRA_Enable),
    .CGRA_Reset(RegBConfig__CGRA_Reset),
    .ConfigIn(RegBConfig__ConfigIn),
    .ConfigOut(RegBConfig__ConfigOut),
    .Config_Clock(RegBConfig__Config_Clock),
    .Config_Reset(RegBConfig__Config_Reset),
    .select(RegBConfig__select)
  );

  //Wire declarations for instance 'alu_muxa' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxa__in0;
  wire [31:0] alu_muxa__in1;
  wire [31:0] alu_muxa__out;
  wire [0:0] alu_muxa__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxa(
    .in0(alu_muxa__in0),
    .in1(alu_muxa__in1),
    .out(alu_muxa__out),
    .select(alu_muxa__select)
  );

  //Wire declarations for instance 'alu_muxb' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxb__in0;
  wire [31:0] alu_muxb__in1;
  wire [31:0] alu_muxb__out;
  wire [0:0] alu_muxb__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxb(
    .in0(alu_muxb__in0),
    .in1(alu_muxb__in1),
    .out(alu_muxb__out),
    .select(alu_muxb__select)
  );

  //Wire declarations for instance 'const' (Module const_32b_U15)
  wire  const__CGRA_Clock;
  wire  const__CGRA_Enable;
  wire  const__CGRA_Reset;
  wire  const__ConfigIn;
  wire  const__ConfigOut;
  wire  const__Config_Clock;
  wire  const__Config_Reset;
  wire [31:0] const__out;
  const_32b_U15 const(
    .CGRA_Clock(const__CGRA_Clock),
    .CGRA_Enable(const__CGRA_Enable),
    .CGRA_Reset(const__CGRA_Reset),
    .ConfigIn(const__ConfigIn),
    .ConfigOut(const__ConfigOut),
    .Config_Clock(const__Config_Clock),
    .Config_Reset(const__Config_Reset),
    .out(const__out)
  );

  //Wire declarations for instance 'crossbar' (Module crossbar_8in_8out_U16)
  wire  crossbar__CGRA_Clock;
  wire  crossbar__CGRA_Enable;
  wire  crossbar__CGRA_Reset;
  wire  crossbar__ConfigIn;
  wire  crossbar__ConfigOut;
  wire  crossbar__Config_Clock;
  wire  crossbar__Config_Reset;
  wire [31:0] crossbar__in0;
  wire [31:0] crossbar__in1;
  wire [31:0] crossbar__in2;
  wire [31:0] crossbar__in3;
  wire [31:0] crossbar__in4;
  wire [31:0] crossbar__in5;
  wire [31:0] crossbar__in6;
  wire [31:0] crossbar__in7;
  wire [31:0] crossbar__out0;
  wire [31:0] crossbar__out1;
  wire [31:0] crossbar__out2;
  wire [31:0] crossbar__out3;
  wire [31:0] crossbar__out4;
  wire [31:0] crossbar__out5;
  wire [31:0] crossbar__out6;
  wire [31:0] crossbar__out7;
  crossbar_8in_8out_U16 crossbar(
    .CGRA_Clock(crossbar__CGRA_Clock),
    .CGRA_Enable(crossbar__CGRA_Enable),
    .CGRA_Reset(crossbar__CGRA_Reset),
    .ConfigIn(crossbar__ConfigIn),
    .ConfigOut(crossbar__ConfigOut),
    .Config_Clock(crossbar__Config_Clock),
    .Config_Reset(crossbar__Config_Reset),
    .in0(crossbar__in0),
    .in1(crossbar__in1),
    .in2(crossbar__in2),
    .in3(crossbar__in3),
    .in4(crossbar__in4),
    .in5(crossbar__in5),
    .in6(crossbar__in6),
    .in7(crossbar__in7),
    .out0(crossbar__out0),
    .out1(crossbar__out1),
    .out2(crossbar__out2),
    .out3(crossbar__out3),
    .out4(crossbar__out4),
    .out5(crossbar__out5),
    .out6(crossbar__out6),
    .out7(crossbar__out7)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__out;
  wire [0:0] mux_0__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__out;
  wire [0:0] mux_1__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__out;
  wire [0:0] mux_2__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__out;
  wire [0:0] mux_3__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_4' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_4__in0;
  wire [31:0] mux_4__in1;
  wire [31:0] mux_4__out;
  wire [0:0] mux_4__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_4(
    .in0(mux_4__in0),
    .in1(mux_4__in1),
    .out(mux_4__out),
    .select(mux_4__select)
  );

  //Wire declarations for instance 'mux_5' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_5__in0;
  wire [31:0] mux_5__in1;
  wire [31:0] mux_5__out;
  wire [0:0] mux_5__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_5(
    .in0(mux_5__in0),
    .in1(mux_5__in1),
    .out(mux_5__out),
    .select(mux_5__select)
  );

  //Wire declarations for instance 'reg0' (Module cgrame_register_32b)
  wire  reg0__CGRA_Clock;
  wire  reg0__CGRA_Enable;
  wire  reg0__CGRA_Reset;
  wire [0:0] reg0__enable;
  wire [31:0] reg0__in;
  wire [31:0] reg0__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg0(
    .CGRA_Clock(reg0__CGRA_Clock),
    .CGRA_Enable(reg0__CGRA_Enable),
    .CGRA_Reset(reg0__CGRA_Reset),
    .enable(reg0__enable),
    .in(reg0__in),
    .out(reg0__out)
  );

  //Wire declarations for instance 'reg1' (Module cgrame_register_32b)
  wire  reg1__CGRA_Clock;
  wire  reg1__CGRA_Enable;
  wire  reg1__CGRA_Reset;
  wire [0:0] reg1__enable;
  wire [31:0] reg1__in;
  wire [31:0] reg1__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg1(
    .CGRA_Clock(reg1__CGRA_Clock),
    .CGRA_Enable(reg1__CGRA_Enable),
    .CGRA_Reset(reg1__CGRA_Reset),
    .enable(reg1__enable),
    .in(reg1__in),
    .out(reg1__out)
  );

  //Wire declarations for instance 'reg2' (Module cgrame_register_32b)
  wire  reg2__CGRA_Clock;
  wire  reg2__CGRA_Enable;
  wire  reg2__CGRA_Reset;
  wire [0:0] reg2__enable;
  wire [31:0] reg2__in;
  wire [31:0] reg2__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg2(
    .CGRA_Clock(reg2__CGRA_Clock),
    .CGRA_Enable(reg2__CGRA_Enable),
    .CGRA_Reset(reg2__CGRA_Reset),
    .enable(reg2__enable),
    .in(reg2__in),
    .out(reg2__out)
  );

  //Wire declarations for instance 'reg3' (Module cgrame_register_32b)
  wire  reg3__CGRA_Clock;
  wire  reg3__CGRA_Enable;
  wire  reg3__CGRA_Reset;
  wire [0:0] reg3__enable;
  wire [31:0] reg3__in;
  wire [31:0] reg3__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg3(
    .CGRA_Clock(reg3__CGRA_Clock),
    .CGRA_Enable(reg3__CGRA_Enable),
    .CGRA_Reset(reg3__CGRA_Reset),
    .enable(reg3__enable),
    .in(reg3__in),
    .out(reg3__out)
  );

  //Wire declarations for instance 'reg4' (Module cgrame_register_32b)
  wire  reg4__CGRA_Clock;
  wire  reg4__CGRA_Enable;
  wire  reg4__CGRA_Reset;
  wire [0:0] reg4__enable;
  wire [31:0] reg4__in;
  wire [31:0] reg4__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg4(
    .CGRA_Clock(reg4__CGRA_Clock),
    .CGRA_Enable(reg4__CGRA_Enable),
    .CGRA_Reset(reg4__CGRA_Reset),
    .enable(reg4__enable),
    .in(reg4__in),
    .out(reg4__out)
  );

  //Wire declarations for instance 'reg5' (Module cgrame_register_32b)
  wire  reg5__CGRA_Clock;
  wire  reg5__CGRA_Enable;
  wire  reg5__CGRA_Reset;
  wire [0:0] reg5__enable;
  wire [31:0] reg5__in;
  wire [31:0] reg5__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg5(
    .CGRA_Clock(reg5__CGRA_Clock),
    .CGRA_Enable(reg5__CGRA_Enable),
    .CGRA_Reset(reg5__CGRA_Reset),
    .enable(reg5__enable),
    .in(reg5__in),
    .out(reg5__out)
  );

  //Wire declarations for instance 'rega' (Module cgrame_register_32b)
  wire  rega__CGRA_Clock;
  wire  rega__CGRA_Enable;
  wire  rega__CGRA_Reset;
  wire [0:0] rega__enable;
  wire [31:0] rega__in;
  wire [31:0] rega__out;
  cgrame_register_32b #(.contexts(1),.size(32)) rega(
    .CGRA_Clock(rega__CGRA_Clock),
    .CGRA_Enable(rega__CGRA_Enable),
    .CGRA_Reset(rega__CGRA_Reset),
    .enable(rega__enable),
    .in(rega__in),
    .out(rega__out)
  );

  //Wire declarations for instance 'regb' (Module cgrame_register_32b)
  wire  regb__CGRA_Clock;
  wire  regb__CGRA_Enable;
  wire  regb__CGRA_Reset;
  wire [0:0] regb__enable;
  wire [31:0] regb__in;
  wire [31:0] regb__out;
  cgrame_register_32b #(.contexts(1),.size(32)) regb(
    .CGRA_Clock(regb__CGRA_Clock),
    .CGRA_Enable(regb__CGRA_Enable),
    .CGRA_Reset(regb__CGRA_Reset),
    .enable(regb__enable),
    .in(regb__in),
    .out(regb__out)
  );

  //All the connections
  assign ALU__CGRA_Reset = CGRA_Reset;
  assign ALU__CGRA_Clock = CGRA_Clock;
  assign RES__CGRA_Clock = CGRA_Clock;
  assign const__CGRA_Clock = CGRA_Clock;
  assign crossbar__CGRA_Clock = CGRA_Clock;
  assign reg0__CGRA_Clock = CGRA_Clock;
  assign reg1__CGRA_Clock = CGRA_Clock;
  assign reg2__CGRA_Clock = CGRA_Clock;
  assign reg3__CGRA_Clock = CGRA_Clock;
  assign reg4__CGRA_Clock = CGRA_Clock;
  assign reg5__CGRA_Clock = CGRA_Clock;
  assign rega__CGRA_Clock = CGRA_Clock;
  assign regb__CGRA_Clock = CGRA_Clock;
  assign ALUMuxAconfig__CGRA_Clock = CGRA_Clock;
  assign ALUMuxBconfig__CGRA_Clock = CGRA_Clock;
  assign ALUconfig__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux4config__CGRA_Clock = CGRA_Clock;
  assign Mux5config__CGRA_Clock = CGRA_Clock;
  assign RESConfig__CGRA_Clock = CGRA_Clock;
  assign Reg0config__CGRA_Clock = CGRA_Clock;
  assign Reg1config__CGRA_Clock = CGRA_Clock;
  assign Reg2config__CGRA_Clock = CGRA_Clock;
  assign Reg3config__CGRA_Clock = CGRA_Clock;
  assign Reg4config__CGRA_Clock = CGRA_Clock;
  assign Reg5config__CGRA_Clock = CGRA_Clock;
  assign RegAConfig__CGRA_Clock = CGRA_Clock;
  assign RegBConfig__CGRA_Clock = CGRA_Clock;
  assign ALU__CGRA_Enable = CGRA_Enable;
  assign RES__CGRA_Reset = CGRA_Reset;
  assign const__CGRA_Reset = CGRA_Reset;
  assign crossbar__CGRA_Reset = CGRA_Reset;
  assign reg0__CGRA_Reset = CGRA_Reset;
  assign reg1__CGRA_Reset = CGRA_Reset;
  assign reg2__CGRA_Reset = CGRA_Reset;
  assign reg3__CGRA_Reset = CGRA_Reset;
  assign reg4__CGRA_Reset = CGRA_Reset;
  assign reg5__CGRA_Reset = CGRA_Reset;
  assign rega__CGRA_Reset = CGRA_Reset;
  assign regb__CGRA_Reset = CGRA_Reset;
  assign ALUMuxAconfig__CGRA_Reset = CGRA_Reset;
  assign ALUMuxBconfig__CGRA_Reset = CGRA_Reset;
  assign ALUconfig__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux4config__CGRA_Reset = CGRA_Reset;
  assign Mux5config__CGRA_Reset = CGRA_Reset;
  assign RESConfig__CGRA_Reset = CGRA_Reset;
  assign Reg0config__CGRA_Reset = CGRA_Reset;
  assign Reg1config__CGRA_Reset = CGRA_Reset;
  assign Reg2config__CGRA_Reset = CGRA_Reset;
  assign Reg3config__CGRA_Reset = CGRA_Reset;
  assign Reg4config__CGRA_Reset = CGRA_Reset;
  assign Reg5config__CGRA_Reset = CGRA_Reset;
  assign RegAConfig__CGRA_Reset = CGRA_Reset;
  assign RegBConfig__CGRA_Reset = CGRA_Reset;
  assign RES__CGRA_Enable = CGRA_Enable;
  assign const__CGRA_Enable = CGRA_Enable;
  assign crossbar__CGRA_Enable = CGRA_Enable;
  assign reg0__CGRA_Enable = CGRA_Enable;
  assign reg1__CGRA_Enable = CGRA_Enable;
  assign reg2__CGRA_Enable = CGRA_Enable;
  assign reg3__CGRA_Enable = CGRA_Enable;
  assign reg4__CGRA_Enable = CGRA_Enable;
  assign reg5__CGRA_Enable = CGRA_Enable;
  assign rega__CGRA_Enable = CGRA_Enable;
  assign regb__CGRA_Enable = CGRA_Enable;
  assign ALUMuxAconfig__CGRA_Enable = CGRA_Enable;
  assign ALUMuxBconfig__CGRA_Enable = CGRA_Enable;
  assign ALUconfig__CGRA_Enable = CGRA_Enable;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux4config__CGRA_Enable = CGRA_Enable;
  assign Mux5config__CGRA_Enable = CGRA_Enable;
  assign RESConfig__CGRA_Enable = CGRA_Enable;
  assign Reg0config__CGRA_Enable = CGRA_Enable;
  assign Reg1config__CGRA_Enable = CGRA_Enable;
  assign Reg2config__CGRA_Enable = CGRA_Enable;
  assign Reg3config__CGRA_Enable = CGRA_Enable;
  assign Reg4config__CGRA_Enable = CGRA_Enable;
  assign Reg5config__CGRA_Enable = CGRA_Enable;
  assign RegAConfig__CGRA_Enable = CGRA_Enable;
  assign RegBConfig__CGRA_Enable = CGRA_Enable;
  assign const__Config_Clock = Config_Clock;
  assign const__Config_Reset = Config_Reset;
  assign crossbar__Config_Clock = Config_Clock;
  assign ALUMuxAconfig__Config_Clock = Config_Clock;
  assign ALUMuxBconfig__Config_Clock = Config_Clock;
  assign ALUconfig__Config_Clock = Config_Clock;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux4config__Config_Clock = Config_Clock;
  assign Mux5config__Config_Clock = Config_Clock;
  assign RESConfig__Config_Clock = Config_Clock;
  assign Reg0config__Config_Clock = Config_Clock;
  assign Reg1config__Config_Clock = Config_Clock;
  assign Reg2config__Config_Clock = Config_Clock;
  assign Reg3config__Config_Clock = Config_Clock;
  assign Reg4config__Config_Clock = Config_Clock;
  assign Reg5config__Config_Clock = Config_Clock;
  assign RegAConfig__Config_Clock = Config_Clock;
  assign RegBConfig__Config_Clock = Config_Clock;
  assign crossbar__Config_Reset = Config_Reset;
  assign ALUMuxAconfig__Config_Reset = Config_Reset;
  assign ALUMuxBconfig__Config_Reset = Config_Reset;
  assign ALUconfig__Config_Reset = Config_Reset;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux4config__Config_Reset = Config_Reset;
  assign Mux5config__Config_Reset = Config_Reset;
  assign RESConfig__Config_Reset = Config_Reset;
  assign Reg0config__Config_Reset = Config_Reset;
  assign Reg1config__Config_Reset = Config_Reset;
  assign Reg2config__Config_Reset = Config_Reset;
  assign Reg3config__Config_Reset = Config_Reset;
  assign Reg4config__Config_Reset = Config_Reset;
  assign Reg5config__Config_Reset = Config_Reset;
  assign RegAConfig__Config_Reset = Config_Reset;
  assign RegBConfig__Config_Reset = Config_Reset;
  assign crossbar__in6[31:0] = ALU__out[31:0];
  assign RES__in[31:0] = ALU__out[31:0];
  assign out0[31:0] = crossbar__out0[31:0];
  assign out1[31:0] = crossbar__out1[31:0];
  assign out2[31:0] = crossbar__out2[31:0];
  assign out3[31:0] = crossbar__out3[31:0];
  assign out4[31:0] = crossbar__out4[31:0];
  assign out5[31:0] = crossbar__out5[31:0];
  assign rega__in[31:0] = crossbar__out6[31:0];
  assign regb__in[31:0] = crossbar__out7[31:0];
  assign crossbar__in7[31:0] = RES__out[31:0];
  assign alu_muxa__in0[31:0] = rega__out[31:0];
  assign alu_muxb__in0[31:0] = regb__out[31:0];
  assign alu_muxa__in1[31:0] = const__out[31:0];
  assign alu_muxb__in1[31:0] = const__out[31:0];
  assign ALU__in_a[31:0] = alu_muxa__out[31:0];
  assign ALU__in_b[31:0] = alu_muxb__out[31:0];
  assign mux_0__in1[31:0] = reg0__out[31:0];
  assign crossbar__in0[31:0] = mux_0__out[31:0];
  assign mux_1__in1[31:0] = reg1__out[31:0];
  assign crossbar__in1[31:0] = mux_1__out[31:0];
  assign mux_2__in1[31:0] = reg2__out[31:0];
  assign crossbar__in2[31:0] = mux_2__out[31:0];
  assign mux_3__in1[31:0] = reg3__out[31:0];
  assign crossbar__in3[31:0] = mux_3__out[31:0];
  assign mux_4__in1[31:0] = reg4__out[31:0];
  assign crossbar__in4[31:0] = mux_4__out[31:0];
  assign mux_5__in1[31:0] = reg5__out[31:0];
  assign crossbar__in5[31:0] = mux_5__out[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign reg0__in[31:0] = in0[31:0];
  assign mux_1__in0[31:0] = in1[31:0];
  assign reg1__in[31:0] = in1[31:0];
  assign mux_2__in0[31:0] = in2[31:0];
  assign reg2__in[31:0] = in2[31:0];
  assign mux_3__in0[31:0] = in3[31:0];
  assign reg3__in[31:0] = in3[31:0];
  assign mux_4__in0[31:0] = in4[31:0];
  assign reg4__in[31:0] = in4[31:0];
  assign mux_5__in0[31:0] = in5[31:0];
  assign reg5__in[31:0] = in5[31:0];
  assign alu_muxa__select[0:0] = ALUMuxAconfig__select[0:0];
  assign alu_muxb__select[0:0] = ALUMuxBconfig__select[0:0];
  assign ALU__select[3:0] = ALUconfig__select[3:0];
  assign mux_0__select[0:0] = Mux0config__select[0:0];
  assign mux_1__select[0:0] = Mux1config__select[0:0];
  assign mux_2__select[0:0] = Mux2config__select[0:0];
  assign mux_3__select[0:0] = Mux3config__select[0:0];
  assign mux_4__select[0:0] = Mux4config__select[0:0];
  assign mux_5__select[0:0] = Mux5config__select[0:0];
  assign RES__enable[0:0] = RESConfig__select[0:0];
  assign reg0__enable[0:0] = Reg0config__select[0:0];
  assign reg1__enable[0:0] = Reg1config__select[0:0];
  assign reg2__enable[0:0] = Reg2config__select[0:0];
  assign reg3__enable[0:0] = Reg3config__select[0:0];
  assign reg4__enable[0:0] = Reg4config__select[0:0];
  assign reg5__enable[0:0] = Reg5config__select[0:0];
  assign regb__enable[0:0] = RegAConfig__select[0:0];
  assign rega__enable[0:0] = RegBConfig__select[0:0];
  assign ALUMuxAconfig__ConfigIn = ConfigIn;
  assign ALUMuxBconfig__ConfigIn = ALUMuxAconfig__ConfigOut;
  assign ALUconfig__ConfigIn = ALUMuxBconfig__ConfigOut;
  assign Mux0config__ConfigIn = ALUconfig__ConfigOut;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux4config__ConfigIn = Mux3config__ConfigOut;
  assign Mux5config__ConfigIn = Mux4config__ConfigOut;
  assign RESConfig__ConfigIn = Mux5config__ConfigOut;
  assign Reg0config__ConfigIn = RESConfig__ConfigOut;
  assign Reg1config__ConfigIn = Reg0config__ConfigOut;
  assign Reg2config__ConfigIn = Reg1config__ConfigOut;
  assign Reg3config__ConfigIn = Reg2config__ConfigOut;
  assign Reg4config__ConfigIn = Reg3config__ConfigOut;
  assign Reg5config__ConfigIn = Reg4config__ConfigOut;
  assign RegAConfig__ConfigIn = Reg5config__ConfigOut;
  assign RegBConfig__ConfigIn = RegAConfig__ConfigOut;
  assign const__ConfigIn = RegBConfig__ConfigOut;
  assign crossbar__ConfigIn = const__ConfigOut;
  assign ConfigOut = crossbar__ConfigOut;

endmodule //hycube_no_pred_in63_out63_U8

module hycube_no_pred_in79_out79_U3 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in6,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out6
);
  //Wire declarations for instance 'ALU' (Module func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12)
  wire  ALU__CGRA_Clock;
  wire  ALU__CGRA_Enable;
  wire  ALU__CGRA_Reset;
  wire [31:0] ALU__in_a;
  wire [31:0] ALU__in_b;
  wire [31:0] ALU__out;
  wire [3:0] ALU__select;
  func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12 ALU(
    .CGRA_Clock(ALU__CGRA_Clock),
    .CGRA_Enable(ALU__CGRA_Enable),
    .CGRA_Reset(ALU__CGRA_Reset),
    .in_a(ALU__in_a),
    .in_b(ALU__in_b),
    .out(ALU__out),
    .select(ALU__select)
  );

  //Wire declarations for instance 'ALUMuxAconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxAconfig__CGRA_Clock;
  wire  ALUMuxAconfig__CGRA_Enable;
  wire  ALUMuxAconfig__CGRA_Reset;
  wire  ALUMuxAconfig__ConfigIn;
  wire  ALUMuxAconfig__ConfigOut;
  wire  ALUMuxAconfig__Config_Clock;
  wire  ALUMuxAconfig__Config_Reset;
  wire [0:0] ALUMuxAconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxAconfig(
    .CGRA_Clock(ALUMuxAconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxAconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxAconfig__CGRA_Reset),
    .ConfigIn(ALUMuxAconfig__ConfigIn),
    .ConfigOut(ALUMuxAconfig__ConfigOut),
    .Config_Clock(ALUMuxAconfig__Config_Clock),
    .Config_Reset(ALUMuxAconfig__Config_Reset),
    .select(ALUMuxAconfig__select)
  );

  //Wire declarations for instance 'ALUMuxBconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxBconfig__CGRA_Clock;
  wire  ALUMuxBconfig__CGRA_Enable;
  wire  ALUMuxBconfig__CGRA_Reset;
  wire  ALUMuxBconfig__ConfigIn;
  wire  ALUMuxBconfig__ConfigOut;
  wire  ALUMuxBconfig__Config_Clock;
  wire  ALUMuxBconfig__Config_Reset;
  wire [0:0] ALUMuxBconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxBconfig(
    .CGRA_Clock(ALUMuxBconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxBconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxBconfig__CGRA_Reset),
    .ConfigIn(ALUMuxBconfig__ConfigIn),
    .ConfigOut(ALUMuxBconfig__ConfigOut),
    .Config_Clock(ALUMuxBconfig__Config_Clock),
    .Config_Reset(ALUMuxBconfig__Config_Reset),
    .select(ALUMuxBconfig__select)
  );

  //Wire declarations for instance 'ALUconfig' (Module cgrame_configcell_context1_size4)
  wire  ALUconfig__CGRA_Clock;
  wire  ALUconfig__CGRA_Enable;
  wire  ALUconfig__CGRA_Reset;
  wire  ALUconfig__ConfigIn;
  wire  ALUconfig__ConfigOut;
  wire  ALUconfig__Config_Clock;
  wire  ALUconfig__Config_Reset;
  wire [3:0] ALUconfig__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) ALUconfig(
    .CGRA_Clock(ALUconfig__CGRA_Clock),
    .CGRA_Enable(ALUconfig__CGRA_Enable),
    .CGRA_Reset(ALUconfig__CGRA_Reset),
    .ConfigIn(ALUconfig__ConfigIn),
    .ConfigOut(ALUconfig__ConfigOut),
    .Config_Clock(ALUconfig__Config_Clock),
    .Config_Reset(ALUconfig__Config_Reset),
    .select(ALUconfig__select)
  );

  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size1)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [0:0] Mux0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size1)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [0:0] Mux1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size1)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [0:0] Mux2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size1)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [0:0] Mux3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux6config' (Module cgrame_configcell_context1_size1)
  wire  Mux6config__CGRA_Clock;
  wire  Mux6config__CGRA_Enable;
  wire  Mux6config__CGRA_Reset;
  wire  Mux6config__ConfigIn;
  wire  Mux6config__ConfigOut;
  wire  Mux6config__Config_Clock;
  wire  Mux6config__Config_Reset;
  wire [0:0] Mux6config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux6config(
    .CGRA_Clock(Mux6config__CGRA_Clock),
    .CGRA_Enable(Mux6config__CGRA_Enable),
    .CGRA_Reset(Mux6config__CGRA_Reset),
    .ConfigIn(Mux6config__ConfigIn),
    .ConfigOut(Mux6config__ConfigOut),
    .Config_Clock(Mux6config__Config_Clock),
    .Config_Reset(Mux6config__Config_Reset),
    .select(Mux6config__select)
  );

  //Wire declarations for instance 'RES' (Module cgrame_register_32b)
  wire  RES__CGRA_Clock;
  wire  RES__CGRA_Enable;
  wire  RES__CGRA_Reset;
  wire [0:0] RES__enable;
  wire [31:0] RES__in;
  wire [31:0] RES__out;
  cgrame_register_32b #(.contexts(1),.size(32)) RES(
    .CGRA_Clock(RES__CGRA_Clock),
    .CGRA_Enable(RES__CGRA_Enable),
    .CGRA_Reset(RES__CGRA_Reset),
    .enable(RES__enable),
    .in(RES__in),
    .out(RES__out)
  );

  //Wire declarations for instance 'RESConfig' (Module cgrame_configcell_context1_size1)
  wire  RESConfig__CGRA_Clock;
  wire  RESConfig__CGRA_Enable;
  wire  RESConfig__CGRA_Reset;
  wire  RESConfig__ConfigIn;
  wire  RESConfig__ConfigOut;
  wire  RESConfig__Config_Clock;
  wire  RESConfig__Config_Reset;
  wire [0:0] RESConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RESConfig(
    .CGRA_Clock(RESConfig__CGRA_Clock),
    .CGRA_Enable(RESConfig__CGRA_Enable),
    .CGRA_Reset(RESConfig__CGRA_Reset),
    .ConfigIn(RESConfig__ConfigIn),
    .ConfigOut(RESConfig__ConfigOut),
    .Config_Clock(RESConfig__Config_Clock),
    .Config_Reset(RESConfig__Config_Reset),
    .select(RESConfig__select)
  );

  //Wire declarations for instance 'Reg0config' (Module cgrame_configcell_context1_size1)
  wire  Reg0config__CGRA_Clock;
  wire  Reg0config__CGRA_Enable;
  wire  Reg0config__CGRA_Reset;
  wire  Reg0config__ConfigIn;
  wire  Reg0config__ConfigOut;
  wire  Reg0config__Config_Clock;
  wire  Reg0config__Config_Reset;
  wire [0:0] Reg0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg0config(
    .CGRA_Clock(Reg0config__CGRA_Clock),
    .CGRA_Enable(Reg0config__CGRA_Enable),
    .CGRA_Reset(Reg0config__CGRA_Reset),
    .ConfigIn(Reg0config__ConfigIn),
    .ConfigOut(Reg0config__ConfigOut),
    .Config_Clock(Reg0config__Config_Clock),
    .Config_Reset(Reg0config__Config_Reset),
    .select(Reg0config__select)
  );

  //Wire declarations for instance 'Reg1config' (Module cgrame_configcell_context1_size1)
  wire  Reg1config__CGRA_Clock;
  wire  Reg1config__CGRA_Enable;
  wire  Reg1config__CGRA_Reset;
  wire  Reg1config__ConfigIn;
  wire  Reg1config__ConfigOut;
  wire  Reg1config__Config_Clock;
  wire  Reg1config__Config_Reset;
  wire [0:0] Reg1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg1config(
    .CGRA_Clock(Reg1config__CGRA_Clock),
    .CGRA_Enable(Reg1config__CGRA_Enable),
    .CGRA_Reset(Reg1config__CGRA_Reset),
    .ConfigIn(Reg1config__ConfigIn),
    .ConfigOut(Reg1config__ConfigOut),
    .Config_Clock(Reg1config__Config_Clock),
    .Config_Reset(Reg1config__Config_Reset),
    .select(Reg1config__select)
  );

  //Wire declarations for instance 'Reg2config' (Module cgrame_configcell_context1_size1)
  wire  Reg2config__CGRA_Clock;
  wire  Reg2config__CGRA_Enable;
  wire  Reg2config__CGRA_Reset;
  wire  Reg2config__ConfigIn;
  wire  Reg2config__ConfigOut;
  wire  Reg2config__Config_Clock;
  wire  Reg2config__Config_Reset;
  wire [0:0] Reg2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg2config(
    .CGRA_Clock(Reg2config__CGRA_Clock),
    .CGRA_Enable(Reg2config__CGRA_Enable),
    .CGRA_Reset(Reg2config__CGRA_Reset),
    .ConfigIn(Reg2config__ConfigIn),
    .ConfigOut(Reg2config__ConfigOut),
    .Config_Clock(Reg2config__Config_Clock),
    .Config_Reset(Reg2config__Config_Reset),
    .select(Reg2config__select)
  );

  //Wire declarations for instance 'Reg3config' (Module cgrame_configcell_context1_size1)
  wire  Reg3config__CGRA_Clock;
  wire  Reg3config__CGRA_Enable;
  wire  Reg3config__CGRA_Reset;
  wire  Reg3config__ConfigIn;
  wire  Reg3config__ConfigOut;
  wire  Reg3config__Config_Clock;
  wire  Reg3config__Config_Reset;
  wire [0:0] Reg3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg3config(
    .CGRA_Clock(Reg3config__CGRA_Clock),
    .CGRA_Enable(Reg3config__CGRA_Enable),
    .CGRA_Reset(Reg3config__CGRA_Reset),
    .ConfigIn(Reg3config__ConfigIn),
    .ConfigOut(Reg3config__ConfigOut),
    .Config_Clock(Reg3config__Config_Clock),
    .Config_Reset(Reg3config__Config_Reset),
    .select(Reg3config__select)
  );

  //Wire declarations for instance 'Reg6config' (Module cgrame_configcell_context1_size1)
  wire  Reg6config__CGRA_Clock;
  wire  Reg6config__CGRA_Enable;
  wire  Reg6config__CGRA_Reset;
  wire  Reg6config__ConfigIn;
  wire  Reg6config__ConfigOut;
  wire  Reg6config__Config_Clock;
  wire  Reg6config__Config_Reset;
  wire [0:0] Reg6config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg6config(
    .CGRA_Clock(Reg6config__CGRA_Clock),
    .CGRA_Enable(Reg6config__CGRA_Enable),
    .CGRA_Reset(Reg6config__CGRA_Reset),
    .ConfigIn(Reg6config__ConfigIn),
    .ConfigOut(Reg6config__ConfigOut),
    .Config_Clock(Reg6config__Config_Clock),
    .Config_Reset(Reg6config__Config_Reset),
    .select(Reg6config__select)
  );

  //Wire declarations for instance 'RegAConfig' (Module cgrame_configcell_context1_size1)
  wire  RegAConfig__CGRA_Clock;
  wire  RegAConfig__CGRA_Enable;
  wire  RegAConfig__CGRA_Reset;
  wire  RegAConfig__ConfigIn;
  wire  RegAConfig__ConfigOut;
  wire  RegAConfig__Config_Clock;
  wire  RegAConfig__Config_Reset;
  wire [0:0] RegAConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegAConfig(
    .CGRA_Clock(RegAConfig__CGRA_Clock),
    .CGRA_Enable(RegAConfig__CGRA_Enable),
    .CGRA_Reset(RegAConfig__CGRA_Reset),
    .ConfigIn(RegAConfig__ConfigIn),
    .ConfigOut(RegAConfig__ConfigOut),
    .Config_Clock(RegAConfig__Config_Clock),
    .Config_Reset(RegAConfig__Config_Reset),
    .select(RegAConfig__select)
  );

  //Wire declarations for instance 'RegBConfig' (Module cgrame_configcell_context1_size1)
  wire  RegBConfig__CGRA_Clock;
  wire  RegBConfig__CGRA_Enable;
  wire  RegBConfig__CGRA_Reset;
  wire  RegBConfig__ConfigIn;
  wire  RegBConfig__ConfigOut;
  wire  RegBConfig__Config_Clock;
  wire  RegBConfig__Config_Reset;
  wire [0:0] RegBConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegBConfig(
    .CGRA_Clock(RegBConfig__CGRA_Clock),
    .CGRA_Enable(RegBConfig__CGRA_Enable),
    .CGRA_Reset(RegBConfig__CGRA_Reset),
    .ConfigIn(RegBConfig__ConfigIn),
    .ConfigOut(RegBConfig__ConfigOut),
    .Config_Clock(RegBConfig__Config_Clock),
    .Config_Reset(RegBConfig__Config_Reset),
    .select(RegBConfig__select)
  );

  //Wire declarations for instance 'alu_muxa' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxa__in0;
  wire [31:0] alu_muxa__in1;
  wire [31:0] alu_muxa__out;
  wire [0:0] alu_muxa__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxa(
    .in0(alu_muxa__in0),
    .in1(alu_muxa__in1),
    .out(alu_muxa__out),
    .select(alu_muxa__select)
  );

  //Wire declarations for instance 'alu_muxb' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxb__in0;
  wire [31:0] alu_muxb__in1;
  wire [31:0] alu_muxb__out;
  wire [0:0] alu_muxb__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxb(
    .in0(alu_muxb__in0),
    .in1(alu_muxb__in1),
    .out(alu_muxb__out),
    .select(alu_muxb__select)
  );

  //Wire declarations for instance 'const' (Module const_32b_U15)
  wire  const__CGRA_Clock;
  wire  const__CGRA_Enable;
  wire  const__CGRA_Reset;
  wire  const__ConfigIn;
  wire  const__ConfigOut;
  wire  const__Config_Clock;
  wire  const__Config_Reset;
  wire [31:0] const__out;
  const_32b_U15 const(
    .CGRA_Clock(const__CGRA_Clock),
    .CGRA_Enable(const__CGRA_Enable),
    .CGRA_Reset(const__CGRA_Reset),
    .ConfigIn(const__ConfigIn),
    .ConfigOut(const__ConfigOut),
    .Config_Clock(const__Config_Clock),
    .Config_Reset(const__Config_Reset),
    .out(const__out)
  );

  //Wire declarations for instance 'crossbar' (Module crossbar_7in_7out_U19)
  wire  crossbar__CGRA_Clock;
  wire  crossbar__CGRA_Enable;
  wire  crossbar__CGRA_Reset;
  wire  crossbar__ConfigIn;
  wire  crossbar__ConfigOut;
  wire  crossbar__Config_Clock;
  wire  crossbar__Config_Reset;
  wire [31:0] crossbar__in0;
  wire [31:0] crossbar__in1;
  wire [31:0] crossbar__in2;
  wire [31:0] crossbar__in3;
  wire [31:0] crossbar__in4;
  wire [31:0] crossbar__in5;
  wire [31:0] crossbar__in6;
  wire [31:0] crossbar__out0;
  wire [31:0] crossbar__out1;
  wire [31:0] crossbar__out2;
  wire [31:0] crossbar__out3;
  wire [31:0] crossbar__out4;
  wire [31:0] crossbar__out5;
  wire [31:0] crossbar__out6;
  crossbar_7in_7out_U19 crossbar(
    .CGRA_Clock(crossbar__CGRA_Clock),
    .CGRA_Enable(crossbar__CGRA_Enable),
    .CGRA_Reset(crossbar__CGRA_Reset),
    .ConfigIn(crossbar__ConfigIn),
    .ConfigOut(crossbar__ConfigOut),
    .Config_Clock(crossbar__Config_Clock),
    .Config_Reset(crossbar__Config_Reset),
    .in0(crossbar__in0),
    .in1(crossbar__in1),
    .in2(crossbar__in2),
    .in3(crossbar__in3),
    .in4(crossbar__in4),
    .in5(crossbar__in5),
    .in6(crossbar__in6),
    .out0(crossbar__out0),
    .out1(crossbar__out1),
    .out2(crossbar__out2),
    .out3(crossbar__out3),
    .out4(crossbar__out4),
    .out5(crossbar__out5),
    .out6(crossbar__out6)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__out;
  wire [0:0] mux_0__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__out;
  wire [0:0] mux_1__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__out;
  wire [0:0] mux_2__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__out;
  wire [0:0] mux_3__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_6' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_6__in0;
  wire [31:0] mux_6__in1;
  wire [31:0] mux_6__out;
  wire [0:0] mux_6__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_6(
    .in0(mux_6__in0),
    .in1(mux_6__in1),
    .out(mux_6__out),
    .select(mux_6__select)
  );

  //Wire declarations for instance 'reg0' (Module cgrame_register_32b)
  wire  reg0__CGRA_Clock;
  wire  reg0__CGRA_Enable;
  wire  reg0__CGRA_Reset;
  wire [0:0] reg0__enable;
  wire [31:0] reg0__in;
  wire [31:0] reg0__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg0(
    .CGRA_Clock(reg0__CGRA_Clock),
    .CGRA_Enable(reg0__CGRA_Enable),
    .CGRA_Reset(reg0__CGRA_Reset),
    .enable(reg0__enable),
    .in(reg0__in),
    .out(reg0__out)
  );

  //Wire declarations for instance 'reg1' (Module cgrame_register_32b)
  wire  reg1__CGRA_Clock;
  wire  reg1__CGRA_Enable;
  wire  reg1__CGRA_Reset;
  wire [0:0] reg1__enable;
  wire [31:0] reg1__in;
  wire [31:0] reg1__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg1(
    .CGRA_Clock(reg1__CGRA_Clock),
    .CGRA_Enable(reg1__CGRA_Enable),
    .CGRA_Reset(reg1__CGRA_Reset),
    .enable(reg1__enable),
    .in(reg1__in),
    .out(reg1__out)
  );

  //Wire declarations for instance 'reg2' (Module cgrame_register_32b)
  wire  reg2__CGRA_Clock;
  wire  reg2__CGRA_Enable;
  wire  reg2__CGRA_Reset;
  wire [0:0] reg2__enable;
  wire [31:0] reg2__in;
  wire [31:0] reg2__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg2(
    .CGRA_Clock(reg2__CGRA_Clock),
    .CGRA_Enable(reg2__CGRA_Enable),
    .CGRA_Reset(reg2__CGRA_Reset),
    .enable(reg2__enable),
    .in(reg2__in),
    .out(reg2__out)
  );

  //Wire declarations for instance 'reg3' (Module cgrame_register_32b)
  wire  reg3__CGRA_Clock;
  wire  reg3__CGRA_Enable;
  wire  reg3__CGRA_Reset;
  wire [0:0] reg3__enable;
  wire [31:0] reg3__in;
  wire [31:0] reg3__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg3(
    .CGRA_Clock(reg3__CGRA_Clock),
    .CGRA_Enable(reg3__CGRA_Enable),
    .CGRA_Reset(reg3__CGRA_Reset),
    .enable(reg3__enable),
    .in(reg3__in),
    .out(reg3__out)
  );

  //Wire declarations for instance 'reg6' (Module cgrame_register_32b)
  wire  reg6__CGRA_Clock;
  wire  reg6__CGRA_Enable;
  wire  reg6__CGRA_Reset;
  wire [0:0] reg6__enable;
  wire [31:0] reg6__in;
  wire [31:0] reg6__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg6(
    .CGRA_Clock(reg6__CGRA_Clock),
    .CGRA_Enable(reg6__CGRA_Enable),
    .CGRA_Reset(reg6__CGRA_Reset),
    .enable(reg6__enable),
    .in(reg6__in),
    .out(reg6__out)
  );

  //Wire declarations for instance 'rega' (Module cgrame_register_32b)
  wire  rega__CGRA_Clock;
  wire  rega__CGRA_Enable;
  wire  rega__CGRA_Reset;
  wire [0:0] rega__enable;
  wire [31:0] rega__in;
  wire [31:0] rega__out;
  cgrame_register_32b #(.contexts(1),.size(32)) rega(
    .CGRA_Clock(rega__CGRA_Clock),
    .CGRA_Enable(rega__CGRA_Enable),
    .CGRA_Reset(rega__CGRA_Reset),
    .enable(rega__enable),
    .in(rega__in),
    .out(rega__out)
  );

  //Wire declarations for instance 'regb' (Module cgrame_register_32b)
  wire  regb__CGRA_Clock;
  wire  regb__CGRA_Enable;
  wire  regb__CGRA_Reset;
  wire [0:0] regb__enable;
  wire [31:0] regb__in;
  wire [31:0] regb__out;
  cgrame_register_32b #(.contexts(1),.size(32)) regb(
    .CGRA_Clock(regb__CGRA_Clock),
    .CGRA_Enable(regb__CGRA_Enable),
    .CGRA_Reset(regb__CGRA_Reset),
    .enable(regb__enable),
    .in(regb__in),
    .out(regb__out)
  );

  //All the connections
  assign ALU__CGRA_Reset = CGRA_Reset;
  assign ALU__CGRA_Clock = CGRA_Clock;
  assign RES__CGRA_Clock = CGRA_Clock;
  assign const__CGRA_Clock = CGRA_Clock;
  assign crossbar__CGRA_Clock = CGRA_Clock;
  assign reg0__CGRA_Clock = CGRA_Clock;
  assign reg1__CGRA_Clock = CGRA_Clock;
  assign reg2__CGRA_Clock = CGRA_Clock;
  assign reg3__CGRA_Clock = CGRA_Clock;
  assign reg6__CGRA_Clock = CGRA_Clock;
  assign rega__CGRA_Clock = CGRA_Clock;
  assign regb__CGRA_Clock = CGRA_Clock;
  assign ALUMuxAconfig__CGRA_Clock = CGRA_Clock;
  assign ALUMuxBconfig__CGRA_Clock = CGRA_Clock;
  assign ALUconfig__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux6config__CGRA_Clock = CGRA_Clock;
  assign RESConfig__CGRA_Clock = CGRA_Clock;
  assign Reg0config__CGRA_Clock = CGRA_Clock;
  assign Reg1config__CGRA_Clock = CGRA_Clock;
  assign Reg2config__CGRA_Clock = CGRA_Clock;
  assign Reg3config__CGRA_Clock = CGRA_Clock;
  assign Reg6config__CGRA_Clock = CGRA_Clock;
  assign RegAConfig__CGRA_Clock = CGRA_Clock;
  assign RegBConfig__CGRA_Clock = CGRA_Clock;
  assign ALU__CGRA_Enable = CGRA_Enable;
  assign RES__CGRA_Reset = CGRA_Reset;
  assign const__CGRA_Reset = CGRA_Reset;
  assign crossbar__CGRA_Reset = CGRA_Reset;
  assign reg0__CGRA_Reset = CGRA_Reset;
  assign reg1__CGRA_Reset = CGRA_Reset;
  assign reg2__CGRA_Reset = CGRA_Reset;
  assign reg3__CGRA_Reset = CGRA_Reset;
  assign reg6__CGRA_Reset = CGRA_Reset;
  assign rega__CGRA_Reset = CGRA_Reset;
  assign regb__CGRA_Reset = CGRA_Reset;
  assign ALUMuxAconfig__CGRA_Reset = CGRA_Reset;
  assign ALUMuxBconfig__CGRA_Reset = CGRA_Reset;
  assign ALUconfig__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux6config__CGRA_Reset = CGRA_Reset;
  assign RESConfig__CGRA_Reset = CGRA_Reset;
  assign Reg0config__CGRA_Reset = CGRA_Reset;
  assign Reg1config__CGRA_Reset = CGRA_Reset;
  assign Reg2config__CGRA_Reset = CGRA_Reset;
  assign Reg3config__CGRA_Reset = CGRA_Reset;
  assign Reg6config__CGRA_Reset = CGRA_Reset;
  assign RegAConfig__CGRA_Reset = CGRA_Reset;
  assign RegBConfig__CGRA_Reset = CGRA_Reset;
  assign RES__CGRA_Enable = CGRA_Enable;
  assign const__CGRA_Enable = CGRA_Enable;
  assign crossbar__CGRA_Enable = CGRA_Enable;
  assign reg0__CGRA_Enable = CGRA_Enable;
  assign reg1__CGRA_Enable = CGRA_Enable;
  assign reg2__CGRA_Enable = CGRA_Enable;
  assign reg3__CGRA_Enable = CGRA_Enable;
  assign reg6__CGRA_Enable = CGRA_Enable;
  assign rega__CGRA_Enable = CGRA_Enable;
  assign regb__CGRA_Enable = CGRA_Enable;
  assign ALUMuxAconfig__CGRA_Enable = CGRA_Enable;
  assign ALUMuxBconfig__CGRA_Enable = CGRA_Enable;
  assign ALUconfig__CGRA_Enable = CGRA_Enable;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux6config__CGRA_Enable = CGRA_Enable;
  assign RESConfig__CGRA_Enable = CGRA_Enable;
  assign Reg0config__CGRA_Enable = CGRA_Enable;
  assign Reg1config__CGRA_Enable = CGRA_Enable;
  assign Reg2config__CGRA_Enable = CGRA_Enable;
  assign Reg3config__CGRA_Enable = CGRA_Enable;
  assign Reg6config__CGRA_Enable = CGRA_Enable;
  assign RegAConfig__CGRA_Enable = CGRA_Enable;
  assign RegBConfig__CGRA_Enable = CGRA_Enable;
  assign const__Config_Clock = Config_Clock;
  assign const__Config_Reset = Config_Reset;
  assign crossbar__Config_Clock = Config_Clock;
  assign ALUMuxAconfig__Config_Clock = Config_Clock;
  assign ALUMuxBconfig__Config_Clock = Config_Clock;
  assign ALUconfig__Config_Clock = Config_Clock;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux6config__Config_Clock = Config_Clock;
  assign RESConfig__Config_Clock = Config_Clock;
  assign Reg0config__Config_Clock = Config_Clock;
  assign Reg1config__Config_Clock = Config_Clock;
  assign Reg2config__Config_Clock = Config_Clock;
  assign Reg3config__Config_Clock = Config_Clock;
  assign Reg6config__Config_Clock = Config_Clock;
  assign RegAConfig__Config_Clock = Config_Clock;
  assign RegBConfig__Config_Clock = Config_Clock;
  assign crossbar__Config_Reset = Config_Reset;
  assign ALUMuxAconfig__Config_Reset = Config_Reset;
  assign ALUMuxBconfig__Config_Reset = Config_Reset;
  assign ALUconfig__Config_Reset = Config_Reset;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux6config__Config_Reset = Config_Reset;
  assign RESConfig__Config_Reset = Config_Reset;
  assign Reg0config__Config_Reset = Config_Reset;
  assign Reg1config__Config_Reset = Config_Reset;
  assign Reg2config__Config_Reset = Config_Reset;
  assign Reg3config__Config_Reset = Config_Reset;
  assign Reg6config__Config_Reset = Config_Reset;
  assign RegAConfig__Config_Reset = Config_Reset;
  assign RegBConfig__Config_Reset = Config_Reset;
  assign crossbar__in5[31:0] = ALU__out[31:0];
  assign RES__in[31:0] = ALU__out[31:0];
  assign out0[31:0] = crossbar__out0[31:0];
  assign out1[31:0] = crossbar__out1[31:0];
  assign out2[31:0] = crossbar__out2[31:0];
  assign out3[31:0] = crossbar__out3[31:0];
  assign out6[31:0] = crossbar__out4[31:0];
  assign rega__in[31:0] = crossbar__out5[31:0];
  assign regb__in[31:0] = crossbar__out6[31:0];
  assign crossbar__in6[31:0] = RES__out[31:0];
  assign alu_muxa__in0[31:0] = rega__out[31:0];
  assign alu_muxb__in0[31:0] = regb__out[31:0];
  assign alu_muxa__in1[31:0] = const__out[31:0];
  assign alu_muxb__in1[31:0] = const__out[31:0];
  assign ALU__in_a[31:0] = alu_muxa__out[31:0];
  assign ALU__in_b[31:0] = alu_muxb__out[31:0];
  assign mux_0__in1[31:0] = reg0__out[31:0];
  assign crossbar__in0[31:0] = mux_0__out[31:0];
  assign mux_1__in1[31:0] = reg1__out[31:0];
  assign crossbar__in1[31:0] = mux_1__out[31:0];
  assign mux_2__in1[31:0] = reg2__out[31:0];
  assign crossbar__in2[31:0] = mux_2__out[31:0];
  assign mux_3__in1[31:0] = reg3__out[31:0];
  assign crossbar__in3[31:0] = mux_3__out[31:0];
  assign mux_6__in1[31:0] = reg6__out[31:0];
  assign crossbar__in4[31:0] = mux_6__out[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign reg0__in[31:0] = in0[31:0];
  assign mux_1__in0[31:0] = in1[31:0];
  assign reg1__in[31:0] = in1[31:0];
  assign mux_2__in0[31:0] = in2[31:0];
  assign reg2__in[31:0] = in2[31:0];
  assign mux_3__in0[31:0] = in3[31:0];
  assign reg3__in[31:0] = in3[31:0];
  assign mux_6__in0[31:0] = in6[31:0];
  assign reg6__in[31:0] = in6[31:0];
  assign alu_muxa__select[0:0] = ALUMuxAconfig__select[0:0];
  assign alu_muxb__select[0:0] = ALUMuxBconfig__select[0:0];
  assign ALU__select[3:0] = ALUconfig__select[3:0];
  assign mux_0__select[0:0] = Mux0config__select[0:0];
  assign mux_1__select[0:0] = Mux1config__select[0:0];
  assign mux_2__select[0:0] = Mux2config__select[0:0];
  assign mux_3__select[0:0] = Mux3config__select[0:0];
  assign mux_6__select[0:0] = Mux6config__select[0:0];
  assign RES__enable[0:0] = RESConfig__select[0:0];
  assign reg0__enable[0:0] = Reg0config__select[0:0];
  assign reg1__enable[0:0] = Reg1config__select[0:0];
  assign reg2__enable[0:0] = Reg2config__select[0:0];
  assign reg3__enable[0:0] = Reg3config__select[0:0];
  assign reg6__enable[0:0] = Reg6config__select[0:0];
  assign regb__enable[0:0] = RegAConfig__select[0:0];
  assign rega__enable[0:0] = RegBConfig__select[0:0];
  assign ALUMuxAconfig__ConfigIn = ConfigIn;
  assign ALUMuxBconfig__ConfigIn = ALUMuxAconfig__ConfigOut;
  assign ALUconfig__ConfigIn = ALUMuxBconfig__ConfigOut;
  assign Mux0config__ConfigIn = ALUconfig__ConfigOut;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux6config__ConfigIn = Mux3config__ConfigOut;
  assign RESConfig__ConfigIn = Mux6config__ConfigOut;
  assign Reg0config__ConfigIn = RESConfig__ConfigOut;
  assign Reg1config__ConfigIn = Reg0config__ConfigOut;
  assign Reg2config__ConfigIn = Reg1config__ConfigOut;
  assign Reg3config__ConfigIn = Reg2config__ConfigOut;
  assign Reg6config__ConfigIn = Reg3config__ConfigOut;
  assign RegAConfig__ConfigIn = Reg6config__ConfigOut;
  assign RegBConfig__ConfigIn = RegAConfig__ConfigOut;
  assign const__ConfigIn = RegBConfig__ConfigOut;
  assign crossbar__ConfigIn = const__ConfigOut;
  assign ConfigOut = crossbar__ConfigOut;

endmodule //hycube_no_pred_in79_out79_U3

module hycube_no_pred_in111_out111_U4 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in5,
  input [31:0] in6,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out5,
  output [31:0] out6
);
  //Wire declarations for instance 'ALU' (Module func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12)
  wire  ALU__CGRA_Clock;
  wire  ALU__CGRA_Enable;
  wire  ALU__CGRA_Reset;
  wire [31:0] ALU__in_a;
  wire [31:0] ALU__in_b;
  wire [31:0] ALU__out;
  wire [3:0] ALU__select;
  func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12 ALU(
    .CGRA_Clock(ALU__CGRA_Clock),
    .CGRA_Enable(ALU__CGRA_Enable),
    .CGRA_Reset(ALU__CGRA_Reset),
    .in_a(ALU__in_a),
    .in_b(ALU__in_b),
    .out(ALU__out),
    .select(ALU__select)
  );

  //Wire declarations for instance 'ALUMuxAconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxAconfig__CGRA_Clock;
  wire  ALUMuxAconfig__CGRA_Enable;
  wire  ALUMuxAconfig__CGRA_Reset;
  wire  ALUMuxAconfig__ConfigIn;
  wire  ALUMuxAconfig__ConfigOut;
  wire  ALUMuxAconfig__Config_Clock;
  wire  ALUMuxAconfig__Config_Reset;
  wire [0:0] ALUMuxAconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxAconfig(
    .CGRA_Clock(ALUMuxAconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxAconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxAconfig__CGRA_Reset),
    .ConfigIn(ALUMuxAconfig__ConfigIn),
    .ConfigOut(ALUMuxAconfig__ConfigOut),
    .Config_Clock(ALUMuxAconfig__Config_Clock),
    .Config_Reset(ALUMuxAconfig__Config_Reset),
    .select(ALUMuxAconfig__select)
  );

  //Wire declarations for instance 'ALUMuxBconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxBconfig__CGRA_Clock;
  wire  ALUMuxBconfig__CGRA_Enable;
  wire  ALUMuxBconfig__CGRA_Reset;
  wire  ALUMuxBconfig__ConfigIn;
  wire  ALUMuxBconfig__ConfigOut;
  wire  ALUMuxBconfig__Config_Clock;
  wire  ALUMuxBconfig__Config_Reset;
  wire [0:0] ALUMuxBconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxBconfig(
    .CGRA_Clock(ALUMuxBconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxBconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxBconfig__CGRA_Reset),
    .ConfigIn(ALUMuxBconfig__ConfigIn),
    .ConfigOut(ALUMuxBconfig__ConfigOut),
    .Config_Clock(ALUMuxBconfig__Config_Clock),
    .Config_Reset(ALUMuxBconfig__Config_Reset),
    .select(ALUMuxBconfig__select)
  );

  //Wire declarations for instance 'ALUconfig' (Module cgrame_configcell_context1_size4)
  wire  ALUconfig__CGRA_Clock;
  wire  ALUconfig__CGRA_Enable;
  wire  ALUconfig__CGRA_Reset;
  wire  ALUconfig__ConfigIn;
  wire  ALUconfig__ConfigOut;
  wire  ALUconfig__Config_Clock;
  wire  ALUconfig__Config_Reset;
  wire [3:0] ALUconfig__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) ALUconfig(
    .CGRA_Clock(ALUconfig__CGRA_Clock),
    .CGRA_Enable(ALUconfig__CGRA_Enable),
    .CGRA_Reset(ALUconfig__CGRA_Reset),
    .ConfigIn(ALUconfig__ConfigIn),
    .ConfigOut(ALUconfig__ConfigOut),
    .Config_Clock(ALUconfig__Config_Clock),
    .Config_Reset(ALUconfig__Config_Reset),
    .select(ALUconfig__select)
  );

  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size1)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [0:0] Mux0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size1)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [0:0] Mux1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size1)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [0:0] Mux2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size1)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [0:0] Mux3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux5config' (Module cgrame_configcell_context1_size1)
  wire  Mux5config__CGRA_Clock;
  wire  Mux5config__CGRA_Enable;
  wire  Mux5config__CGRA_Reset;
  wire  Mux5config__ConfigIn;
  wire  Mux5config__ConfigOut;
  wire  Mux5config__Config_Clock;
  wire  Mux5config__Config_Reset;
  wire [0:0] Mux5config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux5config(
    .CGRA_Clock(Mux5config__CGRA_Clock),
    .CGRA_Enable(Mux5config__CGRA_Enable),
    .CGRA_Reset(Mux5config__CGRA_Reset),
    .ConfigIn(Mux5config__ConfigIn),
    .ConfigOut(Mux5config__ConfigOut),
    .Config_Clock(Mux5config__Config_Clock),
    .Config_Reset(Mux5config__Config_Reset),
    .select(Mux5config__select)
  );

  //Wire declarations for instance 'Mux6config' (Module cgrame_configcell_context1_size1)
  wire  Mux6config__CGRA_Clock;
  wire  Mux6config__CGRA_Enable;
  wire  Mux6config__CGRA_Reset;
  wire  Mux6config__ConfigIn;
  wire  Mux6config__ConfigOut;
  wire  Mux6config__Config_Clock;
  wire  Mux6config__Config_Reset;
  wire [0:0] Mux6config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux6config(
    .CGRA_Clock(Mux6config__CGRA_Clock),
    .CGRA_Enable(Mux6config__CGRA_Enable),
    .CGRA_Reset(Mux6config__CGRA_Reset),
    .ConfigIn(Mux6config__ConfigIn),
    .ConfigOut(Mux6config__ConfigOut),
    .Config_Clock(Mux6config__Config_Clock),
    .Config_Reset(Mux6config__Config_Reset),
    .select(Mux6config__select)
  );

  //Wire declarations for instance 'RES' (Module cgrame_register_32b)
  wire  RES__CGRA_Clock;
  wire  RES__CGRA_Enable;
  wire  RES__CGRA_Reset;
  wire [0:0] RES__enable;
  wire [31:0] RES__in;
  wire [31:0] RES__out;
  cgrame_register_32b #(.contexts(1),.size(32)) RES(
    .CGRA_Clock(RES__CGRA_Clock),
    .CGRA_Enable(RES__CGRA_Enable),
    .CGRA_Reset(RES__CGRA_Reset),
    .enable(RES__enable),
    .in(RES__in),
    .out(RES__out)
  );

  //Wire declarations for instance 'RESConfig' (Module cgrame_configcell_context1_size1)
  wire  RESConfig__CGRA_Clock;
  wire  RESConfig__CGRA_Enable;
  wire  RESConfig__CGRA_Reset;
  wire  RESConfig__ConfigIn;
  wire  RESConfig__ConfigOut;
  wire  RESConfig__Config_Clock;
  wire  RESConfig__Config_Reset;
  wire [0:0] RESConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RESConfig(
    .CGRA_Clock(RESConfig__CGRA_Clock),
    .CGRA_Enable(RESConfig__CGRA_Enable),
    .CGRA_Reset(RESConfig__CGRA_Reset),
    .ConfigIn(RESConfig__ConfigIn),
    .ConfigOut(RESConfig__ConfigOut),
    .Config_Clock(RESConfig__Config_Clock),
    .Config_Reset(RESConfig__Config_Reset),
    .select(RESConfig__select)
  );

  //Wire declarations for instance 'Reg0config' (Module cgrame_configcell_context1_size1)
  wire  Reg0config__CGRA_Clock;
  wire  Reg0config__CGRA_Enable;
  wire  Reg0config__CGRA_Reset;
  wire  Reg0config__ConfigIn;
  wire  Reg0config__ConfigOut;
  wire  Reg0config__Config_Clock;
  wire  Reg0config__Config_Reset;
  wire [0:0] Reg0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg0config(
    .CGRA_Clock(Reg0config__CGRA_Clock),
    .CGRA_Enable(Reg0config__CGRA_Enable),
    .CGRA_Reset(Reg0config__CGRA_Reset),
    .ConfigIn(Reg0config__ConfigIn),
    .ConfigOut(Reg0config__ConfigOut),
    .Config_Clock(Reg0config__Config_Clock),
    .Config_Reset(Reg0config__Config_Reset),
    .select(Reg0config__select)
  );

  //Wire declarations for instance 'Reg1config' (Module cgrame_configcell_context1_size1)
  wire  Reg1config__CGRA_Clock;
  wire  Reg1config__CGRA_Enable;
  wire  Reg1config__CGRA_Reset;
  wire  Reg1config__ConfigIn;
  wire  Reg1config__ConfigOut;
  wire  Reg1config__Config_Clock;
  wire  Reg1config__Config_Reset;
  wire [0:0] Reg1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg1config(
    .CGRA_Clock(Reg1config__CGRA_Clock),
    .CGRA_Enable(Reg1config__CGRA_Enable),
    .CGRA_Reset(Reg1config__CGRA_Reset),
    .ConfigIn(Reg1config__ConfigIn),
    .ConfigOut(Reg1config__ConfigOut),
    .Config_Clock(Reg1config__Config_Clock),
    .Config_Reset(Reg1config__Config_Reset),
    .select(Reg1config__select)
  );

  //Wire declarations for instance 'Reg2config' (Module cgrame_configcell_context1_size1)
  wire  Reg2config__CGRA_Clock;
  wire  Reg2config__CGRA_Enable;
  wire  Reg2config__CGRA_Reset;
  wire  Reg2config__ConfigIn;
  wire  Reg2config__ConfigOut;
  wire  Reg2config__Config_Clock;
  wire  Reg2config__Config_Reset;
  wire [0:0] Reg2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg2config(
    .CGRA_Clock(Reg2config__CGRA_Clock),
    .CGRA_Enable(Reg2config__CGRA_Enable),
    .CGRA_Reset(Reg2config__CGRA_Reset),
    .ConfigIn(Reg2config__ConfigIn),
    .ConfigOut(Reg2config__ConfigOut),
    .Config_Clock(Reg2config__Config_Clock),
    .Config_Reset(Reg2config__Config_Reset),
    .select(Reg2config__select)
  );

  //Wire declarations for instance 'Reg3config' (Module cgrame_configcell_context1_size1)
  wire  Reg3config__CGRA_Clock;
  wire  Reg3config__CGRA_Enable;
  wire  Reg3config__CGRA_Reset;
  wire  Reg3config__ConfigIn;
  wire  Reg3config__ConfigOut;
  wire  Reg3config__Config_Clock;
  wire  Reg3config__Config_Reset;
  wire [0:0] Reg3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg3config(
    .CGRA_Clock(Reg3config__CGRA_Clock),
    .CGRA_Enable(Reg3config__CGRA_Enable),
    .CGRA_Reset(Reg3config__CGRA_Reset),
    .ConfigIn(Reg3config__ConfigIn),
    .ConfigOut(Reg3config__ConfigOut),
    .Config_Clock(Reg3config__Config_Clock),
    .Config_Reset(Reg3config__Config_Reset),
    .select(Reg3config__select)
  );

  //Wire declarations for instance 'Reg5config' (Module cgrame_configcell_context1_size1)
  wire  Reg5config__CGRA_Clock;
  wire  Reg5config__CGRA_Enable;
  wire  Reg5config__CGRA_Reset;
  wire  Reg5config__ConfigIn;
  wire  Reg5config__ConfigOut;
  wire  Reg5config__Config_Clock;
  wire  Reg5config__Config_Reset;
  wire [0:0] Reg5config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg5config(
    .CGRA_Clock(Reg5config__CGRA_Clock),
    .CGRA_Enable(Reg5config__CGRA_Enable),
    .CGRA_Reset(Reg5config__CGRA_Reset),
    .ConfigIn(Reg5config__ConfigIn),
    .ConfigOut(Reg5config__ConfigOut),
    .Config_Clock(Reg5config__Config_Clock),
    .Config_Reset(Reg5config__Config_Reset),
    .select(Reg5config__select)
  );

  //Wire declarations for instance 'Reg6config' (Module cgrame_configcell_context1_size1)
  wire  Reg6config__CGRA_Clock;
  wire  Reg6config__CGRA_Enable;
  wire  Reg6config__CGRA_Reset;
  wire  Reg6config__ConfigIn;
  wire  Reg6config__ConfigOut;
  wire  Reg6config__Config_Clock;
  wire  Reg6config__Config_Reset;
  wire [0:0] Reg6config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg6config(
    .CGRA_Clock(Reg6config__CGRA_Clock),
    .CGRA_Enable(Reg6config__CGRA_Enable),
    .CGRA_Reset(Reg6config__CGRA_Reset),
    .ConfigIn(Reg6config__ConfigIn),
    .ConfigOut(Reg6config__ConfigOut),
    .Config_Clock(Reg6config__Config_Clock),
    .Config_Reset(Reg6config__Config_Reset),
    .select(Reg6config__select)
  );

  //Wire declarations for instance 'RegAConfig' (Module cgrame_configcell_context1_size1)
  wire  RegAConfig__CGRA_Clock;
  wire  RegAConfig__CGRA_Enable;
  wire  RegAConfig__CGRA_Reset;
  wire  RegAConfig__ConfigIn;
  wire  RegAConfig__ConfigOut;
  wire  RegAConfig__Config_Clock;
  wire  RegAConfig__Config_Reset;
  wire [0:0] RegAConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegAConfig(
    .CGRA_Clock(RegAConfig__CGRA_Clock),
    .CGRA_Enable(RegAConfig__CGRA_Enable),
    .CGRA_Reset(RegAConfig__CGRA_Reset),
    .ConfigIn(RegAConfig__ConfigIn),
    .ConfigOut(RegAConfig__ConfigOut),
    .Config_Clock(RegAConfig__Config_Clock),
    .Config_Reset(RegAConfig__Config_Reset),
    .select(RegAConfig__select)
  );

  //Wire declarations for instance 'RegBConfig' (Module cgrame_configcell_context1_size1)
  wire  RegBConfig__CGRA_Clock;
  wire  RegBConfig__CGRA_Enable;
  wire  RegBConfig__CGRA_Reset;
  wire  RegBConfig__ConfigIn;
  wire  RegBConfig__ConfigOut;
  wire  RegBConfig__Config_Clock;
  wire  RegBConfig__Config_Reset;
  wire [0:0] RegBConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegBConfig(
    .CGRA_Clock(RegBConfig__CGRA_Clock),
    .CGRA_Enable(RegBConfig__CGRA_Enable),
    .CGRA_Reset(RegBConfig__CGRA_Reset),
    .ConfigIn(RegBConfig__ConfigIn),
    .ConfigOut(RegBConfig__ConfigOut),
    .Config_Clock(RegBConfig__Config_Clock),
    .Config_Reset(RegBConfig__Config_Reset),
    .select(RegBConfig__select)
  );

  //Wire declarations for instance 'alu_muxa' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxa__in0;
  wire [31:0] alu_muxa__in1;
  wire [31:0] alu_muxa__out;
  wire [0:0] alu_muxa__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxa(
    .in0(alu_muxa__in0),
    .in1(alu_muxa__in1),
    .out(alu_muxa__out),
    .select(alu_muxa__select)
  );

  //Wire declarations for instance 'alu_muxb' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxb__in0;
  wire [31:0] alu_muxb__in1;
  wire [31:0] alu_muxb__out;
  wire [0:0] alu_muxb__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxb(
    .in0(alu_muxb__in0),
    .in1(alu_muxb__in1),
    .out(alu_muxb__out),
    .select(alu_muxb__select)
  );

  //Wire declarations for instance 'const' (Module const_32b_U15)
  wire  const__CGRA_Clock;
  wire  const__CGRA_Enable;
  wire  const__CGRA_Reset;
  wire  const__ConfigIn;
  wire  const__ConfigOut;
  wire  const__Config_Clock;
  wire  const__Config_Reset;
  wire [31:0] const__out;
  const_32b_U15 const(
    .CGRA_Clock(const__CGRA_Clock),
    .CGRA_Enable(const__CGRA_Enable),
    .CGRA_Reset(const__CGRA_Reset),
    .ConfigIn(const__ConfigIn),
    .ConfigOut(const__ConfigOut),
    .Config_Clock(const__Config_Clock),
    .Config_Reset(const__Config_Reset),
    .out(const__out)
  );

  //Wire declarations for instance 'crossbar' (Module crossbar_8in_8out_U16)
  wire  crossbar__CGRA_Clock;
  wire  crossbar__CGRA_Enable;
  wire  crossbar__CGRA_Reset;
  wire  crossbar__ConfigIn;
  wire  crossbar__ConfigOut;
  wire  crossbar__Config_Clock;
  wire  crossbar__Config_Reset;
  wire [31:0] crossbar__in0;
  wire [31:0] crossbar__in1;
  wire [31:0] crossbar__in2;
  wire [31:0] crossbar__in3;
  wire [31:0] crossbar__in4;
  wire [31:0] crossbar__in5;
  wire [31:0] crossbar__in6;
  wire [31:0] crossbar__in7;
  wire [31:0] crossbar__out0;
  wire [31:0] crossbar__out1;
  wire [31:0] crossbar__out2;
  wire [31:0] crossbar__out3;
  wire [31:0] crossbar__out4;
  wire [31:0] crossbar__out5;
  wire [31:0] crossbar__out6;
  wire [31:0] crossbar__out7;
  crossbar_8in_8out_U16 crossbar(
    .CGRA_Clock(crossbar__CGRA_Clock),
    .CGRA_Enable(crossbar__CGRA_Enable),
    .CGRA_Reset(crossbar__CGRA_Reset),
    .ConfigIn(crossbar__ConfigIn),
    .ConfigOut(crossbar__ConfigOut),
    .Config_Clock(crossbar__Config_Clock),
    .Config_Reset(crossbar__Config_Reset),
    .in0(crossbar__in0),
    .in1(crossbar__in1),
    .in2(crossbar__in2),
    .in3(crossbar__in3),
    .in4(crossbar__in4),
    .in5(crossbar__in5),
    .in6(crossbar__in6),
    .in7(crossbar__in7),
    .out0(crossbar__out0),
    .out1(crossbar__out1),
    .out2(crossbar__out2),
    .out3(crossbar__out3),
    .out4(crossbar__out4),
    .out5(crossbar__out5),
    .out6(crossbar__out6),
    .out7(crossbar__out7)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__out;
  wire [0:0] mux_0__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__out;
  wire [0:0] mux_1__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__out;
  wire [0:0] mux_2__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__out;
  wire [0:0] mux_3__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_5' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_5__in0;
  wire [31:0] mux_5__in1;
  wire [31:0] mux_5__out;
  wire [0:0] mux_5__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_5(
    .in0(mux_5__in0),
    .in1(mux_5__in1),
    .out(mux_5__out),
    .select(mux_5__select)
  );

  //Wire declarations for instance 'mux_6' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_6__in0;
  wire [31:0] mux_6__in1;
  wire [31:0] mux_6__out;
  wire [0:0] mux_6__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_6(
    .in0(mux_6__in0),
    .in1(mux_6__in1),
    .out(mux_6__out),
    .select(mux_6__select)
  );

  //Wire declarations for instance 'reg0' (Module cgrame_register_32b)
  wire  reg0__CGRA_Clock;
  wire  reg0__CGRA_Enable;
  wire  reg0__CGRA_Reset;
  wire [0:0] reg0__enable;
  wire [31:0] reg0__in;
  wire [31:0] reg0__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg0(
    .CGRA_Clock(reg0__CGRA_Clock),
    .CGRA_Enable(reg0__CGRA_Enable),
    .CGRA_Reset(reg0__CGRA_Reset),
    .enable(reg0__enable),
    .in(reg0__in),
    .out(reg0__out)
  );

  //Wire declarations for instance 'reg1' (Module cgrame_register_32b)
  wire  reg1__CGRA_Clock;
  wire  reg1__CGRA_Enable;
  wire  reg1__CGRA_Reset;
  wire [0:0] reg1__enable;
  wire [31:0] reg1__in;
  wire [31:0] reg1__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg1(
    .CGRA_Clock(reg1__CGRA_Clock),
    .CGRA_Enable(reg1__CGRA_Enable),
    .CGRA_Reset(reg1__CGRA_Reset),
    .enable(reg1__enable),
    .in(reg1__in),
    .out(reg1__out)
  );

  //Wire declarations for instance 'reg2' (Module cgrame_register_32b)
  wire  reg2__CGRA_Clock;
  wire  reg2__CGRA_Enable;
  wire  reg2__CGRA_Reset;
  wire [0:0] reg2__enable;
  wire [31:0] reg2__in;
  wire [31:0] reg2__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg2(
    .CGRA_Clock(reg2__CGRA_Clock),
    .CGRA_Enable(reg2__CGRA_Enable),
    .CGRA_Reset(reg2__CGRA_Reset),
    .enable(reg2__enable),
    .in(reg2__in),
    .out(reg2__out)
  );

  //Wire declarations for instance 'reg3' (Module cgrame_register_32b)
  wire  reg3__CGRA_Clock;
  wire  reg3__CGRA_Enable;
  wire  reg3__CGRA_Reset;
  wire [0:0] reg3__enable;
  wire [31:0] reg3__in;
  wire [31:0] reg3__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg3(
    .CGRA_Clock(reg3__CGRA_Clock),
    .CGRA_Enable(reg3__CGRA_Enable),
    .CGRA_Reset(reg3__CGRA_Reset),
    .enable(reg3__enable),
    .in(reg3__in),
    .out(reg3__out)
  );

  //Wire declarations for instance 'reg5' (Module cgrame_register_32b)
  wire  reg5__CGRA_Clock;
  wire  reg5__CGRA_Enable;
  wire  reg5__CGRA_Reset;
  wire [0:0] reg5__enable;
  wire [31:0] reg5__in;
  wire [31:0] reg5__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg5(
    .CGRA_Clock(reg5__CGRA_Clock),
    .CGRA_Enable(reg5__CGRA_Enable),
    .CGRA_Reset(reg5__CGRA_Reset),
    .enable(reg5__enable),
    .in(reg5__in),
    .out(reg5__out)
  );

  //Wire declarations for instance 'reg6' (Module cgrame_register_32b)
  wire  reg6__CGRA_Clock;
  wire  reg6__CGRA_Enable;
  wire  reg6__CGRA_Reset;
  wire [0:0] reg6__enable;
  wire [31:0] reg6__in;
  wire [31:0] reg6__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg6(
    .CGRA_Clock(reg6__CGRA_Clock),
    .CGRA_Enable(reg6__CGRA_Enable),
    .CGRA_Reset(reg6__CGRA_Reset),
    .enable(reg6__enable),
    .in(reg6__in),
    .out(reg6__out)
  );

  //Wire declarations for instance 'rega' (Module cgrame_register_32b)
  wire  rega__CGRA_Clock;
  wire  rega__CGRA_Enable;
  wire  rega__CGRA_Reset;
  wire [0:0] rega__enable;
  wire [31:0] rega__in;
  wire [31:0] rega__out;
  cgrame_register_32b #(.contexts(1),.size(32)) rega(
    .CGRA_Clock(rega__CGRA_Clock),
    .CGRA_Enable(rega__CGRA_Enable),
    .CGRA_Reset(rega__CGRA_Reset),
    .enable(rega__enable),
    .in(rega__in),
    .out(rega__out)
  );

  //Wire declarations for instance 'regb' (Module cgrame_register_32b)
  wire  regb__CGRA_Clock;
  wire  regb__CGRA_Enable;
  wire  regb__CGRA_Reset;
  wire [0:0] regb__enable;
  wire [31:0] regb__in;
  wire [31:0] regb__out;
  cgrame_register_32b #(.contexts(1),.size(32)) regb(
    .CGRA_Clock(regb__CGRA_Clock),
    .CGRA_Enable(regb__CGRA_Enable),
    .CGRA_Reset(regb__CGRA_Reset),
    .enable(regb__enable),
    .in(regb__in),
    .out(regb__out)
  );

  //All the connections
  assign ALU__CGRA_Reset = CGRA_Reset;
  assign ALU__CGRA_Clock = CGRA_Clock;
  assign RES__CGRA_Clock = CGRA_Clock;
  assign const__CGRA_Clock = CGRA_Clock;
  assign crossbar__CGRA_Clock = CGRA_Clock;
  assign reg0__CGRA_Clock = CGRA_Clock;
  assign reg1__CGRA_Clock = CGRA_Clock;
  assign reg2__CGRA_Clock = CGRA_Clock;
  assign reg3__CGRA_Clock = CGRA_Clock;
  assign reg5__CGRA_Clock = CGRA_Clock;
  assign reg6__CGRA_Clock = CGRA_Clock;
  assign rega__CGRA_Clock = CGRA_Clock;
  assign regb__CGRA_Clock = CGRA_Clock;
  assign ALUMuxAconfig__CGRA_Clock = CGRA_Clock;
  assign ALUMuxBconfig__CGRA_Clock = CGRA_Clock;
  assign ALUconfig__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux5config__CGRA_Clock = CGRA_Clock;
  assign Mux6config__CGRA_Clock = CGRA_Clock;
  assign RESConfig__CGRA_Clock = CGRA_Clock;
  assign Reg0config__CGRA_Clock = CGRA_Clock;
  assign Reg1config__CGRA_Clock = CGRA_Clock;
  assign Reg2config__CGRA_Clock = CGRA_Clock;
  assign Reg3config__CGRA_Clock = CGRA_Clock;
  assign Reg5config__CGRA_Clock = CGRA_Clock;
  assign Reg6config__CGRA_Clock = CGRA_Clock;
  assign RegAConfig__CGRA_Clock = CGRA_Clock;
  assign RegBConfig__CGRA_Clock = CGRA_Clock;
  assign ALU__CGRA_Enable = CGRA_Enable;
  assign RES__CGRA_Reset = CGRA_Reset;
  assign const__CGRA_Reset = CGRA_Reset;
  assign crossbar__CGRA_Reset = CGRA_Reset;
  assign reg0__CGRA_Reset = CGRA_Reset;
  assign reg1__CGRA_Reset = CGRA_Reset;
  assign reg2__CGRA_Reset = CGRA_Reset;
  assign reg3__CGRA_Reset = CGRA_Reset;
  assign reg5__CGRA_Reset = CGRA_Reset;
  assign reg6__CGRA_Reset = CGRA_Reset;
  assign rega__CGRA_Reset = CGRA_Reset;
  assign regb__CGRA_Reset = CGRA_Reset;
  assign ALUMuxAconfig__CGRA_Reset = CGRA_Reset;
  assign ALUMuxBconfig__CGRA_Reset = CGRA_Reset;
  assign ALUconfig__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux5config__CGRA_Reset = CGRA_Reset;
  assign Mux6config__CGRA_Reset = CGRA_Reset;
  assign RESConfig__CGRA_Reset = CGRA_Reset;
  assign Reg0config__CGRA_Reset = CGRA_Reset;
  assign Reg1config__CGRA_Reset = CGRA_Reset;
  assign Reg2config__CGRA_Reset = CGRA_Reset;
  assign Reg3config__CGRA_Reset = CGRA_Reset;
  assign Reg5config__CGRA_Reset = CGRA_Reset;
  assign Reg6config__CGRA_Reset = CGRA_Reset;
  assign RegAConfig__CGRA_Reset = CGRA_Reset;
  assign RegBConfig__CGRA_Reset = CGRA_Reset;
  assign RES__CGRA_Enable = CGRA_Enable;
  assign const__CGRA_Enable = CGRA_Enable;
  assign crossbar__CGRA_Enable = CGRA_Enable;
  assign reg0__CGRA_Enable = CGRA_Enable;
  assign reg1__CGRA_Enable = CGRA_Enable;
  assign reg2__CGRA_Enable = CGRA_Enable;
  assign reg3__CGRA_Enable = CGRA_Enable;
  assign reg5__CGRA_Enable = CGRA_Enable;
  assign reg6__CGRA_Enable = CGRA_Enable;
  assign rega__CGRA_Enable = CGRA_Enable;
  assign regb__CGRA_Enable = CGRA_Enable;
  assign ALUMuxAconfig__CGRA_Enable = CGRA_Enable;
  assign ALUMuxBconfig__CGRA_Enable = CGRA_Enable;
  assign ALUconfig__CGRA_Enable = CGRA_Enable;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux5config__CGRA_Enable = CGRA_Enable;
  assign Mux6config__CGRA_Enable = CGRA_Enable;
  assign RESConfig__CGRA_Enable = CGRA_Enable;
  assign Reg0config__CGRA_Enable = CGRA_Enable;
  assign Reg1config__CGRA_Enable = CGRA_Enable;
  assign Reg2config__CGRA_Enable = CGRA_Enable;
  assign Reg3config__CGRA_Enable = CGRA_Enable;
  assign Reg5config__CGRA_Enable = CGRA_Enable;
  assign Reg6config__CGRA_Enable = CGRA_Enable;
  assign RegAConfig__CGRA_Enable = CGRA_Enable;
  assign RegBConfig__CGRA_Enable = CGRA_Enable;
  assign const__Config_Clock = Config_Clock;
  assign const__Config_Reset = Config_Reset;
  assign crossbar__Config_Clock = Config_Clock;
  assign ALUMuxAconfig__Config_Clock = Config_Clock;
  assign ALUMuxBconfig__Config_Clock = Config_Clock;
  assign ALUconfig__Config_Clock = Config_Clock;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux5config__Config_Clock = Config_Clock;
  assign Mux6config__Config_Clock = Config_Clock;
  assign RESConfig__Config_Clock = Config_Clock;
  assign Reg0config__Config_Clock = Config_Clock;
  assign Reg1config__Config_Clock = Config_Clock;
  assign Reg2config__Config_Clock = Config_Clock;
  assign Reg3config__Config_Clock = Config_Clock;
  assign Reg5config__Config_Clock = Config_Clock;
  assign Reg6config__Config_Clock = Config_Clock;
  assign RegAConfig__Config_Clock = Config_Clock;
  assign RegBConfig__Config_Clock = Config_Clock;
  assign crossbar__Config_Reset = Config_Reset;
  assign ALUMuxAconfig__Config_Reset = Config_Reset;
  assign ALUMuxBconfig__Config_Reset = Config_Reset;
  assign ALUconfig__Config_Reset = Config_Reset;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux5config__Config_Reset = Config_Reset;
  assign Mux6config__Config_Reset = Config_Reset;
  assign RESConfig__Config_Reset = Config_Reset;
  assign Reg0config__Config_Reset = Config_Reset;
  assign Reg1config__Config_Reset = Config_Reset;
  assign Reg2config__Config_Reset = Config_Reset;
  assign Reg3config__Config_Reset = Config_Reset;
  assign Reg5config__Config_Reset = Config_Reset;
  assign Reg6config__Config_Reset = Config_Reset;
  assign RegAConfig__Config_Reset = Config_Reset;
  assign RegBConfig__Config_Reset = Config_Reset;
  assign crossbar__in6[31:0] = ALU__out[31:0];
  assign RES__in[31:0] = ALU__out[31:0];
  assign out0[31:0] = crossbar__out0[31:0];
  assign out1[31:0] = crossbar__out1[31:0];
  assign out2[31:0] = crossbar__out2[31:0];
  assign out3[31:0] = crossbar__out3[31:0];
  assign out5[31:0] = crossbar__out4[31:0];
  assign out6[31:0] = crossbar__out5[31:0];
  assign rega__in[31:0] = crossbar__out6[31:0];
  assign regb__in[31:0] = crossbar__out7[31:0];
  assign crossbar__in7[31:0] = RES__out[31:0];
  assign alu_muxa__in0[31:0] = rega__out[31:0];
  assign alu_muxb__in0[31:0] = regb__out[31:0];
  assign alu_muxa__in1[31:0] = const__out[31:0];
  assign alu_muxb__in1[31:0] = const__out[31:0];
  assign ALU__in_a[31:0] = alu_muxa__out[31:0];
  assign ALU__in_b[31:0] = alu_muxb__out[31:0];
  assign mux_0__in1[31:0] = reg0__out[31:0];
  assign crossbar__in0[31:0] = mux_0__out[31:0];
  assign mux_1__in1[31:0] = reg1__out[31:0];
  assign crossbar__in1[31:0] = mux_1__out[31:0];
  assign mux_2__in1[31:0] = reg2__out[31:0];
  assign crossbar__in2[31:0] = mux_2__out[31:0];
  assign mux_3__in1[31:0] = reg3__out[31:0];
  assign crossbar__in3[31:0] = mux_3__out[31:0];
  assign mux_5__in1[31:0] = reg5__out[31:0];
  assign crossbar__in4[31:0] = mux_5__out[31:0];
  assign mux_6__in1[31:0] = reg6__out[31:0];
  assign crossbar__in5[31:0] = mux_6__out[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign reg0__in[31:0] = in0[31:0];
  assign mux_1__in0[31:0] = in1[31:0];
  assign reg1__in[31:0] = in1[31:0];
  assign mux_2__in0[31:0] = in2[31:0];
  assign reg2__in[31:0] = in2[31:0];
  assign mux_3__in0[31:0] = in3[31:0];
  assign reg3__in[31:0] = in3[31:0];
  assign mux_5__in0[31:0] = in5[31:0];
  assign reg5__in[31:0] = in5[31:0];
  assign mux_6__in0[31:0] = in6[31:0];
  assign reg6__in[31:0] = in6[31:0];
  assign alu_muxa__select[0:0] = ALUMuxAconfig__select[0:0];
  assign alu_muxb__select[0:0] = ALUMuxBconfig__select[0:0];
  assign ALU__select[3:0] = ALUconfig__select[3:0];
  assign mux_0__select[0:0] = Mux0config__select[0:0];
  assign mux_1__select[0:0] = Mux1config__select[0:0];
  assign mux_2__select[0:0] = Mux2config__select[0:0];
  assign mux_3__select[0:0] = Mux3config__select[0:0];
  assign mux_5__select[0:0] = Mux5config__select[0:0];
  assign mux_6__select[0:0] = Mux6config__select[0:0];
  assign RES__enable[0:0] = RESConfig__select[0:0];
  assign reg0__enable[0:0] = Reg0config__select[0:0];
  assign reg1__enable[0:0] = Reg1config__select[0:0];
  assign reg2__enable[0:0] = Reg2config__select[0:0];
  assign reg3__enable[0:0] = Reg3config__select[0:0];
  assign reg5__enable[0:0] = Reg5config__select[0:0];
  assign reg6__enable[0:0] = Reg6config__select[0:0];
  assign regb__enable[0:0] = RegAConfig__select[0:0];
  assign rega__enable[0:0] = RegBConfig__select[0:0];
  assign ALUMuxAconfig__ConfigIn = ConfigIn;
  assign ALUMuxBconfig__ConfigIn = ALUMuxAconfig__ConfigOut;
  assign ALUconfig__ConfigIn = ALUMuxBconfig__ConfigOut;
  assign Mux0config__ConfigIn = ALUconfig__ConfigOut;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux5config__ConfigIn = Mux3config__ConfigOut;
  assign Mux6config__ConfigIn = Mux5config__ConfigOut;
  assign RESConfig__ConfigIn = Mux6config__ConfigOut;
  assign Reg0config__ConfigIn = RESConfig__ConfigOut;
  assign Reg1config__ConfigIn = Reg0config__ConfigOut;
  assign Reg2config__ConfigIn = Reg1config__ConfigOut;
  assign Reg3config__ConfigIn = Reg2config__ConfigOut;
  assign Reg5config__ConfigIn = Reg3config__ConfigOut;
  assign Reg6config__ConfigIn = Reg5config__ConfigOut;
  assign RegAConfig__ConfigIn = Reg6config__ConfigOut;
  assign RegBConfig__ConfigIn = RegAConfig__ConfigOut;
  assign const__ConfigIn = RegBConfig__ConfigOut;
  assign crossbar__ConfigIn = const__ConfigOut;
  assign ConfigOut = crossbar__ConfigOut;

endmodule //hycube_no_pred_in111_out111_U4

module hycube_no_pred_in255_out255_U7 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in4,
  input [31:0] in5,
  input [31:0] in6,
  input [31:0] in7,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out4,
  output [31:0] out5,
  output [31:0] out6,
  output [31:0] out7
);
  //Wire declarations for instance 'ALU' (Module func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12)
  wire  ALU__CGRA_Clock;
  wire  ALU__CGRA_Enable;
  wire  ALU__CGRA_Reset;
  wire [31:0] ALU__in_a;
  wire [31:0] ALU__in_b;
  wire [31:0] ALU__out;
  wire [3:0] ALU__select;
  func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12 ALU(
    .CGRA_Clock(ALU__CGRA_Clock),
    .CGRA_Enable(ALU__CGRA_Enable),
    .CGRA_Reset(ALU__CGRA_Reset),
    .in_a(ALU__in_a),
    .in_b(ALU__in_b),
    .out(ALU__out),
    .select(ALU__select)
  );

  //Wire declarations for instance 'ALUMuxAconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxAconfig__CGRA_Clock;
  wire  ALUMuxAconfig__CGRA_Enable;
  wire  ALUMuxAconfig__CGRA_Reset;
  wire  ALUMuxAconfig__ConfigIn;
  wire  ALUMuxAconfig__ConfigOut;
  wire  ALUMuxAconfig__Config_Clock;
  wire  ALUMuxAconfig__Config_Reset;
  wire [0:0] ALUMuxAconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxAconfig(
    .CGRA_Clock(ALUMuxAconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxAconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxAconfig__CGRA_Reset),
    .ConfigIn(ALUMuxAconfig__ConfigIn),
    .ConfigOut(ALUMuxAconfig__ConfigOut),
    .Config_Clock(ALUMuxAconfig__Config_Clock),
    .Config_Reset(ALUMuxAconfig__Config_Reset),
    .select(ALUMuxAconfig__select)
  );

  //Wire declarations for instance 'ALUMuxBconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxBconfig__CGRA_Clock;
  wire  ALUMuxBconfig__CGRA_Enable;
  wire  ALUMuxBconfig__CGRA_Reset;
  wire  ALUMuxBconfig__ConfigIn;
  wire  ALUMuxBconfig__ConfigOut;
  wire  ALUMuxBconfig__Config_Clock;
  wire  ALUMuxBconfig__Config_Reset;
  wire [0:0] ALUMuxBconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxBconfig(
    .CGRA_Clock(ALUMuxBconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxBconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxBconfig__CGRA_Reset),
    .ConfigIn(ALUMuxBconfig__ConfigIn),
    .ConfigOut(ALUMuxBconfig__ConfigOut),
    .Config_Clock(ALUMuxBconfig__Config_Clock),
    .Config_Reset(ALUMuxBconfig__Config_Reset),
    .select(ALUMuxBconfig__select)
  );

  //Wire declarations for instance 'ALUconfig' (Module cgrame_configcell_context1_size4)
  wire  ALUconfig__CGRA_Clock;
  wire  ALUconfig__CGRA_Enable;
  wire  ALUconfig__CGRA_Reset;
  wire  ALUconfig__ConfigIn;
  wire  ALUconfig__ConfigOut;
  wire  ALUconfig__Config_Clock;
  wire  ALUconfig__Config_Reset;
  wire [3:0] ALUconfig__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) ALUconfig(
    .CGRA_Clock(ALUconfig__CGRA_Clock),
    .CGRA_Enable(ALUconfig__CGRA_Enable),
    .CGRA_Reset(ALUconfig__CGRA_Reset),
    .ConfigIn(ALUconfig__ConfigIn),
    .ConfigOut(ALUconfig__ConfigOut),
    .Config_Clock(ALUconfig__Config_Clock),
    .Config_Reset(ALUconfig__Config_Reset),
    .select(ALUconfig__select)
  );

  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size1)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [0:0] Mux0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size1)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [0:0] Mux1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size1)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [0:0] Mux2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size1)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [0:0] Mux3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux4config' (Module cgrame_configcell_context1_size1)
  wire  Mux4config__CGRA_Clock;
  wire  Mux4config__CGRA_Enable;
  wire  Mux4config__CGRA_Reset;
  wire  Mux4config__ConfigIn;
  wire  Mux4config__ConfigOut;
  wire  Mux4config__Config_Clock;
  wire  Mux4config__Config_Reset;
  wire [0:0] Mux4config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux4config(
    .CGRA_Clock(Mux4config__CGRA_Clock),
    .CGRA_Enable(Mux4config__CGRA_Enable),
    .CGRA_Reset(Mux4config__CGRA_Reset),
    .ConfigIn(Mux4config__ConfigIn),
    .ConfigOut(Mux4config__ConfigOut),
    .Config_Clock(Mux4config__Config_Clock),
    .Config_Reset(Mux4config__Config_Reset),
    .select(Mux4config__select)
  );

  //Wire declarations for instance 'Mux5config' (Module cgrame_configcell_context1_size1)
  wire  Mux5config__CGRA_Clock;
  wire  Mux5config__CGRA_Enable;
  wire  Mux5config__CGRA_Reset;
  wire  Mux5config__ConfigIn;
  wire  Mux5config__ConfigOut;
  wire  Mux5config__Config_Clock;
  wire  Mux5config__Config_Reset;
  wire [0:0] Mux5config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux5config(
    .CGRA_Clock(Mux5config__CGRA_Clock),
    .CGRA_Enable(Mux5config__CGRA_Enable),
    .CGRA_Reset(Mux5config__CGRA_Reset),
    .ConfigIn(Mux5config__ConfigIn),
    .ConfigOut(Mux5config__ConfigOut),
    .Config_Clock(Mux5config__Config_Clock),
    .Config_Reset(Mux5config__Config_Reset),
    .select(Mux5config__select)
  );

  //Wire declarations for instance 'Mux6config' (Module cgrame_configcell_context1_size1)
  wire  Mux6config__CGRA_Clock;
  wire  Mux6config__CGRA_Enable;
  wire  Mux6config__CGRA_Reset;
  wire  Mux6config__ConfigIn;
  wire  Mux6config__ConfigOut;
  wire  Mux6config__Config_Clock;
  wire  Mux6config__Config_Reset;
  wire [0:0] Mux6config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux6config(
    .CGRA_Clock(Mux6config__CGRA_Clock),
    .CGRA_Enable(Mux6config__CGRA_Enable),
    .CGRA_Reset(Mux6config__CGRA_Reset),
    .ConfigIn(Mux6config__ConfigIn),
    .ConfigOut(Mux6config__ConfigOut),
    .Config_Clock(Mux6config__Config_Clock),
    .Config_Reset(Mux6config__Config_Reset),
    .select(Mux6config__select)
  );

  //Wire declarations for instance 'Mux7config' (Module cgrame_configcell_context1_size1)
  wire  Mux7config__CGRA_Clock;
  wire  Mux7config__CGRA_Enable;
  wire  Mux7config__CGRA_Reset;
  wire  Mux7config__ConfigIn;
  wire  Mux7config__ConfigOut;
  wire  Mux7config__Config_Clock;
  wire  Mux7config__Config_Reset;
  wire [0:0] Mux7config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux7config(
    .CGRA_Clock(Mux7config__CGRA_Clock),
    .CGRA_Enable(Mux7config__CGRA_Enable),
    .CGRA_Reset(Mux7config__CGRA_Reset),
    .ConfigIn(Mux7config__ConfigIn),
    .ConfigOut(Mux7config__ConfigOut),
    .Config_Clock(Mux7config__Config_Clock),
    .Config_Reset(Mux7config__Config_Reset),
    .select(Mux7config__select)
  );

  //Wire declarations for instance 'RES' (Module cgrame_register_32b)
  wire  RES__CGRA_Clock;
  wire  RES__CGRA_Enable;
  wire  RES__CGRA_Reset;
  wire [0:0] RES__enable;
  wire [31:0] RES__in;
  wire [31:0] RES__out;
  cgrame_register_32b #(.contexts(1),.size(32)) RES(
    .CGRA_Clock(RES__CGRA_Clock),
    .CGRA_Enable(RES__CGRA_Enable),
    .CGRA_Reset(RES__CGRA_Reset),
    .enable(RES__enable),
    .in(RES__in),
    .out(RES__out)
  );

  //Wire declarations for instance 'RESConfig' (Module cgrame_configcell_context1_size1)
  wire  RESConfig__CGRA_Clock;
  wire  RESConfig__CGRA_Enable;
  wire  RESConfig__CGRA_Reset;
  wire  RESConfig__ConfigIn;
  wire  RESConfig__ConfigOut;
  wire  RESConfig__Config_Clock;
  wire  RESConfig__Config_Reset;
  wire [0:0] RESConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RESConfig(
    .CGRA_Clock(RESConfig__CGRA_Clock),
    .CGRA_Enable(RESConfig__CGRA_Enable),
    .CGRA_Reset(RESConfig__CGRA_Reset),
    .ConfigIn(RESConfig__ConfigIn),
    .ConfigOut(RESConfig__ConfigOut),
    .Config_Clock(RESConfig__Config_Clock),
    .Config_Reset(RESConfig__Config_Reset),
    .select(RESConfig__select)
  );

  //Wire declarations for instance 'Reg0config' (Module cgrame_configcell_context1_size1)
  wire  Reg0config__CGRA_Clock;
  wire  Reg0config__CGRA_Enable;
  wire  Reg0config__CGRA_Reset;
  wire  Reg0config__ConfigIn;
  wire  Reg0config__ConfigOut;
  wire  Reg0config__Config_Clock;
  wire  Reg0config__Config_Reset;
  wire [0:0] Reg0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg0config(
    .CGRA_Clock(Reg0config__CGRA_Clock),
    .CGRA_Enable(Reg0config__CGRA_Enable),
    .CGRA_Reset(Reg0config__CGRA_Reset),
    .ConfigIn(Reg0config__ConfigIn),
    .ConfigOut(Reg0config__ConfigOut),
    .Config_Clock(Reg0config__Config_Clock),
    .Config_Reset(Reg0config__Config_Reset),
    .select(Reg0config__select)
  );

  //Wire declarations for instance 'Reg1config' (Module cgrame_configcell_context1_size1)
  wire  Reg1config__CGRA_Clock;
  wire  Reg1config__CGRA_Enable;
  wire  Reg1config__CGRA_Reset;
  wire  Reg1config__ConfigIn;
  wire  Reg1config__ConfigOut;
  wire  Reg1config__Config_Clock;
  wire  Reg1config__Config_Reset;
  wire [0:0] Reg1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg1config(
    .CGRA_Clock(Reg1config__CGRA_Clock),
    .CGRA_Enable(Reg1config__CGRA_Enable),
    .CGRA_Reset(Reg1config__CGRA_Reset),
    .ConfigIn(Reg1config__ConfigIn),
    .ConfigOut(Reg1config__ConfigOut),
    .Config_Clock(Reg1config__Config_Clock),
    .Config_Reset(Reg1config__Config_Reset),
    .select(Reg1config__select)
  );

  //Wire declarations for instance 'Reg2config' (Module cgrame_configcell_context1_size1)
  wire  Reg2config__CGRA_Clock;
  wire  Reg2config__CGRA_Enable;
  wire  Reg2config__CGRA_Reset;
  wire  Reg2config__ConfigIn;
  wire  Reg2config__ConfigOut;
  wire  Reg2config__Config_Clock;
  wire  Reg2config__Config_Reset;
  wire [0:0] Reg2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg2config(
    .CGRA_Clock(Reg2config__CGRA_Clock),
    .CGRA_Enable(Reg2config__CGRA_Enable),
    .CGRA_Reset(Reg2config__CGRA_Reset),
    .ConfigIn(Reg2config__ConfigIn),
    .ConfigOut(Reg2config__ConfigOut),
    .Config_Clock(Reg2config__Config_Clock),
    .Config_Reset(Reg2config__Config_Reset),
    .select(Reg2config__select)
  );

  //Wire declarations for instance 'Reg3config' (Module cgrame_configcell_context1_size1)
  wire  Reg3config__CGRA_Clock;
  wire  Reg3config__CGRA_Enable;
  wire  Reg3config__CGRA_Reset;
  wire  Reg3config__ConfigIn;
  wire  Reg3config__ConfigOut;
  wire  Reg3config__Config_Clock;
  wire  Reg3config__Config_Reset;
  wire [0:0] Reg3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg3config(
    .CGRA_Clock(Reg3config__CGRA_Clock),
    .CGRA_Enable(Reg3config__CGRA_Enable),
    .CGRA_Reset(Reg3config__CGRA_Reset),
    .ConfigIn(Reg3config__ConfigIn),
    .ConfigOut(Reg3config__ConfigOut),
    .Config_Clock(Reg3config__Config_Clock),
    .Config_Reset(Reg3config__Config_Reset),
    .select(Reg3config__select)
  );

  //Wire declarations for instance 'Reg4config' (Module cgrame_configcell_context1_size1)
  wire  Reg4config__CGRA_Clock;
  wire  Reg4config__CGRA_Enable;
  wire  Reg4config__CGRA_Reset;
  wire  Reg4config__ConfigIn;
  wire  Reg4config__ConfigOut;
  wire  Reg4config__Config_Clock;
  wire  Reg4config__Config_Reset;
  wire [0:0] Reg4config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg4config(
    .CGRA_Clock(Reg4config__CGRA_Clock),
    .CGRA_Enable(Reg4config__CGRA_Enable),
    .CGRA_Reset(Reg4config__CGRA_Reset),
    .ConfigIn(Reg4config__ConfigIn),
    .ConfigOut(Reg4config__ConfigOut),
    .Config_Clock(Reg4config__Config_Clock),
    .Config_Reset(Reg4config__Config_Reset),
    .select(Reg4config__select)
  );

  //Wire declarations for instance 'Reg5config' (Module cgrame_configcell_context1_size1)
  wire  Reg5config__CGRA_Clock;
  wire  Reg5config__CGRA_Enable;
  wire  Reg5config__CGRA_Reset;
  wire  Reg5config__ConfigIn;
  wire  Reg5config__ConfigOut;
  wire  Reg5config__Config_Clock;
  wire  Reg5config__Config_Reset;
  wire [0:0] Reg5config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg5config(
    .CGRA_Clock(Reg5config__CGRA_Clock),
    .CGRA_Enable(Reg5config__CGRA_Enable),
    .CGRA_Reset(Reg5config__CGRA_Reset),
    .ConfigIn(Reg5config__ConfigIn),
    .ConfigOut(Reg5config__ConfigOut),
    .Config_Clock(Reg5config__Config_Clock),
    .Config_Reset(Reg5config__Config_Reset),
    .select(Reg5config__select)
  );

  //Wire declarations for instance 'Reg6config' (Module cgrame_configcell_context1_size1)
  wire  Reg6config__CGRA_Clock;
  wire  Reg6config__CGRA_Enable;
  wire  Reg6config__CGRA_Reset;
  wire  Reg6config__ConfigIn;
  wire  Reg6config__ConfigOut;
  wire  Reg6config__Config_Clock;
  wire  Reg6config__Config_Reset;
  wire [0:0] Reg6config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg6config(
    .CGRA_Clock(Reg6config__CGRA_Clock),
    .CGRA_Enable(Reg6config__CGRA_Enable),
    .CGRA_Reset(Reg6config__CGRA_Reset),
    .ConfigIn(Reg6config__ConfigIn),
    .ConfigOut(Reg6config__ConfigOut),
    .Config_Clock(Reg6config__Config_Clock),
    .Config_Reset(Reg6config__Config_Reset),
    .select(Reg6config__select)
  );

  //Wire declarations for instance 'Reg7config' (Module cgrame_configcell_context1_size1)
  wire  Reg7config__CGRA_Clock;
  wire  Reg7config__CGRA_Enable;
  wire  Reg7config__CGRA_Reset;
  wire  Reg7config__ConfigIn;
  wire  Reg7config__ConfigOut;
  wire  Reg7config__Config_Clock;
  wire  Reg7config__Config_Reset;
  wire [0:0] Reg7config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg7config(
    .CGRA_Clock(Reg7config__CGRA_Clock),
    .CGRA_Enable(Reg7config__CGRA_Enable),
    .CGRA_Reset(Reg7config__CGRA_Reset),
    .ConfigIn(Reg7config__ConfigIn),
    .ConfigOut(Reg7config__ConfigOut),
    .Config_Clock(Reg7config__Config_Clock),
    .Config_Reset(Reg7config__Config_Reset),
    .select(Reg7config__select)
  );

  //Wire declarations for instance 'RegAConfig' (Module cgrame_configcell_context1_size1)
  wire  RegAConfig__CGRA_Clock;
  wire  RegAConfig__CGRA_Enable;
  wire  RegAConfig__CGRA_Reset;
  wire  RegAConfig__ConfigIn;
  wire  RegAConfig__ConfigOut;
  wire  RegAConfig__Config_Clock;
  wire  RegAConfig__Config_Reset;
  wire [0:0] RegAConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegAConfig(
    .CGRA_Clock(RegAConfig__CGRA_Clock),
    .CGRA_Enable(RegAConfig__CGRA_Enable),
    .CGRA_Reset(RegAConfig__CGRA_Reset),
    .ConfigIn(RegAConfig__ConfigIn),
    .ConfigOut(RegAConfig__ConfigOut),
    .Config_Clock(RegAConfig__Config_Clock),
    .Config_Reset(RegAConfig__Config_Reset),
    .select(RegAConfig__select)
  );

  //Wire declarations for instance 'RegBConfig' (Module cgrame_configcell_context1_size1)
  wire  RegBConfig__CGRA_Clock;
  wire  RegBConfig__CGRA_Enable;
  wire  RegBConfig__CGRA_Reset;
  wire  RegBConfig__ConfigIn;
  wire  RegBConfig__ConfigOut;
  wire  RegBConfig__Config_Clock;
  wire  RegBConfig__Config_Reset;
  wire [0:0] RegBConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegBConfig(
    .CGRA_Clock(RegBConfig__CGRA_Clock),
    .CGRA_Enable(RegBConfig__CGRA_Enable),
    .CGRA_Reset(RegBConfig__CGRA_Reset),
    .ConfigIn(RegBConfig__ConfigIn),
    .ConfigOut(RegBConfig__ConfigOut),
    .Config_Clock(RegBConfig__Config_Clock),
    .Config_Reset(RegBConfig__Config_Reset),
    .select(RegBConfig__select)
  );

  //Wire declarations for instance 'alu_muxa' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxa__in0;
  wire [31:0] alu_muxa__in1;
  wire [31:0] alu_muxa__out;
  wire [0:0] alu_muxa__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxa(
    .in0(alu_muxa__in0),
    .in1(alu_muxa__in1),
    .out(alu_muxa__out),
    .select(alu_muxa__select)
  );

  //Wire declarations for instance 'alu_muxb' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxb__in0;
  wire [31:0] alu_muxb__in1;
  wire [31:0] alu_muxb__out;
  wire [0:0] alu_muxb__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxb(
    .in0(alu_muxb__in0),
    .in1(alu_muxb__in1),
    .out(alu_muxb__out),
    .select(alu_muxb__select)
  );

  //Wire declarations for instance 'const' (Module const_32b_U15)
  wire  const__CGRA_Clock;
  wire  const__CGRA_Enable;
  wire  const__CGRA_Reset;
  wire  const__ConfigIn;
  wire  const__ConfigOut;
  wire  const__Config_Clock;
  wire  const__Config_Reset;
  wire [31:0] const__out;
  const_32b_U15 const(
    .CGRA_Clock(const__CGRA_Clock),
    .CGRA_Enable(const__CGRA_Enable),
    .CGRA_Reset(const__CGRA_Reset),
    .ConfigIn(const__ConfigIn),
    .ConfigOut(const__ConfigOut),
    .Config_Clock(const__Config_Clock),
    .Config_Reset(const__Config_Reset),
    .out(const__out)
  );

  //Wire declarations for instance 'crossbar' (Module crossbar_10in_10out_U20)
  wire  crossbar__CGRA_Clock;
  wire  crossbar__CGRA_Enable;
  wire  crossbar__CGRA_Reset;
  wire  crossbar__ConfigIn;
  wire  crossbar__ConfigOut;
  wire  crossbar__Config_Clock;
  wire  crossbar__Config_Reset;
  wire [31:0] crossbar__in0;
  wire [31:0] crossbar__in1;
  wire [31:0] crossbar__in2;
  wire [31:0] crossbar__in3;
  wire [31:0] crossbar__in4;
  wire [31:0] crossbar__in5;
  wire [31:0] crossbar__in6;
  wire [31:0] crossbar__in7;
  wire [31:0] crossbar__in8;
  wire [31:0] crossbar__in9;
  wire [31:0] crossbar__out0;
  wire [31:0] crossbar__out1;
  wire [31:0] crossbar__out2;
  wire [31:0] crossbar__out3;
  wire [31:0] crossbar__out4;
  wire [31:0] crossbar__out5;
  wire [31:0] crossbar__out6;
  wire [31:0] crossbar__out7;
  wire [31:0] crossbar__out8;
  wire [31:0] crossbar__out9;
  crossbar_10in_10out_U20 crossbar(
    .CGRA_Clock(crossbar__CGRA_Clock),
    .CGRA_Enable(crossbar__CGRA_Enable),
    .CGRA_Reset(crossbar__CGRA_Reset),
    .ConfigIn(crossbar__ConfigIn),
    .ConfigOut(crossbar__ConfigOut),
    .Config_Clock(crossbar__Config_Clock),
    .Config_Reset(crossbar__Config_Reset),
    .in0(crossbar__in0),
    .in1(crossbar__in1),
    .in2(crossbar__in2),
    .in3(crossbar__in3),
    .in4(crossbar__in4),
    .in5(crossbar__in5),
    .in6(crossbar__in6),
    .in7(crossbar__in7),
    .in8(crossbar__in8),
    .in9(crossbar__in9),
    .out0(crossbar__out0),
    .out1(crossbar__out1),
    .out2(crossbar__out2),
    .out3(crossbar__out3),
    .out4(crossbar__out4),
    .out5(crossbar__out5),
    .out6(crossbar__out6),
    .out7(crossbar__out7),
    .out8(crossbar__out8),
    .out9(crossbar__out9)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__out;
  wire [0:0] mux_0__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__out;
  wire [0:0] mux_1__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__out;
  wire [0:0] mux_2__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__out;
  wire [0:0] mux_3__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_4' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_4__in0;
  wire [31:0] mux_4__in1;
  wire [31:0] mux_4__out;
  wire [0:0] mux_4__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_4(
    .in0(mux_4__in0),
    .in1(mux_4__in1),
    .out(mux_4__out),
    .select(mux_4__select)
  );

  //Wire declarations for instance 'mux_5' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_5__in0;
  wire [31:0] mux_5__in1;
  wire [31:0] mux_5__out;
  wire [0:0] mux_5__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_5(
    .in0(mux_5__in0),
    .in1(mux_5__in1),
    .out(mux_5__out),
    .select(mux_5__select)
  );

  //Wire declarations for instance 'mux_6' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_6__in0;
  wire [31:0] mux_6__in1;
  wire [31:0] mux_6__out;
  wire [0:0] mux_6__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_6(
    .in0(mux_6__in0),
    .in1(mux_6__in1),
    .out(mux_6__out),
    .select(mux_6__select)
  );

  //Wire declarations for instance 'mux_7' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_7__in0;
  wire [31:0] mux_7__in1;
  wire [31:0] mux_7__out;
  wire [0:0] mux_7__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_7(
    .in0(mux_7__in0),
    .in1(mux_7__in1),
    .out(mux_7__out),
    .select(mux_7__select)
  );

  //Wire declarations for instance 'reg0' (Module cgrame_register_32b)
  wire  reg0__CGRA_Clock;
  wire  reg0__CGRA_Enable;
  wire  reg0__CGRA_Reset;
  wire [0:0] reg0__enable;
  wire [31:0] reg0__in;
  wire [31:0] reg0__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg0(
    .CGRA_Clock(reg0__CGRA_Clock),
    .CGRA_Enable(reg0__CGRA_Enable),
    .CGRA_Reset(reg0__CGRA_Reset),
    .enable(reg0__enable),
    .in(reg0__in),
    .out(reg0__out)
  );

  //Wire declarations for instance 'reg1' (Module cgrame_register_32b)
  wire  reg1__CGRA_Clock;
  wire  reg1__CGRA_Enable;
  wire  reg1__CGRA_Reset;
  wire [0:0] reg1__enable;
  wire [31:0] reg1__in;
  wire [31:0] reg1__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg1(
    .CGRA_Clock(reg1__CGRA_Clock),
    .CGRA_Enable(reg1__CGRA_Enable),
    .CGRA_Reset(reg1__CGRA_Reset),
    .enable(reg1__enable),
    .in(reg1__in),
    .out(reg1__out)
  );

  //Wire declarations for instance 'reg2' (Module cgrame_register_32b)
  wire  reg2__CGRA_Clock;
  wire  reg2__CGRA_Enable;
  wire  reg2__CGRA_Reset;
  wire [0:0] reg2__enable;
  wire [31:0] reg2__in;
  wire [31:0] reg2__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg2(
    .CGRA_Clock(reg2__CGRA_Clock),
    .CGRA_Enable(reg2__CGRA_Enable),
    .CGRA_Reset(reg2__CGRA_Reset),
    .enable(reg2__enable),
    .in(reg2__in),
    .out(reg2__out)
  );

  //Wire declarations for instance 'reg3' (Module cgrame_register_32b)
  wire  reg3__CGRA_Clock;
  wire  reg3__CGRA_Enable;
  wire  reg3__CGRA_Reset;
  wire [0:0] reg3__enable;
  wire [31:0] reg3__in;
  wire [31:0] reg3__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg3(
    .CGRA_Clock(reg3__CGRA_Clock),
    .CGRA_Enable(reg3__CGRA_Enable),
    .CGRA_Reset(reg3__CGRA_Reset),
    .enable(reg3__enable),
    .in(reg3__in),
    .out(reg3__out)
  );

  //Wire declarations for instance 'reg4' (Module cgrame_register_32b)
  wire  reg4__CGRA_Clock;
  wire  reg4__CGRA_Enable;
  wire  reg4__CGRA_Reset;
  wire [0:0] reg4__enable;
  wire [31:0] reg4__in;
  wire [31:0] reg4__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg4(
    .CGRA_Clock(reg4__CGRA_Clock),
    .CGRA_Enable(reg4__CGRA_Enable),
    .CGRA_Reset(reg4__CGRA_Reset),
    .enable(reg4__enable),
    .in(reg4__in),
    .out(reg4__out)
  );

  //Wire declarations for instance 'reg5' (Module cgrame_register_32b)
  wire  reg5__CGRA_Clock;
  wire  reg5__CGRA_Enable;
  wire  reg5__CGRA_Reset;
  wire [0:0] reg5__enable;
  wire [31:0] reg5__in;
  wire [31:0] reg5__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg5(
    .CGRA_Clock(reg5__CGRA_Clock),
    .CGRA_Enable(reg5__CGRA_Enable),
    .CGRA_Reset(reg5__CGRA_Reset),
    .enable(reg5__enable),
    .in(reg5__in),
    .out(reg5__out)
  );

  //Wire declarations for instance 'reg6' (Module cgrame_register_32b)
  wire  reg6__CGRA_Clock;
  wire  reg6__CGRA_Enable;
  wire  reg6__CGRA_Reset;
  wire [0:0] reg6__enable;
  wire [31:0] reg6__in;
  wire [31:0] reg6__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg6(
    .CGRA_Clock(reg6__CGRA_Clock),
    .CGRA_Enable(reg6__CGRA_Enable),
    .CGRA_Reset(reg6__CGRA_Reset),
    .enable(reg6__enable),
    .in(reg6__in),
    .out(reg6__out)
  );

  //Wire declarations for instance 'reg7' (Module cgrame_register_32b)
  wire  reg7__CGRA_Clock;
  wire  reg7__CGRA_Enable;
  wire  reg7__CGRA_Reset;
  wire [0:0] reg7__enable;
  wire [31:0] reg7__in;
  wire [31:0] reg7__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg7(
    .CGRA_Clock(reg7__CGRA_Clock),
    .CGRA_Enable(reg7__CGRA_Enable),
    .CGRA_Reset(reg7__CGRA_Reset),
    .enable(reg7__enable),
    .in(reg7__in),
    .out(reg7__out)
  );

  //Wire declarations for instance 'rega' (Module cgrame_register_32b)
  wire  rega__CGRA_Clock;
  wire  rega__CGRA_Enable;
  wire  rega__CGRA_Reset;
  wire [0:0] rega__enable;
  wire [31:0] rega__in;
  wire [31:0] rega__out;
  cgrame_register_32b #(.contexts(1),.size(32)) rega(
    .CGRA_Clock(rega__CGRA_Clock),
    .CGRA_Enable(rega__CGRA_Enable),
    .CGRA_Reset(rega__CGRA_Reset),
    .enable(rega__enable),
    .in(rega__in),
    .out(rega__out)
  );

  //Wire declarations for instance 'regb' (Module cgrame_register_32b)
  wire  regb__CGRA_Clock;
  wire  regb__CGRA_Enable;
  wire  regb__CGRA_Reset;
  wire [0:0] regb__enable;
  wire [31:0] regb__in;
  wire [31:0] regb__out;
  cgrame_register_32b #(.contexts(1),.size(32)) regb(
    .CGRA_Clock(regb__CGRA_Clock),
    .CGRA_Enable(regb__CGRA_Enable),
    .CGRA_Reset(regb__CGRA_Reset),
    .enable(regb__enable),
    .in(regb__in),
    .out(regb__out)
  );

  //All the connections
  assign ALU__CGRA_Reset = CGRA_Reset;
  assign ALU__CGRA_Clock = CGRA_Clock;
  assign RES__CGRA_Clock = CGRA_Clock;
  assign const__CGRA_Clock = CGRA_Clock;
  assign crossbar__CGRA_Clock = CGRA_Clock;
  assign reg0__CGRA_Clock = CGRA_Clock;
  assign reg1__CGRA_Clock = CGRA_Clock;
  assign reg2__CGRA_Clock = CGRA_Clock;
  assign reg3__CGRA_Clock = CGRA_Clock;
  assign reg4__CGRA_Clock = CGRA_Clock;
  assign reg5__CGRA_Clock = CGRA_Clock;
  assign reg6__CGRA_Clock = CGRA_Clock;
  assign reg7__CGRA_Clock = CGRA_Clock;
  assign rega__CGRA_Clock = CGRA_Clock;
  assign regb__CGRA_Clock = CGRA_Clock;
  assign ALUMuxAconfig__CGRA_Clock = CGRA_Clock;
  assign ALUMuxBconfig__CGRA_Clock = CGRA_Clock;
  assign ALUconfig__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux4config__CGRA_Clock = CGRA_Clock;
  assign Mux5config__CGRA_Clock = CGRA_Clock;
  assign Mux6config__CGRA_Clock = CGRA_Clock;
  assign Mux7config__CGRA_Clock = CGRA_Clock;
  assign RESConfig__CGRA_Clock = CGRA_Clock;
  assign Reg0config__CGRA_Clock = CGRA_Clock;
  assign Reg1config__CGRA_Clock = CGRA_Clock;
  assign Reg2config__CGRA_Clock = CGRA_Clock;
  assign Reg3config__CGRA_Clock = CGRA_Clock;
  assign Reg4config__CGRA_Clock = CGRA_Clock;
  assign Reg5config__CGRA_Clock = CGRA_Clock;
  assign Reg6config__CGRA_Clock = CGRA_Clock;
  assign Reg7config__CGRA_Clock = CGRA_Clock;
  assign RegAConfig__CGRA_Clock = CGRA_Clock;
  assign RegBConfig__CGRA_Clock = CGRA_Clock;
  assign ALU__CGRA_Enable = CGRA_Enable;
  assign RES__CGRA_Reset = CGRA_Reset;
  assign const__CGRA_Reset = CGRA_Reset;
  assign crossbar__CGRA_Reset = CGRA_Reset;
  assign reg0__CGRA_Reset = CGRA_Reset;
  assign reg1__CGRA_Reset = CGRA_Reset;
  assign reg2__CGRA_Reset = CGRA_Reset;
  assign reg3__CGRA_Reset = CGRA_Reset;
  assign reg4__CGRA_Reset = CGRA_Reset;
  assign reg5__CGRA_Reset = CGRA_Reset;
  assign reg6__CGRA_Reset = CGRA_Reset;
  assign reg7__CGRA_Reset = CGRA_Reset;
  assign rega__CGRA_Reset = CGRA_Reset;
  assign regb__CGRA_Reset = CGRA_Reset;
  assign ALUMuxAconfig__CGRA_Reset = CGRA_Reset;
  assign ALUMuxBconfig__CGRA_Reset = CGRA_Reset;
  assign ALUconfig__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux4config__CGRA_Reset = CGRA_Reset;
  assign Mux5config__CGRA_Reset = CGRA_Reset;
  assign Mux6config__CGRA_Reset = CGRA_Reset;
  assign Mux7config__CGRA_Reset = CGRA_Reset;
  assign RESConfig__CGRA_Reset = CGRA_Reset;
  assign Reg0config__CGRA_Reset = CGRA_Reset;
  assign Reg1config__CGRA_Reset = CGRA_Reset;
  assign Reg2config__CGRA_Reset = CGRA_Reset;
  assign Reg3config__CGRA_Reset = CGRA_Reset;
  assign Reg4config__CGRA_Reset = CGRA_Reset;
  assign Reg5config__CGRA_Reset = CGRA_Reset;
  assign Reg6config__CGRA_Reset = CGRA_Reset;
  assign Reg7config__CGRA_Reset = CGRA_Reset;
  assign RegAConfig__CGRA_Reset = CGRA_Reset;
  assign RegBConfig__CGRA_Reset = CGRA_Reset;
  assign RES__CGRA_Enable = CGRA_Enable;
  assign const__CGRA_Enable = CGRA_Enable;
  assign crossbar__CGRA_Enable = CGRA_Enable;
  assign reg0__CGRA_Enable = CGRA_Enable;
  assign reg1__CGRA_Enable = CGRA_Enable;
  assign reg2__CGRA_Enable = CGRA_Enable;
  assign reg3__CGRA_Enable = CGRA_Enable;
  assign reg4__CGRA_Enable = CGRA_Enable;
  assign reg5__CGRA_Enable = CGRA_Enable;
  assign reg6__CGRA_Enable = CGRA_Enable;
  assign reg7__CGRA_Enable = CGRA_Enable;
  assign rega__CGRA_Enable = CGRA_Enable;
  assign regb__CGRA_Enable = CGRA_Enable;
  assign ALUMuxAconfig__CGRA_Enable = CGRA_Enable;
  assign ALUMuxBconfig__CGRA_Enable = CGRA_Enable;
  assign ALUconfig__CGRA_Enable = CGRA_Enable;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux4config__CGRA_Enable = CGRA_Enable;
  assign Mux5config__CGRA_Enable = CGRA_Enable;
  assign Mux6config__CGRA_Enable = CGRA_Enable;
  assign Mux7config__CGRA_Enable = CGRA_Enable;
  assign RESConfig__CGRA_Enable = CGRA_Enable;
  assign Reg0config__CGRA_Enable = CGRA_Enable;
  assign Reg1config__CGRA_Enable = CGRA_Enable;
  assign Reg2config__CGRA_Enable = CGRA_Enable;
  assign Reg3config__CGRA_Enable = CGRA_Enable;
  assign Reg4config__CGRA_Enable = CGRA_Enable;
  assign Reg5config__CGRA_Enable = CGRA_Enable;
  assign Reg6config__CGRA_Enable = CGRA_Enable;
  assign Reg7config__CGRA_Enable = CGRA_Enable;
  assign RegAConfig__CGRA_Enable = CGRA_Enable;
  assign RegBConfig__CGRA_Enable = CGRA_Enable;
  assign const__Config_Clock = Config_Clock;
  assign const__Config_Reset = Config_Reset;
  assign crossbar__Config_Clock = Config_Clock;
  assign ALUMuxAconfig__Config_Clock = Config_Clock;
  assign ALUMuxBconfig__Config_Clock = Config_Clock;
  assign ALUconfig__Config_Clock = Config_Clock;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux4config__Config_Clock = Config_Clock;
  assign Mux5config__Config_Clock = Config_Clock;
  assign Mux6config__Config_Clock = Config_Clock;
  assign Mux7config__Config_Clock = Config_Clock;
  assign RESConfig__Config_Clock = Config_Clock;
  assign Reg0config__Config_Clock = Config_Clock;
  assign Reg1config__Config_Clock = Config_Clock;
  assign Reg2config__Config_Clock = Config_Clock;
  assign Reg3config__Config_Clock = Config_Clock;
  assign Reg4config__Config_Clock = Config_Clock;
  assign Reg5config__Config_Clock = Config_Clock;
  assign Reg6config__Config_Clock = Config_Clock;
  assign Reg7config__Config_Clock = Config_Clock;
  assign RegAConfig__Config_Clock = Config_Clock;
  assign RegBConfig__Config_Clock = Config_Clock;
  assign crossbar__Config_Reset = Config_Reset;
  assign ALUMuxAconfig__Config_Reset = Config_Reset;
  assign ALUMuxBconfig__Config_Reset = Config_Reset;
  assign ALUconfig__Config_Reset = Config_Reset;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux4config__Config_Reset = Config_Reset;
  assign Mux5config__Config_Reset = Config_Reset;
  assign Mux6config__Config_Reset = Config_Reset;
  assign Mux7config__Config_Reset = Config_Reset;
  assign RESConfig__Config_Reset = Config_Reset;
  assign Reg0config__Config_Reset = Config_Reset;
  assign Reg1config__Config_Reset = Config_Reset;
  assign Reg2config__Config_Reset = Config_Reset;
  assign Reg3config__Config_Reset = Config_Reset;
  assign Reg4config__Config_Reset = Config_Reset;
  assign Reg5config__Config_Reset = Config_Reset;
  assign Reg6config__Config_Reset = Config_Reset;
  assign Reg7config__Config_Reset = Config_Reset;
  assign RegAConfig__Config_Reset = Config_Reset;
  assign RegBConfig__Config_Reset = Config_Reset;
  assign crossbar__in8[31:0] = ALU__out[31:0];
  assign RES__in[31:0] = ALU__out[31:0];
  assign out0[31:0] = crossbar__out0[31:0];
  assign out1[31:0] = crossbar__out1[31:0];
  assign out2[31:0] = crossbar__out2[31:0];
  assign out3[31:0] = crossbar__out3[31:0];
  assign out4[31:0] = crossbar__out4[31:0];
  assign out5[31:0] = crossbar__out5[31:0];
  assign out6[31:0] = crossbar__out6[31:0];
  assign out7[31:0] = crossbar__out7[31:0];
  assign rega__in[31:0] = crossbar__out8[31:0];
  assign regb__in[31:0] = crossbar__out9[31:0];
  assign crossbar__in9[31:0] = RES__out[31:0];
  assign alu_muxa__in0[31:0] = rega__out[31:0];
  assign alu_muxb__in0[31:0] = regb__out[31:0];
  assign alu_muxa__in1[31:0] = const__out[31:0];
  assign alu_muxb__in1[31:0] = const__out[31:0];
  assign ALU__in_a[31:0] = alu_muxa__out[31:0];
  assign ALU__in_b[31:0] = alu_muxb__out[31:0];
  assign mux_0__in1[31:0] = reg0__out[31:0];
  assign crossbar__in0[31:0] = mux_0__out[31:0];
  assign mux_1__in1[31:0] = reg1__out[31:0];
  assign crossbar__in1[31:0] = mux_1__out[31:0];
  assign mux_2__in1[31:0] = reg2__out[31:0];
  assign crossbar__in2[31:0] = mux_2__out[31:0];
  assign mux_3__in1[31:0] = reg3__out[31:0];
  assign crossbar__in3[31:0] = mux_3__out[31:0];
  assign mux_4__in1[31:0] = reg4__out[31:0];
  assign crossbar__in4[31:0] = mux_4__out[31:0];
  assign mux_5__in1[31:0] = reg5__out[31:0];
  assign crossbar__in5[31:0] = mux_5__out[31:0];
  assign mux_6__in1[31:0] = reg6__out[31:0];
  assign crossbar__in6[31:0] = mux_6__out[31:0];
  assign mux_7__in1[31:0] = reg7__out[31:0];
  assign crossbar__in7[31:0] = mux_7__out[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign reg0__in[31:0] = in0[31:0];
  assign mux_1__in0[31:0] = in1[31:0];
  assign reg1__in[31:0] = in1[31:0];
  assign mux_2__in0[31:0] = in2[31:0];
  assign reg2__in[31:0] = in2[31:0];
  assign mux_3__in0[31:0] = in3[31:0];
  assign reg3__in[31:0] = in3[31:0];
  assign mux_4__in0[31:0] = in4[31:0];
  assign reg4__in[31:0] = in4[31:0];
  assign mux_5__in0[31:0] = in5[31:0];
  assign reg5__in[31:0] = in5[31:0];
  assign mux_6__in0[31:0] = in6[31:0];
  assign reg6__in[31:0] = in6[31:0];
  assign mux_7__in0[31:0] = in7[31:0];
  assign reg7__in[31:0] = in7[31:0];
  assign alu_muxa__select[0:0] = ALUMuxAconfig__select[0:0];
  assign alu_muxb__select[0:0] = ALUMuxBconfig__select[0:0];
  assign ALU__select[3:0] = ALUconfig__select[3:0];
  assign mux_0__select[0:0] = Mux0config__select[0:0];
  assign mux_1__select[0:0] = Mux1config__select[0:0];
  assign mux_2__select[0:0] = Mux2config__select[0:0];
  assign mux_3__select[0:0] = Mux3config__select[0:0];
  assign mux_4__select[0:0] = Mux4config__select[0:0];
  assign mux_5__select[0:0] = Mux5config__select[0:0];
  assign mux_6__select[0:0] = Mux6config__select[0:0];
  assign mux_7__select[0:0] = Mux7config__select[0:0];
  assign RES__enable[0:0] = RESConfig__select[0:0];
  assign reg0__enable[0:0] = Reg0config__select[0:0];
  assign reg1__enable[0:0] = Reg1config__select[0:0];
  assign reg2__enable[0:0] = Reg2config__select[0:0];
  assign reg3__enable[0:0] = Reg3config__select[0:0];
  assign reg4__enable[0:0] = Reg4config__select[0:0];
  assign reg5__enable[0:0] = Reg5config__select[0:0];
  assign reg6__enable[0:0] = Reg6config__select[0:0];
  assign reg7__enable[0:0] = Reg7config__select[0:0];
  assign regb__enable[0:0] = RegAConfig__select[0:0];
  assign rega__enable[0:0] = RegBConfig__select[0:0];
  assign ALUMuxAconfig__ConfigIn = ConfigIn;
  assign ALUMuxBconfig__ConfigIn = ALUMuxAconfig__ConfigOut;
  assign ALUconfig__ConfigIn = ALUMuxBconfig__ConfigOut;
  assign Mux0config__ConfigIn = ALUconfig__ConfigOut;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux4config__ConfigIn = Mux3config__ConfigOut;
  assign Mux5config__ConfigIn = Mux4config__ConfigOut;
  assign Mux6config__ConfigIn = Mux5config__ConfigOut;
  assign Mux7config__ConfigIn = Mux6config__ConfigOut;
  assign RESConfig__ConfigIn = Mux7config__ConfigOut;
  assign Reg0config__ConfigIn = RESConfig__ConfigOut;
  assign Reg1config__ConfigIn = Reg0config__ConfigOut;
  assign Reg2config__ConfigIn = Reg1config__ConfigOut;
  assign Reg3config__ConfigIn = Reg2config__ConfigOut;
  assign Reg4config__ConfigIn = Reg3config__ConfigOut;
  assign Reg5config__ConfigIn = Reg4config__ConfigOut;
  assign Reg6config__ConfigIn = Reg5config__ConfigOut;
  assign Reg7config__ConfigIn = Reg6config__ConfigOut;
  assign RegAConfig__ConfigIn = Reg7config__ConfigOut;
  assign RegBConfig__ConfigIn = RegAConfig__ConfigOut;
  assign const__ConfigIn = RegBConfig__ConfigOut;
  assign crossbar__ConfigIn = const__ConfigOut;
  assign ConfigOut = crossbar__ConfigOut;

endmodule //hycube_no_pred_in255_out255_U7

module hycube_no_pred_in143_out143_U9 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in7,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out7
);
  //Wire declarations for instance 'ALU' (Module func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12)
  wire  ALU__CGRA_Clock;
  wire  ALU__CGRA_Enable;
  wire  ALU__CGRA_Reset;
  wire [31:0] ALU__in_a;
  wire [31:0] ALU__in_b;
  wire [31:0] ALU__out;
  wire [3:0] ALU__select;
  func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12 ALU(
    .CGRA_Clock(ALU__CGRA_Clock),
    .CGRA_Enable(ALU__CGRA_Enable),
    .CGRA_Reset(ALU__CGRA_Reset),
    .in_a(ALU__in_a),
    .in_b(ALU__in_b),
    .out(ALU__out),
    .select(ALU__select)
  );

  //Wire declarations for instance 'ALUMuxAconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxAconfig__CGRA_Clock;
  wire  ALUMuxAconfig__CGRA_Enable;
  wire  ALUMuxAconfig__CGRA_Reset;
  wire  ALUMuxAconfig__ConfigIn;
  wire  ALUMuxAconfig__ConfigOut;
  wire  ALUMuxAconfig__Config_Clock;
  wire  ALUMuxAconfig__Config_Reset;
  wire [0:0] ALUMuxAconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxAconfig(
    .CGRA_Clock(ALUMuxAconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxAconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxAconfig__CGRA_Reset),
    .ConfigIn(ALUMuxAconfig__ConfigIn),
    .ConfigOut(ALUMuxAconfig__ConfigOut),
    .Config_Clock(ALUMuxAconfig__Config_Clock),
    .Config_Reset(ALUMuxAconfig__Config_Reset),
    .select(ALUMuxAconfig__select)
  );

  //Wire declarations for instance 'ALUMuxBconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxBconfig__CGRA_Clock;
  wire  ALUMuxBconfig__CGRA_Enable;
  wire  ALUMuxBconfig__CGRA_Reset;
  wire  ALUMuxBconfig__ConfigIn;
  wire  ALUMuxBconfig__ConfigOut;
  wire  ALUMuxBconfig__Config_Clock;
  wire  ALUMuxBconfig__Config_Reset;
  wire [0:0] ALUMuxBconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxBconfig(
    .CGRA_Clock(ALUMuxBconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxBconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxBconfig__CGRA_Reset),
    .ConfigIn(ALUMuxBconfig__ConfigIn),
    .ConfigOut(ALUMuxBconfig__ConfigOut),
    .Config_Clock(ALUMuxBconfig__Config_Clock),
    .Config_Reset(ALUMuxBconfig__Config_Reset),
    .select(ALUMuxBconfig__select)
  );

  //Wire declarations for instance 'ALUconfig' (Module cgrame_configcell_context1_size4)
  wire  ALUconfig__CGRA_Clock;
  wire  ALUconfig__CGRA_Enable;
  wire  ALUconfig__CGRA_Reset;
  wire  ALUconfig__ConfigIn;
  wire  ALUconfig__ConfigOut;
  wire  ALUconfig__Config_Clock;
  wire  ALUconfig__Config_Reset;
  wire [3:0] ALUconfig__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) ALUconfig(
    .CGRA_Clock(ALUconfig__CGRA_Clock),
    .CGRA_Enable(ALUconfig__CGRA_Enable),
    .CGRA_Reset(ALUconfig__CGRA_Reset),
    .ConfigIn(ALUconfig__ConfigIn),
    .ConfigOut(ALUconfig__ConfigOut),
    .Config_Clock(ALUconfig__Config_Clock),
    .Config_Reset(ALUconfig__Config_Reset),
    .select(ALUconfig__select)
  );

  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size1)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [0:0] Mux0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size1)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [0:0] Mux1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size1)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [0:0] Mux2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size1)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [0:0] Mux3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux7config' (Module cgrame_configcell_context1_size1)
  wire  Mux7config__CGRA_Clock;
  wire  Mux7config__CGRA_Enable;
  wire  Mux7config__CGRA_Reset;
  wire  Mux7config__ConfigIn;
  wire  Mux7config__ConfigOut;
  wire  Mux7config__Config_Clock;
  wire  Mux7config__Config_Reset;
  wire [0:0] Mux7config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux7config(
    .CGRA_Clock(Mux7config__CGRA_Clock),
    .CGRA_Enable(Mux7config__CGRA_Enable),
    .CGRA_Reset(Mux7config__CGRA_Reset),
    .ConfigIn(Mux7config__ConfigIn),
    .ConfigOut(Mux7config__ConfigOut),
    .Config_Clock(Mux7config__Config_Clock),
    .Config_Reset(Mux7config__Config_Reset),
    .select(Mux7config__select)
  );

  //Wire declarations for instance 'RES' (Module cgrame_register_32b)
  wire  RES__CGRA_Clock;
  wire  RES__CGRA_Enable;
  wire  RES__CGRA_Reset;
  wire [0:0] RES__enable;
  wire [31:0] RES__in;
  wire [31:0] RES__out;
  cgrame_register_32b #(.contexts(1),.size(32)) RES(
    .CGRA_Clock(RES__CGRA_Clock),
    .CGRA_Enable(RES__CGRA_Enable),
    .CGRA_Reset(RES__CGRA_Reset),
    .enable(RES__enable),
    .in(RES__in),
    .out(RES__out)
  );

  //Wire declarations for instance 'RESConfig' (Module cgrame_configcell_context1_size1)
  wire  RESConfig__CGRA_Clock;
  wire  RESConfig__CGRA_Enable;
  wire  RESConfig__CGRA_Reset;
  wire  RESConfig__ConfigIn;
  wire  RESConfig__ConfigOut;
  wire  RESConfig__Config_Clock;
  wire  RESConfig__Config_Reset;
  wire [0:0] RESConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RESConfig(
    .CGRA_Clock(RESConfig__CGRA_Clock),
    .CGRA_Enable(RESConfig__CGRA_Enable),
    .CGRA_Reset(RESConfig__CGRA_Reset),
    .ConfigIn(RESConfig__ConfigIn),
    .ConfigOut(RESConfig__ConfigOut),
    .Config_Clock(RESConfig__Config_Clock),
    .Config_Reset(RESConfig__Config_Reset),
    .select(RESConfig__select)
  );

  //Wire declarations for instance 'Reg0config' (Module cgrame_configcell_context1_size1)
  wire  Reg0config__CGRA_Clock;
  wire  Reg0config__CGRA_Enable;
  wire  Reg0config__CGRA_Reset;
  wire  Reg0config__ConfigIn;
  wire  Reg0config__ConfigOut;
  wire  Reg0config__Config_Clock;
  wire  Reg0config__Config_Reset;
  wire [0:0] Reg0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg0config(
    .CGRA_Clock(Reg0config__CGRA_Clock),
    .CGRA_Enable(Reg0config__CGRA_Enable),
    .CGRA_Reset(Reg0config__CGRA_Reset),
    .ConfigIn(Reg0config__ConfigIn),
    .ConfigOut(Reg0config__ConfigOut),
    .Config_Clock(Reg0config__Config_Clock),
    .Config_Reset(Reg0config__Config_Reset),
    .select(Reg0config__select)
  );

  //Wire declarations for instance 'Reg1config' (Module cgrame_configcell_context1_size1)
  wire  Reg1config__CGRA_Clock;
  wire  Reg1config__CGRA_Enable;
  wire  Reg1config__CGRA_Reset;
  wire  Reg1config__ConfigIn;
  wire  Reg1config__ConfigOut;
  wire  Reg1config__Config_Clock;
  wire  Reg1config__Config_Reset;
  wire [0:0] Reg1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg1config(
    .CGRA_Clock(Reg1config__CGRA_Clock),
    .CGRA_Enable(Reg1config__CGRA_Enable),
    .CGRA_Reset(Reg1config__CGRA_Reset),
    .ConfigIn(Reg1config__ConfigIn),
    .ConfigOut(Reg1config__ConfigOut),
    .Config_Clock(Reg1config__Config_Clock),
    .Config_Reset(Reg1config__Config_Reset),
    .select(Reg1config__select)
  );

  //Wire declarations for instance 'Reg2config' (Module cgrame_configcell_context1_size1)
  wire  Reg2config__CGRA_Clock;
  wire  Reg2config__CGRA_Enable;
  wire  Reg2config__CGRA_Reset;
  wire  Reg2config__ConfigIn;
  wire  Reg2config__ConfigOut;
  wire  Reg2config__Config_Clock;
  wire  Reg2config__Config_Reset;
  wire [0:0] Reg2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg2config(
    .CGRA_Clock(Reg2config__CGRA_Clock),
    .CGRA_Enable(Reg2config__CGRA_Enable),
    .CGRA_Reset(Reg2config__CGRA_Reset),
    .ConfigIn(Reg2config__ConfigIn),
    .ConfigOut(Reg2config__ConfigOut),
    .Config_Clock(Reg2config__Config_Clock),
    .Config_Reset(Reg2config__Config_Reset),
    .select(Reg2config__select)
  );

  //Wire declarations for instance 'Reg3config' (Module cgrame_configcell_context1_size1)
  wire  Reg3config__CGRA_Clock;
  wire  Reg3config__CGRA_Enable;
  wire  Reg3config__CGRA_Reset;
  wire  Reg3config__ConfigIn;
  wire  Reg3config__ConfigOut;
  wire  Reg3config__Config_Clock;
  wire  Reg3config__Config_Reset;
  wire [0:0] Reg3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg3config(
    .CGRA_Clock(Reg3config__CGRA_Clock),
    .CGRA_Enable(Reg3config__CGRA_Enable),
    .CGRA_Reset(Reg3config__CGRA_Reset),
    .ConfigIn(Reg3config__ConfigIn),
    .ConfigOut(Reg3config__ConfigOut),
    .Config_Clock(Reg3config__Config_Clock),
    .Config_Reset(Reg3config__Config_Reset),
    .select(Reg3config__select)
  );

  //Wire declarations for instance 'Reg7config' (Module cgrame_configcell_context1_size1)
  wire  Reg7config__CGRA_Clock;
  wire  Reg7config__CGRA_Enable;
  wire  Reg7config__CGRA_Reset;
  wire  Reg7config__ConfigIn;
  wire  Reg7config__ConfigOut;
  wire  Reg7config__Config_Clock;
  wire  Reg7config__Config_Reset;
  wire [0:0] Reg7config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg7config(
    .CGRA_Clock(Reg7config__CGRA_Clock),
    .CGRA_Enable(Reg7config__CGRA_Enable),
    .CGRA_Reset(Reg7config__CGRA_Reset),
    .ConfigIn(Reg7config__ConfigIn),
    .ConfigOut(Reg7config__ConfigOut),
    .Config_Clock(Reg7config__Config_Clock),
    .Config_Reset(Reg7config__Config_Reset),
    .select(Reg7config__select)
  );

  //Wire declarations for instance 'RegAConfig' (Module cgrame_configcell_context1_size1)
  wire  RegAConfig__CGRA_Clock;
  wire  RegAConfig__CGRA_Enable;
  wire  RegAConfig__CGRA_Reset;
  wire  RegAConfig__ConfigIn;
  wire  RegAConfig__ConfigOut;
  wire  RegAConfig__Config_Clock;
  wire  RegAConfig__Config_Reset;
  wire [0:0] RegAConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegAConfig(
    .CGRA_Clock(RegAConfig__CGRA_Clock),
    .CGRA_Enable(RegAConfig__CGRA_Enable),
    .CGRA_Reset(RegAConfig__CGRA_Reset),
    .ConfigIn(RegAConfig__ConfigIn),
    .ConfigOut(RegAConfig__ConfigOut),
    .Config_Clock(RegAConfig__Config_Clock),
    .Config_Reset(RegAConfig__Config_Reset),
    .select(RegAConfig__select)
  );

  //Wire declarations for instance 'RegBConfig' (Module cgrame_configcell_context1_size1)
  wire  RegBConfig__CGRA_Clock;
  wire  RegBConfig__CGRA_Enable;
  wire  RegBConfig__CGRA_Reset;
  wire  RegBConfig__ConfigIn;
  wire  RegBConfig__ConfigOut;
  wire  RegBConfig__Config_Clock;
  wire  RegBConfig__Config_Reset;
  wire [0:0] RegBConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegBConfig(
    .CGRA_Clock(RegBConfig__CGRA_Clock),
    .CGRA_Enable(RegBConfig__CGRA_Enable),
    .CGRA_Reset(RegBConfig__CGRA_Reset),
    .ConfigIn(RegBConfig__ConfigIn),
    .ConfigOut(RegBConfig__ConfigOut),
    .Config_Clock(RegBConfig__Config_Clock),
    .Config_Reset(RegBConfig__Config_Reset),
    .select(RegBConfig__select)
  );

  //Wire declarations for instance 'alu_muxa' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxa__in0;
  wire [31:0] alu_muxa__in1;
  wire [31:0] alu_muxa__out;
  wire [0:0] alu_muxa__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxa(
    .in0(alu_muxa__in0),
    .in1(alu_muxa__in1),
    .out(alu_muxa__out),
    .select(alu_muxa__select)
  );

  //Wire declarations for instance 'alu_muxb' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxb__in0;
  wire [31:0] alu_muxb__in1;
  wire [31:0] alu_muxb__out;
  wire [0:0] alu_muxb__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxb(
    .in0(alu_muxb__in0),
    .in1(alu_muxb__in1),
    .out(alu_muxb__out),
    .select(alu_muxb__select)
  );

  //Wire declarations for instance 'const' (Module const_32b_U15)
  wire  const__CGRA_Clock;
  wire  const__CGRA_Enable;
  wire  const__CGRA_Reset;
  wire  const__ConfigIn;
  wire  const__ConfigOut;
  wire  const__Config_Clock;
  wire  const__Config_Reset;
  wire [31:0] const__out;
  const_32b_U15 const(
    .CGRA_Clock(const__CGRA_Clock),
    .CGRA_Enable(const__CGRA_Enable),
    .CGRA_Reset(const__CGRA_Reset),
    .ConfigIn(const__ConfigIn),
    .ConfigOut(const__ConfigOut),
    .Config_Clock(const__Config_Clock),
    .Config_Reset(const__Config_Reset),
    .out(const__out)
  );

  //Wire declarations for instance 'crossbar' (Module crossbar_7in_7out_U19)
  wire  crossbar__CGRA_Clock;
  wire  crossbar__CGRA_Enable;
  wire  crossbar__CGRA_Reset;
  wire  crossbar__ConfigIn;
  wire  crossbar__ConfigOut;
  wire  crossbar__Config_Clock;
  wire  crossbar__Config_Reset;
  wire [31:0] crossbar__in0;
  wire [31:0] crossbar__in1;
  wire [31:0] crossbar__in2;
  wire [31:0] crossbar__in3;
  wire [31:0] crossbar__in4;
  wire [31:0] crossbar__in5;
  wire [31:0] crossbar__in6;
  wire [31:0] crossbar__out0;
  wire [31:0] crossbar__out1;
  wire [31:0] crossbar__out2;
  wire [31:0] crossbar__out3;
  wire [31:0] crossbar__out4;
  wire [31:0] crossbar__out5;
  wire [31:0] crossbar__out6;
  crossbar_7in_7out_U19 crossbar(
    .CGRA_Clock(crossbar__CGRA_Clock),
    .CGRA_Enable(crossbar__CGRA_Enable),
    .CGRA_Reset(crossbar__CGRA_Reset),
    .ConfigIn(crossbar__ConfigIn),
    .ConfigOut(crossbar__ConfigOut),
    .Config_Clock(crossbar__Config_Clock),
    .Config_Reset(crossbar__Config_Reset),
    .in0(crossbar__in0),
    .in1(crossbar__in1),
    .in2(crossbar__in2),
    .in3(crossbar__in3),
    .in4(crossbar__in4),
    .in5(crossbar__in5),
    .in6(crossbar__in6),
    .out0(crossbar__out0),
    .out1(crossbar__out1),
    .out2(crossbar__out2),
    .out3(crossbar__out3),
    .out4(crossbar__out4),
    .out5(crossbar__out5),
    .out6(crossbar__out6)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__out;
  wire [0:0] mux_0__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__out;
  wire [0:0] mux_1__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__out;
  wire [0:0] mux_2__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__out;
  wire [0:0] mux_3__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_7' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_7__in0;
  wire [31:0] mux_7__in1;
  wire [31:0] mux_7__out;
  wire [0:0] mux_7__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_7(
    .in0(mux_7__in0),
    .in1(mux_7__in1),
    .out(mux_7__out),
    .select(mux_7__select)
  );

  //Wire declarations for instance 'reg0' (Module cgrame_register_32b)
  wire  reg0__CGRA_Clock;
  wire  reg0__CGRA_Enable;
  wire  reg0__CGRA_Reset;
  wire [0:0] reg0__enable;
  wire [31:0] reg0__in;
  wire [31:0] reg0__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg0(
    .CGRA_Clock(reg0__CGRA_Clock),
    .CGRA_Enable(reg0__CGRA_Enable),
    .CGRA_Reset(reg0__CGRA_Reset),
    .enable(reg0__enable),
    .in(reg0__in),
    .out(reg0__out)
  );

  //Wire declarations for instance 'reg1' (Module cgrame_register_32b)
  wire  reg1__CGRA_Clock;
  wire  reg1__CGRA_Enable;
  wire  reg1__CGRA_Reset;
  wire [0:0] reg1__enable;
  wire [31:0] reg1__in;
  wire [31:0] reg1__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg1(
    .CGRA_Clock(reg1__CGRA_Clock),
    .CGRA_Enable(reg1__CGRA_Enable),
    .CGRA_Reset(reg1__CGRA_Reset),
    .enable(reg1__enable),
    .in(reg1__in),
    .out(reg1__out)
  );

  //Wire declarations for instance 'reg2' (Module cgrame_register_32b)
  wire  reg2__CGRA_Clock;
  wire  reg2__CGRA_Enable;
  wire  reg2__CGRA_Reset;
  wire [0:0] reg2__enable;
  wire [31:0] reg2__in;
  wire [31:0] reg2__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg2(
    .CGRA_Clock(reg2__CGRA_Clock),
    .CGRA_Enable(reg2__CGRA_Enable),
    .CGRA_Reset(reg2__CGRA_Reset),
    .enable(reg2__enable),
    .in(reg2__in),
    .out(reg2__out)
  );

  //Wire declarations for instance 'reg3' (Module cgrame_register_32b)
  wire  reg3__CGRA_Clock;
  wire  reg3__CGRA_Enable;
  wire  reg3__CGRA_Reset;
  wire [0:0] reg3__enable;
  wire [31:0] reg3__in;
  wire [31:0] reg3__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg3(
    .CGRA_Clock(reg3__CGRA_Clock),
    .CGRA_Enable(reg3__CGRA_Enable),
    .CGRA_Reset(reg3__CGRA_Reset),
    .enable(reg3__enable),
    .in(reg3__in),
    .out(reg3__out)
  );

  //Wire declarations for instance 'reg7' (Module cgrame_register_32b)
  wire  reg7__CGRA_Clock;
  wire  reg7__CGRA_Enable;
  wire  reg7__CGRA_Reset;
  wire [0:0] reg7__enable;
  wire [31:0] reg7__in;
  wire [31:0] reg7__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg7(
    .CGRA_Clock(reg7__CGRA_Clock),
    .CGRA_Enable(reg7__CGRA_Enable),
    .CGRA_Reset(reg7__CGRA_Reset),
    .enable(reg7__enable),
    .in(reg7__in),
    .out(reg7__out)
  );

  //Wire declarations for instance 'rega' (Module cgrame_register_32b)
  wire  rega__CGRA_Clock;
  wire  rega__CGRA_Enable;
  wire  rega__CGRA_Reset;
  wire [0:0] rega__enable;
  wire [31:0] rega__in;
  wire [31:0] rega__out;
  cgrame_register_32b #(.contexts(1),.size(32)) rega(
    .CGRA_Clock(rega__CGRA_Clock),
    .CGRA_Enable(rega__CGRA_Enable),
    .CGRA_Reset(rega__CGRA_Reset),
    .enable(rega__enable),
    .in(rega__in),
    .out(rega__out)
  );

  //Wire declarations for instance 'regb' (Module cgrame_register_32b)
  wire  regb__CGRA_Clock;
  wire  regb__CGRA_Enable;
  wire  regb__CGRA_Reset;
  wire [0:0] regb__enable;
  wire [31:0] regb__in;
  wire [31:0] regb__out;
  cgrame_register_32b #(.contexts(1),.size(32)) regb(
    .CGRA_Clock(regb__CGRA_Clock),
    .CGRA_Enable(regb__CGRA_Enable),
    .CGRA_Reset(regb__CGRA_Reset),
    .enable(regb__enable),
    .in(regb__in),
    .out(regb__out)
  );

  //All the connections
  assign ALU__CGRA_Reset = CGRA_Reset;
  assign ALU__CGRA_Clock = CGRA_Clock;
  assign RES__CGRA_Clock = CGRA_Clock;
  assign const__CGRA_Clock = CGRA_Clock;
  assign crossbar__CGRA_Clock = CGRA_Clock;
  assign reg0__CGRA_Clock = CGRA_Clock;
  assign reg1__CGRA_Clock = CGRA_Clock;
  assign reg2__CGRA_Clock = CGRA_Clock;
  assign reg3__CGRA_Clock = CGRA_Clock;
  assign reg7__CGRA_Clock = CGRA_Clock;
  assign rega__CGRA_Clock = CGRA_Clock;
  assign regb__CGRA_Clock = CGRA_Clock;
  assign ALUMuxAconfig__CGRA_Clock = CGRA_Clock;
  assign ALUMuxBconfig__CGRA_Clock = CGRA_Clock;
  assign ALUconfig__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux7config__CGRA_Clock = CGRA_Clock;
  assign RESConfig__CGRA_Clock = CGRA_Clock;
  assign Reg0config__CGRA_Clock = CGRA_Clock;
  assign Reg1config__CGRA_Clock = CGRA_Clock;
  assign Reg2config__CGRA_Clock = CGRA_Clock;
  assign Reg3config__CGRA_Clock = CGRA_Clock;
  assign Reg7config__CGRA_Clock = CGRA_Clock;
  assign RegAConfig__CGRA_Clock = CGRA_Clock;
  assign RegBConfig__CGRA_Clock = CGRA_Clock;
  assign ALU__CGRA_Enable = CGRA_Enable;
  assign RES__CGRA_Reset = CGRA_Reset;
  assign const__CGRA_Reset = CGRA_Reset;
  assign crossbar__CGRA_Reset = CGRA_Reset;
  assign reg0__CGRA_Reset = CGRA_Reset;
  assign reg1__CGRA_Reset = CGRA_Reset;
  assign reg2__CGRA_Reset = CGRA_Reset;
  assign reg3__CGRA_Reset = CGRA_Reset;
  assign reg7__CGRA_Reset = CGRA_Reset;
  assign rega__CGRA_Reset = CGRA_Reset;
  assign regb__CGRA_Reset = CGRA_Reset;
  assign ALUMuxAconfig__CGRA_Reset = CGRA_Reset;
  assign ALUMuxBconfig__CGRA_Reset = CGRA_Reset;
  assign ALUconfig__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux7config__CGRA_Reset = CGRA_Reset;
  assign RESConfig__CGRA_Reset = CGRA_Reset;
  assign Reg0config__CGRA_Reset = CGRA_Reset;
  assign Reg1config__CGRA_Reset = CGRA_Reset;
  assign Reg2config__CGRA_Reset = CGRA_Reset;
  assign Reg3config__CGRA_Reset = CGRA_Reset;
  assign Reg7config__CGRA_Reset = CGRA_Reset;
  assign RegAConfig__CGRA_Reset = CGRA_Reset;
  assign RegBConfig__CGRA_Reset = CGRA_Reset;
  assign RES__CGRA_Enable = CGRA_Enable;
  assign const__CGRA_Enable = CGRA_Enable;
  assign crossbar__CGRA_Enable = CGRA_Enable;
  assign reg0__CGRA_Enable = CGRA_Enable;
  assign reg1__CGRA_Enable = CGRA_Enable;
  assign reg2__CGRA_Enable = CGRA_Enable;
  assign reg3__CGRA_Enable = CGRA_Enable;
  assign reg7__CGRA_Enable = CGRA_Enable;
  assign rega__CGRA_Enable = CGRA_Enable;
  assign regb__CGRA_Enable = CGRA_Enable;
  assign ALUMuxAconfig__CGRA_Enable = CGRA_Enable;
  assign ALUMuxBconfig__CGRA_Enable = CGRA_Enable;
  assign ALUconfig__CGRA_Enable = CGRA_Enable;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux7config__CGRA_Enable = CGRA_Enable;
  assign RESConfig__CGRA_Enable = CGRA_Enable;
  assign Reg0config__CGRA_Enable = CGRA_Enable;
  assign Reg1config__CGRA_Enable = CGRA_Enable;
  assign Reg2config__CGRA_Enable = CGRA_Enable;
  assign Reg3config__CGRA_Enable = CGRA_Enable;
  assign Reg7config__CGRA_Enable = CGRA_Enable;
  assign RegAConfig__CGRA_Enable = CGRA_Enable;
  assign RegBConfig__CGRA_Enable = CGRA_Enable;
  assign const__Config_Clock = Config_Clock;
  assign const__Config_Reset = Config_Reset;
  assign crossbar__Config_Clock = Config_Clock;
  assign ALUMuxAconfig__Config_Clock = Config_Clock;
  assign ALUMuxBconfig__Config_Clock = Config_Clock;
  assign ALUconfig__Config_Clock = Config_Clock;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux7config__Config_Clock = Config_Clock;
  assign RESConfig__Config_Clock = Config_Clock;
  assign Reg0config__Config_Clock = Config_Clock;
  assign Reg1config__Config_Clock = Config_Clock;
  assign Reg2config__Config_Clock = Config_Clock;
  assign Reg3config__Config_Clock = Config_Clock;
  assign Reg7config__Config_Clock = Config_Clock;
  assign RegAConfig__Config_Clock = Config_Clock;
  assign RegBConfig__Config_Clock = Config_Clock;
  assign crossbar__Config_Reset = Config_Reset;
  assign ALUMuxAconfig__Config_Reset = Config_Reset;
  assign ALUMuxBconfig__Config_Reset = Config_Reset;
  assign ALUconfig__Config_Reset = Config_Reset;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux7config__Config_Reset = Config_Reset;
  assign RESConfig__Config_Reset = Config_Reset;
  assign Reg0config__Config_Reset = Config_Reset;
  assign Reg1config__Config_Reset = Config_Reset;
  assign Reg2config__Config_Reset = Config_Reset;
  assign Reg3config__Config_Reset = Config_Reset;
  assign Reg7config__Config_Reset = Config_Reset;
  assign RegAConfig__Config_Reset = Config_Reset;
  assign RegBConfig__Config_Reset = Config_Reset;
  assign crossbar__in5[31:0] = ALU__out[31:0];
  assign RES__in[31:0] = ALU__out[31:0];
  assign out0[31:0] = crossbar__out0[31:0];
  assign out1[31:0] = crossbar__out1[31:0];
  assign out2[31:0] = crossbar__out2[31:0];
  assign out3[31:0] = crossbar__out3[31:0];
  assign out7[31:0] = crossbar__out4[31:0];
  assign rega__in[31:0] = crossbar__out5[31:0];
  assign regb__in[31:0] = crossbar__out6[31:0];
  assign crossbar__in6[31:0] = RES__out[31:0];
  assign alu_muxa__in0[31:0] = rega__out[31:0];
  assign alu_muxb__in0[31:0] = regb__out[31:0];
  assign alu_muxa__in1[31:0] = const__out[31:0];
  assign alu_muxb__in1[31:0] = const__out[31:0];
  assign ALU__in_a[31:0] = alu_muxa__out[31:0];
  assign ALU__in_b[31:0] = alu_muxb__out[31:0];
  assign mux_0__in1[31:0] = reg0__out[31:0];
  assign crossbar__in0[31:0] = mux_0__out[31:0];
  assign mux_1__in1[31:0] = reg1__out[31:0];
  assign crossbar__in1[31:0] = mux_1__out[31:0];
  assign mux_2__in1[31:0] = reg2__out[31:0];
  assign crossbar__in2[31:0] = mux_2__out[31:0];
  assign mux_3__in1[31:0] = reg3__out[31:0];
  assign crossbar__in3[31:0] = mux_3__out[31:0];
  assign mux_7__in1[31:0] = reg7__out[31:0];
  assign crossbar__in4[31:0] = mux_7__out[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign reg0__in[31:0] = in0[31:0];
  assign mux_1__in0[31:0] = in1[31:0];
  assign reg1__in[31:0] = in1[31:0];
  assign mux_2__in0[31:0] = in2[31:0];
  assign reg2__in[31:0] = in2[31:0];
  assign mux_3__in0[31:0] = in3[31:0];
  assign reg3__in[31:0] = in3[31:0];
  assign mux_7__in0[31:0] = in7[31:0];
  assign reg7__in[31:0] = in7[31:0];
  assign alu_muxa__select[0:0] = ALUMuxAconfig__select[0:0];
  assign alu_muxb__select[0:0] = ALUMuxBconfig__select[0:0];
  assign ALU__select[3:0] = ALUconfig__select[3:0];
  assign mux_0__select[0:0] = Mux0config__select[0:0];
  assign mux_1__select[0:0] = Mux1config__select[0:0];
  assign mux_2__select[0:0] = Mux2config__select[0:0];
  assign mux_3__select[0:0] = Mux3config__select[0:0];
  assign mux_7__select[0:0] = Mux7config__select[0:0];
  assign RES__enable[0:0] = RESConfig__select[0:0];
  assign reg0__enable[0:0] = Reg0config__select[0:0];
  assign reg1__enable[0:0] = Reg1config__select[0:0];
  assign reg2__enable[0:0] = Reg2config__select[0:0];
  assign reg3__enable[0:0] = Reg3config__select[0:0];
  assign reg7__enable[0:0] = Reg7config__select[0:0];
  assign regb__enable[0:0] = RegAConfig__select[0:0];
  assign rega__enable[0:0] = RegBConfig__select[0:0];
  assign ALUMuxAconfig__ConfigIn = ConfigIn;
  assign ALUMuxBconfig__ConfigIn = ALUMuxAconfig__ConfigOut;
  assign ALUconfig__ConfigIn = ALUMuxBconfig__ConfigOut;
  assign Mux0config__ConfigIn = ALUconfig__ConfigOut;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux7config__ConfigIn = Mux3config__ConfigOut;
  assign RESConfig__ConfigIn = Mux7config__ConfigOut;
  assign Reg0config__ConfigIn = RESConfig__ConfigOut;
  assign Reg1config__ConfigIn = Reg0config__ConfigOut;
  assign Reg2config__ConfigIn = Reg1config__ConfigOut;
  assign Reg3config__ConfigIn = Reg2config__ConfigOut;
  assign Reg7config__ConfigIn = Reg3config__ConfigOut;
  assign RegAConfig__ConfigIn = Reg7config__ConfigOut;
  assign RegBConfig__ConfigIn = RegAConfig__ConfigOut;
  assign const__ConfigIn = RegBConfig__ConfigOut;
  assign crossbar__ConfigIn = const__ConfigOut;
  assign ConfigOut = crossbar__ConfigOut;

endmodule //hycube_no_pred_in143_out143_U9

module hycube_no_pred_in207_out207_U6 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] in0,
  input [31:0] in1,
  input [31:0] in2,
  input [31:0] in3,
  input [31:0] in6,
  input [31:0] in7,
  output [31:0] out0,
  output [31:0] out1,
  output [31:0] out2,
  output [31:0] out3,
  output [31:0] out6,
  output [31:0] out7
);
  //Wire declarations for instance 'ALU' (Module func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12)
  wire  ALU__CGRA_Clock;
  wire  ALU__CGRA_Enable;
  wire  ALU__CGRA_Reset;
  wire [31:0] ALU__in_a;
  wire [31:0] ALU__in_b;
  wire [31:0] ALU__out;
  wire [3:0] ALU__select;
  func_32b_add_sub_multiply_and_or_xor_shl_ashr_lshr_cmp_U12 ALU(
    .CGRA_Clock(ALU__CGRA_Clock),
    .CGRA_Enable(ALU__CGRA_Enable),
    .CGRA_Reset(ALU__CGRA_Reset),
    .in_a(ALU__in_a),
    .in_b(ALU__in_b),
    .out(ALU__out),
    .select(ALU__select)
  );

  //Wire declarations for instance 'ALUMuxAconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxAconfig__CGRA_Clock;
  wire  ALUMuxAconfig__CGRA_Enable;
  wire  ALUMuxAconfig__CGRA_Reset;
  wire  ALUMuxAconfig__ConfigIn;
  wire  ALUMuxAconfig__ConfigOut;
  wire  ALUMuxAconfig__Config_Clock;
  wire  ALUMuxAconfig__Config_Reset;
  wire [0:0] ALUMuxAconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxAconfig(
    .CGRA_Clock(ALUMuxAconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxAconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxAconfig__CGRA_Reset),
    .ConfigIn(ALUMuxAconfig__ConfigIn),
    .ConfigOut(ALUMuxAconfig__ConfigOut),
    .Config_Clock(ALUMuxAconfig__Config_Clock),
    .Config_Reset(ALUMuxAconfig__Config_Reset),
    .select(ALUMuxAconfig__select)
  );

  //Wire declarations for instance 'ALUMuxBconfig' (Module cgrame_configcell_context1_size1)
  wire  ALUMuxBconfig__CGRA_Clock;
  wire  ALUMuxBconfig__CGRA_Enable;
  wire  ALUMuxBconfig__CGRA_Reset;
  wire  ALUMuxBconfig__ConfigIn;
  wire  ALUMuxBconfig__ConfigOut;
  wire  ALUMuxBconfig__Config_Clock;
  wire  ALUMuxBconfig__Config_Reset;
  wire [0:0] ALUMuxBconfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) ALUMuxBconfig(
    .CGRA_Clock(ALUMuxBconfig__CGRA_Clock),
    .CGRA_Enable(ALUMuxBconfig__CGRA_Enable),
    .CGRA_Reset(ALUMuxBconfig__CGRA_Reset),
    .ConfigIn(ALUMuxBconfig__ConfigIn),
    .ConfigOut(ALUMuxBconfig__ConfigOut),
    .Config_Clock(ALUMuxBconfig__Config_Clock),
    .Config_Reset(ALUMuxBconfig__Config_Reset),
    .select(ALUMuxBconfig__select)
  );

  //Wire declarations for instance 'ALUconfig' (Module cgrame_configcell_context1_size4)
  wire  ALUconfig__CGRA_Clock;
  wire  ALUconfig__CGRA_Enable;
  wire  ALUconfig__CGRA_Reset;
  wire  ALUconfig__ConfigIn;
  wire  ALUconfig__ConfigOut;
  wire  ALUconfig__Config_Clock;
  wire  ALUconfig__Config_Reset;
  wire [3:0] ALUconfig__select;
  cgrame_configcell_context1_size4 #(.contexts(1),.size(4)) ALUconfig(
    .CGRA_Clock(ALUconfig__CGRA_Clock),
    .CGRA_Enable(ALUconfig__CGRA_Enable),
    .CGRA_Reset(ALUconfig__CGRA_Reset),
    .ConfigIn(ALUconfig__ConfigIn),
    .ConfigOut(ALUconfig__ConfigOut),
    .Config_Clock(ALUconfig__Config_Clock),
    .Config_Reset(ALUconfig__Config_Reset),
    .select(ALUconfig__select)
  );

  //Wire declarations for instance 'Mux0config' (Module cgrame_configcell_context1_size1)
  wire  Mux0config__CGRA_Clock;
  wire  Mux0config__CGRA_Enable;
  wire  Mux0config__CGRA_Reset;
  wire  Mux0config__ConfigIn;
  wire  Mux0config__ConfigOut;
  wire  Mux0config__Config_Clock;
  wire  Mux0config__Config_Reset;
  wire [0:0] Mux0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux0config(
    .CGRA_Clock(Mux0config__CGRA_Clock),
    .CGRA_Enable(Mux0config__CGRA_Enable),
    .CGRA_Reset(Mux0config__CGRA_Reset),
    .ConfigIn(Mux0config__ConfigIn),
    .ConfigOut(Mux0config__ConfigOut),
    .Config_Clock(Mux0config__Config_Clock),
    .Config_Reset(Mux0config__Config_Reset),
    .select(Mux0config__select)
  );

  //Wire declarations for instance 'Mux1config' (Module cgrame_configcell_context1_size1)
  wire  Mux1config__CGRA_Clock;
  wire  Mux1config__CGRA_Enable;
  wire  Mux1config__CGRA_Reset;
  wire  Mux1config__ConfigIn;
  wire  Mux1config__ConfigOut;
  wire  Mux1config__Config_Clock;
  wire  Mux1config__Config_Reset;
  wire [0:0] Mux1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux1config(
    .CGRA_Clock(Mux1config__CGRA_Clock),
    .CGRA_Enable(Mux1config__CGRA_Enable),
    .CGRA_Reset(Mux1config__CGRA_Reset),
    .ConfigIn(Mux1config__ConfigIn),
    .ConfigOut(Mux1config__ConfigOut),
    .Config_Clock(Mux1config__Config_Clock),
    .Config_Reset(Mux1config__Config_Reset),
    .select(Mux1config__select)
  );

  //Wire declarations for instance 'Mux2config' (Module cgrame_configcell_context1_size1)
  wire  Mux2config__CGRA_Clock;
  wire  Mux2config__CGRA_Enable;
  wire  Mux2config__CGRA_Reset;
  wire  Mux2config__ConfigIn;
  wire  Mux2config__ConfigOut;
  wire  Mux2config__Config_Clock;
  wire  Mux2config__Config_Reset;
  wire [0:0] Mux2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux2config(
    .CGRA_Clock(Mux2config__CGRA_Clock),
    .CGRA_Enable(Mux2config__CGRA_Enable),
    .CGRA_Reset(Mux2config__CGRA_Reset),
    .ConfigIn(Mux2config__ConfigIn),
    .ConfigOut(Mux2config__ConfigOut),
    .Config_Clock(Mux2config__Config_Clock),
    .Config_Reset(Mux2config__Config_Reset),
    .select(Mux2config__select)
  );

  //Wire declarations for instance 'Mux3config' (Module cgrame_configcell_context1_size1)
  wire  Mux3config__CGRA_Clock;
  wire  Mux3config__CGRA_Enable;
  wire  Mux3config__CGRA_Reset;
  wire  Mux3config__ConfigIn;
  wire  Mux3config__ConfigOut;
  wire  Mux3config__Config_Clock;
  wire  Mux3config__Config_Reset;
  wire [0:0] Mux3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux3config(
    .CGRA_Clock(Mux3config__CGRA_Clock),
    .CGRA_Enable(Mux3config__CGRA_Enable),
    .CGRA_Reset(Mux3config__CGRA_Reset),
    .ConfigIn(Mux3config__ConfigIn),
    .ConfigOut(Mux3config__ConfigOut),
    .Config_Clock(Mux3config__Config_Clock),
    .Config_Reset(Mux3config__Config_Reset),
    .select(Mux3config__select)
  );

  //Wire declarations for instance 'Mux6config' (Module cgrame_configcell_context1_size1)
  wire  Mux6config__CGRA_Clock;
  wire  Mux6config__CGRA_Enable;
  wire  Mux6config__CGRA_Reset;
  wire  Mux6config__ConfigIn;
  wire  Mux6config__ConfigOut;
  wire  Mux6config__Config_Clock;
  wire  Mux6config__Config_Reset;
  wire [0:0] Mux6config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux6config(
    .CGRA_Clock(Mux6config__CGRA_Clock),
    .CGRA_Enable(Mux6config__CGRA_Enable),
    .CGRA_Reset(Mux6config__CGRA_Reset),
    .ConfigIn(Mux6config__ConfigIn),
    .ConfigOut(Mux6config__ConfigOut),
    .Config_Clock(Mux6config__Config_Clock),
    .Config_Reset(Mux6config__Config_Reset),
    .select(Mux6config__select)
  );

  //Wire declarations for instance 'Mux7config' (Module cgrame_configcell_context1_size1)
  wire  Mux7config__CGRA_Clock;
  wire  Mux7config__CGRA_Enable;
  wire  Mux7config__CGRA_Reset;
  wire  Mux7config__ConfigIn;
  wire  Mux7config__ConfigOut;
  wire  Mux7config__Config_Clock;
  wire  Mux7config__Config_Reset;
  wire [0:0] Mux7config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Mux7config(
    .CGRA_Clock(Mux7config__CGRA_Clock),
    .CGRA_Enable(Mux7config__CGRA_Enable),
    .CGRA_Reset(Mux7config__CGRA_Reset),
    .ConfigIn(Mux7config__ConfigIn),
    .ConfigOut(Mux7config__ConfigOut),
    .Config_Clock(Mux7config__Config_Clock),
    .Config_Reset(Mux7config__Config_Reset),
    .select(Mux7config__select)
  );

  //Wire declarations for instance 'RES' (Module cgrame_register_32b)
  wire  RES__CGRA_Clock;
  wire  RES__CGRA_Enable;
  wire  RES__CGRA_Reset;
  wire [0:0] RES__enable;
  wire [31:0] RES__in;
  wire [31:0] RES__out;
  cgrame_register_32b #(.contexts(1),.size(32)) RES(
    .CGRA_Clock(RES__CGRA_Clock),
    .CGRA_Enable(RES__CGRA_Enable),
    .CGRA_Reset(RES__CGRA_Reset),
    .enable(RES__enable),
    .in(RES__in),
    .out(RES__out)
  );

  //Wire declarations for instance 'RESConfig' (Module cgrame_configcell_context1_size1)
  wire  RESConfig__CGRA_Clock;
  wire  RESConfig__CGRA_Enable;
  wire  RESConfig__CGRA_Reset;
  wire  RESConfig__ConfigIn;
  wire  RESConfig__ConfigOut;
  wire  RESConfig__Config_Clock;
  wire  RESConfig__Config_Reset;
  wire [0:0] RESConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RESConfig(
    .CGRA_Clock(RESConfig__CGRA_Clock),
    .CGRA_Enable(RESConfig__CGRA_Enable),
    .CGRA_Reset(RESConfig__CGRA_Reset),
    .ConfigIn(RESConfig__ConfigIn),
    .ConfigOut(RESConfig__ConfigOut),
    .Config_Clock(RESConfig__Config_Clock),
    .Config_Reset(RESConfig__Config_Reset),
    .select(RESConfig__select)
  );

  //Wire declarations for instance 'Reg0config' (Module cgrame_configcell_context1_size1)
  wire  Reg0config__CGRA_Clock;
  wire  Reg0config__CGRA_Enable;
  wire  Reg0config__CGRA_Reset;
  wire  Reg0config__ConfigIn;
  wire  Reg0config__ConfigOut;
  wire  Reg0config__Config_Clock;
  wire  Reg0config__Config_Reset;
  wire [0:0] Reg0config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg0config(
    .CGRA_Clock(Reg0config__CGRA_Clock),
    .CGRA_Enable(Reg0config__CGRA_Enable),
    .CGRA_Reset(Reg0config__CGRA_Reset),
    .ConfigIn(Reg0config__ConfigIn),
    .ConfigOut(Reg0config__ConfigOut),
    .Config_Clock(Reg0config__Config_Clock),
    .Config_Reset(Reg0config__Config_Reset),
    .select(Reg0config__select)
  );

  //Wire declarations for instance 'Reg1config' (Module cgrame_configcell_context1_size1)
  wire  Reg1config__CGRA_Clock;
  wire  Reg1config__CGRA_Enable;
  wire  Reg1config__CGRA_Reset;
  wire  Reg1config__ConfigIn;
  wire  Reg1config__ConfigOut;
  wire  Reg1config__Config_Clock;
  wire  Reg1config__Config_Reset;
  wire [0:0] Reg1config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg1config(
    .CGRA_Clock(Reg1config__CGRA_Clock),
    .CGRA_Enable(Reg1config__CGRA_Enable),
    .CGRA_Reset(Reg1config__CGRA_Reset),
    .ConfigIn(Reg1config__ConfigIn),
    .ConfigOut(Reg1config__ConfigOut),
    .Config_Clock(Reg1config__Config_Clock),
    .Config_Reset(Reg1config__Config_Reset),
    .select(Reg1config__select)
  );

  //Wire declarations for instance 'Reg2config' (Module cgrame_configcell_context1_size1)
  wire  Reg2config__CGRA_Clock;
  wire  Reg2config__CGRA_Enable;
  wire  Reg2config__CGRA_Reset;
  wire  Reg2config__ConfigIn;
  wire  Reg2config__ConfigOut;
  wire  Reg2config__Config_Clock;
  wire  Reg2config__Config_Reset;
  wire [0:0] Reg2config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg2config(
    .CGRA_Clock(Reg2config__CGRA_Clock),
    .CGRA_Enable(Reg2config__CGRA_Enable),
    .CGRA_Reset(Reg2config__CGRA_Reset),
    .ConfigIn(Reg2config__ConfigIn),
    .ConfigOut(Reg2config__ConfigOut),
    .Config_Clock(Reg2config__Config_Clock),
    .Config_Reset(Reg2config__Config_Reset),
    .select(Reg2config__select)
  );

  //Wire declarations for instance 'Reg3config' (Module cgrame_configcell_context1_size1)
  wire  Reg3config__CGRA_Clock;
  wire  Reg3config__CGRA_Enable;
  wire  Reg3config__CGRA_Reset;
  wire  Reg3config__ConfigIn;
  wire  Reg3config__ConfigOut;
  wire  Reg3config__Config_Clock;
  wire  Reg3config__Config_Reset;
  wire [0:0] Reg3config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg3config(
    .CGRA_Clock(Reg3config__CGRA_Clock),
    .CGRA_Enable(Reg3config__CGRA_Enable),
    .CGRA_Reset(Reg3config__CGRA_Reset),
    .ConfigIn(Reg3config__ConfigIn),
    .ConfigOut(Reg3config__ConfigOut),
    .Config_Clock(Reg3config__Config_Clock),
    .Config_Reset(Reg3config__Config_Reset),
    .select(Reg3config__select)
  );

  //Wire declarations for instance 'Reg6config' (Module cgrame_configcell_context1_size1)
  wire  Reg6config__CGRA_Clock;
  wire  Reg6config__CGRA_Enable;
  wire  Reg6config__CGRA_Reset;
  wire  Reg6config__ConfigIn;
  wire  Reg6config__ConfigOut;
  wire  Reg6config__Config_Clock;
  wire  Reg6config__Config_Reset;
  wire [0:0] Reg6config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg6config(
    .CGRA_Clock(Reg6config__CGRA_Clock),
    .CGRA_Enable(Reg6config__CGRA_Enable),
    .CGRA_Reset(Reg6config__CGRA_Reset),
    .ConfigIn(Reg6config__ConfigIn),
    .ConfigOut(Reg6config__ConfigOut),
    .Config_Clock(Reg6config__Config_Clock),
    .Config_Reset(Reg6config__Config_Reset),
    .select(Reg6config__select)
  );

  //Wire declarations for instance 'Reg7config' (Module cgrame_configcell_context1_size1)
  wire  Reg7config__CGRA_Clock;
  wire  Reg7config__CGRA_Enable;
  wire  Reg7config__CGRA_Reset;
  wire  Reg7config__ConfigIn;
  wire  Reg7config__ConfigOut;
  wire  Reg7config__Config_Clock;
  wire  Reg7config__Config_Reset;
  wire [0:0] Reg7config__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) Reg7config(
    .CGRA_Clock(Reg7config__CGRA_Clock),
    .CGRA_Enable(Reg7config__CGRA_Enable),
    .CGRA_Reset(Reg7config__CGRA_Reset),
    .ConfigIn(Reg7config__ConfigIn),
    .ConfigOut(Reg7config__ConfigOut),
    .Config_Clock(Reg7config__Config_Clock),
    .Config_Reset(Reg7config__Config_Reset),
    .select(Reg7config__select)
  );

  //Wire declarations for instance 'RegAConfig' (Module cgrame_configcell_context1_size1)
  wire  RegAConfig__CGRA_Clock;
  wire  RegAConfig__CGRA_Enable;
  wire  RegAConfig__CGRA_Reset;
  wire  RegAConfig__ConfigIn;
  wire  RegAConfig__ConfigOut;
  wire  RegAConfig__Config_Clock;
  wire  RegAConfig__Config_Reset;
  wire [0:0] RegAConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegAConfig(
    .CGRA_Clock(RegAConfig__CGRA_Clock),
    .CGRA_Enable(RegAConfig__CGRA_Enable),
    .CGRA_Reset(RegAConfig__CGRA_Reset),
    .ConfigIn(RegAConfig__ConfigIn),
    .ConfigOut(RegAConfig__ConfigOut),
    .Config_Clock(RegAConfig__Config_Clock),
    .Config_Reset(RegAConfig__Config_Reset),
    .select(RegAConfig__select)
  );

  //Wire declarations for instance 'RegBConfig' (Module cgrame_configcell_context1_size1)
  wire  RegBConfig__CGRA_Clock;
  wire  RegBConfig__CGRA_Enable;
  wire  RegBConfig__CGRA_Reset;
  wire  RegBConfig__ConfigIn;
  wire  RegBConfig__ConfigOut;
  wire  RegBConfig__Config_Clock;
  wire  RegBConfig__Config_Reset;
  wire [0:0] RegBConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegBConfig(
    .CGRA_Clock(RegBConfig__CGRA_Clock),
    .CGRA_Enable(RegBConfig__CGRA_Enable),
    .CGRA_Reset(RegBConfig__CGRA_Reset),
    .ConfigIn(RegBConfig__ConfigIn),
    .ConfigOut(RegBConfig__ConfigOut),
    .Config_Clock(RegBConfig__Config_Clock),
    .Config_Reset(RegBConfig__Config_Reset),
    .select(RegBConfig__select)
  );

  //Wire declarations for instance 'alu_muxa' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxa__in0;
  wire [31:0] alu_muxa__in1;
  wire [31:0] alu_muxa__out;
  wire [0:0] alu_muxa__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxa(
    .in0(alu_muxa__in0),
    .in1(alu_muxa__in1),
    .out(alu_muxa__out),
    .select(alu_muxa__select)
  );

  //Wire declarations for instance 'alu_muxb' (Module cgrame_mux_2to1_32b)
  wire [31:0] alu_muxb__in0;
  wire [31:0] alu_muxb__in1;
  wire [31:0] alu_muxb__out;
  wire [0:0] alu_muxb__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) alu_muxb(
    .in0(alu_muxb__in0),
    .in1(alu_muxb__in1),
    .out(alu_muxb__out),
    .select(alu_muxb__select)
  );

  //Wire declarations for instance 'const' (Module const_32b_U15)
  wire  const__CGRA_Clock;
  wire  const__CGRA_Enable;
  wire  const__CGRA_Reset;
  wire  const__ConfigIn;
  wire  const__ConfigOut;
  wire  const__Config_Clock;
  wire  const__Config_Reset;
  wire [31:0] const__out;
  const_32b_U15 const(
    .CGRA_Clock(const__CGRA_Clock),
    .CGRA_Enable(const__CGRA_Enable),
    .CGRA_Reset(const__CGRA_Reset),
    .ConfigIn(const__ConfigIn),
    .ConfigOut(const__ConfigOut),
    .Config_Clock(const__Config_Clock),
    .Config_Reset(const__Config_Reset),
    .out(const__out)
  );

  //Wire declarations for instance 'crossbar' (Module crossbar_8in_8out_U16)
  wire  crossbar__CGRA_Clock;
  wire  crossbar__CGRA_Enable;
  wire  crossbar__CGRA_Reset;
  wire  crossbar__ConfigIn;
  wire  crossbar__ConfigOut;
  wire  crossbar__Config_Clock;
  wire  crossbar__Config_Reset;
  wire [31:0] crossbar__in0;
  wire [31:0] crossbar__in1;
  wire [31:0] crossbar__in2;
  wire [31:0] crossbar__in3;
  wire [31:0] crossbar__in4;
  wire [31:0] crossbar__in5;
  wire [31:0] crossbar__in6;
  wire [31:0] crossbar__in7;
  wire [31:0] crossbar__out0;
  wire [31:0] crossbar__out1;
  wire [31:0] crossbar__out2;
  wire [31:0] crossbar__out3;
  wire [31:0] crossbar__out4;
  wire [31:0] crossbar__out5;
  wire [31:0] crossbar__out6;
  wire [31:0] crossbar__out7;
  crossbar_8in_8out_U16 crossbar(
    .CGRA_Clock(crossbar__CGRA_Clock),
    .CGRA_Enable(crossbar__CGRA_Enable),
    .CGRA_Reset(crossbar__CGRA_Reset),
    .ConfigIn(crossbar__ConfigIn),
    .ConfigOut(crossbar__ConfigOut),
    .Config_Clock(crossbar__Config_Clock),
    .Config_Reset(crossbar__Config_Reset),
    .in0(crossbar__in0),
    .in1(crossbar__in1),
    .in2(crossbar__in2),
    .in3(crossbar__in3),
    .in4(crossbar__in4),
    .in5(crossbar__in5),
    .in6(crossbar__in6),
    .in7(crossbar__in7),
    .out0(crossbar__out0),
    .out1(crossbar__out1),
    .out2(crossbar__out2),
    .out3(crossbar__out3),
    .out4(crossbar__out4),
    .out5(crossbar__out5),
    .out6(crossbar__out6),
    .out7(crossbar__out7)
  );

  //Wire declarations for instance 'mux_0' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_0__in0;
  wire [31:0] mux_0__in1;
  wire [31:0] mux_0__out;
  wire [0:0] mux_0__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_0(
    .in0(mux_0__in0),
    .in1(mux_0__in1),
    .out(mux_0__out),
    .select(mux_0__select)
  );

  //Wire declarations for instance 'mux_1' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_1__in0;
  wire [31:0] mux_1__in1;
  wire [31:0] mux_1__out;
  wire [0:0] mux_1__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_1(
    .in0(mux_1__in0),
    .in1(mux_1__in1),
    .out(mux_1__out),
    .select(mux_1__select)
  );

  //Wire declarations for instance 'mux_2' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_2__in0;
  wire [31:0] mux_2__in1;
  wire [31:0] mux_2__out;
  wire [0:0] mux_2__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_2(
    .in0(mux_2__in0),
    .in1(mux_2__in1),
    .out(mux_2__out),
    .select(mux_2__select)
  );

  //Wire declarations for instance 'mux_3' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_3__in0;
  wire [31:0] mux_3__in1;
  wire [31:0] mux_3__out;
  wire [0:0] mux_3__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_3(
    .in0(mux_3__in0),
    .in1(mux_3__in1),
    .out(mux_3__out),
    .select(mux_3__select)
  );

  //Wire declarations for instance 'mux_6' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_6__in0;
  wire [31:0] mux_6__in1;
  wire [31:0] mux_6__out;
  wire [0:0] mux_6__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_6(
    .in0(mux_6__in0),
    .in1(mux_6__in1),
    .out(mux_6__out),
    .select(mux_6__select)
  );

  //Wire declarations for instance 'mux_7' (Module cgrame_mux_2to1_32b)
  wire [31:0] mux_7__in0;
  wire [31:0] mux_7__in1;
  wire [31:0] mux_7__out;
  wire [0:0] mux_7__select;
  cgrame_mux_2to1_32b #(.contexts(1),.size(32)) mux_7(
    .in0(mux_7__in0),
    .in1(mux_7__in1),
    .out(mux_7__out),
    .select(mux_7__select)
  );

  //Wire declarations for instance 'reg0' (Module cgrame_register_32b)
  wire  reg0__CGRA_Clock;
  wire  reg0__CGRA_Enable;
  wire  reg0__CGRA_Reset;
  wire [0:0] reg0__enable;
  wire [31:0] reg0__in;
  wire [31:0] reg0__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg0(
    .CGRA_Clock(reg0__CGRA_Clock),
    .CGRA_Enable(reg0__CGRA_Enable),
    .CGRA_Reset(reg0__CGRA_Reset),
    .enable(reg0__enable),
    .in(reg0__in),
    .out(reg0__out)
  );

  //Wire declarations for instance 'reg1' (Module cgrame_register_32b)
  wire  reg1__CGRA_Clock;
  wire  reg1__CGRA_Enable;
  wire  reg1__CGRA_Reset;
  wire [0:0] reg1__enable;
  wire [31:0] reg1__in;
  wire [31:0] reg1__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg1(
    .CGRA_Clock(reg1__CGRA_Clock),
    .CGRA_Enable(reg1__CGRA_Enable),
    .CGRA_Reset(reg1__CGRA_Reset),
    .enable(reg1__enable),
    .in(reg1__in),
    .out(reg1__out)
  );

  //Wire declarations for instance 'reg2' (Module cgrame_register_32b)
  wire  reg2__CGRA_Clock;
  wire  reg2__CGRA_Enable;
  wire  reg2__CGRA_Reset;
  wire [0:0] reg2__enable;
  wire [31:0] reg2__in;
  wire [31:0] reg2__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg2(
    .CGRA_Clock(reg2__CGRA_Clock),
    .CGRA_Enable(reg2__CGRA_Enable),
    .CGRA_Reset(reg2__CGRA_Reset),
    .enable(reg2__enable),
    .in(reg2__in),
    .out(reg2__out)
  );

  //Wire declarations for instance 'reg3' (Module cgrame_register_32b)
  wire  reg3__CGRA_Clock;
  wire  reg3__CGRA_Enable;
  wire  reg3__CGRA_Reset;
  wire [0:0] reg3__enable;
  wire [31:0] reg3__in;
  wire [31:0] reg3__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg3(
    .CGRA_Clock(reg3__CGRA_Clock),
    .CGRA_Enable(reg3__CGRA_Enable),
    .CGRA_Reset(reg3__CGRA_Reset),
    .enable(reg3__enable),
    .in(reg3__in),
    .out(reg3__out)
  );

  //Wire declarations for instance 'reg6' (Module cgrame_register_32b)
  wire  reg6__CGRA_Clock;
  wire  reg6__CGRA_Enable;
  wire  reg6__CGRA_Reset;
  wire [0:0] reg6__enable;
  wire [31:0] reg6__in;
  wire [31:0] reg6__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg6(
    .CGRA_Clock(reg6__CGRA_Clock),
    .CGRA_Enable(reg6__CGRA_Enable),
    .CGRA_Reset(reg6__CGRA_Reset),
    .enable(reg6__enable),
    .in(reg6__in),
    .out(reg6__out)
  );

  //Wire declarations for instance 'reg7' (Module cgrame_register_32b)
  wire  reg7__CGRA_Clock;
  wire  reg7__CGRA_Enable;
  wire  reg7__CGRA_Reset;
  wire [0:0] reg7__enable;
  wire [31:0] reg7__in;
  wire [31:0] reg7__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg7(
    .CGRA_Clock(reg7__CGRA_Clock),
    .CGRA_Enable(reg7__CGRA_Enable),
    .CGRA_Reset(reg7__CGRA_Reset),
    .enable(reg7__enable),
    .in(reg7__in),
    .out(reg7__out)
  );

  //Wire declarations for instance 'rega' (Module cgrame_register_32b)
  wire  rega__CGRA_Clock;
  wire  rega__CGRA_Enable;
  wire  rega__CGRA_Reset;
  wire [0:0] rega__enable;
  wire [31:0] rega__in;
  wire [31:0] rega__out;
  cgrame_register_32b #(.contexts(1),.size(32)) rega(
    .CGRA_Clock(rega__CGRA_Clock),
    .CGRA_Enable(rega__CGRA_Enable),
    .CGRA_Reset(rega__CGRA_Reset),
    .enable(rega__enable),
    .in(rega__in),
    .out(rega__out)
  );

  //Wire declarations for instance 'regb' (Module cgrame_register_32b)
  wire  regb__CGRA_Clock;
  wire  regb__CGRA_Enable;
  wire  regb__CGRA_Reset;
  wire [0:0] regb__enable;
  wire [31:0] regb__in;
  wire [31:0] regb__out;
  cgrame_register_32b #(.contexts(1),.size(32)) regb(
    .CGRA_Clock(regb__CGRA_Clock),
    .CGRA_Enable(regb__CGRA_Enable),
    .CGRA_Reset(regb__CGRA_Reset),
    .enable(regb__enable),
    .in(regb__in),
    .out(regb__out)
  );

  //All the connections
  assign ALU__CGRA_Reset = CGRA_Reset;
  assign ALU__CGRA_Clock = CGRA_Clock;
  assign RES__CGRA_Clock = CGRA_Clock;
  assign const__CGRA_Clock = CGRA_Clock;
  assign crossbar__CGRA_Clock = CGRA_Clock;
  assign reg0__CGRA_Clock = CGRA_Clock;
  assign reg1__CGRA_Clock = CGRA_Clock;
  assign reg2__CGRA_Clock = CGRA_Clock;
  assign reg3__CGRA_Clock = CGRA_Clock;
  assign reg6__CGRA_Clock = CGRA_Clock;
  assign reg7__CGRA_Clock = CGRA_Clock;
  assign rega__CGRA_Clock = CGRA_Clock;
  assign regb__CGRA_Clock = CGRA_Clock;
  assign ALUMuxAconfig__CGRA_Clock = CGRA_Clock;
  assign ALUMuxBconfig__CGRA_Clock = CGRA_Clock;
  assign ALUconfig__CGRA_Clock = CGRA_Clock;
  assign Mux0config__CGRA_Clock = CGRA_Clock;
  assign Mux1config__CGRA_Clock = CGRA_Clock;
  assign Mux2config__CGRA_Clock = CGRA_Clock;
  assign Mux3config__CGRA_Clock = CGRA_Clock;
  assign Mux6config__CGRA_Clock = CGRA_Clock;
  assign Mux7config__CGRA_Clock = CGRA_Clock;
  assign RESConfig__CGRA_Clock = CGRA_Clock;
  assign Reg0config__CGRA_Clock = CGRA_Clock;
  assign Reg1config__CGRA_Clock = CGRA_Clock;
  assign Reg2config__CGRA_Clock = CGRA_Clock;
  assign Reg3config__CGRA_Clock = CGRA_Clock;
  assign Reg6config__CGRA_Clock = CGRA_Clock;
  assign Reg7config__CGRA_Clock = CGRA_Clock;
  assign RegAConfig__CGRA_Clock = CGRA_Clock;
  assign RegBConfig__CGRA_Clock = CGRA_Clock;
  assign ALU__CGRA_Enable = CGRA_Enable;
  assign RES__CGRA_Reset = CGRA_Reset;
  assign const__CGRA_Reset = CGRA_Reset;
  assign crossbar__CGRA_Reset = CGRA_Reset;
  assign reg0__CGRA_Reset = CGRA_Reset;
  assign reg1__CGRA_Reset = CGRA_Reset;
  assign reg2__CGRA_Reset = CGRA_Reset;
  assign reg3__CGRA_Reset = CGRA_Reset;
  assign reg6__CGRA_Reset = CGRA_Reset;
  assign reg7__CGRA_Reset = CGRA_Reset;
  assign rega__CGRA_Reset = CGRA_Reset;
  assign regb__CGRA_Reset = CGRA_Reset;
  assign ALUMuxAconfig__CGRA_Reset = CGRA_Reset;
  assign ALUMuxBconfig__CGRA_Reset = CGRA_Reset;
  assign ALUconfig__CGRA_Reset = CGRA_Reset;
  assign Mux0config__CGRA_Reset = CGRA_Reset;
  assign Mux1config__CGRA_Reset = CGRA_Reset;
  assign Mux2config__CGRA_Reset = CGRA_Reset;
  assign Mux3config__CGRA_Reset = CGRA_Reset;
  assign Mux6config__CGRA_Reset = CGRA_Reset;
  assign Mux7config__CGRA_Reset = CGRA_Reset;
  assign RESConfig__CGRA_Reset = CGRA_Reset;
  assign Reg0config__CGRA_Reset = CGRA_Reset;
  assign Reg1config__CGRA_Reset = CGRA_Reset;
  assign Reg2config__CGRA_Reset = CGRA_Reset;
  assign Reg3config__CGRA_Reset = CGRA_Reset;
  assign Reg6config__CGRA_Reset = CGRA_Reset;
  assign Reg7config__CGRA_Reset = CGRA_Reset;
  assign RegAConfig__CGRA_Reset = CGRA_Reset;
  assign RegBConfig__CGRA_Reset = CGRA_Reset;
  assign RES__CGRA_Enable = CGRA_Enable;
  assign const__CGRA_Enable = CGRA_Enable;
  assign crossbar__CGRA_Enable = CGRA_Enable;
  assign reg0__CGRA_Enable = CGRA_Enable;
  assign reg1__CGRA_Enable = CGRA_Enable;
  assign reg2__CGRA_Enable = CGRA_Enable;
  assign reg3__CGRA_Enable = CGRA_Enable;
  assign reg6__CGRA_Enable = CGRA_Enable;
  assign reg7__CGRA_Enable = CGRA_Enable;
  assign rega__CGRA_Enable = CGRA_Enable;
  assign regb__CGRA_Enable = CGRA_Enable;
  assign ALUMuxAconfig__CGRA_Enable = CGRA_Enable;
  assign ALUMuxBconfig__CGRA_Enable = CGRA_Enable;
  assign ALUconfig__CGRA_Enable = CGRA_Enable;
  assign Mux0config__CGRA_Enable = CGRA_Enable;
  assign Mux1config__CGRA_Enable = CGRA_Enable;
  assign Mux2config__CGRA_Enable = CGRA_Enable;
  assign Mux3config__CGRA_Enable = CGRA_Enable;
  assign Mux6config__CGRA_Enable = CGRA_Enable;
  assign Mux7config__CGRA_Enable = CGRA_Enable;
  assign RESConfig__CGRA_Enable = CGRA_Enable;
  assign Reg0config__CGRA_Enable = CGRA_Enable;
  assign Reg1config__CGRA_Enable = CGRA_Enable;
  assign Reg2config__CGRA_Enable = CGRA_Enable;
  assign Reg3config__CGRA_Enable = CGRA_Enable;
  assign Reg6config__CGRA_Enable = CGRA_Enable;
  assign Reg7config__CGRA_Enable = CGRA_Enable;
  assign RegAConfig__CGRA_Enable = CGRA_Enable;
  assign RegBConfig__CGRA_Enable = CGRA_Enable;
  assign const__Config_Clock = Config_Clock;
  assign const__Config_Reset = Config_Reset;
  assign crossbar__Config_Clock = Config_Clock;
  assign ALUMuxAconfig__Config_Clock = Config_Clock;
  assign ALUMuxBconfig__Config_Clock = Config_Clock;
  assign ALUconfig__Config_Clock = Config_Clock;
  assign Mux0config__Config_Clock = Config_Clock;
  assign Mux1config__Config_Clock = Config_Clock;
  assign Mux2config__Config_Clock = Config_Clock;
  assign Mux3config__Config_Clock = Config_Clock;
  assign Mux6config__Config_Clock = Config_Clock;
  assign Mux7config__Config_Clock = Config_Clock;
  assign RESConfig__Config_Clock = Config_Clock;
  assign Reg0config__Config_Clock = Config_Clock;
  assign Reg1config__Config_Clock = Config_Clock;
  assign Reg2config__Config_Clock = Config_Clock;
  assign Reg3config__Config_Clock = Config_Clock;
  assign Reg6config__Config_Clock = Config_Clock;
  assign Reg7config__Config_Clock = Config_Clock;
  assign RegAConfig__Config_Clock = Config_Clock;
  assign RegBConfig__Config_Clock = Config_Clock;
  assign crossbar__Config_Reset = Config_Reset;
  assign ALUMuxAconfig__Config_Reset = Config_Reset;
  assign ALUMuxBconfig__Config_Reset = Config_Reset;
  assign ALUconfig__Config_Reset = Config_Reset;
  assign Mux0config__Config_Reset = Config_Reset;
  assign Mux1config__Config_Reset = Config_Reset;
  assign Mux2config__Config_Reset = Config_Reset;
  assign Mux3config__Config_Reset = Config_Reset;
  assign Mux6config__Config_Reset = Config_Reset;
  assign Mux7config__Config_Reset = Config_Reset;
  assign RESConfig__Config_Reset = Config_Reset;
  assign Reg0config__Config_Reset = Config_Reset;
  assign Reg1config__Config_Reset = Config_Reset;
  assign Reg2config__Config_Reset = Config_Reset;
  assign Reg3config__Config_Reset = Config_Reset;
  assign Reg6config__Config_Reset = Config_Reset;
  assign Reg7config__Config_Reset = Config_Reset;
  assign RegAConfig__Config_Reset = Config_Reset;
  assign RegBConfig__Config_Reset = Config_Reset;
  assign crossbar__in6[31:0] = ALU__out[31:0];
  assign RES__in[31:0] = ALU__out[31:0];
  assign out0[31:0] = crossbar__out0[31:0];
  assign out1[31:0] = crossbar__out1[31:0];
  assign out2[31:0] = crossbar__out2[31:0];
  assign out3[31:0] = crossbar__out3[31:0];
  assign out6[31:0] = crossbar__out4[31:0];
  assign out7[31:0] = crossbar__out5[31:0];
  assign rega__in[31:0] = crossbar__out6[31:0];
  assign regb__in[31:0] = crossbar__out7[31:0];
  assign crossbar__in7[31:0] = RES__out[31:0];
  assign alu_muxa__in0[31:0] = rega__out[31:0];
  assign alu_muxb__in0[31:0] = regb__out[31:0];
  assign alu_muxa__in1[31:0] = const__out[31:0];
  assign alu_muxb__in1[31:0] = const__out[31:0];
  assign ALU__in_a[31:0] = alu_muxa__out[31:0];
  assign ALU__in_b[31:0] = alu_muxb__out[31:0];
  assign mux_0__in1[31:0] = reg0__out[31:0];
  assign crossbar__in0[31:0] = mux_0__out[31:0];
  assign mux_1__in1[31:0] = reg1__out[31:0];
  assign crossbar__in1[31:0] = mux_1__out[31:0];
  assign mux_2__in1[31:0] = reg2__out[31:0];
  assign crossbar__in2[31:0] = mux_2__out[31:0];
  assign mux_3__in1[31:0] = reg3__out[31:0];
  assign crossbar__in3[31:0] = mux_3__out[31:0];
  assign mux_6__in1[31:0] = reg6__out[31:0];
  assign crossbar__in4[31:0] = mux_6__out[31:0];
  assign mux_7__in1[31:0] = reg7__out[31:0];
  assign crossbar__in5[31:0] = mux_7__out[31:0];
  assign mux_0__in0[31:0] = in0[31:0];
  assign reg0__in[31:0] = in0[31:0];
  assign mux_1__in0[31:0] = in1[31:0];
  assign reg1__in[31:0] = in1[31:0];
  assign mux_2__in0[31:0] = in2[31:0];
  assign reg2__in[31:0] = in2[31:0];
  assign mux_3__in0[31:0] = in3[31:0];
  assign reg3__in[31:0] = in3[31:0];
  assign mux_6__in0[31:0] = in6[31:0];
  assign reg6__in[31:0] = in6[31:0];
  assign mux_7__in0[31:0] = in7[31:0];
  assign reg7__in[31:0] = in7[31:0];
  assign alu_muxa__select[0:0] = ALUMuxAconfig__select[0:0];
  assign alu_muxb__select[0:0] = ALUMuxBconfig__select[0:0];
  assign ALU__select[3:0] = ALUconfig__select[3:0];
  assign mux_0__select[0:0] = Mux0config__select[0:0];
  assign mux_1__select[0:0] = Mux1config__select[0:0];
  assign mux_2__select[0:0] = Mux2config__select[0:0];
  assign mux_3__select[0:0] = Mux3config__select[0:0];
  assign mux_6__select[0:0] = Mux6config__select[0:0];
  assign mux_7__select[0:0] = Mux7config__select[0:0];
  assign RES__enable[0:0] = RESConfig__select[0:0];
  assign reg0__enable[0:0] = Reg0config__select[0:0];
  assign reg1__enable[0:0] = Reg1config__select[0:0];
  assign reg2__enable[0:0] = Reg2config__select[0:0];
  assign reg3__enable[0:0] = Reg3config__select[0:0];
  assign reg6__enable[0:0] = Reg6config__select[0:0];
  assign reg7__enable[0:0] = Reg7config__select[0:0];
  assign regb__enable[0:0] = RegAConfig__select[0:0];
  assign rega__enable[0:0] = RegBConfig__select[0:0];
  assign ALUMuxAconfig__ConfigIn = ConfigIn;
  assign ALUMuxBconfig__ConfigIn = ALUMuxAconfig__ConfigOut;
  assign ALUconfig__ConfigIn = ALUMuxBconfig__ConfigOut;
  assign Mux0config__ConfigIn = ALUconfig__ConfigOut;
  assign Mux1config__ConfigIn = Mux0config__ConfigOut;
  assign Mux2config__ConfigIn = Mux1config__ConfigOut;
  assign Mux3config__ConfigIn = Mux2config__ConfigOut;
  assign Mux6config__ConfigIn = Mux3config__ConfigOut;
  assign Mux7config__ConfigIn = Mux6config__ConfigOut;
  assign RESConfig__ConfigIn = Mux7config__ConfigOut;
  assign Reg0config__ConfigIn = RESConfig__ConfigOut;
  assign Reg1config__ConfigIn = Reg0config__ConfigOut;
  assign Reg2config__ConfigIn = Reg1config__ConfigOut;
  assign Reg3config__ConfigIn = Reg2config__ConfigOut;
  assign Reg6config__ConfigIn = Reg3config__ConfigOut;
  assign Reg7config__ConfigIn = Reg6config__ConfigOut;
  assign RegAConfig__ConfigIn = Reg7config__ConfigOut;
  assign RegBConfig__ConfigIn = RegAConfig__ConfigOut;
  assign const__ConfigIn = RegBConfig__ConfigOut;
  assign crossbar__ConfigIn = const__ConfigOut;
  assign ConfigOut = crossbar__ConfigOut;

endmodule //hycube_no_pred_in207_out207_U6

module cgrame_tristate_32b #(parameter contexts=1, parameter size=1) (
  enable,
  in,
  out,
  bidir_in,
  bidir_out
);
    input [0:0] enable;
    input [size-1:0] in;
    output [size-1:0] out;
    input [size-1:0] bidir_in;
    output [size-1:0] bidir_out;
    assign out = enable ? in : bidir_in;
    assign bidir_out = out;

endmodule //cgrame_tristate_32b

module io_32b_U1 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] IOPin_bidir_in,
  output [31:0] IOPin_bidir_out,
  input [31:0] in,
  output [31:0] out
);
  //Wire declarations for instance 'IOPin' (Module cgrame_tristate_32b)
  wire [31:0] IOPin__bidir_in;
  wire [31:0] IOPin__bidir_out;
  wire [0:0] IOPin__enable;
  wire [31:0] IOPin__in;
  wire [31:0] IOPin__out;
  cgrame_tristate_32b #(.contexts(1),.size(32)) IOPin(
    .bidir_in(IOPin__bidir_in),
    .bidir_out(IOPin__bidir_out),
    .enable(IOPin__enable),
    .in(IOPin__in),
    .out(IOPin__out)
  );

  //Wire declarations for instance 'IOPinConfig' (Module cgrame_configcell_context1_size1)
  wire  IOPinConfig__CGRA_Clock;
  wire  IOPinConfig__CGRA_Enable;
  wire  IOPinConfig__CGRA_Reset;
  wire  IOPinConfig__ConfigIn;
  wire  IOPinConfig__ConfigOut;
  wire  IOPinConfig__Config_Clock;
  wire  IOPinConfig__Config_Reset;
  wire [0:0] IOPinConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) IOPinConfig(
    .CGRA_Clock(IOPinConfig__CGRA_Clock),
    .CGRA_Enable(IOPinConfig__CGRA_Enable),
    .CGRA_Reset(IOPinConfig__CGRA_Reset),
    .ConfigIn(IOPinConfig__ConfigIn),
    .ConfigOut(IOPinConfig__ConfigOut),
    .Config_Clock(IOPinConfig__Config_Clock),
    .Config_Reset(IOPinConfig__Config_Reset),
    .select(IOPinConfig__select)
  );

  //Wire declarations for instance 'RegInConfig' (Module cgrame_configcell_context1_size1)
  wire  RegInConfig__CGRA_Clock;
  wire  RegInConfig__CGRA_Enable;
  wire  RegInConfig__CGRA_Reset;
  wire  RegInConfig__ConfigIn;
  wire  RegInConfig__ConfigOut;
  wire  RegInConfig__Config_Clock;
  wire  RegInConfig__Config_Reset;
  wire [0:0] RegInConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegInConfig(
    .CGRA_Clock(RegInConfig__CGRA_Clock),
    .CGRA_Enable(RegInConfig__CGRA_Enable),
    .CGRA_Reset(RegInConfig__CGRA_Reset),
    .ConfigIn(RegInConfig__ConfigIn),
    .ConfigOut(RegInConfig__ConfigOut),
    .Config_Clock(RegInConfig__Config_Clock),
    .Config_Reset(RegInConfig__Config_Reset),
    .select(RegInConfig__select)
  );

  //Wire declarations for instance 'RegOutConfig' (Module cgrame_configcell_context1_size1)
  wire  RegOutConfig__CGRA_Clock;
  wire  RegOutConfig__CGRA_Enable;
  wire  RegOutConfig__CGRA_Reset;
  wire  RegOutConfig__ConfigIn;
  wire  RegOutConfig__ConfigOut;
  wire  RegOutConfig__Config_Clock;
  wire  RegOutConfig__Config_Reset;
  wire [0:0] RegOutConfig__select;
  cgrame_configcell_context1_size1 #(.contexts(1),.size(1)) RegOutConfig(
    .CGRA_Clock(RegOutConfig__CGRA_Clock),
    .CGRA_Enable(RegOutConfig__CGRA_Enable),
    .CGRA_Reset(RegOutConfig__CGRA_Reset),
    .ConfigIn(RegOutConfig__ConfigIn),
    .ConfigOut(RegOutConfig__ConfigOut),
    .Config_Clock(RegOutConfig__Config_Clock),
    .Config_Reset(RegOutConfig__Config_Reset),
    .select(RegOutConfig__select)
  );

  //Wire declarations for instance 'reg_in' (Module cgrame_register_32b)
  wire  reg_in__CGRA_Clock;
  wire  reg_in__CGRA_Enable;
  wire  reg_in__CGRA_Reset;
  wire [0:0] reg_in__enable;
  wire [31:0] reg_in__in;
  wire [31:0] reg_in__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg_in(
    .CGRA_Clock(reg_in__CGRA_Clock),
    .CGRA_Enable(reg_in__CGRA_Enable),
    .CGRA_Reset(reg_in__CGRA_Reset),
    .enable(reg_in__enable),
    .in(reg_in__in),
    .out(reg_in__out)
  );

  //Wire declarations for instance 'reg_out' (Module cgrame_register_32b)
  wire  reg_out__CGRA_Clock;
  wire  reg_out__CGRA_Enable;
  wire  reg_out__CGRA_Reset;
  wire [0:0] reg_out__enable;
  wire [31:0] reg_out__in;
  wire [31:0] reg_out__out;
  cgrame_register_32b #(.contexts(1),.size(32)) reg_out(
    .CGRA_Clock(reg_out__CGRA_Clock),
    .CGRA_Enable(reg_out__CGRA_Enable),
    .CGRA_Reset(reg_out__CGRA_Reset),
    .enable(reg_out__enable),
    .in(reg_out__in),
    .out(reg_out__out)
  );

  //All the connections
  assign reg_in__CGRA_Reset = CGRA_Reset;
  assign reg_in__CGRA_Clock = CGRA_Clock;
  assign reg_out__CGRA_Clock = CGRA_Clock;
  assign IOPinConfig__CGRA_Clock = CGRA_Clock;
  assign RegInConfig__CGRA_Clock = CGRA_Clock;
  assign RegOutConfig__CGRA_Clock = CGRA_Clock;
  assign reg_in__CGRA_Enable = CGRA_Enable;
  assign reg_out__CGRA_Reset = CGRA_Reset;
  assign IOPinConfig__CGRA_Reset = CGRA_Reset;
  assign RegInConfig__CGRA_Reset = CGRA_Reset;
  assign RegOutConfig__CGRA_Reset = CGRA_Reset;
  assign reg_out__CGRA_Enable = CGRA_Enable;
  assign IOPinConfig__CGRA_Enable = CGRA_Enable;
  assign RegInConfig__CGRA_Enable = CGRA_Enable;
  assign RegOutConfig__CGRA_Enable = CGRA_Enable;
  assign IOPinConfig__Config_Clock = Config_Clock;
  assign IOPinConfig__Config_Reset = Config_Reset;
  assign RegInConfig__Config_Clock = Config_Clock;
  assign RegOutConfig__Config_Clock = Config_Clock;
  assign RegInConfig__Config_Reset = Config_Reset;
  assign RegOutConfig__Config_Reset = Config_Reset;
  assign reg_in__in[31:0] = in[31:0];
  assign IOPin__in[31:0] = reg_in__out[31:0];
  assign out[31:0] = reg_out__out[31:0];
  assign reg_out__in[31:0] = IOPin__out[31:0];
  assign IOPin__enable[0:0] = IOPinConfig__select[0:0];
  assign reg_in__enable[0:0] = RegInConfig__select[0:0];
  assign reg_out__enable[0:0] = RegOutConfig__select[0:0];
  assign IOPinConfig__ConfigIn = ConfigIn;
  assign RegInConfig__ConfigIn = IOPinConfig__ConfigOut;
  assign RegOutConfig__ConfigIn = RegInConfig__ConfigOut;
  assign ConfigOut = RegOutConfig__ConfigOut;
  assign IOPin__bidir_in[31:0] = IOPin_bidir_in[31:0];
  assign IOPin_bidir_out[31:0] = IOPin__bidir_out[31:0];

endmodule //io_32b_U1

module cgra_U0 (
  input  CGRA_Clock,
  input  CGRA_Enable,
  input  CGRA_Reset,
  input  ConfigIn,
  output  ConfigOut,
  input  Config_Clock,
  input  Config_Reset,
  input [31:0] io_bottom_0_IOPin_bidir_in,
  output [31:0] io_bottom_0_IOPin_bidir_out,
  input [31:0] io_bottom_1_IOPin_bidir_in,
  output [31:0] io_bottom_1_IOPin_bidir_out,
  input [31:0] io_bottom_2_IOPin_bidir_in,
  output [31:0] io_bottom_2_IOPin_bidir_out,
  input [31:0] io_bottom_3_IOPin_bidir_in,
  output [31:0] io_bottom_3_IOPin_bidir_out,
  input [31:0] io_right_0_IOPin_bidir_in,
  output [31:0] io_right_0_IOPin_bidir_out,
  input [31:0] io_right_1_IOPin_bidir_in,
  output [31:0] io_right_1_IOPin_bidir_out,
  input [31:0] io_right_2_IOPin_bidir_in,
  output [31:0] io_right_2_IOPin_bidir_out,
  input [31:0] io_right_3_IOPin_bidir_in,
  output [31:0] io_right_3_IOPin_bidir_out,
  input [31:0] io_top_0_IOPin_bidir_in,
  output [31:0] io_top_0_IOPin_bidir_out,
  input [31:0] io_top_1_IOPin_bidir_in,
  output [31:0] io_top_1_IOPin_bidir_out,
  input [31:0] io_top_2_IOPin_bidir_in,
  output [31:0] io_top_2_IOPin_bidir_out,
  input [31:0] io_top_3_IOPin_bidir_in,
  output [31:0] io_top_3_IOPin_bidir_out,
  output [31:0] mem_0_mem_unit_addr_to_ram,
  output [31:0] mem_0_mem_unit_data_in_to_ram,
  input [31:0] mem_0_mem_unit_data_out_from_ram,
  output [0:0] mem_0_mem_unit_w_rq_to_ram,
  output [31:0] mem_1_mem_unit_addr_to_ram,
  output [31:0] mem_1_mem_unit_data_in_to_ram,
  input [31:0] mem_1_mem_unit_data_out_from_ram,
  output [0:0] mem_1_mem_unit_w_rq_to_ram,
  output [31:0] mem_2_mem_unit_addr_to_ram,
  output [31:0] mem_2_mem_unit_data_in_to_ram,
  input [31:0] mem_2_mem_unit_data_out_from_ram,
  output [0:0] mem_2_mem_unit_w_rq_to_ram,
  output [31:0] mem_3_mem_unit_addr_to_ram,
  output [31:0] mem_3_mem_unit_data_in_to_ram,
  input [31:0] mem_3_mem_unit_data_out_from_ram,
  output [0:0] mem_3_mem_unit_w_rq_to_ram
);
  //Wire declarations for instance 'io_bottom_0' (Module io_32b_U1)
  wire  io_bottom_0__CGRA_Clock;
  wire  io_bottom_0__CGRA_Enable;
  wire  io_bottom_0__CGRA_Reset;
  wire  io_bottom_0__ConfigIn;
  wire  io_bottom_0__ConfigOut;
  wire  io_bottom_0__Config_Clock;
  wire  io_bottom_0__Config_Reset;
  wire [31:0] io_bottom_0__IOPin_bidir_in;
  wire [31:0] io_bottom_0__IOPin_bidir_out;
  wire [31:0] io_bottom_0__in;
  wire [31:0] io_bottom_0__out;
  io_32b_U1 io_bottom_0(
    .CGRA_Clock(io_bottom_0__CGRA_Clock),
    .CGRA_Enable(io_bottom_0__CGRA_Enable),
    .CGRA_Reset(io_bottom_0__CGRA_Reset),
    .ConfigIn(io_bottom_0__ConfigIn),
    .ConfigOut(io_bottom_0__ConfigOut),
    .Config_Clock(io_bottom_0__Config_Clock),
    .Config_Reset(io_bottom_0__Config_Reset),
    .IOPin_bidir_in(io_bottom_0__IOPin_bidir_in),
    .IOPin_bidir_out(io_bottom_0__IOPin_bidir_out),
    .in(io_bottom_0__in),
    .out(io_bottom_0__out)
  );

  //Wire declarations for instance 'io_bottom_1' (Module io_32b_U1)
  wire  io_bottom_1__CGRA_Clock;
  wire  io_bottom_1__CGRA_Enable;
  wire  io_bottom_1__CGRA_Reset;
  wire  io_bottom_1__ConfigIn;
  wire  io_bottom_1__ConfigOut;
  wire  io_bottom_1__Config_Clock;
  wire  io_bottom_1__Config_Reset;
  wire [31:0] io_bottom_1__IOPin_bidir_in;
  wire [31:0] io_bottom_1__IOPin_bidir_out;
  wire [31:0] io_bottom_1__in;
  wire [31:0] io_bottom_1__out;
  io_32b_U1 io_bottom_1(
    .CGRA_Clock(io_bottom_1__CGRA_Clock),
    .CGRA_Enable(io_bottom_1__CGRA_Enable),
    .CGRA_Reset(io_bottom_1__CGRA_Reset),
    .ConfigIn(io_bottom_1__ConfigIn),
    .ConfigOut(io_bottom_1__ConfigOut),
    .Config_Clock(io_bottom_1__Config_Clock),
    .Config_Reset(io_bottom_1__Config_Reset),
    .IOPin_bidir_in(io_bottom_1__IOPin_bidir_in),
    .IOPin_bidir_out(io_bottom_1__IOPin_bidir_out),
    .in(io_bottom_1__in),
    .out(io_bottom_1__out)
  );

  //Wire declarations for instance 'io_bottom_2' (Module io_32b_U1)
  wire  io_bottom_2__CGRA_Clock;
  wire  io_bottom_2__CGRA_Enable;
  wire  io_bottom_2__CGRA_Reset;
  wire  io_bottom_2__ConfigIn;
  wire  io_bottom_2__ConfigOut;
  wire  io_bottom_2__Config_Clock;
  wire  io_bottom_2__Config_Reset;
  wire [31:0] io_bottom_2__IOPin_bidir_in;
  wire [31:0] io_bottom_2__IOPin_bidir_out;
  wire [31:0] io_bottom_2__in;
  wire [31:0] io_bottom_2__out;
  io_32b_U1 io_bottom_2(
    .CGRA_Clock(io_bottom_2__CGRA_Clock),
    .CGRA_Enable(io_bottom_2__CGRA_Enable),
    .CGRA_Reset(io_bottom_2__CGRA_Reset),
    .ConfigIn(io_bottom_2__ConfigIn),
    .ConfigOut(io_bottom_2__ConfigOut),
    .Config_Clock(io_bottom_2__Config_Clock),
    .Config_Reset(io_bottom_2__Config_Reset),
    .IOPin_bidir_in(io_bottom_2__IOPin_bidir_in),
    .IOPin_bidir_out(io_bottom_2__IOPin_bidir_out),
    .in(io_bottom_2__in),
    .out(io_bottom_2__out)
  );

  //Wire declarations for instance 'io_bottom_3' (Module io_32b_U1)
  wire  io_bottom_3__CGRA_Clock;
  wire  io_bottom_3__CGRA_Enable;
  wire  io_bottom_3__CGRA_Reset;
  wire  io_bottom_3__ConfigIn;
  wire  io_bottom_3__ConfigOut;
  wire  io_bottom_3__Config_Clock;
  wire  io_bottom_3__Config_Reset;
  wire [31:0] io_bottom_3__IOPin_bidir_in;
  wire [31:0] io_bottom_3__IOPin_bidir_out;
  wire [31:0] io_bottom_3__in;
  wire [31:0] io_bottom_3__out;
  io_32b_U1 io_bottom_3(
    .CGRA_Clock(io_bottom_3__CGRA_Clock),
    .CGRA_Enable(io_bottom_3__CGRA_Enable),
    .CGRA_Reset(io_bottom_3__CGRA_Reset),
    .ConfigIn(io_bottom_3__ConfigIn),
    .ConfigOut(io_bottom_3__ConfigOut),
    .Config_Clock(io_bottom_3__Config_Clock),
    .Config_Reset(io_bottom_3__Config_Reset),
    .IOPin_bidir_in(io_bottom_3__IOPin_bidir_in),
    .IOPin_bidir_out(io_bottom_3__IOPin_bidir_out),
    .in(io_bottom_3__in),
    .out(io_bottom_3__out)
  );

  //Wire declarations for instance 'io_right_0' (Module io_32b_U1)
  wire  io_right_0__CGRA_Clock;
  wire  io_right_0__CGRA_Enable;
  wire  io_right_0__CGRA_Reset;
  wire  io_right_0__ConfigIn;
  wire  io_right_0__ConfigOut;
  wire  io_right_0__Config_Clock;
  wire  io_right_0__Config_Reset;
  wire [31:0] io_right_0__IOPin_bidir_in;
  wire [31:0] io_right_0__IOPin_bidir_out;
  wire [31:0] io_right_0__in;
  wire [31:0] io_right_0__out;
  io_32b_U1 io_right_0(
    .CGRA_Clock(io_right_0__CGRA_Clock),
    .CGRA_Enable(io_right_0__CGRA_Enable),
    .CGRA_Reset(io_right_0__CGRA_Reset),
    .ConfigIn(io_right_0__ConfigIn),
    .ConfigOut(io_right_0__ConfigOut),
    .Config_Clock(io_right_0__Config_Clock),
    .Config_Reset(io_right_0__Config_Reset),
    .IOPin_bidir_in(io_right_0__IOPin_bidir_in),
    .IOPin_bidir_out(io_right_0__IOPin_bidir_out),
    .in(io_right_0__in),
    .out(io_right_0__out)
  );

  //Wire declarations for instance 'io_right_1' (Module io_32b_U1)
  wire  io_right_1__CGRA_Clock;
  wire  io_right_1__CGRA_Enable;
  wire  io_right_1__CGRA_Reset;
  wire  io_right_1__ConfigIn;
  wire  io_right_1__ConfigOut;
  wire  io_right_1__Config_Clock;
  wire  io_right_1__Config_Reset;
  wire [31:0] io_right_1__IOPin_bidir_in;
  wire [31:0] io_right_1__IOPin_bidir_out;
  wire [31:0] io_right_1__in;
  wire [31:0] io_right_1__out;
  io_32b_U1 io_right_1(
    .CGRA_Clock(io_right_1__CGRA_Clock),
    .CGRA_Enable(io_right_1__CGRA_Enable),
    .CGRA_Reset(io_right_1__CGRA_Reset),
    .ConfigIn(io_right_1__ConfigIn),
    .ConfigOut(io_right_1__ConfigOut),
    .Config_Clock(io_right_1__Config_Clock),
    .Config_Reset(io_right_1__Config_Reset),
    .IOPin_bidir_in(io_right_1__IOPin_bidir_in),
    .IOPin_bidir_out(io_right_1__IOPin_bidir_out),
    .in(io_right_1__in),
    .out(io_right_1__out)
  );

  //Wire declarations for instance 'io_right_2' (Module io_32b_U1)
  wire  io_right_2__CGRA_Clock;
  wire  io_right_2__CGRA_Enable;
  wire  io_right_2__CGRA_Reset;
  wire  io_right_2__ConfigIn;
  wire  io_right_2__ConfigOut;
  wire  io_right_2__Config_Clock;
  wire  io_right_2__Config_Reset;
  wire [31:0] io_right_2__IOPin_bidir_in;
  wire [31:0] io_right_2__IOPin_bidir_out;
  wire [31:0] io_right_2__in;
  wire [31:0] io_right_2__out;
  io_32b_U1 io_right_2(
    .CGRA_Clock(io_right_2__CGRA_Clock),
    .CGRA_Enable(io_right_2__CGRA_Enable),
    .CGRA_Reset(io_right_2__CGRA_Reset),
    .ConfigIn(io_right_2__ConfigIn),
    .ConfigOut(io_right_2__ConfigOut),
    .Config_Clock(io_right_2__Config_Clock),
    .Config_Reset(io_right_2__Config_Reset),
    .IOPin_bidir_in(io_right_2__IOPin_bidir_in),
    .IOPin_bidir_out(io_right_2__IOPin_bidir_out),
    .in(io_right_2__in),
    .out(io_right_2__out)
  );

  //Wire declarations for instance 'io_right_3' (Module io_32b_U1)
  wire  io_right_3__CGRA_Clock;
  wire  io_right_3__CGRA_Enable;
  wire  io_right_3__CGRA_Reset;
  wire  io_right_3__ConfigIn;
  wire  io_right_3__ConfigOut;
  wire  io_right_3__Config_Clock;
  wire  io_right_3__Config_Reset;
  wire [31:0] io_right_3__IOPin_bidir_in;
  wire [31:0] io_right_3__IOPin_bidir_out;
  wire [31:0] io_right_3__in;
  wire [31:0] io_right_3__out;
  io_32b_U1 io_right_3(
    .CGRA_Clock(io_right_3__CGRA_Clock),
    .CGRA_Enable(io_right_3__CGRA_Enable),
    .CGRA_Reset(io_right_3__CGRA_Reset),
    .ConfigIn(io_right_3__ConfigIn),
    .ConfigOut(io_right_3__ConfigOut),
    .Config_Clock(io_right_3__Config_Clock),
    .Config_Reset(io_right_3__Config_Reset),
    .IOPin_bidir_in(io_right_3__IOPin_bidir_in),
    .IOPin_bidir_out(io_right_3__IOPin_bidir_out),
    .in(io_right_3__in),
    .out(io_right_3__out)
  );

  //Wire declarations for instance 'io_top_0' (Module io_32b_U1)
  wire  io_top_0__CGRA_Clock;
  wire  io_top_0__CGRA_Enable;
  wire  io_top_0__CGRA_Reset;
  wire  io_top_0__ConfigIn;
  wire  io_top_0__ConfigOut;
  wire  io_top_0__Config_Clock;
  wire  io_top_0__Config_Reset;
  wire [31:0] io_top_0__IOPin_bidir_in;
  wire [31:0] io_top_0__IOPin_bidir_out;
  wire [31:0] io_top_0__in;
  wire [31:0] io_top_0__out;
  io_32b_U1 io_top_0(
    .CGRA_Clock(io_top_0__CGRA_Clock),
    .CGRA_Enable(io_top_0__CGRA_Enable),
    .CGRA_Reset(io_top_0__CGRA_Reset),
    .ConfigIn(io_top_0__ConfigIn),
    .ConfigOut(io_top_0__ConfigOut),
    .Config_Clock(io_top_0__Config_Clock),
    .Config_Reset(io_top_0__Config_Reset),
    .IOPin_bidir_in(io_top_0__IOPin_bidir_in),
    .IOPin_bidir_out(io_top_0__IOPin_bidir_out),
    .in(io_top_0__in),
    .out(io_top_0__out)
  );

  //Wire declarations for instance 'io_top_1' (Module io_32b_U1)
  wire  io_top_1__CGRA_Clock;
  wire  io_top_1__CGRA_Enable;
  wire  io_top_1__CGRA_Reset;
  wire  io_top_1__ConfigIn;
  wire  io_top_1__ConfigOut;
  wire  io_top_1__Config_Clock;
  wire  io_top_1__Config_Reset;
  wire [31:0] io_top_1__IOPin_bidir_in;
  wire [31:0] io_top_1__IOPin_bidir_out;
  wire [31:0] io_top_1__in;
  wire [31:0] io_top_1__out;
  io_32b_U1 io_top_1(
    .CGRA_Clock(io_top_1__CGRA_Clock),
    .CGRA_Enable(io_top_1__CGRA_Enable),
    .CGRA_Reset(io_top_1__CGRA_Reset),
    .ConfigIn(io_top_1__ConfigIn),
    .ConfigOut(io_top_1__ConfigOut),
    .Config_Clock(io_top_1__Config_Clock),
    .Config_Reset(io_top_1__Config_Reset),
    .IOPin_bidir_in(io_top_1__IOPin_bidir_in),
    .IOPin_bidir_out(io_top_1__IOPin_bidir_out),
    .in(io_top_1__in),
    .out(io_top_1__out)
  );

  //Wire declarations for instance 'io_top_2' (Module io_32b_U1)
  wire  io_top_2__CGRA_Clock;
  wire  io_top_2__CGRA_Enable;
  wire  io_top_2__CGRA_Reset;
  wire  io_top_2__ConfigIn;
  wire  io_top_2__ConfigOut;
  wire  io_top_2__Config_Clock;
  wire  io_top_2__Config_Reset;
  wire [31:0] io_top_2__IOPin_bidir_in;
  wire [31:0] io_top_2__IOPin_bidir_out;
  wire [31:0] io_top_2__in;
  wire [31:0] io_top_2__out;
  io_32b_U1 io_top_2(
    .CGRA_Clock(io_top_2__CGRA_Clock),
    .CGRA_Enable(io_top_2__CGRA_Enable),
    .CGRA_Reset(io_top_2__CGRA_Reset),
    .ConfigIn(io_top_2__ConfigIn),
    .ConfigOut(io_top_2__ConfigOut),
    .Config_Clock(io_top_2__Config_Clock),
    .Config_Reset(io_top_2__Config_Reset),
    .IOPin_bidir_in(io_top_2__IOPin_bidir_in),
    .IOPin_bidir_out(io_top_2__IOPin_bidir_out),
    .in(io_top_2__in),
    .out(io_top_2__out)
  );

  //Wire declarations for instance 'io_top_3' (Module io_32b_U1)
  wire  io_top_3__CGRA_Clock;
  wire  io_top_3__CGRA_Enable;
  wire  io_top_3__CGRA_Reset;
  wire  io_top_3__ConfigIn;
  wire  io_top_3__ConfigOut;
  wire  io_top_3__Config_Clock;
  wire  io_top_3__Config_Reset;
  wire [31:0] io_top_3__IOPin_bidir_in;
  wire [31:0] io_top_3__IOPin_bidir_out;
  wire [31:0] io_top_3__in;
  wire [31:0] io_top_3__out;
  io_32b_U1 io_top_3(
    .CGRA_Clock(io_top_3__CGRA_Clock),
    .CGRA_Enable(io_top_3__CGRA_Enable),
    .CGRA_Reset(io_top_3__CGRA_Reset),
    .ConfigIn(io_top_3__ConfigIn),
    .ConfigOut(io_top_3__ConfigOut),
    .Config_Clock(io_top_3__Config_Clock),
    .Config_Reset(io_top_3__Config_Reset),
    .IOPin_bidir_in(io_top_3__IOPin_bidir_in),
    .IOPin_bidir_out(io_top_3__IOPin_bidir_out),
    .in(io_top_3__in),
    .out(io_top_3__out)
  );

  //Wire declarations for instance 'mem_0' (Module memoryPort_4connect_32b_U2)
  wire  mem_0__CGRA_Clock;
  wire  mem_0__CGRA_Enable;
  wire  mem_0__CGRA_Reset;
  wire  mem_0__ConfigIn;
  wire  mem_0__ConfigOut;
  wire  mem_0__Config_Clock;
  wire  mem_0__Config_Reset;
  wire [31:0] mem_0__in0;
  wire [31:0] mem_0__in1;
  wire [31:0] mem_0__in2;
  wire [31:0] mem_0__in3;
  wire [31:0] mem_0__mem_unit_addr_to_ram;
  wire [31:0] mem_0__mem_unit_data_in_to_ram;
  wire [31:0] mem_0__mem_unit_data_out_from_ram;
  wire [0:0] mem_0__mem_unit_w_rq_to_ram;
  wire [31:0] mem_0__out;
  memoryPort_4connect_32b_U2 mem_0(
    .CGRA_Clock(mem_0__CGRA_Clock),
    .CGRA_Enable(mem_0__CGRA_Enable),
    .CGRA_Reset(mem_0__CGRA_Reset),
    .ConfigIn(mem_0__ConfigIn),
    .ConfigOut(mem_0__ConfigOut),
    .Config_Clock(mem_0__Config_Clock),
    .Config_Reset(mem_0__Config_Reset),
    .in0(mem_0__in0),
    .in1(mem_0__in1),
    .in2(mem_0__in2),
    .in3(mem_0__in3),
    .mem_unit_addr_to_ram(mem_0__mem_unit_addr_to_ram),
    .mem_unit_data_in_to_ram(mem_0__mem_unit_data_in_to_ram),
    .mem_unit_data_out_from_ram(mem_0__mem_unit_data_out_from_ram),
    .mem_unit_w_rq_to_ram(mem_0__mem_unit_w_rq_to_ram),
    .out(mem_0__out)
  );

  //Wire declarations for instance 'mem_1' (Module memoryPort_4connect_32b_U2)
  wire  mem_1__CGRA_Clock;
  wire  mem_1__CGRA_Enable;
  wire  mem_1__CGRA_Reset;
  wire  mem_1__ConfigIn;
  wire  mem_1__ConfigOut;
  wire  mem_1__Config_Clock;
  wire  mem_1__Config_Reset;
  wire [31:0] mem_1__in0;
  wire [31:0] mem_1__in1;
  wire [31:0] mem_1__in2;
  wire [31:0] mem_1__in3;
  wire [31:0] mem_1__mem_unit_addr_to_ram;
  wire [31:0] mem_1__mem_unit_data_in_to_ram;
  wire [31:0] mem_1__mem_unit_data_out_from_ram;
  wire [0:0] mem_1__mem_unit_w_rq_to_ram;
  wire [31:0] mem_1__out;
  memoryPort_4connect_32b_U2 mem_1(
    .CGRA_Clock(mem_1__CGRA_Clock),
    .CGRA_Enable(mem_1__CGRA_Enable),
    .CGRA_Reset(mem_1__CGRA_Reset),
    .ConfigIn(mem_1__ConfigIn),
    .ConfigOut(mem_1__ConfigOut),
    .Config_Clock(mem_1__Config_Clock),
    .Config_Reset(mem_1__Config_Reset),
    .in0(mem_1__in0),
    .in1(mem_1__in1),
    .in2(mem_1__in2),
    .in3(mem_1__in3),
    .mem_unit_addr_to_ram(mem_1__mem_unit_addr_to_ram),
    .mem_unit_data_in_to_ram(mem_1__mem_unit_data_in_to_ram),
    .mem_unit_data_out_from_ram(mem_1__mem_unit_data_out_from_ram),
    .mem_unit_w_rq_to_ram(mem_1__mem_unit_w_rq_to_ram),
    .out(mem_1__out)
  );

  //Wire declarations for instance 'mem_2' (Module memoryPort_4connect_32b_U2)
  wire  mem_2__CGRA_Clock;
  wire  mem_2__CGRA_Enable;
  wire  mem_2__CGRA_Reset;
  wire  mem_2__ConfigIn;
  wire  mem_2__ConfigOut;
  wire  mem_2__Config_Clock;
  wire  mem_2__Config_Reset;
  wire [31:0] mem_2__in0;
  wire [31:0] mem_2__in1;
  wire [31:0] mem_2__in2;
  wire [31:0] mem_2__in3;
  wire [31:0] mem_2__mem_unit_addr_to_ram;
  wire [31:0] mem_2__mem_unit_data_in_to_ram;
  wire [31:0] mem_2__mem_unit_data_out_from_ram;
  wire [0:0] mem_2__mem_unit_w_rq_to_ram;
  wire [31:0] mem_2__out;
  memoryPort_4connect_32b_U2 mem_2(
    .CGRA_Clock(mem_2__CGRA_Clock),
    .CGRA_Enable(mem_2__CGRA_Enable),
    .CGRA_Reset(mem_2__CGRA_Reset),
    .ConfigIn(mem_2__ConfigIn),
    .ConfigOut(mem_2__ConfigOut),
    .Config_Clock(mem_2__Config_Clock),
    .Config_Reset(mem_2__Config_Reset),
    .in0(mem_2__in0),
    .in1(mem_2__in1),
    .in2(mem_2__in2),
    .in3(mem_2__in3),
    .mem_unit_addr_to_ram(mem_2__mem_unit_addr_to_ram),
    .mem_unit_data_in_to_ram(mem_2__mem_unit_data_in_to_ram),
    .mem_unit_data_out_from_ram(mem_2__mem_unit_data_out_from_ram),
    .mem_unit_w_rq_to_ram(mem_2__mem_unit_w_rq_to_ram),
    .out(mem_2__out)
  );

  //Wire declarations for instance 'mem_3' (Module memoryPort_4connect_32b_U2)
  wire  mem_3__CGRA_Clock;
  wire  mem_3__CGRA_Enable;
  wire  mem_3__CGRA_Reset;
  wire  mem_3__ConfigIn;
  wire  mem_3__ConfigOut;
  wire  mem_3__Config_Clock;
  wire  mem_3__Config_Reset;
  wire [31:0] mem_3__in0;
  wire [31:0] mem_3__in1;
  wire [31:0] mem_3__in2;
  wire [31:0] mem_3__in3;
  wire [31:0] mem_3__mem_unit_addr_to_ram;
  wire [31:0] mem_3__mem_unit_data_in_to_ram;
  wire [31:0] mem_3__mem_unit_data_out_from_ram;
  wire [0:0] mem_3__mem_unit_w_rq_to_ram;
  wire [31:0] mem_3__out;
  memoryPort_4connect_32b_U2 mem_3(
    .CGRA_Clock(mem_3__CGRA_Clock),
    .CGRA_Enable(mem_3__CGRA_Enable),
    .CGRA_Reset(mem_3__CGRA_Reset),
    .ConfigIn(mem_3__ConfigIn),
    .ConfigOut(mem_3__ConfigOut),
    .Config_Clock(mem_3__Config_Clock),
    .Config_Reset(mem_3__Config_Reset),
    .in0(mem_3__in0),
    .in1(mem_3__in1),
    .in2(mem_3__in2),
    .in3(mem_3__in3),
    .mem_unit_addr_to_ram(mem_3__mem_unit_addr_to_ram),
    .mem_unit_data_in_to_ram(mem_3__mem_unit_data_in_to_ram),
    .mem_unit_data_out_from_ram(mem_3__mem_unit_data_out_from_ram),
    .mem_unit_w_rq_to_ram(mem_3__mem_unit_w_rq_to_ram),
    .out(mem_3__out)
  );

  //Wire declarations for instance 'pe_c0_r0' (Module hycube_no_pred_in79_out79_U3)
  wire  pe_c0_r0__CGRA_Clock;
  wire  pe_c0_r0__CGRA_Enable;
  wire  pe_c0_r0__CGRA_Reset;
  wire  pe_c0_r0__ConfigIn;
  wire  pe_c0_r0__ConfigOut;
  wire  pe_c0_r0__Config_Clock;
  wire  pe_c0_r0__Config_Reset;
  wire [31:0] pe_c0_r0__in0;
  wire [31:0] pe_c0_r0__in1;
  wire [31:0] pe_c0_r0__in2;
  wire [31:0] pe_c0_r0__in3;
  wire [31:0] pe_c0_r0__in6;
  wire [31:0] pe_c0_r0__out0;
  wire [31:0] pe_c0_r0__out1;
  wire [31:0] pe_c0_r0__out2;
  wire [31:0] pe_c0_r0__out3;
  wire [31:0] pe_c0_r0__out6;
  hycube_no_pred_in79_out79_U3 pe_c0_r0(
    .CGRA_Clock(pe_c0_r0__CGRA_Clock),
    .CGRA_Enable(pe_c0_r0__CGRA_Enable),
    .CGRA_Reset(pe_c0_r0__CGRA_Reset),
    .ConfigIn(pe_c0_r0__ConfigIn),
    .ConfigOut(pe_c0_r0__ConfigOut),
    .Config_Clock(pe_c0_r0__Config_Clock),
    .Config_Reset(pe_c0_r0__Config_Reset),
    .in0(pe_c0_r0__in0),
    .in1(pe_c0_r0__in1),
    .in2(pe_c0_r0__in2),
    .in3(pe_c0_r0__in3),
    .in6(pe_c0_r0__in6),
    .out0(pe_c0_r0__out0),
    .out1(pe_c0_r0__out1),
    .out2(pe_c0_r0__out2),
    .out3(pe_c0_r0__out3),
    .out6(pe_c0_r0__out6)
  );

  //Wire declarations for instance 'pe_c0_r1' (Module hycube_no_pred_in111_out111_U4)
  wire  pe_c0_r1__CGRA_Clock;
  wire  pe_c0_r1__CGRA_Enable;
  wire  pe_c0_r1__CGRA_Reset;
  wire  pe_c0_r1__ConfigIn;
  wire  pe_c0_r1__ConfigOut;
  wire  pe_c0_r1__Config_Clock;
  wire  pe_c0_r1__Config_Reset;
  wire [31:0] pe_c0_r1__in0;
  wire [31:0] pe_c0_r1__in1;
  wire [31:0] pe_c0_r1__in2;
  wire [31:0] pe_c0_r1__in3;
  wire [31:0] pe_c0_r1__in5;
  wire [31:0] pe_c0_r1__in6;
  wire [31:0] pe_c0_r1__out0;
  wire [31:0] pe_c0_r1__out1;
  wire [31:0] pe_c0_r1__out2;
  wire [31:0] pe_c0_r1__out3;
  wire [31:0] pe_c0_r1__out5;
  wire [31:0] pe_c0_r1__out6;
  hycube_no_pred_in111_out111_U4 pe_c0_r1(
    .CGRA_Clock(pe_c0_r1__CGRA_Clock),
    .CGRA_Enable(pe_c0_r1__CGRA_Enable),
    .CGRA_Reset(pe_c0_r1__CGRA_Reset),
    .ConfigIn(pe_c0_r1__ConfigIn),
    .ConfigOut(pe_c0_r1__ConfigOut),
    .Config_Clock(pe_c0_r1__Config_Clock),
    .Config_Reset(pe_c0_r1__Config_Reset),
    .in0(pe_c0_r1__in0),
    .in1(pe_c0_r1__in1),
    .in2(pe_c0_r1__in2),
    .in3(pe_c0_r1__in3),
    .in5(pe_c0_r1__in5),
    .in6(pe_c0_r1__in6),
    .out0(pe_c0_r1__out0),
    .out1(pe_c0_r1__out1),
    .out2(pe_c0_r1__out2),
    .out3(pe_c0_r1__out3),
    .out5(pe_c0_r1__out5),
    .out6(pe_c0_r1__out6)
  );

  //Wire declarations for instance 'pe_c0_r2' (Module hycube_no_pred_in111_out111_U4)
  wire  pe_c0_r2__CGRA_Clock;
  wire  pe_c0_r2__CGRA_Enable;
  wire  pe_c0_r2__CGRA_Reset;
  wire  pe_c0_r2__ConfigIn;
  wire  pe_c0_r2__ConfigOut;
  wire  pe_c0_r2__Config_Clock;
  wire  pe_c0_r2__Config_Reset;
  wire [31:0] pe_c0_r2__in0;
  wire [31:0] pe_c0_r2__in1;
  wire [31:0] pe_c0_r2__in2;
  wire [31:0] pe_c0_r2__in3;
  wire [31:0] pe_c0_r2__in5;
  wire [31:0] pe_c0_r2__in6;
  wire [31:0] pe_c0_r2__out0;
  wire [31:0] pe_c0_r2__out1;
  wire [31:0] pe_c0_r2__out2;
  wire [31:0] pe_c0_r2__out3;
  wire [31:0] pe_c0_r2__out5;
  wire [31:0] pe_c0_r2__out6;
  hycube_no_pred_in111_out111_U4 pe_c0_r2(
    .CGRA_Clock(pe_c0_r2__CGRA_Clock),
    .CGRA_Enable(pe_c0_r2__CGRA_Enable),
    .CGRA_Reset(pe_c0_r2__CGRA_Reset),
    .ConfigIn(pe_c0_r2__ConfigIn),
    .ConfigOut(pe_c0_r2__ConfigOut),
    .Config_Clock(pe_c0_r2__Config_Clock),
    .Config_Reset(pe_c0_r2__Config_Reset),
    .in0(pe_c0_r2__in0),
    .in1(pe_c0_r2__in1),
    .in2(pe_c0_r2__in2),
    .in3(pe_c0_r2__in3),
    .in5(pe_c0_r2__in5),
    .in6(pe_c0_r2__in6),
    .out0(pe_c0_r2__out0),
    .out1(pe_c0_r2__out1),
    .out2(pe_c0_r2__out2),
    .out3(pe_c0_r2__out3),
    .out5(pe_c0_r2__out5),
    .out6(pe_c0_r2__out6)
  );

  //Wire declarations for instance 'pe_c0_r3' (Module hycube_no_pred_in47_out47_U5)
  wire  pe_c0_r3__CGRA_Clock;
  wire  pe_c0_r3__CGRA_Enable;
  wire  pe_c0_r3__CGRA_Reset;
  wire  pe_c0_r3__ConfigIn;
  wire  pe_c0_r3__ConfigOut;
  wire  pe_c0_r3__Config_Clock;
  wire  pe_c0_r3__Config_Reset;
  wire [31:0] pe_c0_r3__in0;
  wire [31:0] pe_c0_r3__in1;
  wire [31:0] pe_c0_r3__in2;
  wire [31:0] pe_c0_r3__in3;
  wire [31:0] pe_c0_r3__in5;
  wire [31:0] pe_c0_r3__out0;
  wire [31:0] pe_c0_r3__out1;
  wire [31:0] pe_c0_r3__out2;
  wire [31:0] pe_c0_r3__out3;
  wire [31:0] pe_c0_r3__out5;
  hycube_no_pred_in47_out47_U5 pe_c0_r3(
    .CGRA_Clock(pe_c0_r3__CGRA_Clock),
    .CGRA_Enable(pe_c0_r3__CGRA_Enable),
    .CGRA_Reset(pe_c0_r3__CGRA_Reset),
    .ConfigIn(pe_c0_r3__ConfigIn),
    .ConfigOut(pe_c0_r3__ConfigOut),
    .Config_Clock(pe_c0_r3__Config_Clock),
    .Config_Reset(pe_c0_r3__Config_Reset),
    .in0(pe_c0_r3__in0),
    .in1(pe_c0_r3__in1),
    .in2(pe_c0_r3__in2),
    .in3(pe_c0_r3__in3),
    .in5(pe_c0_r3__in5),
    .out0(pe_c0_r3__out0),
    .out1(pe_c0_r3__out1),
    .out2(pe_c0_r3__out2),
    .out3(pe_c0_r3__out3),
    .out5(pe_c0_r3__out5)
  );

  //Wire declarations for instance 'pe_c1_r0' (Module hycube_no_pred_in207_out207_U6)
  wire  pe_c1_r0__CGRA_Clock;
  wire  pe_c1_r0__CGRA_Enable;
  wire  pe_c1_r0__CGRA_Reset;
  wire  pe_c1_r0__ConfigIn;
  wire  pe_c1_r0__ConfigOut;
  wire  pe_c1_r0__Config_Clock;
  wire  pe_c1_r0__Config_Reset;
  wire [31:0] pe_c1_r0__in0;
  wire [31:0] pe_c1_r0__in1;
  wire [31:0] pe_c1_r0__in2;
  wire [31:0] pe_c1_r0__in3;
  wire [31:0] pe_c1_r0__in6;
  wire [31:0] pe_c1_r0__in7;
  wire [31:0] pe_c1_r0__out0;
  wire [31:0] pe_c1_r0__out1;
  wire [31:0] pe_c1_r0__out2;
  wire [31:0] pe_c1_r0__out3;
  wire [31:0] pe_c1_r0__out6;
  wire [31:0] pe_c1_r0__out7;
  hycube_no_pred_in207_out207_U6 pe_c1_r0(
    .CGRA_Clock(pe_c1_r0__CGRA_Clock),
    .CGRA_Enable(pe_c1_r0__CGRA_Enable),
    .CGRA_Reset(pe_c1_r0__CGRA_Reset),
    .ConfigIn(pe_c1_r0__ConfigIn),
    .ConfigOut(pe_c1_r0__ConfigOut),
    .Config_Clock(pe_c1_r0__Config_Clock),
    .Config_Reset(pe_c1_r0__Config_Reset),
    .in0(pe_c1_r0__in0),
    .in1(pe_c1_r0__in1),
    .in2(pe_c1_r0__in2),
    .in3(pe_c1_r0__in3),
    .in6(pe_c1_r0__in6),
    .in7(pe_c1_r0__in7),
    .out0(pe_c1_r0__out0),
    .out1(pe_c1_r0__out1),
    .out2(pe_c1_r0__out2),
    .out3(pe_c1_r0__out3),
    .out6(pe_c1_r0__out6),
    .out7(pe_c1_r0__out7)
  );

  //Wire declarations for instance 'pe_c1_r1' (Module hycube_no_pred_in255_out255_U7)
  wire  pe_c1_r1__CGRA_Clock;
  wire  pe_c1_r1__CGRA_Enable;
  wire  pe_c1_r1__CGRA_Reset;
  wire  pe_c1_r1__ConfigIn;
  wire  pe_c1_r1__ConfigOut;
  wire  pe_c1_r1__Config_Clock;
  wire  pe_c1_r1__Config_Reset;
  wire [31:0] pe_c1_r1__in0;
  wire [31:0] pe_c1_r1__in1;
  wire [31:0] pe_c1_r1__in2;
  wire [31:0] pe_c1_r1__in3;
  wire [31:0] pe_c1_r1__in4;
  wire [31:0] pe_c1_r1__in5;
  wire [31:0] pe_c1_r1__in6;
  wire [31:0] pe_c1_r1__in7;
  wire [31:0] pe_c1_r1__out0;
  wire [31:0] pe_c1_r1__out1;
  wire [31:0] pe_c1_r1__out2;
  wire [31:0] pe_c1_r1__out3;
  wire [31:0] pe_c1_r1__out4;
  wire [31:0] pe_c1_r1__out5;
  wire [31:0] pe_c1_r1__out6;
  wire [31:0] pe_c1_r1__out7;
  hycube_no_pred_in255_out255_U7 pe_c1_r1(
    .CGRA_Clock(pe_c1_r1__CGRA_Clock),
    .CGRA_Enable(pe_c1_r1__CGRA_Enable),
    .CGRA_Reset(pe_c1_r1__CGRA_Reset),
    .ConfigIn(pe_c1_r1__ConfigIn),
    .ConfigOut(pe_c1_r1__ConfigOut),
    .Config_Clock(pe_c1_r1__Config_Clock),
    .Config_Reset(pe_c1_r1__Config_Reset),
    .in0(pe_c1_r1__in0),
    .in1(pe_c1_r1__in1),
    .in2(pe_c1_r1__in2),
    .in3(pe_c1_r1__in3),
    .in4(pe_c1_r1__in4),
    .in5(pe_c1_r1__in5),
    .in6(pe_c1_r1__in6),
    .in7(pe_c1_r1__in7),
    .out0(pe_c1_r1__out0),
    .out1(pe_c1_r1__out1),
    .out2(pe_c1_r1__out2),
    .out3(pe_c1_r1__out3),
    .out4(pe_c1_r1__out4),
    .out5(pe_c1_r1__out5),
    .out6(pe_c1_r1__out6),
    .out7(pe_c1_r1__out7)
  );

  //Wire declarations for instance 'pe_c1_r2' (Module hycube_no_pred_in255_out255_U7)
  wire  pe_c1_r2__CGRA_Clock;
  wire  pe_c1_r2__CGRA_Enable;
  wire  pe_c1_r2__CGRA_Reset;
  wire  pe_c1_r2__ConfigIn;
  wire  pe_c1_r2__ConfigOut;
  wire  pe_c1_r2__Config_Clock;
  wire  pe_c1_r2__Config_Reset;
  wire [31:0] pe_c1_r2__in0;
  wire [31:0] pe_c1_r2__in1;
  wire [31:0] pe_c1_r2__in2;
  wire [31:0] pe_c1_r2__in3;
  wire [31:0] pe_c1_r2__in4;
  wire [31:0] pe_c1_r2__in5;
  wire [31:0] pe_c1_r2__in6;
  wire [31:0] pe_c1_r2__in7;
  wire [31:0] pe_c1_r2__out0;
  wire [31:0] pe_c1_r2__out1;
  wire [31:0] pe_c1_r2__out2;
  wire [31:0] pe_c1_r2__out3;
  wire [31:0] pe_c1_r2__out4;
  wire [31:0] pe_c1_r2__out5;
  wire [31:0] pe_c1_r2__out6;
  wire [31:0] pe_c1_r2__out7;
  hycube_no_pred_in255_out255_U7 pe_c1_r2(
    .CGRA_Clock(pe_c1_r2__CGRA_Clock),
    .CGRA_Enable(pe_c1_r2__CGRA_Enable),
    .CGRA_Reset(pe_c1_r2__CGRA_Reset),
    .ConfigIn(pe_c1_r2__ConfigIn),
    .ConfigOut(pe_c1_r2__ConfigOut),
    .Config_Clock(pe_c1_r2__Config_Clock),
    .Config_Reset(pe_c1_r2__Config_Reset),
    .in0(pe_c1_r2__in0),
    .in1(pe_c1_r2__in1),
    .in2(pe_c1_r2__in2),
    .in3(pe_c1_r2__in3),
    .in4(pe_c1_r2__in4),
    .in5(pe_c1_r2__in5),
    .in6(pe_c1_r2__in6),
    .in7(pe_c1_r2__in7),
    .out0(pe_c1_r2__out0),
    .out1(pe_c1_r2__out1),
    .out2(pe_c1_r2__out2),
    .out3(pe_c1_r2__out3),
    .out4(pe_c1_r2__out4),
    .out5(pe_c1_r2__out5),
    .out6(pe_c1_r2__out6),
    .out7(pe_c1_r2__out7)
  );

  //Wire declarations for instance 'pe_c1_r3' (Module hycube_no_pred_in63_out63_U8)
  wire  pe_c1_r3__CGRA_Clock;
  wire  pe_c1_r3__CGRA_Enable;
  wire  pe_c1_r3__CGRA_Reset;
  wire  pe_c1_r3__ConfigIn;
  wire  pe_c1_r3__ConfigOut;
  wire  pe_c1_r3__Config_Clock;
  wire  pe_c1_r3__Config_Reset;
  wire [31:0] pe_c1_r3__in0;
  wire [31:0] pe_c1_r3__in1;
  wire [31:0] pe_c1_r3__in2;
  wire [31:0] pe_c1_r3__in3;
  wire [31:0] pe_c1_r3__in4;
  wire [31:0] pe_c1_r3__in5;
  wire [31:0] pe_c1_r3__out0;
  wire [31:0] pe_c1_r3__out1;
  wire [31:0] pe_c1_r3__out2;
  wire [31:0] pe_c1_r3__out3;
  wire [31:0] pe_c1_r3__out4;
  wire [31:0] pe_c1_r3__out5;
  hycube_no_pred_in63_out63_U8 pe_c1_r3(
    .CGRA_Clock(pe_c1_r3__CGRA_Clock),
    .CGRA_Enable(pe_c1_r3__CGRA_Enable),
    .CGRA_Reset(pe_c1_r3__CGRA_Reset),
    .ConfigIn(pe_c1_r3__ConfigIn),
    .ConfigOut(pe_c1_r3__ConfigOut),
    .Config_Clock(pe_c1_r3__Config_Clock),
    .Config_Reset(pe_c1_r3__Config_Reset),
    .in0(pe_c1_r3__in0),
    .in1(pe_c1_r3__in1),
    .in2(pe_c1_r3__in2),
    .in3(pe_c1_r3__in3),
    .in4(pe_c1_r3__in4),
    .in5(pe_c1_r3__in5),
    .out0(pe_c1_r3__out0),
    .out1(pe_c1_r3__out1),
    .out2(pe_c1_r3__out2),
    .out3(pe_c1_r3__out3),
    .out4(pe_c1_r3__out4),
    .out5(pe_c1_r3__out5)
  );

  //Wire declarations for instance 'pe_c2_r0' (Module hycube_no_pred_in207_out207_U6)
  wire  pe_c2_r0__CGRA_Clock;
  wire  pe_c2_r0__CGRA_Enable;
  wire  pe_c2_r0__CGRA_Reset;
  wire  pe_c2_r0__ConfigIn;
  wire  pe_c2_r0__ConfigOut;
  wire  pe_c2_r0__Config_Clock;
  wire  pe_c2_r0__Config_Reset;
  wire [31:0] pe_c2_r0__in0;
  wire [31:0] pe_c2_r0__in1;
  wire [31:0] pe_c2_r0__in2;
  wire [31:0] pe_c2_r0__in3;
  wire [31:0] pe_c2_r0__in6;
  wire [31:0] pe_c2_r0__in7;
  wire [31:0] pe_c2_r0__out0;
  wire [31:0] pe_c2_r0__out1;
  wire [31:0] pe_c2_r0__out2;
  wire [31:0] pe_c2_r0__out3;
  wire [31:0] pe_c2_r0__out6;
  wire [31:0] pe_c2_r0__out7;
  hycube_no_pred_in207_out207_U6 pe_c2_r0(
    .CGRA_Clock(pe_c2_r0__CGRA_Clock),
    .CGRA_Enable(pe_c2_r0__CGRA_Enable),
    .CGRA_Reset(pe_c2_r0__CGRA_Reset),
    .ConfigIn(pe_c2_r0__ConfigIn),
    .ConfigOut(pe_c2_r0__ConfigOut),
    .Config_Clock(pe_c2_r0__Config_Clock),
    .Config_Reset(pe_c2_r0__Config_Reset),
    .in0(pe_c2_r0__in0),
    .in1(pe_c2_r0__in1),
    .in2(pe_c2_r0__in2),
    .in3(pe_c2_r0__in3),
    .in6(pe_c2_r0__in6),
    .in7(pe_c2_r0__in7),
    .out0(pe_c2_r0__out0),
    .out1(pe_c2_r0__out1),
    .out2(pe_c2_r0__out2),
    .out3(pe_c2_r0__out3),
    .out6(pe_c2_r0__out6),
    .out7(pe_c2_r0__out7)
  );

  //Wire declarations for instance 'pe_c2_r1' (Module hycube_no_pred_in255_out255_U7)
  wire  pe_c2_r1__CGRA_Clock;
  wire  pe_c2_r1__CGRA_Enable;
  wire  pe_c2_r1__CGRA_Reset;
  wire  pe_c2_r1__ConfigIn;
  wire  pe_c2_r1__ConfigOut;
  wire  pe_c2_r1__Config_Clock;
  wire  pe_c2_r1__Config_Reset;
  wire [31:0] pe_c2_r1__in0;
  wire [31:0] pe_c2_r1__in1;
  wire [31:0] pe_c2_r1__in2;
  wire [31:0] pe_c2_r1__in3;
  wire [31:0] pe_c2_r1__in4;
  wire [31:0] pe_c2_r1__in5;
  wire [31:0] pe_c2_r1__in6;
  wire [31:0] pe_c2_r1__in7;
  wire [31:0] pe_c2_r1__out0;
  wire [31:0] pe_c2_r1__out1;
  wire [31:0] pe_c2_r1__out2;
  wire [31:0] pe_c2_r1__out3;
  wire [31:0] pe_c2_r1__out4;
  wire [31:0] pe_c2_r1__out5;
  wire [31:0] pe_c2_r1__out6;
  wire [31:0] pe_c2_r1__out7;
  hycube_no_pred_in255_out255_U7 pe_c2_r1(
    .CGRA_Clock(pe_c2_r1__CGRA_Clock),
    .CGRA_Enable(pe_c2_r1__CGRA_Enable),
    .CGRA_Reset(pe_c2_r1__CGRA_Reset),
    .ConfigIn(pe_c2_r1__ConfigIn),
    .ConfigOut(pe_c2_r1__ConfigOut),
    .Config_Clock(pe_c2_r1__Config_Clock),
    .Config_Reset(pe_c2_r1__Config_Reset),
    .in0(pe_c2_r1__in0),
    .in1(pe_c2_r1__in1),
    .in2(pe_c2_r1__in2),
    .in3(pe_c2_r1__in3),
    .in4(pe_c2_r1__in4),
    .in5(pe_c2_r1__in5),
    .in6(pe_c2_r1__in6),
    .in7(pe_c2_r1__in7),
    .out0(pe_c2_r1__out0),
    .out1(pe_c2_r1__out1),
    .out2(pe_c2_r1__out2),
    .out3(pe_c2_r1__out3),
    .out4(pe_c2_r1__out4),
    .out5(pe_c2_r1__out5),
    .out6(pe_c2_r1__out6),
    .out7(pe_c2_r1__out7)
  );

  //Wire declarations for instance 'pe_c2_r2' (Module hycube_no_pred_in255_out255_U7)
  wire  pe_c2_r2__CGRA_Clock;
  wire  pe_c2_r2__CGRA_Enable;
  wire  pe_c2_r2__CGRA_Reset;
  wire  pe_c2_r2__ConfigIn;
  wire  pe_c2_r2__ConfigOut;
  wire  pe_c2_r2__Config_Clock;
  wire  pe_c2_r2__Config_Reset;
  wire [31:0] pe_c2_r2__in0;
  wire [31:0] pe_c2_r2__in1;
  wire [31:0] pe_c2_r2__in2;
  wire [31:0] pe_c2_r2__in3;
  wire [31:0] pe_c2_r2__in4;
  wire [31:0] pe_c2_r2__in5;
  wire [31:0] pe_c2_r2__in6;
  wire [31:0] pe_c2_r2__in7;
  wire [31:0] pe_c2_r2__out0;
  wire [31:0] pe_c2_r2__out1;
  wire [31:0] pe_c2_r2__out2;
  wire [31:0] pe_c2_r2__out3;
  wire [31:0] pe_c2_r2__out4;
  wire [31:0] pe_c2_r2__out5;
  wire [31:0] pe_c2_r2__out6;
  wire [31:0] pe_c2_r2__out7;
  hycube_no_pred_in255_out255_U7 pe_c2_r2(
    .CGRA_Clock(pe_c2_r2__CGRA_Clock),
    .CGRA_Enable(pe_c2_r2__CGRA_Enable),
    .CGRA_Reset(pe_c2_r2__CGRA_Reset),
    .ConfigIn(pe_c2_r2__ConfigIn),
    .ConfigOut(pe_c2_r2__ConfigOut),
    .Config_Clock(pe_c2_r2__Config_Clock),
    .Config_Reset(pe_c2_r2__Config_Reset),
    .in0(pe_c2_r2__in0),
    .in1(pe_c2_r2__in1),
    .in2(pe_c2_r2__in2),
    .in3(pe_c2_r2__in3),
    .in4(pe_c2_r2__in4),
    .in5(pe_c2_r2__in5),
    .in6(pe_c2_r2__in6),
    .in7(pe_c2_r2__in7),
    .out0(pe_c2_r2__out0),
    .out1(pe_c2_r2__out1),
    .out2(pe_c2_r2__out2),
    .out3(pe_c2_r2__out3),
    .out4(pe_c2_r2__out4),
    .out5(pe_c2_r2__out5),
    .out6(pe_c2_r2__out6),
    .out7(pe_c2_r2__out7)
  );

  //Wire declarations for instance 'pe_c2_r3' (Module hycube_no_pred_in63_out63_U8)
  wire  pe_c2_r3__CGRA_Clock;
  wire  pe_c2_r3__CGRA_Enable;
  wire  pe_c2_r3__CGRA_Reset;
  wire  pe_c2_r3__ConfigIn;
  wire  pe_c2_r3__ConfigOut;
  wire  pe_c2_r3__Config_Clock;
  wire  pe_c2_r3__Config_Reset;
  wire [31:0] pe_c2_r3__in0;
  wire [31:0] pe_c2_r3__in1;
  wire [31:0] pe_c2_r3__in2;
  wire [31:0] pe_c2_r3__in3;
  wire [31:0] pe_c2_r3__in4;
  wire [31:0] pe_c2_r3__in5;
  wire [31:0] pe_c2_r3__out0;
  wire [31:0] pe_c2_r3__out1;
  wire [31:0] pe_c2_r3__out2;
  wire [31:0] pe_c2_r3__out3;
  wire [31:0] pe_c2_r3__out4;
  wire [31:0] pe_c2_r3__out5;
  hycube_no_pred_in63_out63_U8 pe_c2_r3(
    .CGRA_Clock(pe_c2_r3__CGRA_Clock),
    .CGRA_Enable(pe_c2_r3__CGRA_Enable),
    .CGRA_Reset(pe_c2_r3__CGRA_Reset),
    .ConfigIn(pe_c2_r3__ConfigIn),
    .ConfigOut(pe_c2_r3__ConfigOut),
    .Config_Clock(pe_c2_r3__Config_Clock),
    .Config_Reset(pe_c2_r3__Config_Reset),
    .in0(pe_c2_r3__in0),
    .in1(pe_c2_r3__in1),
    .in2(pe_c2_r3__in2),
    .in3(pe_c2_r3__in3),
    .in4(pe_c2_r3__in4),
    .in5(pe_c2_r3__in5),
    .out0(pe_c2_r3__out0),
    .out1(pe_c2_r3__out1),
    .out2(pe_c2_r3__out2),
    .out3(pe_c2_r3__out3),
    .out4(pe_c2_r3__out4),
    .out5(pe_c2_r3__out5)
  );

  //Wire declarations for instance 'pe_c3_r0' (Module hycube_no_pred_in143_out143_U9)
  wire  pe_c3_r0__CGRA_Clock;
  wire  pe_c3_r0__CGRA_Enable;
  wire  pe_c3_r0__CGRA_Reset;
  wire  pe_c3_r0__ConfigIn;
  wire  pe_c3_r0__ConfigOut;
  wire  pe_c3_r0__Config_Clock;
  wire  pe_c3_r0__Config_Reset;
  wire [31:0] pe_c3_r0__in0;
  wire [31:0] pe_c3_r0__in1;
  wire [31:0] pe_c3_r0__in2;
  wire [31:0] pe_c3_r0__in3;
  wire [31:0] pe_c3_r0__in7;
  wire [31:0] pe_c3_r0__out0;
  wire [31:0] pe_c3_r0__out1;
  wire [31:0] pe_c3_r0__out2;
  wire [31:0] pe_c3_r0__out3;
  wire [31:0] pe_c3_r0__out7;
  hycube_no_pred_in143_out143_U9 pe_c3_r0(
    .CGRA_Clock(pe_c3_r0__CGRA_Clock),
    .CGRA_Enable(pe_c3_r0__CGRA_Enable),
    .CGRA_Reset(pe_c3_r0__CGRA_Reset),
    .ConfigIn(pe_c3_r0__ConfigIn),
    .ConfigOut(pe_c3_r0__ConfigOut),
    .Config_Clock(pe_c3_r0__Config_Clock),
    .Config_Reset(pe_c3_r0__Config_Reset),
    .in0(pe_c3_r0__in0),
    .in1(pe_c3_r0__in1),
    .in2(pe_c3_r0__in2),
    .in3(pe_c3_r0__in3),
    .in7(pe_c3_r0__in7),
    .out0(pe_c3_r0__out0),
    .out1(pe_c3_r0__out1),
    .out2(pe_c3_r0__out2),
    .out3(pe_c3_r0__out3),
    .out7(pe_c3_r0__out7)
  );

  //Wire declarations for instance 'pe_c3_r1' (Module hycube_no_pred_in159_out159_U10)
  wire  pe_c3_r1__CGRA_Clock;
  wire  pe_c3_r1__CGRA_Enable;
  wire  pe_c3_r1__CGRA_Reset;
  wire  pe_c3_r1__ConfigIn;
  wire  pe_c3_r1__ConfigOut;
  wire  pe_c3_r1__Config_Clock;
  wire  pe_c3_r1__Config_Reset;
  wire [31:0] pe_c3_r1__in0;
  wire [31:0] pe_c3_r1__in1;
  wire [31:0] pe_c3_r1__in2;
  wire [31:0] pe_c3_r1__in3;
  wire [31:0] pe_c3_r1__in4;
  wire [31:0] pe_c3_r1__in7;
  wire [31:0] pe_c3_r1__out0;
  wire [31:0] pe_c3_r1__out1;
  wire [31:0] pe_c3_r1__out2;
  wire [31:0] pe_c3_r1__out3;
  wire [31:0] pe_c3_r1__out4;
  wire [31:0] pe_c3_r1__out7;
  hycube_no_pred_in159_out159_U10 pe_c3_r1(
    .CGRA_Clock(pe_c3_r1__CGRA_Clock),
    .CGRA_Enable(pe_c3_r1__CGRA_Enable),
    .CGRA_Reset(pe_c3_r1__CGRA_Reset),
    .ConfigIn(pe_c3_r1__ConfigIn),
    .ConfigOut(pe_c3_r1__ConfigOut),
    .Config_Clock(pe_c3_r1__Config_Clock),
    .Config_Reset(pe_c3_r1__Config_Reset),
    .in0(pe_c3_r1__in0),
    .in1(pe_c3_r1__in1),
    .in2(pe_c3_r1__in2),
    .in3(pe_c3_r1__in3),
    .in4(pe_c3_r1__in4),
    .in7(pe_c3_r1__in7),
    .out0(pe_c3_r1__out0),
    .out1(pe_c3_r1__out1),
    .out2(pe_c3_r1__out2),
    .out3(pe_c3_r1__out3),
    .out4(pe_c3_r1__out4),
    .out7(pe_c3_r1__out7)
  );

  //Wire declarations for instance 'pe_c3_r2' (Module hycube_no_pred_in159_out159_U10)
  wire  pe_c3_r2__CGRA_Clock;
  wire  pe_c3_r2__CGRA_Enable;
  wire  pe_c3_r2__CGRA_Reset;
  wire  pe_c3_r2__ConfigIn;
  wire  pe_c3_r2__ConfigOut;
  wire  pe_c3_r2__Config_Clock;
  wire  pe_c3_r2__Config_Reset;
  wire [31:0] pe_c3_r2__in0;
  wire [31:0] pe_c3_r2__in1;
  wire [31:0] pe_c3_r2__in2;
  wire [31:0] pe_c3_r2__in3;
  wire [31:0] pe_c3_r2__in4;
  wire [31:0] pe_c3_r2__in7;
  wire [31:0] pe_c3_r2__out0;
  wire [31:0] pe_c3_r2__out1;
  wire [31:0] pe_c3_r2__out2;
  wire [31:0] pe_c3_r2__out3;
  wire [31:0] pe_c3_r2__out4;
  wire [31:0] pe_c3_r2__out7;
  hycube_no_pred_in159_out159_U10 pe_c3_r2(
    .CGRA_Clock(pe_c3_r2__CGRA_Clock),
    .CGRA_Enable(pe_c3_r2__CGRA_Enable),
    .CGRA_Reset(pe_c3_r2__CGRA_Reset),
    .ConfigIn(pe_c3_r2__ConfigIn),
    .ConfigOut(pe_c3_r2__ConfigOut),
    .Config_Clock(pe_c3_r2__Config_Clock),
    .Config_Reset(pe_c3_r2__Config_Reset),
    .in0(pe_c3_r2__in0),
    .in1(pe_c3_r2__in1),
    .in2(pe_c3_r2__in2),
    .in3(pe_c3_r2__in3),
    .in4(pe_c3_r2__in4),
    .in7(pe_c3_r2__in7),
    .out0(pe_c3_r2__out0),
    .out1(pe_c3_r2__out1),
    .out2(pe_c3_r2__out2),
    .out3(pe_c3_r2__out3),
    .out4(pe_c3_r2__out4),
    .out7(pe_c3_r2__out7)
  );

  //Wire declarations for instance 'pe_c3_r3' (Module hycube_no_pred_in31_out31_U11)
  wire  pe_c3_r3__CGRA_Clock;
  wire  pe_c3_r3__CGRA_Enable;
  wire  pe_c3_r3__CGRA_Reset;
  wire  pe_c3_r3__ConfigIn;
  wire  pe_c3_r3__ConfigOut;
  wire  pe_c3_r3__Config_Clock;
  wire  pe_c3_r3__Config_Reset;
  wire [31:0] pe_c3_r3__in0;
  wire [31:0] pe_c3_r3__in1;
  wire [31:0] pe_c3_r3__in2;
  wire [31:0] pe_c3_r3__in3;
  wire [31:0] pe_c3_r3__in4;
  wire [31:0] pe_c3_r3__out0;
  wire [31:0] pe_c3_r3__out1;
  wire [31:0] pe_c3_r3__out2;
  wire [31:0] pe_c3_r3__out3;
  wire [31:0] pe_c3_r3__out4;
  hycube_no_pred_in31_out31_U11 pe_c3_r3(
    .CGRA_Clock(pe_c3_r3__CGRA_Clock),
    .CGRA_Enable(pe_c3_r3__CGRA_Enable),
    .CGRA_Reset(pe_c3_r3__CGRA_Reset),
    .ConfigIn(pe_c3_r3__ConfigIn),
    .ConfigOut(pe_c3_r3__ConfigOut),
    .Config_Clock(pe_c3_r3__Config_Clock),
    .Config_Reset(pe_c3_r3__Config_Reset),
    .in0(pe_c3_r3__in0),
    .in1(pe_c3_r3__in1),
    .in2(pe_c3_r3__in2),
    .in3(pe_c3_r3__in3),
    .in4(pe_c3_r3__in4),
    .out0(pe_c3_r3__out0),
    .out1(pe_c3_r3__out1),
    .out2(pe_c3_r3__out2),
    .out3(pe_c3_r3__out3),
    .out4(pe_c3_r3__out4)
  );

  //All the connections
  assign io_bottom_0__Config_Clock = Config_Clock;
  assign io_bottom_1__Config_Clock = Config_Clock;
  assign io_bottom_2__Config_Clock = Config_Clock;
  assign io_bottom_3__Config_Clock = Config_Clock;
  assign io_right_0__Config_Clock = Config_Clock;
  assign io_right_1__Config_Clock = Config_Clock;
  assign io_right_2__Config_Clock = Config_Clock;
  assign io_right_3__Config_Clock = Config_Clock;
  assign io_top_0__Config_Clock = Config_Clock;
  assign io_top_1__Config_Clock = Config_Clock;
  assign io_top_2__Config_Clock = Config_Clock;
  assign io_top_3__Config_Clock = Config_Clock;
  assign mem_0__Config_Clock = Config_Clock;
  assign mem_1__Config_Clock = Config_Clock;
  assign mem_2__Config_Clock = Config_Clock;
  assign mem_3__Config_Clock = Config_Clock;
  assign pe_c0_r0__Config_Clock = Config_Clock;
  assign pe_c0_r1__Config_Clock = Config_Clock;
  assign pe_c0_r2__Config_Clock = Config_Clock;
  assign pe_c0_r3__Config_Clock = Config_Clock;
  assign pe_c1_r0__Config_Clock = Config_Clock;
  assign pe_c1_r1__Config_Clock = Config_Clock;
  assign pe_c1_r2__Config_Clock = Config_Clock;
  assign pe_c1_r3__Config_Clock = Config_Clock;
  assign pe_c2_r0__Config_Clock = Config_Clock;
  assign pe_c2_r1__Config_Clock = Config_Clock;
  assign pe_c2_r2__Config_Clock = Config_Clock;
  assign pe_c2_r3__Config_Clock = Config_Clock;
  assign pe_c3_r0__Config_Clock = Config_Clock;
  assign pe_c3_r1__Config_Clock = Config_Clock;
  assign pe_c3_r2__Config_Clock = Config_Clock;
  assign pe_c3_r3__Config_Clock = Config_Clock;
  assign io_bottom_0__CGRA_Clock = CGRA_Clock;
  assign io_bottom_0__Config_Reset = Config_Reset;
  assign io_bottom_1__Config_Reset = Config_Reset;
  assign io_bottom_2__Config_Reset = Config_Reset;
  assign io_bottom_3__Config_Reset = Config_Reset;
  assign io_right_0__Config_Reset = Config_Reset;
  assign io_right_1__Config_Reset = Config_Reset;
  assign io_right_2__Config_Reset = Config_Reset;
  assign io_right_3__Config_Reset = Config_Reset;
  assign io_top_0__Config_Reset = Config_Reset;
  assign io_top_1__Config_Reset = Config_Reset;
  assign io_top_2__Config_Reset = Config_Reset;
  assign io_top_3__Config_Reset = Config_Reset;
  assign mem_0__Config_Reset = Config_Reset;
  assign mem_1__Config_Reset = Config_Reset;
  assign mem_2__Config_Reset = Config_Reset;
  assign mem_3__Config_Reset = Config_Reset;
  assign pe_c0_r0__Config_Reset = Config_Reset;
  assign pe_c0_r1__Config_Reset = Config_Reset;
  assign pe_c0_r2__Config_Reset = Config_Reset;
  assign pe_c0_r3__Config_Reset = Config_Reset;
  assign pe_c1_r0__Config_Reset = Config_Reset;
  assign pe_c1_r1__Config_Reset = Config_Reset;
  assign pe_c1_r2__Config_Reset = Config_Reset;
  assign pe_c1_r3__Config_Reset = Config_Reset;
  assign pe_c2_r0__Config_Reset = Config_Reset;
  assign pe_c2_r1__Config_Reset = Config_Reset;
  assign pe_c2_r2__Config_Reset = Config_Reset;
  assign pe_c2_r3__Config_Reset = Config_Reset;
  assign pe_c3_r0__Config_Reset = Config_Reset;
  assign pe_c3_r1__Config_Reset = Config_Reset;
  assign pe_c3_r2__Config_Reset = Config_Reset;
  assign pe_c3_r3__Config_Reset = Config_Reset;
  assign io_bottom_0__CGRA_Reset = CGRA_Reset;
  assign io_bottom_1__CGRA_Clock = CGRA_Clock;
  assign io_bottom_2__CGRA_Clock = CGRA_Clock;
  assign io_bottom_3__CGRA_Clock = CGRA_Clock;
  assign io_right_0__CGRA_Clock = CGRA_Clock;
  assign io_right_1__CGRA_Clock = CGRA_Clock;
  assign io_right_2__CGRA_Clock = CGRA_Clock;
  assign io_right_3__CGRA_Clock = CGRA_Clock;
  assign io_top_0__CGRA_Clock = CGRA_Clock;
  assign io_top_1__CGRA_Clock = CGRA_Clock;
  assign io_top_2__CGRA_Clock = CGRA_Clock;
  assign io_top_3__CGRA_Clock = CGRA_Clock;
  assign mem_0__CGRA_Clock = CGRA_Clock;
  assign mem_1__CGRA_Clock = CGRA_Clock;
  assign mem_2__CGRA_Clock = CGRA_Clock;
  assign mem_3__CGRA_Clock = CGRA_Clock;
  assign pe_c0_r0__CGRA_Clock = CGRA_Clock;
  assign pe_c0_r1__CGRA_Clock = CGRA_Clock;
  assign pe_c0_r2__CGRA_Clock = CGRA_Clock;
  assign pe_c0_r3__CGRA_Clock = CGRA_Clock;
  assign pe_c1_r0__CGRA_Clock = CGRA_Clock;
  assign pe_c1_r1__CGRA_Clock = CGRA_Clock;
  assign pe_c1_r2__CGRA_Clock = CGRA_Clock;
  assign pe_c1_r3__CGRA_Clock = CGRA_Clock;
  assign pe_c2_r0__CGRA_Clock = CGRA_Clock;
  assign pe_c2_r1__CGRA_Clock = CGRA_Clock;
  assign pe_c2_r2__CGRA_Clock = CGRA_Clock;
  assign pe_c2_r3__CGRA_Clock = CGRA_Clock;
  assign pe_c3_r0__CGRA_Clock = CGRA_Clock;
  assign pe_c3_r1__CGRA_Clock = CGRA_Clock;
  assign pe_c3_r2__CGRA_Clock = CGRA_Clock;
  assign pe_c3_r3__CGRA_Clock = CGRA_Clock;
  assign io_bottom_0__CGRA_Enable = CGRA_Enable;
  assign io_bottom_1__CGRA_Reset = CGRA_Reset;
  assign io_bottom_2__CGRA_Reset = CGRA_Reset;
  assign io_bottom_3__CGRA_Reset = CGRA_Reset;
  assign io_right_0__CGRA_Reset = CGRA_Reset;
  assign io_right_1__CGRA_Reset = CGRA_Reset;
  assign io_right_2__CGRA_Reset = CGRA_Reset;
  assign io_right_3__CGRA_Reset = CGRA_Reset;
  assign io_top_0__CGRA_Reset = CGRA_Reset;
  assign io_top_1__CGRA_Reset = CGRA_Reset;
  assign io_top_2__CGRA_Reset = CGRA_Reset;
  assign io_top_3__CGRA_Reset = CGRA_Reset;
  assign mem_0__CGRA_Reset = CGRA_Reset;
  assign mem_1__CGRA_Reset = CGRA_Reset;
  assign mem_2__CGRA_Reset = CGRA_Reset;
  assign mem_3__CGRA_Reset = CGRA_Reset;
  assign pe_c0_r0__CGRA_Reset = CGRA_Reset;
  assign pe_c0_r1__CGRA_Reset = CGRA_Reset;
  assign pe_c0_r2__CGRA_Reset = CGRA_Reset;
  assign pe_c0_r3__CGRA_Reset = CGRA_Reset;
  assign pe_c1_r0__CGRA_Reset = CGRA_Reset;
  assign pe_c1_r1__CGRA_Reset = CGRA_Reset;
  assign pe_c1_r2__CGRA_Reset = CGRA_Reset;
  assign pe_c1_r3__CGRA_Reset = CGRA_Reset;
  assign pe_c2_r0__CGRA_Reset = CGRA_Reset;
  assign pe_c2_r1__CGRA_Reset = CGRA_Reset;
  assign pe_c2_r2__CGRA_Reset = CGRA_Reset;
  assign pe_c2_r3__CGRA_Reset = CGRA_Reset;
  assign pe_c3_r0__CGRA_Reset = CGRA_Reset;
  assign pe_c3_r1__CGRA_Reset = CGRA_Reset;
  assign pe_c3_r2__CGRA_Reset = CGRA_Reset;
  assign pe_c3_r3__CGRA_Reset = CGRA_Reset;
  assign io_bottom_1__CGRA_Enable = CGRA_Enable;
  assign io_bottom_2__CGRA_Enable = CGRA_Enable;
  assign io_bottom_3__CGRA_Enable = CGRA_Enable;
  assign io_right_0__CGRA_Enable = CGRA_Enable;
  assign io_right_1__CGRA_Enable = CGRA_Enable;
  assign io_right_2__CGRA_Enable = CGRA_Enable;
  assign io_right_3__CGRA_Enable = CGRA_Enable;
  assign io_top_0__CGRA_Enable = CGRA_Enable;
  assign io_top_1__CGRA_Enable = CGRA_Enable;
  assign io_top_2__CGRA_Enable = CGRA_Enable;
  assign io_top_3__CGRA_Enable = CGRA_Enable;
  assign mem_0__CGRA_Enable = CGRA_Enable;
  assign mem_1__CGRA_Enable = CGRA_Enable;
  assign mem_2__CGRA_Enable = CGRA_Enable;
  assign mem_3__CGRA_Enable = CGRA_Enable;
  assign pe_c0_r0__CGRA_Enable = CGRA_Enable;
  assign pe_c0_r1__CGRA_Enable = CGRA_Enable;
  assign pe_c0_r2__CGRA_Enable = CGRA_Enable;
  assign pe_c0_r3__CGRA_Enable = CGRA_Enable;
  assign pe_c1_r0__CGRA_Enable = CGRA_Enable;
  assign pe_c1_r1__CGRA_Enable = CGRA_Enable;
  assign pe_c1_r2__CGRA_Enable = CGRA_Enable;
  assign pe_c1_r3__CGRA_Enable = CGRA_Enable;
  assign pe_c2_r0__CGRA_Enable = CGRA_Enable;
  assign pe_c2_r1__CGRA_Enable = CGRA_Enable;
  assign pe_c2_r2__CGRA_Enable = CGRA_Enable;
  assign pe_c2_r3__CGRA_Enable = CGRA_Enable;
  assign pe_c3_r0__CGRA_Enable = CGRA_Enable;
  assign pe_c3_r1__CGRA_Enable = CGRA_Enable;
  assign pe_c3_r2__CGRA_Enable = CGRA_Enable;
  assign pe_c3_r3__CGRA_Enable = CGRA_Enable;
  assign io_top_0__in[31:0] = pe_c0_r0__out0[31:0];
  assign pe_c1_r0__in3[31:0] = pe_c0_r0__out1[31:0];
  assign pe_c0_r1__in0[31:0] = pe_c0_r0__out2[31:0];
  assign mem_0__in0[31:0] = pe_c0_r0__out3[31:0];
  assign mem_1__in0[31:0] = pe_c0_r0__out3[31:0];
  assign mem_2__in0[31:0] = pe_c0_r0__out3[31:0];
  assign mem_3__in0[31:0] = pe_c0_r0__out3[31:0];
  assign pe_c1_r1__in4[31:0] = pe_c0_r0__out6[31:0];
  assign pe_c0_r0__in2[31:0] = pe_c0_r1__out0[31:0];
  assign pe_c1_r1__in3[31:0] = pe_c0_r1__out1[31:0];
  assign pe_c0_r2__in0[31:0] = pe_c0_r1__out2[31:0];
  assign mem_0__in1[31:0] = pe_c0_r1__out3[31:0];
  assign mem_1__in1[31:0] = pe_c0_r1__out3[31:0];
  assign mem_2__in1[31:0] = pe_c0_r1__out3[31:0];
  assign mem_3__in1[31:0] = pe_c0_r1__out3[31:0];
  assign pe_c1_r0__in7[31:0] = pe_c0_r1__out5[31:0];
  assign pe_c1_r2__in4[31:0] = pe_c0_r1__out6[31:0];
  assign pe_c0_r1__in2[31:0] = pe_c0_r2__out0[31:0];
  assign pe_c1_r2__in3[31:0] = pe_c0_r2__out1[31:0];
  assign pe_c0_r3__in0[31:0] = pe_c0_r2__out2[31:0];
  assign mem_0__in2[31:0] = pe_c0_r2__out3[31:0];
  assign mem_1__in2[31:0] = pe_c0_r2__out3[31:0];
  assign mem_2__in2[31:0] = pe_c0_r2__out3[31:0];
  assign mem_3__in2[31:0] = pe_c0_r2__out3[31:0];
  assign pe_c1_r1__in7[31:0] = pe_c0_r2__out5[31:0];
  assign pe_c1_r3__in4[31:0] = pe_c0_r2__out6[31:0];
  assign pe_c0_r2__in2[31:0] = pe_c0_r3__out0[31:0];
  assign pe_c1_r3__in3[31:0] = pe_c0_r3__out1[31:0];
  assign io_bottom_0__in[31:0] = pe_c0_r3__out2[31:0];
  assign mem_0__in3[31:0] = pe_c0_r3__out3[31:0];
  assign mem_1__in3[31:0] = pe_c0_r3__out3[31:0];
  assign mem_2__in3[31:0] = pe_c0_r3__out3[31:0];
  assign mem_3__in3[31:0] = pe_c0_r3__out3[31:0];
  assign pe_c1_r2__in7[31:0] = pe_c0_r3__out5[31:0];
  assign io_top_1__in[31:0] = pe_c1_r0__out0[31:0];
  assign pe_c2_r0__in3[31:0] = pe_c1_r0__out1[31:0];
  assign pe_c1_r1__in0[31:0] = pe_c1_r0__out2[31:0];
  assign pe_c0_r0__in1[31:0] = pe_c1_r0__out3[31:0];
  assign pe_c2_r1__in4[31:0] = pe_c1_r0__out6[31:0];
  assign pe_c0_r1__in5[31:0] = pe_c1_r0__out7[31:0];
  assign pe_c1_r0__in2[31:0] = pe_c1_r1__out0[31:0];
  assign pe_c2_r1__in3[31:0] = pe_c1_r1__out1[31:0];
  assign pe_c1_r2__in0[31:0] = pe_c1_r1__out2[31:0];
  assign pe_c0_r1__in1[31:0] = pe_c1_r1__out3[31:0];
  assign pe_c0_r0__in6[31:0] = pe_c1_r1__out4[31:0];
  assign pe_c2_r0__in7[31:0] = pe_c1_r1__out5[31:0];
  assign pe_c2_r2__in4[31:0] = pe_c1_r1__out6[31:0];
  assign pe_c0_r2__in5[31:0] = pe_c1_r1__out7[31:0];
  assign pe_c1_r1__in2[31:0] = pe_c1_r2__out0[31:0];
  assign pe_c2_r2__in3[31:0] = pe_c1_r2__out1[31:0];
  assign pe_c1_r3__in0[31:0] = pe_c1_r2__out2[31:0];
  assign pe_c0_r2__in1[31:0] = pe_c1_r2__out3[31:0];
  assign pe_c0_r1__in6[31:0] = pe_c1_r2__out4[31:0];
  assign pe_c2_r1__in7[31:0] = pe_c1_r2__out5[31:0];
  assign pe_c2_r3__in4[31:0] = pe_c1_r2__out6[31:0];
  assign pe_c0_r3__in5[31:0] = pe_c1_r2__out7[31:0];
  assign pe_c1_r2__in2[31:0] = pe_c1_r3__out0[31:0];
  assign pe_c2_r3__in3[31:0] = pe_c1_r3__out1[31:0];
  assign io_bottom_1__in[31:0] = pe_c1_r3__out2[31:0];
  assign pe_c0_r3__in1[31:0] = pe_c1_r3__out3[31:0];
  assign pe_c0_r2__in6[31:0] = pe_c1_r3__out4[31:0];
  assign pe_c2_r2__in7[31:0] = pe_c1_r3__out5[31:0];
  assign io_top_2__in[31:0] = pe_c2_r0__out0[31:0];
  assign pe_c3_r0__in3[31:0] = pe_c2_r0__out1[31:0];
  assign pe_c2_r1__in0[31:0] = pe_c2_r0__out2[31:0];
  assign pe_c1_r0__in1[31:0] = pe_c2_r0__out3[31:0];
  assign pe_c3_r1__in4[31:0] = pe_c2_r0__out6[31:0];
  assign pe_c1_r1__in5[31:0] = pe_c2_r0__out7[31:0];
  assign pe_c2_r0__in2[31:0] = pe_c2_r1__out0[31:0];
  assign pe_c3_r1__in3[31:0] = pe_c2_r1__out1[31:0];
  assign pe_c2_r2__in0[31:0] = pe_c2_r1__out2[31:0];
  assign pe_c1_r1__in1[31:0] = pe_c2_r1__out3[31:0];
  assign pe_c1_r0__in6[31:0] = pe_c2_r1__out4[31:0];
  assign pe_c3_r0__in7[31:0] = pe_c2_r1__out5[31:0];
  assign pe_c3_r2__in4[31:0] = pe_c2_r1__out6[31:0];
  assign pe_c1_r2__in5[31:0] = pe_c2_r1__out7[31:0];
  assign pe_c2_r1__in2[31:0] = pe_c2_r2__out0[31:0];
  assign pe_c3_r2__in3[31:0] = pe_c2_r2__out1[31:0];
  assign pe_c2_r3__in0[31:0] = pe_c2_r2__out2[31:0];
  assign pe_c1_r2__in1[31:0] = pe_c2_r2__out3[31:0];
  assign pe_c1_r1__in6[31:0] = pe_c2_r2__out4[31:0];
  assign pe_c3_r1__in7[31:0] = pe_c2_r2__out5[31:0];
  assign pe_c3_r3__in4[31:0] = pe_c2_r2__out6[31:0];
  assign pe_c1_r3__in5[31:0] = pe_c2_r2__out7[31:0];
  assign pe_c2_r2__in2[31:0] = pe_c2_r3__out0[31:0];
  assign pe_c3_r3__in3[31:0] = pe_c2_r3__out1[31:0];
  assign io_bottom_2__in[31:0] = pe_c2_r3__out2[31:0];
  assign pe_c1_r3__in1[31:0] = pe_c2_r3__out3[31:0];
  assign pe_c1_r2__in6[31:0] = pe_c2_r3__out4[31:0];
  assign pe_c3_r2__in7[31:0] = pe_c2_r3__out5[31:0];
  assign io_top_3__in[31:0] = pe_c3_r0__out0[31:0];
  assign io_right_0__in[31:0] = pe_c3_r0__out1[31:0];
  assign pe_c3_r1__in0[31:0] = pe_c3_r0__out2[31:0];
  assign pe_c2_r0__in1[31:0] = pe_c3_r0__out3[31:0];
  assign pe_c2_r1__in5[31:0] = pe_c3_r0__out7[31:0];
  assign pe_c3_r0__in2[31:0] = pe_c3_r1__out0[31:0];
  assign io_right_1__in[31:0] = pe_c3_r1__out1[31:0];
  assign pe_c3_r2__in0[31:0] = pe_c3_r1__out2[31:0];
  assign pe_c2_r1__in1[31:0] = pe_c3_r1__out3[31:0];
  assign pe_c2_r0__in6[31:0] = pe_c3_r1__out4[31:0];
  assign pe_c2_r2__in5[31:0] = pe_c3_r1__out7[31:0];
  assign pe_c3_r1__in2[31:0] = pe_c3_r2__out0[31:0];
  assign io_right_2__in[31:0] = pe_c3_r2__out1[31:0];
  assign pe_c3_r3__in0[31:0] = pe_c3_r2__out2[31:0];
  assign pe_c2_r2__in1[31:0] = pe_c3_r2__out3[31:0];
  assign pe_c2_r1__in6[31:0] = pe_c3_r2__out4[31:0];
  assign pe_c2_r3__in5[31:0] = pe_c3_r2__out7[31:0];
  assign pe_c3_r2__in2[31:0] = pe_c3_r3__out0[31:0];
  assign io_right_3__in[31:0] = pe_c3_r3__out1[31:0];
  assign io_bottom_3__in[31:0] = pe_c3_r3__out2[31:0];
  assign pe_c2_r3__in1[31:0] = pe_c3_r3__out3[31:0];
  assign pe_c2_r2__in6[31:0] = pe_c3_r3__out4[31:0];
  assign pe_c0_r0__in3[31:0] = mem_0__out[31:0];
  assign pe_c0_r1__in3[31:0] = mem_1__out[31:0];
  assign pe_c0_r2__in3[31:0] = mem_2__out[31:0];
  assign pe_c0_r3__in3[31:0] = mem_3__out[31:0];
  assign pe_c0_r0__in0[31:0] = io_top_0__out[31:0];
  assign pe_c0_r3__in2[31:0] = io_bottom_0__out[31:0];
  assign pe_c1_r0__in0[31:0] = io_top_1__out[31:0];
  assign pe_c1_r3__in2[31:0] = io_bottom_1__out[31:0];
  assign pe_c2_r0__in0[31:0] = io_top_2__out[31:0];
  assign pe_c2_r3__in2[31:0] = io_bottom_2__out[31:0];
  assign pe_c3_r0__in0[31:0] = io_top_3__out[31:0];
  assign pe_c3_r3__in2[31:0] = io_bottom_3__out[31:0];
  assign pe_c3_r0__in1[31:0] = io_right_0__out[31:0];
  assign pe_c3_r1__in1[31:0] = io_right_1__out[31:0];
  assign pe_c3_r2__in1[31:0] = io_right_2__out[31:0];
  assign pe_c3_r3__in1[31:0] = io_right_3__out[31:0];
  assign io_bottom_0__ConfigIn = ConfigIn;
  assign io_bottom_1__ConfigIn = io_bottom_0__ConfigOut;
  assign io_bottom_2__ConfigIn = io_bottom_1__ConfigOut;
  assign io_bottom_3__ConfigIn = io_bottom_2__ConfigOut;
  assign io_right_0__ConfigIn = io_bottom_3__ConfigOut;
  assign io_right_1__ConfigIn = io_right_0__ConfigOut;
  assign io_right_2__ConfigIn = io_right_1__ConfigOut;
  assign io_right_3__ConfigIn = io_right_2__ConfigOut;
  assign io_top_0__ConfigIn = io_right_3__ConfigOut;
  assign io_top_1__ConfigIn = io_top_0__ConfigOut;
  assign io_top_2__ConfigIn = io_top_1__ConfigOut;
  assign io_top_3__ConfigIn = io_top_2__ConfigOut;
  assign mem_0__ConfigIn = io_top_3__ConfigOut;
  assign mem_1__ConfigIn = mem_0__ConfigOut;
  assign mem_2__ConfigIn = mem_1__ConfigOut;
  assign mem_3__ConfigIn = mem_2__ConfigOut;
  assign pe_c0_r0__ConfigIn = mem_3__ConfigOut;
  assign pe_c0_r1__ConfigIn = pe_c0_r0__ConfigOut;
  assign pe_c0_r2__ConfigIn = pe_c0_r1__ConfigOut;
  assign pe_c0_r3__ConfigIn = pe_c0_r2__ConfigOut;
  assign pe_c1_r0__ConfigIn = pe_c0_r3__ConfigOut;
  assign pe_c1_r1__ConfigIn = pe_c1_r0__ConfigOut;
  assign pe_c1_r2__ConfigIn = pe_c1_r1__ConfigOut;
  assign pe_c1_r3__ConfigIn = pe_c1_r2__ConfigOut;
  assign pe_c2_r0__ConfigIn = pe_c1_r3__ConfigOut;
  assign pe_c2_r1__ConfigIn = pe_c2_r0__ConfigOut;
  assign pe_c2_r2__ConfigIn = pe_c2_r1__ConfigOut;
  assign pe_c2_r3__ConfigIn = pe_c2_r2__ConfigOut;
  assign pe_c3_r0__ConfigIn = pe_c2_r3__ConfigOut;
  assign pe_c3_r1__ConfigIn = pe_c3_r0__ConfigOut;
  assign pe_c3_r2__ConfigIn = pe_c3_r1__ConfigOut;
  assign pe_c3_r3__ConfigIn = pe_c3_r2__ConfigOut;
  assign ConfigOut = pe_c3_r3__ConfigOut;
  assign io_bottom_0_IOPin_bidir_out[31:0] = io_bottom_0__IOPin_bidir_out[31:0];
  assign io_bottom_0__IOPin_bidir_in[31:0] = io_bottom_0_IOPin_bidir_in[31:0];
  assign io_bottom_1__IOPin_bidir_in[31:0] = io_bottom_1_IOPin_bidir_in[31:0];
  assign io_bottom_1_IOPin_bidir_out[31:0] = io_bottom_1__IOPin_bidir_out[31:0];
  assign io_bottom_2__IOPin_bidir_in[31:0] = io_bottom_2_IOPin_bidir_in[31:0];
  assign io_bottom_2_IOPin_bidir_out[31:0] = io_bottom_2__IOPin_bidir_out[31:0];
  assign io_bottom_3__IOPin_bidir_in[31:0] = io_bottom_3_IOPin_bidir_in[31:0];
  assign io_bottom_3_IOPin_bidir_out[31:0] = io_bottom_3__IOPin_bidir_out[31:0];
  assign io_right_0__IOPin_bidir_in[31:0] = io_right_0_IOPin_bidir_in[31:0];
  assign io_right_0_IOPin_bidir_out[31:0] = io_right_0__IOPin_bidir_out[31:0];
  assign io_right_1__IOPin_bidir_in[31:0] = io_right_1_IOPin_bidir_in[31:0];
  assign io_right_2__IOPin_bidir_in[31:0] = io_right_2_IOPin_bidir_in[31:0];
  assign io_right_1_IOPin_bidir_out[31:0] = io_right_1__IOPin_bidir_out[31:0];
  assign io_right_3__IOPin_bidir_in[31:0] = io_right_3_IOPin_bidir_in[31:0];
  assign io_right_2_IOPin_bidir_out[31:0] = io_right_2__IOPin_bidir_out[31:0];
  assign io_right_3_IOPin_bidir_out[31:0] = io_right_3__IOPin_bidir_out[31:0];
  assign io_top_0__IOPin_bidir_in[31:0] = io_top_0_IOPin_bidir_in[31:0];
  assign io_top_0_IOPin_bidir_out[31:0] = io_top_0__IOPin_bidir_out[31:0];
  assign io_top_1_IOPin_bidir_out[31:0] = io_top_1__IOPin_bidir_out[31:0];
  assign io_top_1__IOPin_bidir_in[31:0] = io_top_1_IOPin_bidir_in[31:0];
  assign io_top_2__IOPin_bidir_in[31:0] = io_top_2_IOPin_bidir_in[31:0];
  assign io_top_2_IOPin_bidir_out[31:0] = io_top_2__IOPin_bidir_out[31:0];
  assign io_top_3__IOPin_bidir_in[31:0] = io_top_3_IOPin_bidir_in[31:0];
  assign io_top_3_IOPin_bidir_out[31:0] = io_top_3__IOPin_bidir_out[31:0];
  assign mem_0_mem_unit_addr_to_ram[31:0] = mem_0__mem_unit_addr_to_ram[31:0];
  assign mem_0_mem_unit_data_in_to_ram[31:0] = mem_0__mem_unit_data_in_to_ram[31:0];
  assign mem_0_mem_unit_w_rq_to_ram[0:0] = mem_0__mem_unit_w_rq_to_ram[0:0];
  assign mem_1_mem_unit_addr_to_ram[31:0] = mem_1__mem_unit_addr_to_ram[31:0];
  assign mem_0__mem_unit_data_out_from_ram[31:0] = mem_0_mem_unit_data_out_from_ram[31:0];
  assign mem_1_mem_unit_data_in_to_ram[31:0] = mem_1__mem_unit_data_in_to_ram[31:0];
  assign mem_1__mem_unit_data_out_from_ram[31:0] = mem_1_mem_unit_data_out_from_ram[31:0];
  assign mem_1_mem_unit_w_rq_to_ram[0:0] = mem_1__mem_unit_w_rq_to_ram[0:0];
  assign mem_2_mem_unit_addr_to_ram[31:0] = mem_2__mem_unit_addr_to_ram[31:0];
  assign mem_2_mem_unit_data_in_to_ram[31:0] = mem_2__mem_unit_data_in_to_ram[31:0];
  assign mem_2__mem_unit_data_out_from_ram[31:0] = mem_2_mem_unit_data_out_from_ram[31:0];
  assign mem_2_mem_unit_w_rq_to_ram[0:0] = mem_2__mem_unit_w_rq_to_ram[0:0];
  assign mem_3_mem_unit_addr_to_ram[31:0] = mem_3__mem_unit_addr_to_ram[31:0];
  assign mem_3_mem_unit_data_in_to_ram[31:0] = mem_3__mem_unit_data_in_to_ram[31:0];
  assign mem_3__mem_unit_data_out_from_ram[31:0] = mem_3_mem_unit_data_out_from_ram[31:0];
  assign mem_3_mem_unit_w_rq_to_ram[0:0] = mem_3__mem_unit_w_rq_to_ram[0:0];

endmodule //cgra_U0
