/* First Word Fall-Through FIFO implementation. */

module fwft_fifo (
    reset, clk,
    // Interface to source.
    full, write_en, write_data,
    // Interface to sink.
    empty, read_en, read_data,
    // Number of words stored in the FIFO.
    usedw
);

parameter width = 32;
parameter widthad = 4;
parameter depth = 16;

input reset;
input clk;
// Interface to source.
output full;
input write_en;
input [width-1:0] write_data;
// Interface to sink.
output empty;
input read_en;
output [width-1:0] read_data;
// Number of words stored in the FIFO.
output reg [widthad:0] usedw = 0;

// The output data from RAM.
wire [width-1:0] ram_data;
// An extra register to either sample fifo output or write_data.
reg [width-1:0] sample_data;
// Use a mealy FSM with 4 states to handle the special cases.
localparam [1:0] EMPTY = 2'd0;
localparam [1:0] FALL_THRU = 2'd1;
localparam [1:0] LEFT_OVER = 2'd2;
localparam [1:0] STEADY = 2'd3;
reg [1:0] state = 2'd0;

always @ (posedge clk) begin
    if (reset) begin
        state <= EMPTY;
        sample_data <= {width{1'b0}};
    end else begin
        case (state)
            EMPTY:
                if (write_en) begin
                    state <= FALL_THRU;
                    //sample_data <= write_data;
                end else begin
                    state <= EMPTY;
                end
            FALL_THRU:  // usedw must be 1.
                if (write_en & ~read_en) begin
                    state <= STEADY;
                    //sample_data <= {width{1'bX}};
                end else if (~write_en & read_en) begin
                    state <= EMPTY;
                end else if (~write_en & ~read_en) begin
                    state <= STEADY;
                end else begin // write_en & read_en
                    state <= FALL_THRU;
                    //sample_data <= write_data;
                end
            LEFT_OVER:  // usedw must be > 1.
                if (usedw == 1 & read_en & ~write_en) begin
                    state <= EMPTY;
                end else if (usedw == 1 & read_en & write_en) begin
                    state <= FALL_THRU;
                    //sample_data <= write_data;
                end else if (read_en) begin
                    state <= STEADY;
                end else begin // ~read_en
                    state <= LEFT_OVER;
                    //sample_data <= sample_data;
                end
            STEADY:
                if (usedw == 1 & read_en & ~write_en) begin
                    state <= EMPTY;
                end else if (usedw == 1 & read_en & write_en) begin
                    state <= FALL_THRU;
                    sample_data <= write_data;
                end else if (~read_en) begin
                    state <= LEFT_OVER; // Only transition to LEFT_OVER.
                    //sample_data <= ram_data;
                end else begin
                    state <= STEADY;
                end
            default: begin
                 state <= EMPTY;
                 sample_data <= {width{1'b0}};
            end
        endcase
    end
end

assign read_data =  ram_data;

// Full and empty.
assign full = (usedw == depth);
assign empty = (usedw == 0);

// Read/Write port addresses.
reg [widthad-1:0] write_address = 0;
reg [widthad-1:0] read_address = 0;

function [widthad-1:0] increment;
    input [widthad-1:0] address;
    input integer depth;
    increment = (address == depth - 1) ? 0 : address + 1;
endfunction

always @ (posedge clk) begin
    if (reset) begin
        write_address <= 0;
        read_address <= 0;
    end else begin
        if (write_en & ~full)
            write_address <= increment(write_address, depth);
        if ((read_en & ~empty))
            read_address <= increment(read_address, depth);
    end
end

// Usedw.
always @ (posedge clk) begin
    if (reset) begin
        usedw <= 0;
    end else begin
        if ((write_en & ~full) & (read_en & ~empty))
            usedw <= usedw;
        else if (write_en & ~full)
            usedw <= usedw + 1;
        else if (read_en & ~empty)
            usedw <= usedw - 1;
        else
            usedw <= usedw;
    end
end

// byte enable is unsupported by inferred RAMs
reg [width-1:0] ram[depth-1:0];

/* synopsys translate_off */
integer i;
initial begin
		for (i = 0; i < depth; i = i + 1)
			ram[i] = 0;
   
end
/* synopsys translate_on */

always @ (posedge clk) begin // Port A
	if ( write_en & ~full ) begin
		ram[write_address] <= write_data;
	end
end

assign ram_data =ram[read_address];
	
endmodule
