/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

/**

  This is adres arch for mapping.
  All of the archs wont produce accurate RTL.
  Currently there are no configuration cells in this designs.

 **/

#include <CGRA/Exception.h>
#include <CGRA/user-inc/UserArchs.h>
#include <CGRA/user-inc/UserModules.h>
#include <climits>

#define has_input_from_N(conn) (conn & 0b00000001)
#define has_input_from_E(conn) (conn & 0b00000010)
#define has_input_from_S(conn) (conn & 0b00000100)
#define has_input_from_W(conn) (conn & 0b00001000)
#define has_input_from_NW(conn) (conn & 0b00010000)
#define has_input_from_NE(conn) (conn & 0b00100000)
#define has_input_from_SE(conn) (conn & 0b01000000)
#define has_input_from_SW(conn) (conn & 0b10000000)

// Lookup how many ones in a 4-bit data
const unsigned char ones_in_4b[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
#define get_ones_in_8b(v) (ones_in_4b[v & 0x0f] + ones_in_4b[v >> 4])

// Macros determining whether a PE is in corner or on side
#define is_corner_pe(r, c, rows, cols) (\
  (r == 0 && c == 0) ||\
  (r == 0 && c == cols-1) ||\
  (r == rows-1 && c == 0) ||\
  (r == rows-1 && c == cols-1)\
)
#define is_side_pe(r, c, rows, cols) (\
  (r == 0) ||\
  (c == 0) ||\
  (r == rows-1) ||\
  (c == cols-1)\
)
#define is_top_leftmost_pe(r, c, rows, cols) (r == 0 && c == 0)
#define is_top_rightmost_pe(r, c, rows, cols) (r == 0 && c == cols-1)
#define is_bot_leftmost_pe(r, c, rows, cols) (r == rows-1 && c == 0)
#define is_bot_rightmost_pe(r, c, rows, cols) (r == rows-1 && c == cols-1)
#define is_top_pe(r, c, rows, cols) (r == 0)
#define is_bot_pe(r, c, rows, cols) (r == rows-1)
#define is_leftmost_pe(r, c, rows, cols) (c == 0)
#define is_rightmost_pe(r, c, rows, cols) (c == cols-1)

// Masks used to cover invalid connections
#define mask_top_leftmost_pe(conn) (conn & 0b01000110)
#define mask_top_rightmost_pe(conn) (conn & 0b10001100)
#define mask_bot_leftmost_pe(conn) (conn & 0b00100011)
#define mask_bot_rightmost_pe(conn) (conn & 0b00011001)
#define mask_top_pe(conn) (conn & 0b11001110)
#define mask_bot_pe(conn) (conn & 0b00111011)
#define mask_leftmost_pe(conn) (conn & 0b01100111)
#define mask_rightmost_pe(conn) (conn & 0b10011101)


// Gets number of inputs from other PEs for the PE at row `r` and column `c`
// given the connectivity `conn`, total numbers of rows: `rows`, and columns: `cols`
unsigned char get_inter_pe_input_counts(unsigned char conn, int r, int c, int rows, int cols, bool toroid)
{
    int count = 0;
    if (toroid)
    {
        if (is_corner_pe(r, c, rows, cols)) count += 2;
        else if (is_side_pe(r, c, rows, cols)) count++;
    }
    count += get_ones_in_8b(conn);
    // Minus side and corner invalid connections
    if (is_corner_pe(r, c, rows, cols)) { // order of the condition is important
        if (r == 0 && c == 0) {
            // Top left corner does not have input from N NW NE W SW
            count -= has_input_from_N(conn)?1:0;
            count -= has_input_from_NW(conn)?1:0;
            count -= has_input_from_NE(conn)?1:0;
            count -= has_input_from_W(conn)?1:0;
            count -= has_input_from_SW(conn)?1:0;
        } else if (r == 0 && c == (cols - 1)) {
            // Top right corner does not have input from N NW NE E SE
            count -= has_input_from_N(conn)?1:0;
            count -= has_input_from_NW(conn)?1:0;
            count -= has_input_from_NE(conn)?1:0;
            count -= has_input_from_E(conn)?1:0;
            count -= has_input_from_SE(conn)?1:0;
        } else if (r == (rows - 1) && c == 0) {
            // Bottom left corner does not have input from S SW SE W NW
            count -= has_input_from_S(conn)?1:0;
            count -= has_input_from_SW(conn)?1:0;
            count -= has_input_from_SE(conn)?1:0;
            count -= has_input_from_W(conn)?1:0;
            count -= has_input_from_NW(conn)?1:0;
        } else if (r == (rows - 1) && c == (cols - 1)) {
            // Bottom right corner does not have input from S SW SE E NE
            count -= has_input_from_S(conn)?1:0;
            count -= has_input_from_SW(conn)?1:0;
            count -= has_input_from_SE(conn)?1:0;
            count -= has_input_from_E(conn)?1:0;
            count -= has_input_from_NE(conn)?1:0;
        }
        return count;
    } else if (is_side_pe(r, c, rows, cols)) {
        if (r == 0) {
            // Top row does not have input from N NW NE
            count -= has_input_from_N(conn)?1:0;
            count -= has_input_from_NW(conn)?1:0;
            count -= has_input_from_NE(conn)?1:0;
        } else if (c == 0) {
            // Leftmost column does not have input from W NW SW
            count -= has_input_from_W(conn)?1:0;
            count -= has_input_from_NW(conn)?1:0;
            count -= has_input_from_SW(conn)?1:0;
        } else if (r == (rows - 1)) {
            // Bottom row does not have input from S SW SE
            count -= has_input_from_S(conn)?1:0;
            count -= has_input_from_SW(conn)?1:0;
            count -= has_input_from_SE(conn)?1:0;
        } else if (c == (cols - 1)) {
            // Rightmost column does not have input from E NE SE
            count -= has_input_from_E(conn)?1:0;
            count -= has_input_from_SE(conn)?1:0;
            count -= has_input_from_NE(conn)?1:0;
        }
        return count;
    } else {
        return count;
    }
}

std::unique_ptr<CGRA> UserArchs::createAdresArch(const ConfigStore& args)
{
    const int cols = args.getInt("cols");
    const int rows = args.getInt("rows");
    const int toroid = args.getInt("toroid");
    const int hetero_fu_type = args.getInt("fu_type");
    const int fu_latency = args.getInt("fu_latency");
    const int fu_II = args.getInt("fu_II");
    const int rf_cols = args.getInt("rf_cols");
    const int rf_rows = args.getInt("rf_rows");
    const int num_const_addresses = args.getInt("num_const_addresses");
    const bool use_op_div = args.getBool("op_div");
    const bool extra_mem = args.getBool("extra_mem");
    const int pe_conn = args.getInt("pe_conn");
    const int II = args.getInt("II");
    const bool pred = args.getBool("pred");
    const bool reg_bypass = args.getBool("reg_bypass");
    const std::string pred_scheme = args.getString("pred_scheme");

    /* Verify if rows and cols are greater than 2 */
    if (rows < 2 || cols < 2)
        throw cgrame_error("ADRES Arch only accepts rows/cols >= 2");

    /* Verify if rf_rows and rf_cols are of valid values */
    if (((rows - 1) % rf_rows) != 0) // top row excluded
        throw cgrame_error("rows-1 must be divisible by rf_rows");

    if ((cols % rf_cols) != 0)
        throw cgrame_error("cols must be divisible by rf_rows");

    const unsigned RF_SIZE = rf_rows * rf_cols;
    auto cgra_storage = std::make_unique<CGRA>();
    Module* result = &cgra_storage->getTopLevelModule();

    result->isElastic = false;

    /**
     * Visualizataion is based on the naming: IOs on topmost row,
     * memory on the leftmsot row. PEs are arranged in grid with register files
     * in between them.
     */
    double moduleW = 1.0/(2*cols+1);
    double moduleH = 1.0/(2*rows+1);
    if (II > 1) {
        result->addSubModule(new ContextCell("context_cell", II)); // ContextCell stores num of used contexts (II)
    }
    /* IO Ports */
    for (unsigned int i = 0; i < cols; i++)
    {
        Location loc = {i + 1 , 0};
        result->addSubModule(new IO("io_top_" + std::to_string(i), loc, 32, II), (i+1)*moduleW, moduleH/2, moduleW, moduleH/2);
        if (II > 1) {
            result->addSubModule(new ContextCounter("context_counter_io" + std::to_string(i), II));
            result->addConnection("context_cell.Context_Used", "context_counter_io" + std::to_string(i) + ".Context_Used", false);
            result->addConnection("context_counter_io" + std::to_string(i) + ".Context", "io_top_" + std::to_string(i) + ".Context", false);
        }
    }

    /* Memory Ports */
    for (unsigned int i = 0; i < rows; i++)
    {
        Location loc = {cols + 1, i + 1};
        result->addSubModule(new MemPort("mem_" + std::to_string(i), loc, cols, 32, num_const_addresses, pred, II), 0, (i+1)*moduleH, moduleW, moduleH);
        if (extra_mem) {
            result->addSubModule(new MemPort("mem_right_" + std::to_string(i), loc, cols, 32, num_const_addresses, pred, II), 0, (i+1)*moduleH, moduleW, moduleH);
        }
        if (II > 1) {
            result->addSubModule(new ContextCounter("context_counter_mem" + std::to_string(i), II));
            result->addConnection("context_cell.Context_Used", "context_counter_mem" + std::to_string(i) + ".Context_Used", false);
            result->addConnection("context_counter_mem" + std::to_string(i) + ".Context", "mem_" + std::to_string(i) + ".Context", false);
            if (extra_mem) {
                result->addSubModule(new ContextCounter("context_counter_mem_right" + std::to_string(i), II));
                result->addConnection("context_cell.Context_Used", "context_counter_mem_right" + std::to_string(i) + ".Context_Used", false);
                result->addConnection("context_counter_mem_right" + std::to_string(i) + ".Context", "mem_right_" + std::to_string(i) + ".Context", false);
            }
        }
    }

    /* Adres Processing Elements */
    for (unsigned int c = 0; c < cols; c++)
    {
        for (unsigned int r = 0; r < rows; r++)
        {
            // select fu_type based on position: 'vliw' for top row, 'cga' for everything else
            std::string fu_type = "cga";
            if ((r == 0 || hetero_fu_type == 0) ||
                    (hetero_fu_type == 1 && ((r % 2) == 0)) ||
                    (hetero_fu_type == 2 && ((c % 2) == 0))) {
                fu_type = "vliw";
            }
            Location loc = {c+1, r+1};
            // PE needs inputs for 3~4 adjacent PEs, 1 mem port, and 1 regfile
            unsigned char count = get_inter_pe_input_counts(pe_conn, r, c, rows, cols, toroid);
	            result->addSubModule(
                        new AdresPE(
                            "pe_c" + std::to_string(c) + "_r" + std::to_string(r),
                            count + ((r == 0)?2:1) + extra_mem, // +2 inputs for first row due to IO
                            fu_type,
                            fu_II,
                            fu_latency,
                            II,
                            loc,
                            use_op_div,
                            pred,
                            reg_bypass,
                            extra_mem
                        ), (2*c+2)*moduleW, (2*r+2)*moduleH, moduleW, moduleH);
            if (II > 1) {
                result->addSubModule(new ContextCounter("context_counter_pe_c" + std::to_string(c) + "_r" + std::to_string(r), II));
                result->addConnection("context_cell.Context_Used", "context_counter_pe_c" + std::to_string(c) + "_r" + std::to_string(r) + ".Context_Used", false);
                result->addConnection("context_counter_pe_c" + std::to_string(c) + "_r" + std::to_string(r) + ".Context", "pe_c" + std::to_string(c) + "_r" + std::to_string(r) + ".Context", false);
            }
        }
    }

    /* IO-PE connections */
    for (unsigned int c = 0; c < cols; c++)
    {
        std::string io_n = "io_top_" + std::to_string(c);
        std::string blk_n = "pe_c" + std::to_string(c) + "_r0";
        result->addConnection(io_n + ".out", blk_n + ".in0"); //io->blk
        result->addConnection(blk_n + ".out", io_n + ".in"); //blk->io
    }

    /* Mem-PE connections */
    for (unsigned int r = 0; r < rows; r++)
    {
        std::string mem_n = "mem_" + std::to_string(r);
        std::string mem_right_n = "mem_right_" + std::to_string(r);

        for (unsigned int c = 0; c < cols; c++)
        {
            std::string blk_n = "pe_c" + std::to_string(c) + "_r" + std::to_string(r);
            result->addConnection(blk_n + ".out", mem_n + ".in" + std::to_string(c)); // to io
            result->addConnection(mem_n + ".out", blk_n + ((r == 0)?".in1":".in0")); // to block
            if (extra_mem) {
                result->addConnection(blk_n + ".out", mem_right_n + ".in" + std::to_string(c)); // to io
            result->addConnection(mem_right_n + ".out", blk_n + ((r == 0)?".in2":".in1")); // to block
            }
        }
    }

    // Connect VLIW FUs (top row) to one shared register file
    Location drf_loc = {1, UINT_MAX};
    result->addSubModule(new RegisterFile("drf", drf_loc, reg_bypass? cols * 2 : cols, 2*cols, 3, 32, II), 0.3, moduleH, 0.5, moduleH);
    if (II > 1) {
        result->addSubModule(new ContextCounter("context_counter_drf", II));
        result->addConnection("context_cell.Context_Used", "context_counter_drf.Context_Used", false);
        result->addConnection("context_counter_drf.Context", "drf.Context", false);
    }
    for (unsigned int c = 0; c < cols; c++)
    {
        std::string blk  = "pe_c" + std::to_string(c) + "_r0";

        result->addConnection(blk + ".fu_to_rf", "drf.in" + std::to_string(c));
        if (reg_bypass)
            result->addConnection(blk + ".bypass_to_rf", "drf.in" + std::to_string(cols + c));
        result->addConnection("drf.out" + std::to_string(c*2), blk + ".rf_to_muxa");
        result->addConnection("drf.out" + std::to_string(c*2+1), blk + ".rf_to_muxout");
    }

    // Instantiate External Register Files (CGA FUs only; excludes top row VLIW FUs)
    for (unsigned int c = 0; c < cols; c += rf_cols)
    {
        for (unsigned int r = 1; r < rows; r += rf_rows)
        {
            Location rf_loc = {c + 1, r + 1};
            // the name represents the minimal indices of the corner(top left) of RF
            std::string rf = "rf_c" + std::to_string(c) + "_r" + std::to_string(r);
            std::string blk = "pe_c" + std::to_string(c) + "_r" + std::to_string(r);
            result->addSubModule(new RegisterFile(rf, rf_loc, RF_SIZE + reg_bypass, 2, RF_SIZE, 32, II), (2*c+1)*moduleW, (2*r+1)*moduleH, moduleW, moduleH);
            result->addConnection(rf + ".out0", blk + ".rf_to_muxa");
            result->addConnection(rf + ".out1", blk + ".rf_to_muxout");
            if (II > 1) {
                result->addSubModule(new ContextCounter("context_counter_" + rf, II));
                result->addConnection("context_cell.Context_Used", "context_counter_" + rf + ".Context_Used", false);
                result->addConnection("context_counter_" + rf + ".Context", rf + ".Context", false);
            }
            std::string rfcc = "RfC" + std::to_string(c) + "R" + std::to_string(r);
        }
    }
    // Connect external RFs to PEs
    for (unsigned int c = 0; c < cols; c++)
    {
        for (unsigned int r = 1; r < rows; r++)
        {
            int c_mod = c % rf_cols;
            int r_mod = (r - 1) % rf_rows;
            int c_rf = c - c_mod;
            int r_rf = r - r_mod;
            int rf_in_i = 2 * r_mod + c_mod;
            std::string rf_in = std::to_string(rf_in_i);
            std::string rf = "rf_c" + std::to_string(c_rf) + "_r" + std::to_string(r_rf);
            std::string blk = "pe_c" + std::to_string(c) + "_r" + std::to_string(r);
            result->addConnection(blk + ".fu_to_rf", rf + ".in" + rf_in);
            if (reg_bypass) {
                result->addConnection(blk + ".bypass_to_rf", rf + ".in1");
            }
        }
    }

    // Connect PEs to PEs
    for (unsigned int c = 0; c < cols; c++)
    {
        for (unsigned int r = 0; r < rows; r++)
        {
            std::string blk_n_c_r = "pe_c" + std::to_string(c) + "_r" + std::to_string(r);
            std::string adj_blk_N = "pe_c" + std::to_string(c) + "_r" + std::to_string(r-1) + ".out";
            std::string adj_blk_E = "pe_c" + std::to_string(c+1) + "_r" + std::to_string(r) + ".out";
            std::string adj_blk_S = "pe_c" + std::to_string(c) + "_r" + std::to_string(r+1) + ".out";
            std::string adj_blk_W = "pe_c" + std::to_string(c-1) + "_r" + std::to_string(r) + ".out";
            std::string adj_blk_NW = "pe_c" + std::to_string(c-1) + "_r" + std::to_string(r-1) + ".out";
            std::string adj_blk_NE = "pe_c" + std::to_string(c+1) + "_r" + std::to_string(r-1) + ".out";
            std::string adj_blk_SE = "pe_c" + std::to_string(c+1) + "_r" + std::to_string(r+1) + ".out";
            std::string adj_blk_SW = "pe_c" + std::to_string(c-1) + "_r" + std::to_string(r+1) + ".out";

            // Determine the valid set of connections for this PE
            unsigned char true_conn = pe_conn;
            if (is_corner_pe(r, c, rows, cols)) {
                if (is_top_leftmost_pe(r, c, rows, cols)) {
                    true_conn = mask_top_leftmost_pe(pe_conn);
                } else if (is_top_rightmost_pe(r, c, rows, cols)) {
                    true_conn = mask_top_rightmost_pe(pe_conn);
                } else if (is_bot_leftmost_pe(r, c, rows, cols)) {
                    true_conn = mask_bot_leftmost_pe(pe_conn);
                } else if (is_bot_rightmost_pe(r, c, rows, cols)) {
                    true_conn = mask_bot_rightmost_pe(pe_conn);
                }
            } else if (is_side_pe(r, c, rows, cols)) {
                if (is_top_pe(r, c, rows, cols)) {
                    true_conn = mask_top_pe(pe_conn);
                } else if (is_bot_pe(r, c, rows, cols)) {
                    true_conn = mask_bot_pe(pe_conn);
                } else if (is_leftmost_pe(r, c, rows, cols)) {
                    true_conn = mask_leftmost_pe(pe_conn);
                } else if (is_rightmost_pe(r, c, rows, cols)) {
                    true_conn = mask_rightmost_pe(pe_conn);
                }
            }

            // Add connections based on `true_conn`
            int index = ((r == 0)?2:1) + extra_mem; // port starting index (since mem/io can occupy port0 & port1)
            std::string in_port;
            if (has_input_from_N(true_conn)) {
                in_port = ".in" + std::to_string(index);
                result->addConnection(adj_blk_N, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_E(true_conn)) {
                in_port = ".in" + std::to_string(index);
                result->addConnection(adj_blk_E, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_S(true_conn)) {
                in_port = ".in" + std::to_string(index);
                result->addConnection(adj_blk_S, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_W(true_conn)) {
                in_port = ".in" + std::to_string(index);
                result->addConnection(adj_blk_W, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_NW(true_conn)) {
                in_port = ".in" + std::to_string(index);
                result->addConnection(adj_blk_NW, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_NE(true_conn)) {
                in_port = ".in" + std::to_string(index);
                result->addConnection(adj_blk_NE, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_SE(true_conn)) {
                in_port = ".in" + std::to_string(index);
                result->addConnection(adj_blk_SE, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_SW(true_conn)) {
                in_port = ".in" + std::to_string(index);
                result->addConnection(adj_blk_SW, blk_n_c_r + in_port);
                index++;
            }
            if (toroid) {
                if (is_top_pe(r, c, rows, cols)) {
                    std::string src_pe = "pe_c" + std::to_string(c) + "_r" + std::to_string(rows-1) + ".out";
                    in_port = ".in" + std::to_string(index);
                    result->addConnection(src_pe, blk_n_c_r + in_port);
                    index++;
                }
                if (is_bot_pe(r, c, rows, cols)) {
                    std::string src_pe = "pe_c" + std::to_string(c) + "_r" + std::to_string(0) + ".out";
                    in_port = ".in" + std::to_string(index);
                    result->addConnection(src_pe, blk_n_c_r + in_port);
                    index++;
                }
                if (is_leftmost_pe(r, c, rows, cols)) {
                    std::string src_pe = "pe_c" + std::to_string(cols-1) + "_r" + std::to_string(r) + ".out";
                    in_port = ".in" + std::to_string(index);
                    result->addConnection(src_pe, blk_n_c_r + in_port);
                    index++;
                }
                if (is_rightmost_pe(r, c, rows, cols)) {
                    std::string src_pe = "pe_c" + std::to_string(0) + "_r" + std::to_string(r) + ".out";
                    in_port = ".in" + std::to_string(index);
                    result->addConnection(src_pe, blk_n_c_r + in_port);
                    index++;
                }
            }
        }
    }
    if (!pred) return cgra_storage;
    
    /****************** PRED NETWORK BEGINS **************/
    /* The predication network is the same as the normal */
    /* 32-bit architecture however it is a 1-bit wide n- */
    /* etwork.                                           */
    /*****************************************************/

    if (pred_scheme.find("event") != std::string::npos){
        result->addSubModule(new EventTransitionTable("EventTransitionTable", fu_II));
        result->addSubModule(new Multiplexer("event_mux", {0,0}, cols, 1));
        result->addConfig(new ConfigCell("eventMuxConfig"), {"event_mux.select"});
        result->addConnection("event_mux.out", "EventTransitionTable.current_event");
        for (int i = 0; i < cols; i++ ){
            result->addConnection("pe_c" + std::to_string(i) + "_r0.out_pred", 
                            "event_mux.in"+ std::to_string(i));
        }
    } else if (pred_scheme.find("partial") != std::string::npos){

    } else {
        throw cgrame_error("Cannot recognize the predication scheme");
    }
    int size = 1;
    /* IO Ports */
    for (unsigned int i = 0; i < cols; i++)
    {
        Location loc = {i + 1 , 0};
        result->addSubModule(new IO("io_top_pred_" + std::to_string(i), loc, size, II), (i+1)*moduleW, moduleH/2, moduleW, moduleH/2);
        if (II > 1)
            result->addConnection("context_counter_io" + std::to_string(i) + ".Context", "io_top_pred_" + std::to_string(i) + ".Context", false);
    }
    for (unsigned int c = 0; c < cols; c++)
    {
        std::string io_n = "io_top_pred_" + std::to_string(c);
        std::string blk_n = "pe_c" + std::to_string(c) + "_r0";
        result->addConnection(io_n + ".out", blk_n + ".in_pred" + std::to_string(extra_mem?2:1)); //io->blk
        result->addConnection(blk_n + ".out_pred", io_n + ".in"); //blk->io
    }
    /* Mem-PE connections */
    for (unsigned int r = 0; r < rows; r++)
    {
        std::string mem_n = "mem_" + std::to_string(r);
        std::string mem_right_n = "mem_right_" + std::to_string(r);

        for (unsigned int c = 0; c < cols; c++)
        {
            std::string blk_n = "pe_c" + std::to_string(c) + "_r" + std::to_string(r);
            result->addConnection(blk_n + ".out_pred", mem_n + ".pred" + std::to_string(c)); // to io
            if (extra_mem) {
                result->addConnection(blk_n + ".out_pred", mem_right_n + ".pred" + std::to_string(c)); // to io
            }
        }
    }

    Location pred_drf_loc = {1, UINT_MAX};
    // predicate architecture
    // Connect VLIW FUs (top row) to one shared register file
    result->addSubModule(new RegisterFile("drf_pred", pred_drf_loc, reg_bypass? cols * 2 : cols, 2*cols, 3, size, II), 0.3, moduleH, 0.5, moduleH);
    if (II > 1) {
        result->addConnection("context_counter_drf.Context", "drf_pred.Context", false);
    }
    for (unsigned int c = 0; c < cols; c++)
    {
        std::string blk  = "pe_c" + std::to_string(c) + "_r0";

        result->addConnection(blk + ".fu_to_rf_pred", "drf_pred.in" + std::to_string(c));
        if (reg_bypass)
            result->addConnection(blk + ".bypass_to_rf_pred", "drf_pred.in" + std::to_string(cols + c));
        result->addConnection("drf_pred.out" + std::to_string(c*2), blk + ".rf_to_muxa_pred");
        result->addConnection("drf_pred.out" + std::to_string(c*2+1), blk + ".rf_to_muxout_pred");
    }

    // Instantiate External Register Files (CGA FUs only; excludes top row VLIW FUs)
    for (unsigned int c = 0; c < cols; c += rf_cols)
    {
        for (unsigned int r = 1; r < rows; r += rf_rows)
        {
            Location loc = {c + 1, r + 1};
            // the name represents the minimal indices of the corner(top left) of RF
            std::string rf_org = "rf_c" + std::to_string(c) + "_r" + std::to_string(r);
            std::string rf = "rf_pred_c" + std::to_string(c) + "_r" + std::to_string(r);
            std::string blk = "pe_c" + std::to_string(c) + "_r" + std::to_string(r);
            result->addSubModule(new RegisterFile(rf, loc, RF_SIZE + reg_bypass, 2, RF_SIZE, size, II), (2*c+1)*moduleW, (2*r+1)*moduleH, moduleW, moduleH);
            result->addConnection(rf + ".out0", blk + ".rf_to_muxa_pred");
            result->addConnection(rf + ".out1", blk + ".rf_to_muxout_pred");
            if (II > 1)
                result->addConnection("context_counter_" + rf_org + ".Context", rf + ".Context", false);

            std::string rfcc = "RfC" + std::to_string(c) + "R" + std::to_string(r);
        }
    }
    // Connect external RFs to PEs
    for (unsigned int c = 0; c < cols; c++)
    {
        for (unsigned int r = 1; r < rows; r++)
        {
            int c_mod = c % rf_cols;
            int r_mod = (r - 1) % rf_rows;
            int c_rf = c - c_mod;
            int r_rf = r - r_mod;
            int rf_in_i = 2 * r_mod + c_mod;
            std::string rf_in = std::to_string(rf_in_i);
            std::string rf = "rf_pred_c" + std::to_string(c_rf) + "_r" + std::to_string(r_rf);
            std::string blk = "pe_c" + std::to_string(c) + "_r" + std::to_string(r);
            result->addConnection(blk + ".fu_to_rf_pred", rf + ".in" + rf_in);
            if (reg_bypass) {
                result->addConnection(blk + ".bypass_to_rf_pred", rf + ".in1");
            }
        }
    }

    // Connect PEs to PEs
    for (unsigned int c = 0; c < cols; c++)
    {
        for (unsigned int r = 0; r < rows; r++)
        {
            std::string blk_n_c_r = "pe_c" + std::to_string(c) + "_r" + std::to_string(r);
            std::string adj_blk_N = "pe_c" + std::to_string(c) + "_r" + std::to_string(r-1) + ".out_pred";
            std::string adj_blk_E = "pe_c" + std::to_string(c+1) + "_r" + std::to_string(r) + ".out_pred";
            std::string adj_blk_S = "pe_c" + std::to_string(c) + "_r" + std::to_string(r+1) + ".out_pred";
            std::string adj_blk_W = "pe_c" + std::to_string(c-1) + "_r" + std::to_string(r) + ".out_pred";
            std::string adj_blk_NW = "pe_c" + std::to_string(c-1) + "_r" + std::to_string(r-1) + ".out_pred";
            std::string adj_blk_NE = "pe_c" + std::to_string(c+1) + "_r" + std::to_string(r-1) + ".out_pred";
            std::string adj_blk_SE = "pe_c" + std::to_string(c+1) + "_r" + std::to_string(r+1) + ".out_pred";
            std::string adj_blk_SW = "pe_c" + std::to_string(c-1) + "_r" + std::to_string(r+1) + ".out_pred";

            // Determine the valid set of connections for this PE
            unsigned char true_conn = pe_conn;
            if (is_corner_pe(r, c, rows, cols)) {
                if (is_top_leftmost_pe(r, c, rows, cols)) {
                    true_conn = mask_top_leftmost_pe(pe_conn);
                } else if (is_top_rightmost_pe(r, c, rows, cols)) {
                    true_conn = mask_top_rightmost_pe(pe_conn);
                } else if (is_bot_leftmost_pe(r, c, rows, cols)) {
                    true_conn = mask_bot_leftmost_pe(pe_conn);
                } else if (is_bot_rightmost_pe(r, c, rows, cols)) {
                    true_conn = mask_bot_rightmost_pe(pe_conn);
                }
            } else if (is_side_pe(r, c, rows, cols)) {
                if (is_top_pe(r, c, rows, cols)) {
                    true_conn = mask_top_pe(pe_conn);
                } else if (is_bot_pe(r, c, rows, cols)) {
                    true_conn = mask_bot_pe(pe_conn);
                } else if (is_leftmost_pe(r, c, rows, cols)) {
                    true_conn = mask_leftmost_pe(pe_conn);
                } else if (is_rightmost_pe(r, c, rows, cols)) {
                    true_conn = mask_rightmost_pe(pe_conn);
                }
            }

            // Add connections based on `true_conn`
            int index = ((r == 0)?2:1) + extra_mem; // port starting index (since mem/io can occupy port0 & port1)
            std::string in_port;
            if (has_input_from_N(true_conn)) {
                in_port = ".in_pred" + std::to_string(index);
                result->addConnection(adj_blk_N, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_E(true_conn)) {
                in_port = ".in_pred" + std::to_string(index);
                result->addConnection(adj_blk_E, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_S(true_conn)) {
                in_port = ".in_pred" + std::to_string(index);
                result->addConnection(adj_blk_S, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_W(true_conn)) {
                in_port = ".in_pred" + std::to_string(index);
                result->addConnection(adj_blk_W, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_NW(true_conn)) {
                in_port = ".in_pred" + std::to_string(index);
                result->addConnection(adj_blk_NW, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_NE(true_conn)) {
                in_port = ".in_pred" + std::to_string(index);
                result->addConnection(adj_blk_NE, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_SE(true_conn)) {
                in_port = ".in_pred" + std::to_string(index);
                result->addConnection(adj_blk_SE, blk_n_c_r + in_port);
                index++;
            }
            if (has_input_from_SW(true_conn)) {
                in_port = ".in_pred" + std::to_string(index);
                result->addConnection(adj_blk_SW, blk_n_c_r + in_port);
                index++;
            }
            if (toroid) {
                if (is_top_pe(r, c, rows, cols)) {
                    std::string src_pe = "pe_c" + std::to_string(c) + "_r" + std::to_string(rows-1) + ".out_pred";
                    in_port = ".in_pred" + std::to_string(index);
                    result->addConnection(src_pe, blk_n_c_r + in_port);
                    index++;
                }
                if (is_bot_pe(r, c, rows, cols)) {
                    std::string src_pe = "pe_c" + std::to_string(c) + "_r" + std::to_string(0) + ".out_pred";
                    in_port = ".in_pred" + std::to_string(index);
                    result->addConnection(src_pe, blk_n_c_r + in_port);
                    index++;
                }
                if (is_leftmost_pe(r, c, rows, cols)) {
                    std::string src_pe = "pe_c" + std::to_string(cols-1) + "_r" + std::to_string(r) + ".out_pred";
                    in_port = ".in_pred" + std::to_string(index);
                    result->addConnection(src_pe, blk_n_c_r + in_port);
                    index++;
                }
                if (is_rightmost_pe(r, c, rows, cols)) {
                    std::string src_pe = "pe_c" + std::to_string(0) + "_r" + std::to_string(r) + ".out_pred";
                    in_port = ".in_pred" + std::to_string(index);
                    result->addConnection(src_pe, blk_n_c_r + in_port);
                    index++;
                }
            }
        }
    }
    return cgra_storage;
}
