/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __USERARCHS__H__
#define __USERARCHS__H__

#include <CGRA/CGRA.h>
#include <CGRA/ConfigStore.h>
#include <CGRA/Module.h>
#include <CGRA/ModuleComposites.h>

#include <functional>
#include <map>
#include <string>
#include <utility>

using ArchGenFunctionSig = std::unique_ptr<CGRA>(const ConfigStore&);
struct ArchitectureGenerator {
    std::string description; //< arbitrary string used for display purposes
    std::function<ArchGenFunctionSig> gen;
    ConfigStore args_and_defaults; //< the complete list of arguments for this generator (and their default values)

    /**
     * Generate the architecture, while overriding the default arguments
     * with the given `overrides`. `overrides`'s key-set must be a subset of
     * `args_and_defaults`'s; If a key that exists in `overrides` does not exist in
     * `args_and_defaults`, then this function will throw.
     */
    std::unique_ptr<CGRA> operator()(const ConfigStore& overrides = {}) const {
        ConfigStore conf { args_and_defaults };
        override_all(conf, overrides);
        return gen(std::move(conf));
    }
};

class UserArchs
{
    public:

        UserArchs()
            : all_identifiers()
            , generator_storage()
        {
            /**
             * Follow the syntax below, add your identifier, description, fuction pointer/object, and arguments (with defaults)
             *
             * registerGenerator("identifer", ArchitectureGenerator{"This is the description", myFunctionPointerOrObject, {
             * {
             * {"cols", "4"},
             * {"rows", "4"},              // <--- Default arguments
             * {"homogeneous_fu", "1"},
             * }
             * }});
             */

            registerGenerator("adres", ArchitectureGenerator{"Adres CGRA Architecture", createAdresArch, {
                            {
                            {"cols", "4"}, // Columns
                            {"rows", "4"}, // Rows
                            {"II", "1"}, // II of the arch
                            {"toroid", "1"}, // Toroid Connection
                            {"fu_type", "0"}, // Heterogeneous Function Unit Type (0 = homogeneous, 1 = alternate-by-row, 2 = alternate-by-column
                            {"fu_latency", "0"},
                            {"fu_II", "1"},
                            {"rf_cols", "1"}, // Register File Columns
                            {"rf_rows", "1"}, // Register File Rows
                            {"num_const_addresses", "0"}, // number of const addresses each memory port can provide
                            {"op_div", "0"}, // VLIW PEs will come with op divide
                            {"pe_conn", "15"}, // Inter-PE Connectivity Enabling 8-bit Representation:
                            {"pred", "0"}, // Predication (0 = off, 1 = on)
			                {"reg_bypass", "0"}, // Register the bypass through the PE register file
			                {"pred_scheme", "partial"}, // Pred scheme to be used partial or event
			                {"extra_mem", "0"}, // Switch the right IOs with memory ports
                            {"frac", "0"}, // Instantiate PE Type: 0 = AdresPE, 1 = FracAdresPE 
                                /*
                                 *  bit-0: N Connection
                                 *  bit-1: E Connection
                                 *  bit-2: S Connection
                                 *  bit-3: W Connection
                                 *  bit-4: NW Connection
                                 *  bit-5: NE Connection
                                 *  bit-6: SE Connection
                                 *  bit-7: SW Connection
                                 *  e.g. 0d15 = 0x0f = orthogonal, 0d255 = 0xff = full-diagonal
                                 *  NOTE: value of 0~14 are legal, but may not be useful...
                                 */
                            }
            }});

            registerGenerator("hycube", ArchitectureGenerator{"HyCUBE CGRA Architecture", createHyCUBEArch, {
                            {
                            {"cols", "4"}, // Columns
                            {"rows", "4"}, // Rows
                            {"II", "1"}, // II of the arch
                            {"pred", "0"}, // Predication (0 = off, 1 = on)
                            {"fu_II", "1"}, // Allowed number of FuncUnit Initiation Intervals
                            {"fu_latency", "0"}, // Latency of FuncUnit
                            {"num_const_addresses", "0"}, // number of const addresses each memory port can provide
                            {"pe_conn", "15"}, // Specifying inter-Processing Element connections
                            {"pred", "0"}, // Predication (0 = off, 1 = on)
			                {"phi", "0"}, // Phi mux in crossbar (0 = off, 1 = on)
			                {"pred_scheme", "partial"}, // Pred scheme to be used partial or event
			                {"extra_mem", "0"} // Switch the right IOs with memory ports
                            /*
                            *  bit-0: N Connection
                            *  bit-1: E Connection
                            *  bit-2: S Connection
                            *  bit-3: W Connection
                            *  bit-4: NW Connection
                            *  bit-5: NE Connection
                            *  bit-6: SE Connection
                            *  bit-7: SW Connection
                            *  e.g. 0d15 = 0x0f = orthogonal, 0d255 = 0xff = full-diagonal
                            *  NOTE: value of 0~14 are legal, but may not be useful...
                            */
                            }
            }});

            registerGenerator("elastic_RIKEN", ArchitectureGenerator{"Elastic RIKEN Architecture", createElasticRIKENArch, {
                    {
		      {"cols", "4"}, // Columns
		      {"rows", "4"}, // Rows
              {"II", "1"}, // II of the arch
		      {"eb_depth", "2"}, // Elastic Buffer Depth
			  {"eb_enable", "true"}, // Elastic Buffer Depth
			  {"type", "0"}, // PE TYPE (deprecated IGNORE) // janders
			  {"fu_latency", "0"}, // Latency of FuncUnit (IGNORE) // janders
			  {"pe_conn", "255"}, // Specifying inter-Processing Element connections (0xFF, 8 conns)
			  {"fu_II", "1"}, // MUST BE LOCKED TO 1 for RIKEN janders
			  }
            }});

            // backwards compat. Do not recommend adding to this list.
            registerGenerator("1", getGenerator("adres"));
            registerGenerator("2", getGenerator("hycube"));
	        registerGenerator("3", getGenerator("elastic_RIKEN"));
            // for (int i = 0; i <= 2; ++i) {
            //     generator_storage.at(std::to_string(i)).description += " (deprecated ID)";
            // }
        }

        void registerGenerator(std::string identifer, ArchitectureGenerator generator) {
            all_identifiers.push_back(identifer);
            generator_storage.emplace(std::move(identifer), std::move(generator));
        }

        const ArchitectureGenerator& getGenerator(const std::string& identifer) const {
            const auto search_result = generator_storage.find(identifer);
            if (search_result == generator_storage.end()) {
                make_and_throw<cgrame_error>([&](auto&& s) {
                    s << "id `" << identifer << "' has no associated architecture generator";
                });
            } else {
                return search_result->second;
            }
        }

        auto begin() const { return generator_storage.begin(); }
        auto end()   const { return generator_storage.end(); }

    private:
        std::vector<std::string> all_identifiers; //< the key set for `generator_storage`
        std::map<std::string, ArchitectureGenerator> generator_storage;

        // Add your function that create the CGRA in-memory architecture, following the prototype below
        static std::unique_ptr<CGRA> createAdresArch(const ConfigStore& args);
        static std::unique_ptr<CGRA> createHyCUBEArch(const ConfigStore& args);
        static std::unique_ptr<CGRA> createElasticRIKENArch(const ConfigStore& args);
};

struct XY : std::pair<int,int> {
    using std::pair<int,int>::pair;
    auto x() { return first; }
    auto y() { return second; }
};

inline std::string xyName(const std::string& prefix, XY xy) {
    return string_from_stream([&](auto&& s) {
        s << prefix << "_c" << xy.x() << "_r" << xy.y();
    });
}

inline std::string xyidName(const std::string& prefix, XY xy, int id) {
    return xyName(prefix, xy) + string_from_stream([&](auto&& s) {
        s << "_i" << id;
    });
}

#endif

