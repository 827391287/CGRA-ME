/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __CGRA_H__
#define __CGRA_H__

class CGRA;

#include <CGRA/BitSetting.h>
#include <CGRA/Mapping.h>
#include <CGRA/Module.h>

#include <memory>
#include <vector>
#include <unordered_map>

class ConfigCell;
class Module;
class MRRG;
class OpGraph;
enum class VerilogType;

/**
 * Represents a configuration bitstream.
 * Provides storage for BitSettings, and an order among ConfigCells
 */
class BitStream
{
    public:
        using BitArray = std::vector<std::vector<BitSetting>>;

        /**
         * Add the given bits to the configuration database,
         * and place ccell at the end of the ConfigCell order.
         */
        void append(const ConfigCell* ccell, const BitArray& bits, int con_used= 0);

        /**
         * Prints out a Verilog file intended to help with creating a testbench
         * for the CGRA.
         */
        void printTestbench(std::ostream& os, const int& II, bool is_elastic = false) const;

        /**
         * Get the configuration for a particular ConfigCell
         * throws if it does not exist.
         */
        const BitArray& settingFor(const ConfigCell* cc) const { return setting_storage.at(cc); }

        /**
         * The ordering among the ConfigCells.
         */
        auto& ccellOrder() const { return ccell_order; }

        /**
         * Print the bitstream in a human-friendly way.
         */
        friend std::ostream& operator<<(std::ostream& os, const BitStream& bs);

    private:
        std::unordered_map<const ConfigCell*, BitArray> setting_storage = {};
        std::vector<const ConfigCell*> ccell_order = {};
        std::unordered_map<const ConfigCell*, int> contexts_used = {};
};

namespace adl0 { class ADL; }

// This class represents a top level CGRA architecture
class CGRA
{
    public:
        CGRA(std::string name = "CGRA", std::string templateName = "cgra");
        BitStream genBitStream(const Mapping& mapping);  // Function that generates bitstream
        void genTimingConstraints(OpGraph * mappped_opgraph);   // Function that determines timing analysis of design mapped onto CGRA
        void genFloorPlan();                                    // Floorplans CGRA blocks. Only works well with a homogeneous array
        int getNumRows() {return num_floorplan_rows;}
        int getNumCols() {return num_floorplan_columns;}
        void setNumRows(int numRows) {num_floorplan_rows = numRows;}
        void setNumCols(int numCols) {num_floorplan_columns = numCols;}

        const MRRG& getMRRG(int II);
        Module& getTopLevelModule() { return *top_level_module; }
        const Module& getTopLevelModule() const { return *top_level_module; }

        void genVerilog(VerilogType vt, std::string dir, const int& SII);
        void genHybrid(VerilogType vt, std::string dir, int mem_size);

        std::vector<std::string> hybridPorts;
    private:
        friend class adl0::ADL;
        std::unique_ptr<Module> top_level_module;
        std::vector<std::shared_ptr<const MRRG>>      mrrgs = {}; // Keeps references to all the MRRGs for each II
        int num_floorplan_rows = 0;
        int num_floorplan_columns = 0;
};

#endif

