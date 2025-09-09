/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

// internal
#include <CGRA/Mapper.h>
#include <CGRA/Mapping.h>
#include <CGRA/MappingProcedures.h>
#include <CGRA/MRRGProcedures.h>
#include <CGRA/user-inc/UserArchs.h>

// standard library
#include <string>

void generateVerilog(
    std::shared_ptr<CGRA> arch,
    std::string verilog_gen_output_path,
    std::string verilog_opts,
    VerilogType verilog_gen_type, 
    int contexts);

ConfigGraph loadMapping(
    std::string load_mapping_filename,
    std::string* cpp_arch_id,
    std::string* arch_opts,
    int *II
);

void createArchitectureCPP(
    std::shared_ptr<CGRA>* arch,
    std::string cpp_arch_id,
    std::string arch_opts,
    ConfigStore* arch_attrs,
    UserArchs* userarchs,
    int II
);


void createTestbench(
    std::shared_ptr<CGRA> arch,
    Mapping mapping_result,
    int II
);


ConfigStore parseMapperConfig(
    const std::string& ini_str,
    const std::string& mapper_opts,
    const std::string& cpp_arch_id,
    const int verbosity
);