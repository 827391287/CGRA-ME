/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __BITSETTING_H__
#define __BITSETTING_H__

#include <iosfwd>
#include <string>
#include <vector>

enum class BitSetting {
    LOW,
    HIGH,
    DONT_CARE_PREFER_LOW,
    DONT_CARE_PREFER_HIGH,
    HIGH_IMPEDANCE,
};

template<typename T>
T from_bool(bool b);

template<>
BitSetting from_bool<BitSetting>(bool b);
BitSetting force_to_rail(const BitSetting& bs);

std::ostream& operator<<(std::ostream& os, const BitSetting& bs);
std::ostream& operator<<(std::ostream& os, const std::vector<BitSetting>& bits);

struct BitSettingForVerilog {
    BitSetting val;
};

inline BitSettingForVerilog for_verilog(const BitSetting& bs) { return { bs }; }
std::ostream& operator<<(std::ostream& os, const BitSettingForVerilog& bs);

template<typename INTEGRAL>
std::vector<BitSetting>& push_back_int(std::vector<BitSetting>& v, const INTEGRAL& value, int num_bits) {
    for (int ibit = num_bits; ibit != 0; --ibit) {
        v.push_back(from_bool<BitSetting>((value >> (ibit-1)) % 2));
    }
    return v;
}

template<typename INTEGRAL>
std::vector<BitSetting> bitsettings_from_int(const INTEGRAL& value, int num_bits) {
	std::vector<BitSetting> result;
	push_back_int(result, value, num_bits);
	return result;
}

class BitConfig {
    public:
        BitConfig(int n_contexts) : bitSettings(n_contexts){};

        // Add bit setting for next cycle
        void add(std::vector<BitSetting> bitsetting, int cycle) {
            bitSettings[cycle] = std::move(bitsetting);
        };

        void setUsedCycles(int uc){ used_cycles = uc;}
        int getUsedCycles()const {return used_cycles;}

        // Add a bit to the end of the bit setting at cycle `cycle.
        // void add(BitSetting bitsetting, int cycle) {
            // bitSettings[cycle].emplace_back(bitsetting);
        // }

        const std::vector<BitSetting>& operator[](int cycle) const {
            return bitSettings[cycle];
        }

        auto begin() const {return bitSettings.begin();}
        auto end() const {return bitSettings.end();}

        const std::vector<std::vector<BitSetting>>& getBitSetting() const {
            return bitSettings;
        }
    private:
        // Outer vector keeps track of BitSetting per cycle
        // Inner vector are the bits for that cycle
        std::vector<std::vector<BitSetting>> bitSettings;
        int used_cycles = 0;
};

#endif /* __BITSETTING_H__ */
