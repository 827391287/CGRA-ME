/*
    A parametrized elastic (eager) fork module.
*/
module elastic_eager_fork
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
    parameter FANOUT = 8;               // The number of downstream modules this fork connects to
    parameter UNIT_FORK_FANOUT = 2;     // The width of unit forks to use to create this fork
    parameter FORK_LAYERS = 1; // Number of layers in the fork tree

    // Port declarations
    input clk;
    input reset;
    input valid_upstream;
    output stop_upstream;
    output [(FANOUT-1):0] valid_downstream;
    input [(FANOUT-1):0] stop_downstream;
    input [(FANOUT-1):0] enable_downstream;

    // Extra parameters
    parameter ADJUSTED_FANOUT = (FANOUT % UNIT_FORK_FANOUT) ? (FANOUT - (FANOUT % UNIT_FORK_FANOUT) + UNIT_FORK_FANOUT) : FANOUT;    // Rounded up to the nearest multiple of UNIT_FORK_FANOUT

    // Internal signals - we will use the upper-left triangular portion of these matrices
    wire [(ADJUSTED_FANOUT-1):0] internal_valid_wire [FORK_LAYERS:0];
    wire [(ADJUSTED_FANOUT-1):0] internal_stop_wire [FORK_LAYERS:0];

    // Connect top/bottom layers of wires to I/Os - top layer (0) is downstream, bottom layer is upstream
    assign internal_valid_wire[FORK_LAYERS][0] = valid_upstream;
    assign stop_upstream = internal_stop_wire[FORK_LAYERS][0];

    assign valid_downstream = internal_valid_wire[0][(FANOUT-1):0];
    assign internal_stop_wire[0] = {{(ADJUSTED_FANOUT-FANOUT){1'b0}}, stop_downstream};

    // Helper function to determine valid points at elaboration
    function integer get_valid_points;
        input integer layer_idx;
        input integer fanout;
        input integer unit_fanout;
        integer ret, k;
        begin
            ret = fanout;
            for (k = 0; k < layer_idx; k = k + 1) begin
                // For each layer, the amount of valid points is divided by UNIT_FORK_FANOUT (and then ceiled)
                ret = (ret + unit_fanout - 1) / unit_fanout;
            end
            get_valid_points = ret;
        end
    endfunction

    // Instantiate and connect unit forks
    // NOTES:
    // - Each layer has 1/UNIT_FORK_FANOUT (ceiled) the amount of forks
    // - i = layer depth (from downstream), j = position within a layer
    genvar i, j;
    generate
    for (i = 0; i < FORK_LAYERS; i = i + 1) begin : layer_iteration   // Iterate over all layers
        // Calculate the number of valid (required) nodes at this layer and the next - done at compile time
        localparam integer valid_points = get_valid_points(i, FANOUT, UNIT_FORK_FANOUT);
        localparam integer next_layer_valid_points = get_valid_points(i+1, FANOUT, UNIT_FORK_FANOUT);
        // initial $display("Layer %d has %d valid points and is downstream of %d forks.", i, valid_points, next_layer_valid_points);

        for (j = 0; j < next_layer_valid_points; j = j + 1) begin : row_iteration  // Iterate within each layer
            // We additionally need to check if one of the inputs to this fork needs tobe "clipped" as in it reads invalid data
            // to clip we simply disable that downstream node
            if ( (j+1)*UNIT_FORK_FANOUT <= valid_points ) begin : noclip
                elastic_eager_fork_1layer #(UNIT_FORK_FANOUT) UNIT_FORK 
                (
                    .clk(clk),
                    .reset(reset),
                    .valid_upstream(internal_valid_wire[ i+1 ][ j ]),
                    .stop_upstream(internal_stop_wire[ i+1 ][ j ]),
                    .valid_downstream(internal_valid_wire[ i ][ j*UNIT_FORK_FANOUT +: UNIT_FORK_FANOUT ]),
                    .stop_downstream(internal_stop_wire[ i ][ j*UNIT_FORK_FANOUT +: UNIT_FORK_FANOUT ]),
                    .enable_downstream((i == 0)? enable_downstream[ j*UNIT_FORK_FANOUT +: UNIT_FORK_FANOUT ] : {UNIT_FORK_FANOUT{1'b1}})
                );
            end else begin : clip
                elastic_eager_fork_1layer #(UNIT_FORK_FANOUT) UNIT_FORK 
                (
                    .clk(clk),
                    .reset(reset),
                    .valid_upstream(internal_valid_wire[ i+1 ][ j ]),
                    .stop_upstream(internal_stop_wire[ i+1 ][ j ]),
                    .valid_downstream(internal_valid_wire[ i ][ j*UNIT_FORK_FANOUT +: UNIT_FORK_FANOUT ]),
                    .stop_downstream(internal_stop_wire[ i ][ j*UNIT_FORK_FANOUT +: UNIT_FORK_FANOUT ]),
                    .enable_downstream({ {((j+1)*UNIT_FORK_FANOUT - valid_points){1'b0}} , ( i == 0 ? enable_downstream[j*UNIT_FORK_FANOUT +: (valid_points - j*UNIT_FORK_FANOUT)] : {(valid_points - j*UNIT_FORK_FANOUT){1'b1}} ) })
                );
                // initial $display("Fork %d in this layer has to be clipped.", j);
            end
        end
    end
    endgenerate

endmodule
