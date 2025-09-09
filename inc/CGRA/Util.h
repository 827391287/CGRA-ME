/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <utility>

#define cgrame_msg(m) std::cout << "[INFO] " << m << std::endl
#define cgrame_warn(m) std::cout << "[WARNING] " << m << std::endl
#define cgrame_label(l) std::cout << "[" << l << "] " << std::endl
#define cgrame_cmsg(m) std::cout << "[\033[1;34mINFO\033[0m] " << m << std::endl
#define cgrame_cwarn(m) std::cout << "[\033[1;33mWARNING\033[0m] " << m << std::endl
#define cgrame_clabel(l) std::cout << "[\033[1;36m" << l << "\033[0m] " << std::endl

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Based on boost hash_combine function
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
#endif
