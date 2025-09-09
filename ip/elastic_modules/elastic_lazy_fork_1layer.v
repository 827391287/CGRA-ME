/*
    A parametrized elasit (lazy) fork module reqresenting a single fork. Based on fork implemented in https://ieeexplore.ieee.org/document/5642631
*/
module elastic_lazy_fork_1layer
(
    valid_upstream,     // Valid signal from upstream elastic module
    stop_upstream,      // Stop signal to upstream elastic module

    valid_downstream,   // Valid signal to downstream elastic module(s)
    stop_downstream,    // Stop signal from downstream elastic module(s)

    enable_downstream   // Configuration bits to enable downstream modules (1 for enable)
);

    // Parameter declarations
    parameter FANOUT = 2;   // The number of downstream modules this fork connects to
    parameter IMPLEMENTATION_TYPE = 0;  // 1 for LKFork, 0 for standard Lazy Fork

    // Port declarations
    input valid_upstream;
    output stop_upstream;
    output [(FANOUT-1):0] valid_downstream;
    input [(FANOUT-1):0] stop_downstream;
    input [(FANOUT-1):0] enable_downstream;

    // Masked out downstream stop signals
    wire [(FANOUT-1):0] masked_stop_downstream;
    assign masked_stop_downstream = stop_downstream & enable_downstream;

    // Output assignments
    genvar i;
    generate
    if (IMPLEMENTATION_TYPE == 1) begin // @TODO: this LKFork implementation is currently incorrect
        assign stop_upstream = |masked_stop_downstream; // This is ok
        for (i = 0; i < FANOUT; i = i + 1) begin : LKFork_assignment
            assign valid_downstream[i] = valid_upstream & ( &( ~masked_stop_downstream | (1 << i) ) );
        end
        // assign valid_downstream = {FANOUT{valid_upstream}} & ~masked_stop_downstream;
    end else begin
        assign stop_upstream = |masked_stop_downstream;
        assign valid_downstream = {FANOUT{~(|masked_stop_downstream) & valid_upstream}};
        
    end
    endgenerate

endmodule