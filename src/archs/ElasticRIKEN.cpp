/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

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
#define mask_top_pe(conn) ((conn & 0b11001111) | 0b00000001) // Remove NE, NW connections, add N connection
#define mask_bot_pe(conn) ((conn & 0b00111111) | 0b00000100) // Remove SE, SW connections, add S connection
#define mask_leftmost_pe(conn) ((conn & 0b01101111) | 0b00001000) // Remove NW, SW connections, add W connection
#define mask_rightmost_pe(conn) ((conn & 0b10011111) | 0b00000010) // Remove NE, SE connections, add E connection

int RIKENelasticleftshift2(int val)
{   // bitshifts val left by 2 bits, with wraparound
    // val is assumed to only have the right-most 4 bits populated
    return (((val << 2) & 0x0f) | (val >> 2));
}

// This function sets the inter-PE connectivity for all of the PEs
// I.E. it sets the bitmasks indicating if PEs connect to the NORTH, SOUTH, etc.
void RIKENelasticFillPEConnArray(std::vector < std::vector < std::vector<int> > > &pcv, int pe_conn_orig, int rows, int cols)
{   

    // Variable Declaration
    int lefthalf;
    int righthalf;
    int pe_conn_out;

    // 1. Fill all inputs
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            pcv[0][r][c] = pe_conn_orig;
        }
    }

    // 2. Fill all outputs
    // Isolate first 4 and last 4 bits in pe_conn
    lefthalf = (pe_conn_orig >> 4) & 0x0f;
    righthalf = pe_conn_orig & 0x0f;

    // Calculate outputs of left and right half
    lefthalf = RIKENelasticleftshift2(lefthalf);
    righthalf = RIKENelasticleftshift2(righthalf);

    // Put them back together to get pe_conn_out
    pe_conn_out = (lefthalf << 4) | righthalf;

    // Fill vector
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            pcv[1][r][c] = pe_conn_out;
        }
    }

    // 3. Consider HyCUBE-specific edge cases (corners are handled in the process)
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            // Case 1: Top Row PE's
            if(is_top_pe(r, c, rows, cols))
            {
	      // Change inputs
	      pcv[0][r][c] = mask_top_pe(pcv[0][r][c]);
	      // Change outputs
	      pcv[1][r][c] = mask_top_pe(pcv[1][r][c]);
            }

            // Case 2: Bottom Row PE's
            if(is_bot_pe(r, c, rows, cols))
            {
	      // Change inputs
	      pcv[0][r][c] = mask_bot_pe(pcv[0][r][c]);
	      // Change outputs
	      pcv[1][r][c] = mask_bot_pe(pcv[1][r][c]);
            }

            // Case 3: Left Column PE's
            if(is_leftmost_pe(r, c, rows, cols))
            {
	      // Change inputs
	      pcv[0][r][c] = mask_leftmost_pe(pcv[0][r][c]);
	      // Change outputs
	      pcv[1][r][c] = mask_leftmost_pe(pcv[1][r][c]);
            }

            // Case 4: Right Column PE's
            if(is_rightmost_pe(r, c, rows, cols))
            {
	      // Change inputs
	      pcv[0][r][c] = mask_rightmost_pe(pcv[0][r][c]);
	      // Change outputs
	      pcv[1][r][c] = mask_rightmost_pe(pcv[1][r][c]);
            }
        }
    } 
}


std::unique_ptr<CGRA> UserArchs::createElasticRIKENArch(const ConfigStore& args)
{
    // Declare all variables
    const unsigned int cols = args.getInt("cols");
    const unsigned int rows = args.getInt("rows");
    const int contexts = args.getInt("II");
    const int fu_II = args.getInt("fu_II");
    const int pe_conn = args.getInt("pe_conn");
    const int fu_latency = args.getInt("fu_latency");
    const int eb_depth = args.getInt("eb_depth");
    const bool eb_enable = args.getBool("eb_enable");
    const int type = args.getInt("type");
    //const bool pred = args.getBool("pred");
    //std::string pred_type = args.getString("pred_type");

    /*if (pred_type.compare("full") != 0 && pred_type.compare("partial") != 0) {
        throw cgrame_error("Predication type should be full or partial");
    }*/
    
    /**
     * For visualization ONLY -- deprecated
     */
    double moduleW = 1.0/(cols+2);
    double moduleH = 1.0/(rows+2);

    // Set bit width to 32
    const int SIZE = 32;

    // Variables for use when making connections between tiles
    std::string north = std::to_string(0);
    std::string east = std::to_string(1);
    std::string west = std::to_string(3);
    std::string south = std::to_string(2);
    std::string northWest = std::to_string(4);
    std::string northEast = std::to_string(5);
    std::string southEast = std::to_string(6);
    std::string southWest = std::to_string(7);

    // Determine all input and output connections from each PE

    // Vectors describe the incoming and outgoing connections from each PE
    // Organization: pe_conn_vec[in(0)/out(1)] [rows] [cols] ie pe_conn_vec[0][1][2] stores the incoming connections for the PE at row 1, column 2
    std::vector < std::vector < std::vector<int> > > pe_conn_vec(2, std::vector< std::vector<int> >(rows, std::vector<int>(cols)));

    // Populate pe_conn_array
    RIKENelasticFillPEConnArray(pe_conn_vec, pe_conn, rows, cols);
    
    auto cgra_storage = std::make_unique<CGRA>();
    Module* result = &cgra_storage->getTopLevelModule();
    result->isElastic = true;
    bool isElastic = result->isElastic;

    // Instantiate all PEs in the array
    for (unsigned int c = 0; c < cols; c++)
    {
        for(unsigned int r = 0; r < rows; r++)
        {
	        Location loc = {c + 1, r + 1};

            result->addSubModule(new RIKEN_PE_Elastic("pe_c" + std::to_string(c) + "_r" + std::to_string(r), pe_conn_vec[0][r][c], pe_conn_vec[1][r][c], fu_II, fu_latency, loc, type, eb_depth, eb_enable, contexts), (c+1.0)*moduleW, (r+1.0)*moduleH, moduleW, moduleH);
        }
    }

    // IOs on the top and bottom of the array
    for (unsigned int c = 0; c < cols; c++)
    {
        Location loc_bot = {c + 1, rows + 1};   
        Location loc_top = {c + 1, 0};
        result->addSubModule(new IO("io_top_" + std::to_string(c), loc_top, SIZE, contexts, isElastic), c/(cols+2.0), 0, moduleW, moduleH);
        result->addSubModule(new IO("io_bottom_" + std::to_string(c), loc_bot, SIZE, contexts, isElastic), c*moduleW, (rows+1.0)*moduleH, moduleW, moduleH);
    }

    // IOs on the left and right of the array
    for (unsigned int r = 0; r < rows; r++)
    {
        Location loc_l = {0, r + 1};
	Location loc_r = {cols + 1, r + 1};
        result->addSubModule(new IO("io_left_" + std::to_string(r), loc_l, SIZE, contexts, isElastic), (cols+1.0)*moduleW, (r+1.0)*moduleH, moduleW, moduleH);
	result->addSubModule(new IO("io_right_" + std::to_string(r), loc_r, SIZE, contexts, isElastic), (cols+1.0)*moduleW, (r+1.0)*moduleH, moduleW, moduleH);
    }

    // Inter-PE connections based on the connectivity vectors
    for(unsigned int c = 0; c < cols; c++)
    {
        for(unsigned int r = 0; r < rows; r++)
        {
            std::string currC = std::to_string(c);
            std::string currR = std::to_string(r);

            std::string blk_n_c_r = "pe_c" + currC + "_r" + currR;

            // Use pe_conn to determine whether a connection should be made
            if(has_input_from_N(pe_conn_vec[0][r][c]))
            {
                //Special Case: Top row PE's connect to I/O's
                if(r == 0)
                {
		            result->connectPorts("io_top_" + currC + ".out",
			            blk_n_c_r + ".in" + north, isElastic);

		            result->connectPorts(blk_n_c_r + ".out" + north,
			            "io_top_" + currC + ".in", isElastic);
                }else
                {
		            result->connectPorts("pe_c" + currC + "_r" + std::to_string(r-1) + ".out" + south,
			            blk_n_c_r + ".in" + north, isElastic);
                }
            }

            if(has_input_from_E(pe_conn_vec[0][r][c]))
            {
                // Special Case: Right-most PE's connect to I/O's
                if(c == cols - 1)
                {
		            result->connectPorts("io_right_" + currR + ".out",
			            blk_n_c_r + ".in" + east, isElastic);
		    
		            result->connectPorts(blk_n_c_r + ".out" + east,
			            "io_right_" + currR + ".in", isElastic);
                }else
                {
		            result->connectPorts("pe_c" + std::to_string(c+1) + "_r" + currR + ".out" + west,
			            blk_n_c_r + ".in" + east, isElastic);
                }
            }

            if(has_input_from_S(pe_conn_vec[0][r][c]))
            {
                // Special Case: Bottom-most PE's connect to I/O's
                if(r == rows - 1)
                {
	        	    result->connectPorts("io_bottom_" + currC + ".out",
    		    	    blk_n_c_r + ".in" + south, isElastic);
		    
		            result->connectPorts(blk_n_c_r + ".out" + south,
			            "io_bottom_" + currC + ".in", isElastic);
                }else
                {
		            result->connectPorts("pe_c" + currC + "_r" + std::to_string(r+1) + ".out" + north,
			            blk_n_c_r + ".in" + south, isElastic);
                }
            }

            if(has_input_from_W(pe_conn_vec[0][r][c]))
            {
                // Special Case: Left-most PE's connect to I/O's
                if(c == 0)
                {
		            result->connectPorts("io_left_" + currR + ".out",
		            		       blk_n_c_r + ".in" + west, isElastic);

		            result->connectPorts(blk_n_c_r + ".out" + west,
				       "io_left_" + currR + ".in", isElastic);
                }else
                {
                    result->connectPorts("pe_c" + std::to_string(c-1) + "_r" + currR + ".out" + east,
                                          blk_n_c_r + ".in" + west, isElastic);

                }
            }

            if(has_input_from_NW(pe_conn_vec[0][r][c]))
            {
		        result->connectPorts("pe_c" + std::to_string(c-1) + "_r" + std::to_string(r-1) + ".out" + southEast,
			        blk_n_c_r + ".in" + northWest, isElastic);
            }

            if(has_input_from_NE(pe_conn_vec[0][r][c]))
            {
		        result->connectPorts("pe_c" + std::to_string(c+1) + "_r" + std::to_string(r-1) + ".out" + southWest,
			        blk_n_c_r + ".in" + northEast, isElastic);
            }

            if(has_input_from_SE(pe_conn_vec[0][r][c]))
            {
		        result->connectPorts("pe_c" + std::to_string(c+1) + "_r" + std::to_string(r+1) + ".out" + northWest,
			        blk_n_c_r + ".in" + southEast, isElastic);
            }

            if(has_input_from_SW(pe_conn_vec[0][r][c]))
            {
		        result->connectPorts("pe_c" + std::to_string(c-1) + "_r" + std::to_string(r+1) + ".out" + northEast,
			        blk_n_c_r + ".in" + southWest, isElastic);
            }
        }
    }

    return cgra_storage; 
}
