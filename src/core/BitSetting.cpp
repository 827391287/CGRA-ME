/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/BitSetting.h>
#include <CGRA/Exception.h>

#include <iostream>

template<>
BitSetting from_bool<BitSetting>(bool b) {
    if (b) {
        return BitSetting::HIGH;
    } else {
        return BitSetting::LOW;
    }
}

BitSetting force_to_rail(const BitSetting& bs) {
    switch (bs) {
        case BitSetting::LOW:
        case BitSetting::DONT_CARE_PREFER_LOW:
            return BitSetting::LOW;
        case BitSetting::HIGH:
        case BitSetting::DONT_CARE_PREFER_HIGH:
            return BitSetting::HIGH;
        default:
            throw cgrame_error("don't know how to force BitSetting to rail");
    }
}

std::ostream& operator<<(std::ostream& os, const BitSetting& bs) {
    switch (bs) {
        case BitSetting::LOW:
            return os << "0";
        case BitSetting::HIGH:
            return os << "1";
        case BitSetting::DONT_CARE_PREFER_LOW:
        case BitSetting::DONT_CARE_PREFER_HIGH:
            return os << "x";
        case BitSetting::HIGH_IMPEDANCE:
            return os << "z";
        default:
            throw cgrame_error("unhandled BitSetting in stream printing function");
    }    
}

std::ostream& operator<<(std::ostream& os, const std::vector<BitSetting>& bits) {
    os << '{';
    for (const auto& bit : bits) {
        os << bit;
    }
    os << '}';
    return os;
}

std::ostream& operator<<(std::ostream& os, const BitSettingForVerilog& bs) {
    switch (bs.val) {
        case BitSetting::LOW:
            return os << "1'b0";
        case BitSetting::HIGH:
            return os << "1'b1";
        case BitSetting::DONT_CARE_PREFER_LOW:
        case BitSetting::DONT_CARE_PREFER_HIGH:
            return os << "1'bx";
        case BitSetting::HIGH_IMPEDANCE:
            return os << "1'bz";
        default:
            throw cgrame_error("unhandled BitSettingForVerilog in stream printing function");
    }    
}
