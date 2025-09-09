/*
    A parametrized elastic (eager) fork module. Represents a single fork.
*/
module elastic_eager_fork_1layer
(
    clk,                // System clock for internal registers, internal registers clocked at posedge
    reset,              // High-active reset signal

    valid_upstream,     // Valid signal from upstream elastic module
    stop_upstream,      // Stop signal to upstream elastic module

    valid_downstream,   // Valid signal to downstream elastic module(s)
    stop_downstream,    // Stop signal from downstream elastic module(s)

    enable_downstream   // Configuration bits to enable downstream modules (1 for enable)
);

    // Parameter declarations
    parameter FANOUT = 2;   // The number of downstream modules this fork connects to

    // Port declarations
    input clk;
    input reset;
    input valid_upstream;
    output stop_upstream;
    output [(FANOUT-1):0] valid_downstream;
    input [(FANOUT-1):0] stop_downstream;
    input [(FANOUT-1):0] enable_downstream;

    // Internal Variables
    reg [(FANOUT-1):0] state_regs;          // Internal registers to mark whether that doesntream module has received data
    // @TODO: is this the most efficient place to mask out downstream modules?
    wire [(FANOUT-1):0] masked_state_regs;  // Masking the outputs of the state regs to remove certain downstream modules
    wire [(FANOUT-1):0] internal_stops;     // Internal stop signals, one for each downstream module
    wire internal_vas;                      // For simplification, internal signal for upstream valid & stop

    // Combinational assignments
    assign internal_stops = stop_downstream & masked_state_regs;
    assign internal_vas = (|internal_stops) & valid_upstream;
    assign masked_state_regs = state_regs & enable_downstream;

    // Output assignments
    assign stop_upstream = |internal_stops;
    assign valid_downstream = masked_state_regs & {FANOUT{valid_upstream}};

    // Internal register logic
    always @(posedge clk, posedge reset) 
    begin
        if (reset) 
        begin
            // @TODO: reset to 1 based on original elastic circuits paper
            state_regs <= 'h1; 
        end 
        else 
        begin
            state_regs <= internal_stops | {FANOUT{~internal_vas}};
        end
    end

endmodule