/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

//HyCUBE.cpp
//Implements the HyCUBE architecture using the C++ API in CGRA-ME
//Utilizes HyCUBEPE, a processing element specified in the HyCUBE paper

#include <CGRA/Exception.h>
#include <CGRA/user-inc/UserArchs.h>
#include <CGRA/user-inc/UserModules.h>
#include <vector>

// Checking for connections
#define has_input_from_N(conn) (conn & 0b00000001)
#define has_input_from_E(conn) (conn & 0b00000010)
#define has_input_from_S(conn) (conn & 0b00000100)
#define has_input_from_W(conn) (conn & 0b00001000)
#define has_input_from_NW(conn) (conn & 0b00010000)
#define has_input_from_NE(conn) (conn & 0b00100000)
#define has_input_from_SE(conn) (conn & 0b01000000)
#define has_input_from_SW(conn) (conn & 0b10000000)

// Identifying PE position
#define is_top_pe(r, c, rows, cols) (r == 0)
#define is_bot_pe(r, c, rows, cols) (r == rows-1)
#define is_leftmost_pe(r, c, rows, cols) (c == 0)
#define is_rightmost_pe(r, c, rows, cols) (c == cols-1)

// Masks
#define mask_top_pe(conn) ((conn & 0b11001111) | 0b00000001)  // Remove NE, NW connections, add N connection
#define mask_bot_pe(conn) ((conn & 0b00111111) | 0b00000100)  // Remove SE, SW connections, add S connection
#define mask_leftmost_pe(conn) ((conn & 0b01101111) | 0b00001000)  // Remove NW, SW connections, add W connection
#define mask_rightmost_pe(conn) ((conn & 0b10011111) | 0b00000010)  // Remove NE, SE connections, add E connection
// Predication Masks
#define mask_top_pred(conn) ((conn & 0b11001111) | 0b00000001)  // Remove NE, NW connections, add N connection
#define mask_bot_pred(conn) ((conn & 0b00111111) | 0b00000100)  // Remove SE, SW connections, add S connection
#define mask_leftmost_pred(conn) ((conn & 0b01100111))  // Remove NW, SW connections, remove W connection
#define mask_rightmost_pred(conn) ((conn & 0b10011111) | 0b00000010)  // Remove NE, SE connections, add E connection
#define mask_rightmost_pred_mem(conn) ((conn & 0b10011101))  // Remove NE, SE connections, remove E connection
// Event Predication Masks
#define mask_top_event_pred(conn) ((conn & 0b11001111) | 0b00000001)  // Remove NE, NW connections, add N connection

int leftshift2(int val) {
    // bitshifts val left by 2 bits, with wraparound
    // val is assumed to only have the right-most 4 bits populated
    return (((val << 2) & 0x0f) | (val >> 2));
}

void fillPEConnArray(std::vector < std::vector < std::vector<int> > > &pcv,
    int pe_conn_orig, int rows, int cols, bool pred, bool extra_mem = false, std::string pred_scheme = "") {
    // fills the pe_conn_vector with all incoming and outgoing connections, including special cases (architecture-specific)
    // pcv is a vector whose address is set to pe_conn_vector (editing pcv_ptr also edits pe_conn_vector)

    // Variable Declaration
    int lefthalf;
    int righthalf;
    int pe_conn_out;


    // 1. Fill all inputs
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            pcv[0][r][c] = pe_conn_orig;
        }
    }

    // 2. Fill all outputs
    // Isolate first 4 and last 4 bits in pe_conn
    lefthalf = (pe_conn_orig >> 4) & 0x0f;
    righthalf = pe_conn_orig & 0x0f;

    // Calculate outputs of left and right half
    lefthalf = leftshift2(lefthalf);
    righthalf = leftshift2(righthalf);

    // Put them back together to get pe_conn_out
    pe_conn_out = (lefthalf << 4) | righthalf;

    // Fill vector
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            pcv[1][r][c] = pe_conn_out;
        }
    }

    // 3. Consider HyCUBE-specific edge cases (corners are handled in the process)
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            // Case 1: Top Row PE's
            if (is_top_pe(r, c, rows, cols)) {
                if (pred) {
                    if (pred_scheme.find("event") != std::string::npos) {
                        // Change outputs
                        pcv[1][r][c] = mask_top_event_pred(pcv[1][r][c]);
                    } else {
                        // Change outputs
                        pcv[1][r][c] = mask_top_pred(pcv[1][r][c]);
                    }
                    // Change inputs
                    pcv[0][r][c] = mask_top_pred(pcv[0][r][c]);
                } else {
                    // Change inputs
                    pcv[0][r][c] = mask_top_pe(pcv[0][r][c]);
                    // Change outputs
                    pcv[1][r][c] = mask_top_pe(pcv[1][r][c]);
                }
            }

            // Case 2: Bottom Row PE's
            if (is_bot_pe(r, c, rows, cols)) {
                if (pred) {
                    // Change inputs
                    pcv[0][r][c] = mask_bot_pred(pcv[0][r][c]);
                    // Change outputs
                    pcv[1][r][c] = mask_bot_pred(pcv[1][r][c]);
                } else {
                    // Change inputs
                    pcv[0][r][c] = mask_bot_pe(pcv[0][r][c]);
                    // Change outputs
                    pcv[1][r][c] = mask_bot_pe(pcv[1][r][c]);
                }
            }

            // Case 3: Left Column PE's
            if (is_leftmost_pe(r, c, rows, cols)) {
                if (pred) {
                    // Change inputs
                    pcv[0][r][c] = mask_leftmost_pred(pcv[0][r][c]);
                    // Change outputs NOT a mistake TODO change PE
                    pcv[1][r][c] = mask_leftmost_pe(pcv[1][r][c]);
                } else {
                    // Change inputs
                    pcv[0][r][c] = mask_leftmost_pe(pcv[0][r][c]);
                    // Change outputs
                    pcv[1][r][c] = mask_leftmost_pe(pcv[1][r][c]);
                }
            }

            // Case 4: Right Column PE's
            if (is_rightmost_pe(r, c, rows, cols)) {
                if (pred) {
                    // Change inputs
                    if (extra_mem) {
                        pcv[0][r][c] = mask_rightmost_pred_mem(pcv[0][r][c]);
                    } else {
                        pcv[0][r][c] = mask_rightmost_pred(pcv[0][r][c]);
                    }
                    // Change outputs
                    pcv[1][r][c] = mask_rightmost_pred(pcv[1][r][c]);
                } else {
                    // Change inputs
                    pcv[0][r][c] = mask_rightmost_pe(pcv[0][r][c]);
                    // Change outputs
                    pcv[1][r][c] = mask_rightmost_pe(pcv[1][r][c]);
                }
            }
        }
    }
}

std::unique_ptr<CGRA> UserArchs::createHyCUBEArch(const ConfigStore& args) {
    // Declare all variables
    const unsigned int cols = args.getInt("cols");
    const unsigned int rows = args.getInt("rows");
    const int fu_II = args.getInt("fu_II");
    const int pe_conn = args.getInt("pe_conn");
    const int fu_latency = args.getInt("fu_latency");
    const int num_const_addresses = args.getInt("num_const_addresses");
    const bool pred = args.getBool("pred");
    const int II = args.getInt("II");
    const bool extra_mem = args.getBool("extra_mem");
    const std::string pred_scheme = args.getString("pred_scheme");

    /**
     * Visualizataion is based on the naming: IOs on topmost, rightmost and
     * bottommost row, memory on the leftmost row. PEs are arranged in grid
     * between.
     */
    double moduleW = 1.0/(cols+2);
    double moduleH = 1.0/(rows+2);

    // Determine bit width for predication
    const int SIZE = [&]() {
            return 32;
    }();

    // Variables for use when making connections between tiles
    std::string north = std::to_string(0);
    std::string east = std::to_string(1);
    std::string west = std::to_string(3);
    std::string south = std::to_string(2);
    std::string northWest = std::to_string(4);
    std::string northEast = std::to_string(5);
    std::string southEast = std::to_string(6);
    std::string southWest = std::to_string(7);

    // Determine all input and output connections

    // Vector describes the incoming and outgoing connection PE's (including to external units such as memory)
    // Organization: pe_conn_vec[in(0)/out(1)] [rows] [cols] ie pe_conn_vec[0][1][2]
    // stores the incoming connections for the PE at row 1, column 2

    std::vector < std::vector < std::vector<int> > >
        pe_conn_vec(2, std::vector< std::vector<int> >(rows, std::vector<int>(cols)));

    std::vector < std::vector < std::vector<int> > >
        pred_conn_vec(2, std::vector< std::vector<int> >(rows, std::vector<int>(cols)));
    // Populate pe_conn_array
    fillPEConnArray(pe_conn_vec, pe_conn, rows, cols, false);

    fillPEConnArray(pred_conn_vec, pe_conn, rows, cols, pred, extra_mem,     pred_scheme);
    auto cgra_storage = std::make_unique<CGRA>();
    Module* result = &cgra_storage->getTopLevelModule();
    if (II > 1) {
        result->addSubModule(new ContextCell("context_cell", II)); // ContextCell stores num of used contexts (II)
    }
    // 1. Add all elements within the Architecture
    // HyCUBE Processing Elements
    for (unsigned int c = 0; c < cols; c++) {
        for (unsigned int r = 0; r < rows; r++) {
            Location loc = {c + 1, r + 1};
            auto PE = new HyCUBEPE(
                "pe_c" + std::to_string(c) + "_r" + std::to_string(r),
                pred,
                pe_conn_vec[0][r][c],
                pe_conn_vec[1][r][c],
                pred_conn_vec[0][r][c],
                pred_conn_vec[1][r][c],
                fu_II,
                II,
                fu_latency,
                loc,
                pred_scheme);
            result->addSubModule(PE, (c+1.0)*moduleW, (r+1.0)*moduleH, moduleW, moduleH);
            if (II > 1) {
                result->addSubModule(new ContextCounter("context_counter_pe_c" + std::to_string(c) + "_r" + std::to_string(r), II));
                result->addConnection("context_cell.Context_Used", "context_counter_pe_c" + std::to_string(c) + "_r" + std::to_string(r) + ".Context_Used", false);
                result->addConnection("context_counter_pe_c" + std::to_string(c) + "_r" + std::to_string(r) + ".Context", "pe_c" + std::to_string(c) + "_r" + std::to_string(r) + ".Context", false);
            }
        }
        
    }

    // Memory Ports
    for (unsigned int r = 0; r < rows; r++) {
        Location loc = {0, r + 1};
        auto mem_port = new MemPort("mem_" + std::to_string(r), loc, rows, SIZE, num_const_addresses, pred, II);
        result->addSubModule(mem_port, 0, (r+1.0)*moduleH, moduleW, moduleH);
        if (II > 1) {
            result->addSubModule(new ContextCounter("context_counter_mem_" + std::to_string(r), II));
            result->addConnection("context_cell.Context_Used", "context_counter_mem_" + std::to_string(r) + ".Context_Used", false);
            result->addConnection("context_counter_mem_" + std::to_string(r) + ".Context", "mem_" + std::to_string(r) + ".Context", false);
        }
    }

    // I/O's
    for (unsigned int c = 0; c < cols; c++) {
        Location loc_bot = {c + 1, rows + 1};
        Location loc_top = {c + 1, 0};
        auto io_top = new IO("io_top_" + std::to_string(c), loc_top, SIZE, II);
        auto io_bottom = new IO("io_bottom_" + std::to_string(c), loc_bot, SIZE, II);
        result->addSubModule(io_top, c/(cols+2.0), 0, moduleW, moduleH);
        result->addSubModule(io_bottom, c*moduleW, (rows+1.0)*moduleH, moduleW, moduleH);
        if (II > 1) {
            result->addSubModule(new ContextCounter("context_counter_io_top_" + std::to_string(c), II));
            result->addSubModule(new ContextCounter("context_counter_io_bottom_" + std::to_string(c), II));
            result->addConnection("context_cell.Context_Used", "context_counter_io_top_" + std::to_string(c) + ".Context_Used", false);
            result->addConnection("context_cell.Context_Used", "context_counter_io_bottom_" + std::to_string(c) + ".Context_Used", false);
            result->addConnection("context_counter_io_top_" + std::to_string(c) + ".Context", "io_top_" + std::to_string(c) + ".Context", false);
            result->addConnection("context_counter_io_bottom_" + std::to_string(c) + ".Context", "io_bottom_" + std::to_string(c) + ".Context", false);
        }
    }

    for (unsigned int r = 0; r < rows; r++) {
        Location loc = {cols + 1, r + 1};
        if (extra_mem) {
            auto mem_port = new MemPort("mem_right_" + std::to_string(r), loc, rows, SIZE, num_const_addresses, pred, II);
            result->addSubModule(mem_port, 0, (r+1.0)*moduleH, moduleW, moduleH);
            if (II > 1) {
                result->addSubModule(new ContextCounter("context_counter_mem_right_" + std::to_string(r), II));
                result->addConnection("context_cell.Context_Used", "context_counter_mem_right_" + std::to_string(r) + ".Context_Used", false);
                result->addConnection("context_counter_mem_right_" + std::to_string(r) + ".Context", "mem_right_" + std::to_string(r) + ".Context", false);
            }
        } else {
            auto io_right = new IO("io_right_" + std::to_string(r), loc, SIZE, II);
            result->addSubModule(io_right, (cols+1.0)*moduleW, (r+1.0)*moduleH, moduleW, moduleH);
            if (II > 1) {
                result->addSubModule(new ContextCounter("context_counter_io_right_" + std::to_string(r), II));
                result->addConnection("context_cell.Context_Used", "context_counter_io_right_" + std::to_string(r) + ".Context_Used", false);
                result->addConnection("context_counter_io_right_" + std::to_string(r) + ".Context", "io_right_" + std::to_string(r) + ".Context", false);
            }
        }
    }

    // 2. Add Connections
    // Inter-PE Connections (NEWS)
    for (unsigned int c = 0; c < cols; c++) {
        for (unsigned int r = 0; r < rows; r++) {
            std::string currC = std::to_string(c);
            std::string currR = std::to_string(r);

            std::string blk_n_c_r = "pe_c" + currC + "_r" + currR;

            // Use pe_conn to determine whether a connection should be made
            if (has_input_from_N(pe_conn_vec[0][r][c])) {
                // Special Case: Top Row PE's connect to I/O's
                if (r == 0) {
                    result->addConnection("io_top_" + currC + ".out",
                                          blk_n_c_r + ".in" + north);

                    result->addConnection(blk_n_c_r + ".out" + north,
                                          "io_top_" + currC + ".in");
                } else {
                    result->addConnection("pe_c" + currC + "_r" + std::to_string(r-1) + ".out" + south,
                                          blk_n_c_r + ".in" + north);
                }
            }

            if (has_input_from_E(pe_conn_vec[0][r][c])) {
                // Special Case: Right-most PE's connect to I/O's
                if (c == cols - 1) {
                    if (extra_mem) {
                        result->addConnection("mem_right_" + currR + ".out",
                            blk_n_c_r + ".in" + east);

                        std::string _blk_prefix = "pe_c" + currC + "_r";
                        for (int _r = 0; _r < rows; _r++) {
                            std::string _currR = std::to_string(_r);
                            std::string _blk_n_c_r = _blk_prefix + _currR;
                            result->addConnection(_blk_n_c_r + ".out" + east,
                                "mem_right_" + currR + ".in" + _currR);
                        }
                    } else {
                        result->addConnection("io_right_" + currR + ".out",
                                              blk_n_c_r + ".in" + east);

                        result->addConnection(blk_n_c_r + ".out" + east,
                                             "io_right_" + currR + ".in");
                    }
                } else {
                    result->addConnection("pe_c" + std::to_string(c+1) + "_r" + currR + ".out" + west,
                                          blk_n_c_r + ".in" + east);
                }
            }

            if (has_input_from_S(pe_conn_vec[0][r][c])) {
                // Special Case: Bottom-most PE's connect to I/O's
                if (r == rows - 1) {
                    result->addConnection("io_bottom_" + currC + ".out",
                                          blk_n_c_r + ".in" + south);

                    result->addConnection(blk_n_c_r + ".out" + south,
                                          "io_bottom_" + currC + ".in");
                } else {
                    result->addConnection("pe_c" + currC + "_r" + std::to_string(r+1) + ".out" + north,
                                          blk_n_c_r + ".in" + south);
                }
            }

            if (has_input_from_W(pe_conn_vec[0][r][c])) {
                // Special Case: Left-most PE's connect to Memory Ports
                if (c == 0) {
                    result->addConnection("mem_" + currR + ".out",
                                          blk_n_c_r + ".in" + west);

                    std::string _blk_prefix = "pe_c" + currC + "_r";
                    for (int _r = 0; _r < rows; _r++) {
                        std::string _currR = std::to_string(_r);
                        std::string _blk_n_c_r = _blk_prefix + _currR;
                        result->addConnection(_blk_n_c_r + ".out" + west,
                                              "mem_" + currR + ".in" + _currR);
                    }
                } else {
                    result->addConnection("pe_c" + std::to_string(c-1) + "_r" + currR + ".out" + east,
                                          blk_n_c_r + ".in" + west);
                }
            }

            if (has_input_from_NW(pe_conn_vec[0][r][c])) {
                result->addConnection("pe_c" + std::to_string(c-1) + "_r" + std::to_string(r-1) + ".out" + southEast,
                                      blk_n_c_r + ".in" + northWest);
            }

            if (has_input_from_NE(pe_conn_vec[0][r][c])) {
                result->addConnection("pe_c" + std::to_string(c+1) + "_r" + std::to_string(r-1) + ".out" + southWest,
                                      blk_n_c_r + ".in" + northEast);
            }

            if (has_input_from_SE(pe_conn_vec[0][r][c])) {
                result->addConnection("pe_c" + std::to_string(c+1) + "_r" + std::to_string(r+1) + ".out" + northWest,
                                      blk_n_c_r + ".in" + southEast);
            }

            if (has_input_from_SW(pe_conn_vec[0][r][c])) {
                result->addConnection("pe_c" + std::to_string(c-1) + "_r" + std::to_string(r+1) + ".out" + northEast,
                                      blk_n_c_r + ".in" + southWest);
            }
        }
    }

    if (!pred) return cgra_storage;

    /****************** PRED NETWORK BEGINS **************/
    /* The predication network is the same as the normal */
    /* 32-bit architecture however it is a 1-bit wide n- */
    /* etwork.                                           */
    /*****************************************************/

    if (pred_scheme.find("event") != std::string::npos) {
        result->addSubModule(new EventTransitionTable("EventTransitionTable", fu_II));
        result->addSubModule(new Multiplexer("event_mux", {0, 0}, cols, 1));
        result->addConfig(new ConfigCell("eventMuxConfig"), {"event_mux.select"});
        result->addConnection("event_mux.out", "EventTransitionTable.current_event");
        for (int i = 0; i < cols; i++) {
            result->addConnection("pe_c" + std::to_string(i) + "_r0.out_pred" + north,
                            "event_mux.in"+ std::to_string(i));
        }
    } else if (pred_scheme.find("partial") != std::string::npos) {
    } else {
        throw cgrame_error("Cannot recognize the predication scheme");
    }
    // If predication is set to true connect the predicate network.
    // I/O's

    // I/O's
    for (unsigned int c = 0; c < cols; c++) {
        Location loc_bot = {c + 1, rows + 1};
        Location loc_top = {c + 1, 0};
        auto io_top = new IO("io_top_pred_" + std::to_string(c), loc_top, 1, II);
        auto io_bottom = new IO("io_bottom_pred_" + std::to_string(c), loc_bot, 1, II);
        result->addSubModule(io_top, c/(cols+2.0), 0, moduleW, moduleH);
        result->addSubModule(io_bottom, c*moduleW, (rows+1.0)*moduleH, moduleW, moduleH);
        if (II > 1) {
            result->addConnection("context_counter_io_top_" + std::to_string(c) + ".Context", "io_top_pred_" + std::to_string(c) + ".Context", false);
            result->addConnection("context_counter_io_bottom_" + std::to_string(c) + ".Context", "io_bottom_pred_" + std::to_string(c) + ".Context", false);
        }
    }

    for (unsigned int r = 0; r < rows; r++) {
        Location loc = {cols + 1, r + 1};
        if (!extra_mem) {
            auto io_right = new IO("io_right_pred_" + std::to_string(r), loc, 1, II);
            result->addSubModule(io_right, (cols+1.0)*moduleW, (r+1.0)*moduleH, moduleW, moduleH);
            if (II > 1) {
                result->addConnection("context_counter_io_right_" + std::to_string(r) + ".Context", "io_right_pred_" + std::to_string(r) + ".Context", false);
            }
        }
    }

    // 1. Add Connections
    // Inter-PE Connections (NEWS)
    for (unsigned int c = 0; c < cols; c++) {
        for (unsigned int r = 0; r < rows; r++) {
            std::string currC = std::to_string(c);
            std::string currR = std::to_string(r);
            std::string blk_n_c_r = "pe_c" + currC + "_r" + currR;

            // Use pe_conn to determine whether a connection should be made
            if (has_input_from_N(pred_conn_vec[0][r][c])) {
                // Special Case: Top Row PE's connect to I/O's
                if (r == 0) {
                    result->addConnection("io_top_pred_" + currC + ".out",
                                          blk_n_c_r + ".in_pred" + north);

                    result->addConnection(blk_n_c_r + ".out_pred" + north,
                                          "io_top_pred_" + currC + ".in");
                } else {
                    result->addConnection("pe_c" + currC + "_r" + std::to_string(r - 1) + ".out_pred" + south,
                                          blk_n_c_r + ".in_pred" + north);
                }
            }

            if (has_input_from_E(pred_conn_vec[1][r][c])) {
                if (c == cols - 1) {
                    if (extra_mem) {
                        std::string _blk_prefix = "pe_c" + currC + "_r";
                        for (int _r = 0; _r < rows; _r++) {
                            std::string _currR = std::to_string(_r);
                            std::string _blk_n_c_r = _blk_prefix + _currR;
                            result->addConnection(_blk_n_c_r + ".out_pred" + east,
                                                  "mem_right_" + currR + ".pred" + _currR);
                        }
                    } else {
                        result->addConnection("io_right_pred_" + currR + ".out",
                                              blk_n_c_r + ".in_pred" + east);

                        result->addConnection(blk_n_c_r + ".out_pred" + east,
                                              "io_right_pred_" + currR + ".in");
                    }
                } else {
                    result->addConnection("pe_c" + std::to_string(c + 1) + "_r" + currR + ".out_pred" + west,
                                          blk_n_c_r + ".in_pred" + east);
                }
            }

            if (has_input_from_S(pred_conn_vec[0][r][c])) {
                if (r == rows - 1) {
                    result->addConnection("io_bottom_pred_" + currC + ".out",
                                          blk_n_c_r + ".in_pred" + south);

                    result->addConnection(blk_n_c_r + ".out_pred" + south,
                                          "io_bottom_pred_" + currC + ".in");
                } else {
                    result->addConnection("pe_c" + currC + "_r" + std::to_string(r + 1) + ".out_pred" + north,
                                          blk_n_c_r + ".in_pred" + south);
                }
            }

            if (has_input_from_W(pred_conn_vec[1][r][c])) {
                 // Special Case: Left-most PE's connect to Memory Ports
                if (c == 0) {
                    std::string _blk_prefix = "pe_c" + currC + "_r";
                    for (int _r = 0; _r < rows; _r++) {
                        std::string _currR = std::to_string(_r);
                        std::string _blk_n_c_r = _blk_prefix + _currR;
                        result->addConnection(_blk_n_c_r + ".out_pred" + west,
                                              "mem_" + currR + ".pred" + _currR);
                    }
                } else {
                    result->addConnection("pe_c" + std::to_string(c - 1) + "_r" + currR + ".out_pred" + east,
                                          blk_n_c_r + ".in_pred" + west);
                }
            }

            if (has_input_from_NW(pred_conn_vec[0][r][c])) {
                result->addConnection(
                    "pe_c" + std::to_string(c - 1) + "_r" + std::to_string(r - 1) + ".out_pred" + southEast,
                    blk_n_c_r + ".in_pred" + northWest);
            }

            if (has_input_from_NE(pred_conn_vec[0][r][c])) {
                result->addConnection(
                    "pe_c" + std::to_string(c + 1) + "_r" + std::to_string(r - 1) + ".out_pred" + southWest,
                    blk_n_c_r + ".in_pred" + northEast);
            }

            if (has_input_from_SE(pred_conn_vec[0][r][c])) {
                result->addConnection(
                    "pe_c" + std::to_string(c + 1) + "_r" + std::to_string(r + 1) + ".out_pred" + northWest,
                    blk_n_c_r + ".in_pred" + southEast);
            }

            if (has_input_from_SW(pred_conn_vec[0][r][c])) {
                result->addConnection(
                    "pe_c" + std::to_string(c - 1) + "_r" + std::to_string(r + 1) + ".out_pred" + northEast,
                    blk_n_c_r + ".in_pred" + southWest);
            }
        }
    }
    return cgra_storage;
}
