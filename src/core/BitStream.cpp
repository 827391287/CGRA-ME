/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/CGRA.h>
#include <CGRA/Module.h>
#include <CGRA/ModuleElastic.h>


#include <numeric>
#include <ostream>

void BitStream::append(const ConfigCell* ccell, const std::vector<std::vector<BitSetting>>& bits,
                        int con_used) {
    if (not setting_storage.emplace(ccell, bits).second) {
        throw cgrame_error("adding duplicate config setting!");
    }
    contexts_used.emplace(ccell, con_used);
    ccell_order.push_back(ccell);
}

std::ostream& operator<<(std::ostream& os, const BitStream& bs)
{
    /**
     * Prints out the bit settings for each element. For each element, the bit
     * setting is printed from cycle 0 to II, from first bit to last bit
     */
    os << "BitStream {\n";
    for (const auto& ccell : bs.ccellOrder()) {
        for (auto bitSetting : bs.settingFor(ccell)) {
            os << ccell->getName() << ": " << bitSetting << '\n';
        }
    }
    os << '}';
    return os;
}

void BitStream::printTestbench(std::ostream& os, const int& II, bool is_elastic) const
{
    const auto context_storage_size = 1;
    static const char* const total_num_bits_localparam =   "TOTAL_NUM_BITS";
    static const char* const clock_sig =                   "clock";
    static const char* const enable_sig =                  "enable";
    static const char* const sync_reset_sig =              "sync_reset";
    static const char* const bitstream_sig =               "bitstream";
    static const char* const done_sig =                    "done";
    static const char* const storage =                     "storage";
    static const char* const storage_pos =                 "next_pos";
    int num_of_elastic_ccells = 0;
    for (auto ccell_it = rbegin(ccell_order); ccell_it != rend(ccell_order); ++ccell_it) {
        const auto& ccell = *ccell_it;
        if (dynamic_cast<const ElasticConfigCell*>(ccell)){
            num_of_elastic_ccells++;
        }  
    }
    if (num_of_elastic_ccells == 0) num_of_elastic_ccells = 1;
  const int total_num_bits = std::accumulate(begin(setting_storage), end(setting_storage), context_storage_size * num_of_elastic_ccells, [&](auto&& accum, auto&& ccell_and_setting) {
        return accum + ccell_and_setting.first->getStorageSize() * ccell_and_setting.second.size();
    });

    os <<
        "module CGRA_configurator(\n"
        "    input      "<<clock_sig<<",\n"
        "    input      "<<enable_sig<<",\n"
        "    input      "<<sync_reset_sig<<",\n"
        "\n"
        "    output reg "<<bitstream_sig<<",\n"
        "    output reg "<<done_sig<<"\n"
        ");\n"
        "\n"
        "    localparam "<<total_num_bits_localparam<<" = " << total_num_bits << ";\n"
    ;

    /**
     * Bitstream is given all the bits for an element at once. Bits are given
     * in order from last cycle to first cycle. From last bit to first bit.
     */
    os << "\treg [0:"<<total_num_bits_localparam<<"-1] "<<storage<<" = {\n";

    for (auto ccell_it = rbegin(ccell_order); ccell_it != rend(ccell_order); ++ccell_it) {
        const auto& ccell = *ccell_it;
        const auto& setting = setting_storage.at(ccell);
        os << "\t\t";
        for (auto bitsetting_cycle_it = rbegin(setting); bitsetting_cycle_it != rend(setting); ++bitsetting_cycle_it) {
            for (auto bitsetting_it = rbegin(*bitsetting_cycle_it); bitsetting_it != rend(*bitsetting_cycle_it); ++bitsetting_it) {

                const auto& bitsetting = *bitsetting_it;
                os << for_verilog(bitsetting);
                    if (std::next(ccell_it) != rend(ccell_order) || std::next(bitsetting_cycle_it) != rend(setting) || std::next(bitsetting_it) != rend(*bitsetting_cycle_it)) {
                        os << ',';
                    }
            }
        }
        if (dynamic_cast<const ElasticConfigCell*>(ccell) && II > 1){
            auto used_contexts = bitsettings_from_int(contexts_used.at(ccell) > 0 ? contexts_used.at(ccell) -1: 0, context_storage_size);
            for (int i = used_contexts.size(); i > 0; i-- ) {
                os << for_verilog(used_contexts.at(i-1));
                if (std::next(ccell_it) != rend(ccell_order) ) {
                    os << ',';
                }
            }
        }
        os << " // " << ccell->getAllConnectedPorts().at(0)->getModule().parent->getName() << "::" << ccell->getName() << '\n';
    }

    os <<
        "	};\n"
        "\n"
        "	reg [31:0] "<<storage_pos<<";\n"
        "	always @(posedge "<<clock_sig<<") begin\n"
        "		if (sync_reset) begin\n"
        "			"<<storage_pos<<" <= 0;\n"
        "			"<<bitstream_sig<<" <= 1'b0;\n"
        "			"<<done_sig<<" <= 0;\n"
        "		end else if ("<<storage_pos<<" >= "<<total_num_bits_localparam<<") begin\n"
        "			"<<done_sig<<" <= 1;\n"
        "			"<<bitstream_sig<<" <= 1'b0;\n"
        "		end else if ("<<enable_sig<<") begin\n"
        "			"<<bitstream_sig<<" <= "<<storage<<"["<<storage_pos<<"];\n"
        "			"<<storage_pos<<" <= "<<storage_pos<<" + 1;\n"
        "		end\n"
        "	end\n"
        "endmodule\n"
    ;
}
