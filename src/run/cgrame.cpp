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
#include <CGRA/dotparse.h>
#include <CGRA/Exception.h>
#include <CGRA/Mapper.h>
#include <CGRA/Mapping.h>
#include <CGRA/MappingProcedures.h>
#include <CGRA/MRRGProcedures.h>
#include <CGRA/OpGraphProcedures.h>
#include <CGRA/user-inc/UserArchs.h>
#include <CGRA/Util.h>
#include <CGRA/utility/CodeProfiling.h>
#include <CGRA/cgrame.h>

// thirdparty
#include <cxxopts.hpp>
#include <mini.hpp>

// standard library
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <streambuf>
#include <string>

// Linux/Unix
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace {
    const std::string default_config_file_name = "mapper_config.ini";
}

int inner_main(int argc, char* argv[]);


int main(int argc, char* argv[])
{
    try {
        return inner_main(argc, argv);
    } catch (...) {
        printExceptionToOutAndErr(std::current_exception(), "!==>", "TERMINATING! An exception reached the top of the stack");
        //throw; // re-throw the top exception
    }
}

namespace cxxopts {
namespace values {
    template <>
    void parse_value(const std::string& text, VerilogType& value) {
        try {
            size_t end_pos = 0;
            value = static_cast<VerilogType>(std::stoi(text, &end_pos));
            if (end_pos != text.size()) {
                throw 0;
            }
        } catch (...) {
            throw std::logic_error("can't convert " + text + " to VerilogType");
        }
    }
}
}

int inner_main(int argc, char* argv[]) {
    int arg_count = argc;
    PrintOnDestructionChronoSequence timing_seqence("CGRA-ME", &std::cout);

    std::cout << std::endl;
    std::cout << "CGRA - Modelling and Exploration (http://cgra-me.ece.utoronto.ca/)" << std::endl;
    std::cout << "Copyright (c) 2015-2023 University of Toronto. All Rights Reserved." << std::endl;
    std::cout << "For research and academic purposes only. Commercial use is prohibited." << std::endl;
    std::cout << "Please email questions to: Jason Anderson (janders@ece.utoronto.ca)" << std::endl;
    std::cout << "Compiled: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << std::endl;

    //
    // Argument Variables
    // (alphabetically by type then name)
    //

    struct {
        bool exit_after_optimizing_dfg = false;
        bool list_cpp_architectures = false;
        bool make_testbench = false;
        bool no_dfg_transformations = false;
        bool no_reduce_mrrg = false;
        bool print_mapper_help_and_exit = false;
        bool print_mapper_list = false;
        bool printarch = false;

        double timelimit = 7200.0;

        int II = 1;
        int verbosity = 3;
        std::string arch_opts = {};
        std::string arch_opts_list_id = {};
        std::string cpp_arch_id = {};
        std::string dfg_filename = {};
        std::string fix_port_filename = {};
        std::string gen_config_file = {};
        std::string load_mapping_filename = {};
        std::string mapper_id = "BothSmallTimeThenHeurFullTime";
        std::string mapper_opts = {};
        std::string mapping_output_filename = {};
        std::string print_final_dfg_filename = {};
        std::string print_mrrg_filename = {};
        std::string verilog_gen_output_path = {};
        std::string verilog_opts = {};

        VerilogType verilog_gen_type = VerilogType::CoreIR;
    } option_vals;

    //list of string arguments requiring validation
    std::string *argument_list[14] = {
        &option_vals.arch_opts,
        &option_vals.arch_opts_list_id,
        &option_vals.cpp_arch_id,
        &option_vals.dfg_filename,
        &option_vals.fix_port_filename,
        &option_vals.gen_config_file,
        &option_vals.load_mapping_filename,
        &option_vals.mapper_id,
        &option_vals.mapper_opts,
        &option_vals.mapping_output_filename,
        &option_vals.print_final_dfg_filename,
        &option_vals.print_mrrg_filename,
        &option_vals.verilog_gen_output_path,
        &option_vals.verilog_opts
    };
    //
    // Argument definitions
    // (sorted by long option name)
    //

    cxxopts::Options options("CGRA-ME", "CGRA - Modelling and Exploration");

    try {
        options.add_options()
            ("arch-list", "Show the List of Avaliable C++ Architectures with IDs",
                cxxopts::value(option_vals.list_cpp_architectures))
            ("arch-opts", "Override a C++ architecture's default parameters. Space-separated list of <Key>=<Value> pairs (must be a single arugment)",
                cxxopts::value(option_vals.arch_opts), "<\"opts\">")
            ("arch-opts-list", "Show the List of Avaliable Options for a C++ Architecture ID",
                cxxopts::value(option_vals.arch_opts_list_id), "id")
            ("c,cpp", "C++ architecture Identifier. See --arch-list for list.",
                cxxopts::value(option_vals.cpp_arch_id), "id")
            ("g,dfg", "The DFG file to map in dot format",
                cxxopts::value(option_vals.dfg_filename))
            ("exit-after-optimizing-dfg", "Only proceed as far as optimizing the DFG, then exit with success",
                cxxopts::value(option_vals.exit_after_optimizing_dfg))
            ("fixed-ports", "User can input fixed ports to mapper",
                cxxopts::value(option_vals.fix_port_filename))
            ("gen-testbench", "Generate testbench for use in a simulation of the DFG with configuration bitstream",
                cxxopts::value(option_vals.make_testbench))
            ("gen-config-file", "Create a config file at the given path, or the default name in the current directory if none is given. And do nothing else.",
                cxxopts::value(option_vals.gen_config_file)->implicit_value(default_config_file_name))
            ("gen-verilog", "Generate Verilog Implementation of Architecture and Dump to Specified Directory",
                cxxopts::value(option_vals.verilog_gen_output_path), "<Directorypath>")
            ("h,help", "Print Help")
            ("i,II", "Architecture Contexts Used",
                cxxopts::value(option_vals.II)->default_value(std::to_string(option_vals.II)), "<#>")
            ("l,load-mapping", "Load the mapping from a dot file",
                cxxopts::value(option_vals.load_mapping_filename))
            ("m,mapper", "Which mapper to use. See --mapper-list for choices.",
                cxxopts::value(option_vals.mapper_id)->default_value(option_vals.mapper_id), "<#>")
            ("mapper-list", "List all available mappers",
                cxxopts::value(option_vals.print_mapper_list), "<Name>")
            ("mapper-opts", "Override default mapper settings",
                cxxopts::value(option_vals.mapper_opts), "<Key>=<Value> ...")
            ("mapper-help", "Show the list of options and defaults for the selected mapper, then exit",
                cxxopts::value(option_vals.print_mapper_help_and_exit))
            ("no-dfg-transformations", "Disable all DFG transformations and optimizations; pass the input DFG directly the mapper",
                cxxopts::value(option_vals.no_dfg_transformations))
            ("no-reduce-mrrg", "Don't reduce the mrrg prior to mapping",
                cxxopts::value(option_vals.no_reduce_mrrg))
            ("print-arch", "Print Architecture to stdout",
                cxxopts::value(option_vals.printarch))
            ("print-final-dfg", "Print the DFG that is sent to mapper to this file",
                cxxopts::value(option_vals.print_final_dfg_filename))
            ("print-mrrg", "Print Architecture MRRG to file",
                cxxopts::value(option_vals.print_mrrg_filename),"<Filename>")
            ("o,save-to-dot", "Save the mapping to a dot file",
                cxxopts::value(option_vals.mapping_output_filename))
            ("t,timelimit", "Mapper Timelimit",
                cxxopts::value(option_vals.timelimit)->default_value(std::to_string(option_vals.timelimit)), "<#>")
            ("verbose", "Output will contain extra information detailing steps taken by this tool",
                cxxopts::value(option_vals.verbosity)->default_value(std::to_string(option_vals.verbosity))->implicit_value(std::to_string(option_vals.verbosity)), "level")
            ("verilog-opts", "Verilog options with hybrid system and memory size. Space-separated list of <Key>=<Value> pairs (must be a single arugment)",
                cxxopts::value(option_vals.verilog_opts), "<\"opts\">")
            ;

        options.parse(argc, argv);
    } catch (...) {
        std::throw_with_nested(cgrame_error("Problem parsing command-line options"));
    }
    //verify arguments do not start with a dash
    int arguments_size = sizeof(argument_list)/sizeof(argument_list[0]);
    for(int i = 0; i< arguments_size; i++)
    {   
        if(argument_list[i]->rfind("-",0)==0){
            throw cgrame_error("An argument is missing. "+ *argument_list[i] +" cannot be accepted as an argument");
        }
    }
    timing_seqence.tick("parse arguments", 0.01);

    char full_path[1024] = {0}; // initialize to zero, so we don't have to set the null char ourselves
    ssize_t count = readlink("/proc/self/exe", full_path, sizeof(full_path)/sizeof(full_path[0]) - 1);
    std::string exe_path;
    if(count != -1) {
        exe_path = std::string(dirname(full_path));
    } else {
        throw cgrame_error("Readlink is not able to get the executable path");
    }

    UserArchs userarchs;
    const auto mapper_registry = AutoRegisterMapper::getDefaultRegistry(); // make a copy to prevent any funny business

    //
    // Handle mode flags that just print & exit
    //

    if(options.count("help") || arg_count == 1) {
        std::cout << options.help({""}) << std::endl;
        return 0;
    }

    if (not option_vals.gen_config_file.empty()) {
        std::cout << "Writing a config file to " << option_vals.gen_config_file << '\n';
        std::ofstream conf_file(option_vals.gen_config_file);
        if (not conf_file) {
            throw cgrame_error("Unable to open " + option_vals.gen_config_file + "for writing");
        }
        mapper_registry.printDefaultsAsINI(conf_file);
        if (not conf_file) {
            throw cgrame_error("Problem while writing " + option_vals.gen_config_file);
        }
        return 0;
    }

    if(option_vals.list_cpp_architectures) {
        std::cout << "List of C++ CGRA Architectures: [ID: description]" << std::endl;
        for(const auto & id_and_arch_gen : userarchs)
            std::cout << id_and_arch_gen.first << ": " << id_and_arch_gen.second.description << '\n';
        return 0;
    }

    if(not option_vals.arch_opts_list_id.empty()) {
        const auto& arch_generator = userarchs.getGenerator(option_vals.arch_opts_list_id);
        std::cout << "List of options and defaults for architecture `" << option_vals.arch_opts_list_id << "' - `" << arch_generator.description << "'\n" << arch_generator.args_and_defaults << '\n';
        return 0;
    }

    if (option_vals.print_mapper_list) {
        std::cout << "The list of registered mappers follows. The currently selected mapper is " << option_vals.mapper_id << "\n";
        mapper_registry.printMapperList(std::cout);
        return 0;
    }

    //
    // Handle mode flags that just print & exit, but also need the inis
    //
    const std::string ini_full_path = exe_path + "/" + default_config_file_name;
    std::ifstream ini_file(ini_full_path);
    if(!ini_file) {
        throw cgrame_error("Missing mapper_config.ini, Cannot Read Default Parameters for Mappers");
    }

    const ConfigStore effective_inis = [&]() { // (immediately invoked)
        try {
            return parseMapperConfig(
                {std::istreambuf_iterator<char>(ini_file), std::istreambuf_iterator<char>()},
                option_vals.mapper_opts,
                option_vals.cpp_arch_id,
                option_vals.verbosity
            );
        } catch (...) {
            std::throw_with_nested("Problem parsing config. INI file used was at " + ini_full_path);
        }
    }();

    if(option_vals.print_mapper_help_and_exit) {
        if (option_vals.mapper_id.empty()) {
            std::cout << "A mapper must be selected to print its options\n";
            return 1;
        }
        std::cout << "Help for mapper `" << option_vals.mapper_id << "':\n\n";
        mapper_registry.printHelpForMapper(option_vals.mapper_id, effective_inis, std::cout);
        std::cout << '\n';
        return 0;
    }

    //
    // Basic argument checking
    //
    if(option_vals.verilog_gen_output_path.empty() && option_vals.dfg_filename.empty()) {
        if (!(option_vals.printarch || !option_vals.print_mrrg_filename.empty())){
            throw cgrame_error("DFG File Path is Missing, Exiting...");
        }
    }

    if(!option_vals.fix_port_filename.empty() && !option_vals.make_testbench) {
        throw cgrame_error("fix_port option need to be used for generating bitstream");
    }


    if (option_vals.timelimit < 0) {
        throw make_from_stream<cgrame_error>([&](auto&& s) {
            s << "Time limit must not be less than zero (" << option_vals.timelimit << " was given)";
        });
    }

    timing_seqence.tick("argument checking", 0.01);

    //
    // Main CGRA-ME flows
    //

    std::shared_ptr<CGRA> arch = nullptr;

    ConfigGraph parsed_mapping_dot;
    //load mapping from file if provided
    if (not option_vals.load_mapping_filename.empty()) {
            parsed_mapping_dot = loadMapping(
            option_vals.load_mapping_filename,
            &option_vals.cpp_arch_id,
            &option_vals.arch_opts,
            &option_vals.II);
        timing_seqence.tick("load mapping", 0.01);
    }

    // Creating OpGraph
    std::shared_ptr<OpGraph> opgraph;
    if(!option_vals.dfg_filename.empty()){
        cgrame_msg("Parsing DFG...");
        auto dfg_ifstream = std::ifstream(option_vals.dfg_filename);
        const auto parsed_dfg_dot = parseDot(dfg_ifstream, option_vals.dfg_filename);
        timing_seqence.tick("load DFG", 0.01);
        opgraph = std::make_shared<OpGraph>(createOpGraphFromConfig(parsed_dfg_dot));
        if (not option_vals.no_dfg_transformations) {
            *opgraph = removeBranchComputation(std::move(*opgraph));
            if (option_vals.arch_opts.find("pred=1") == std::string::npos)
                *opgraph = removePhiNodes(std::move(*opgraph));
            *opgraph = removeCastNodes(std::move(*opgraph));
            *opgraph = propagateConstants(std::move(*opgraph));
            *opgraph = distributeConstants(std::move(*opgraph));
        }
        timing_seqence.tick("optimize DFG", 0.01);

        // Print OpGraph if requested, exit if requested
        if(not option_vals.print_final_dfg_filename.empty()) {
            cgrame_msg("Printing DFG to " + option_vals.print_final_dfg_filename);
            auto final_dfg_ofstream = std::ofstream{option_vals.print_final_dfg_filename};
            opgraph->serialize(final_dfg_ofstream);
            if (not final_dfg_ofstream) {
                cgrame_warn("DFG printing may not have been successful");
            }
            timing_seqence.tick("print DFG", 0.01);
        }

        if (option_vals.exit_after_optimizing_dfg) {
            return 0;
        }
    }

    ConfigStore arch_attrs;
    //create architecture
    if(!option_vals.cpp_arch_id.empty()) // Use C++
    {
        createArchitectureCPP(&arch,option_vals.cpp_arch_id,option_vals.arch_opts,&arch_attrs,&userarchs, option_vals.II);
    }
    else // Use ADL
    {
        throw cgrame_error("ADL architecture not supported");
    }

    timing_seqence.tick("create architecture", 0.01);

    // Print Architecture if requested
    if(option_vals.printarch)
    {
        std::cout << std::endl << "[ARCHGRAPH]" << std::endl;
        arch->getTopLevelModule().print();
        arch->getTopLevelModule().print_dot();
    }

    // print MRRG if requested
    if (not option_vals.print_mrrg_filename.empty()) {
        std::cout << "Dumping MRRG with option_vals.II = " << option_vals.II << " to file `" << option_vals.print_mrrg_filename << "'\n";
        std::ofstream file_stream{option_vals.print_mrrg_filename};
        arch->getMRRG(option_vals.II).printDot(file_stream);
    }

    timing_seqence.tick("print arch and/or MRRG", 0.01);

    //Exit here if not generating verilog and no application provided
    if(option_vals.verilog_gen_output_path.empty() && option_vals.dfg_filename.empty()){
        return 0;
    }

    // Generate Verilog implementation and exit
    if(not option_vals.verilog_gen_output_path.empty()) {
        generateVerilog(arch, option_vals.verilog_gen_output_path, option_vals.verilog_opts, option_vals.verilog_gen_type, option_vals.II);
        return 0;
    }


    timing_seqence.tick("generate Verilog", 0.01);

    std::cout << "[INFO] Creating MRRG..." << std::endl;
    const auto& mrrg = arch->getMRRG(option_vals.II);
    timing_seqence.tick("create MRRG", 0.01);

    //Parse fixed ports
    std::unordered_map<std::string, std::string> fix_port;
    if (!option_vals.fix_port_filename.empty()) {
        std::ifstream infile(option_vals.fix_port_filename);
        std::string line;
        std::string first;
        std::string second;
        while (std::getline(infile, line)) {
            std::string::size_type pos = line.find(':');
            if(line.npos != pos) {
                second = line.substr(pos + 1);
                first = line.substr(0, pos);
                fix_port[first] = second;
            } else {
                throw cgrame_error("Format for fix_port is not right, \":\" is used as delimiter");
            }
        }
    }

    // Declaring mapping result
    Mapping mapping_result = Mapping(arch, option_vals.II, opgraph);
    CreateMappingGraphResult create_mapping_graph_result{};

    if (not option_vals.load_mapping_filename.empty()) {
        std::cout << "[INFO] Mapping using information from DOT file: " << option_vals.load_mapping_filename << std::endl;
        // Map the empty mapping `mapping_result` and create a Mapping Graph for it
        create_mapping_graph_result = createMappingGraphFromConfig(mapping_result, parsed_mapping_dot, *opgraph, mrrg);

        mapping_result.outputMapping();
        mapping_result.check();
        timing_seqence.tick("created loaded mapping", 0.01);
    }
    else {
        //************************ Start of Mapping ************************
        std::cout << "[INFO] Creating Mapper '" << option_vals.mapper_id << "'..." << std::endl;

        auto mapper = mapper_registry.createMapper(option_vals.mapper_id, arch, option_vals.timelimit, effective_inis);

        timing_seqence.tick("mapper setup", 0.01);

        // common mapping routine
        const auto map_given_mrrg = [&timing_seqence, &mapper, &option_vals, &opgraph, &fix_port](const MRRG& given_mrrg) {
            std::cout << "[INFO] Mapping..." << std::endl;
            auto mapping = mapper->mapOpGraph(opgraph, option_vals.II, given_mrrg, fix_port);
            auto time = timing_seqence.tick("mapping");
            return std::make_pair(mapping, time);
        };

        ChronoSequence::TickResult mapping_tick;
        std::tie(mapping_result, mapping_tick) = [&]() { // an immediately invoked lambda
            // Reduce MRRG if requested
            if (not option_vals.no_reduce_mrrg) {
                std::cout << "[INFO] Reducing MRRG..." << std::endl;
                const auto mrrg_reduction_result = reduceLosslessly(arch->getMRRG(option_vals.II), {});
                timing_seqence.tick("reduce MRRG", 0.01);

                auto mapping_and_tick = map_given_mrrg(mrrg_reduction_result.transformed_mrrg);
                auto untransformed_mapping = transformToOriginalMRRG(std::move(mapping_and_tick.first), mrrg_reduction_result);
                timing_seqence.tick("transform mapping to original", 0.01);
                return std::make_pair(std::move(untransformed_mapping), mapping_and_tick.second);
            } else {
                return map_given_mrrg(mrrg);
            }
        }();

        double solve_time = mapping_result.solve_time_in_seconds;
        if (solve_time < 0) solve_time = mapping_result.solve_time_in_seconds; // fall back to the timing seq number
        std::cout << "[INFO] Mapping Success: " << mapping_result.isMapped() << std::endl;
        std::cout << "[INFO] Mapping Time: " << solve_time << std::endl;
        std::cout << "[INFO] Mapping Timeout: " << mapping_result.isTimeout() << std::endl;

        if (not mapping_result.isMapped()) { return 1; }

        mapping_result.outputMapping();
        mapping_result.check();

        create_mapping_graph_result = createMappingGraph(mrrg, mapping_result);
    }

    const auto final_mapping_graph = removeIsolatedRoutingNodes(create_mapping_graph_result.mg, mrrg, create_mapping_graph_result.toMRRG);

    for (auto message : final_mapping_graph.verify(mrrg, create_mapping_graph_result.toMRRG)) {
        std::cout << message.message;
    }

    //const auto latency_check_result = latencyCheck(final_mapping_graph, mrrg, create_mapping_graph_result.toMRRG);
    //if (not latency_check_result.first) {
    //    throw cgrame_error("Latencies of mapping are not balanced!");
    //}
    timing_seqence.tick("post mapping checks and transforms", 0.01);

    // Save the mapping result in an dot file if requested
    if (not option_vals.mapping_output_filename.empty()) {
        std::cout << "[INFO] Saving mapping result to '" << option_vals.mapping_output_filename << '\'' << std::endl;
        std::ofstream mappingStream(option_vals.mapping_output_filename);
        final_mapping_graph.printDot(mappingStream, mrrg, *opgraph, create_mapping_graph_result.toMRRG, arch_attrs);
        if (not mappingStream) { std::cerr << "Problem writing mapping to '" << option_vals.mapping_output_filename << '\'' << std::endl; }
        timing_seqence.tick("saving mapping", 0.01);
    }


    if (option_vals.make_testbench)
    {
        createTestbench(arch, mapping_result, option_vals.II);
        timing_seqence.tick("generate testbench", 0.01);
    }

    return 0;
}

ConfigStore parseMapperConfig(
    const std::string& ini_str,
    const std::string& mapper_opts,
    const std::string& cpp_arch_id,
    const int verbosity
) {
    // parse command-line overrides
    ConfigStore command_line_overrides;
    std::stringstream ss_mapper_opts(mapper_opts);
    for (;;) {
        std::string opt_pair;
        if (not (ss_mapper_opts >> opt_pair)) { break; }
        auto assign_pos = opt_pair.find('=');

        if(assign_pos == opt_pair.npos) {
            throw cgrame_error("Ill-formatted command-line override: " + opt_pair);
        }

        command_line_overrides.addString(opt_pair.substr(0, assign_pos), opt_pair.substr(assign_pos + 1));
    }

    // read ini file
    mINI raw_inis;
    if(not raw_inis.parse(ini_str)) {
        throw cgrame_error("Error in INI Parser");
    }

    // remove any leading dots & comments
    ConfigStore parsed_inis;
    for (const auto& ini_and_value : raw_inis) {
        const auto& key = ini_and_value.first;
        if (key.empty()) { continue; }
        if (key.find_first_not_of("\t ") == key.npos) { continue; } // all whitespace
        if (key.at(key.find_first_not_of("\t ")) == '#') { continue; } // comments
        const auto dot_pos = key.find_first_of(".");
        if (dot_pos != key.npos && key.at(key.find_first_not_of("\t ", dot_pos+1)) == '#') { continue; } // comments in sections
        if (key.front() == '.') {
            if (key.size() > 1) {
                parsed_inis.setString(key.substr(1), ini_and_value.second);
            }
        } else {
            parsed_inis.setString(key, ini_and_value.second);
        }
    }

    // add options from other flags
    parsed_inis.setString("AllMappers.arch_id", cpp_arch_id);
    parsed_inis.setInt("AllMappers.verbosity", verbosity);

    // set command-line overrides
    const ConfigStore effective_inis = with_set(parsed_inis, command_line_overrides);
    return effective_inis;
}

void generateVerilog(std::shared_ptr<CGRA> arch,
    std::string verilog_gen_output_path,
    std::string verilog_opts,
    VerilogType verilog_gen_type,
    int contexts
){
    struct stat fs_info;
    auto res = stat(verilog_gen_output_path.c_str(), &fs_info);
    // Verify if directory exists
    if (res != 0)
    {
        throw cgrame_error("Cannot open \"" + verilog_gen_output_path + "\"");
    }
    bool is_dir = fs_info.st_mode & S_IFDIR;
    if (!is_dir) // make sure it is directory
    {
        throw cgrame_error("\"" + verilog_gen_output_path + "\" is not a directory");
    }

    std::cout << "[INFO] Generating Verilog Implementation of Specified Architecture Using Generator "<< verilog_gen_type << std::endl;
    if (verilog_gen_output_path.back() != '/')
        verilog_gen_output_path = verilog_gen_output_path + "/";
    arch->genVerilog(verilog_gen_type, verilog_gen_output_path, contexts);
    ConfigStore verilog_attrs;
    std::stringstream ss_verilog_opts(verilog_opts);
    std::string opt_pair;
    while(ss_verilog_opts >> opt_pair)
    {
        auto n = opt_pair.find('=');
        if(n == std::string::npos) { make_and_throw<cgrame_error>([&](auto&& s) {
            s << "Ill-formatted verilog option: " << opt_pair;
        });}
        auto key = opt_pair.substr(0, n);
        auto value = opt_pair.substr(n + 1, std::string::npos);
        if (verilog_attrs.hasKey(key)) {
            std::cout << "[WARNING] duplicate key `" << key << "' verilog arguments. Overwriting with new value `" << value << "'\n";
        }
        verilog_attrs.setString(std::move(key), std::move(value));
    }
    if (verilog_attrs.hasKey("hybrid")) {
        if(verilog_attrs.getString("hybrid").find("on") != std::string::npos) {
            int mem_size = stoi(verilog_attrs.getStringOr("memSize", "22"));
            arch->genHybrid(verilog_gen_type, verilog_gen_output_path, mem_size);
        }
    }
}


ConfigGraph loadMapping(std::string load_mapping_filename,
    std::string* cpp_arch_id,
    std::string* arch_opts,
    int *II
){
    try{
        ConfigGraph parsed_mapping_dot;              
        std::cout << "[INFO] Getting architecture information from DOT file: " << load_mapping_filename << std::endl;

        auto mapping_ifstream = std::ifstream(load_mapping_filename);
        parsed_mapping_dot = parseDot(mapping_ifstream, load_mapping_filename);
        const auto& mapping_attrs = parsed_mapping_dot.graphAttributes();

        if (mapping_attrs.hasKey("cpp_arch_id")) {
            *cpp_arch_id = mapping_attrs.getString("cpp_arch_id");
            *arch_opts = mapping_attrs.getStringOr("arch_opts", "");
        } 

        *II = mapping_attrs.getInt("II");
        return parsed_mapping_dot;
    } catch (...) {
                std::throw_with_nested(cgrame_error("Problem parsing input mapping file `" + load_mapping_filename + "'"));
            }
}

void createArchitectureCPP(
    std::shared_ptr<CGRA>* arch,
    std::string cpp_arch_id,
    std::string arch_opts,
    ConfigStore* arch_attrs,
    UserArchs* userarchs,
    int II
){
    std::cout << "[INFO] Using C++ architecture ID " << cpp_arch_id << '\n';
    auto& arch_generator = userarchs->getGenerator(cpp_arch_id);
    std::cout << "[INFO] Architecture description: " << arch_generator.description << '\n';

    std::stringstream ss_arch_opts(arch_opts);
    std::string opt_pair;
    while(ss_arch_opts >> opt_pair)
    {
        auto n = opt_pair.find('=');
        if(n == std::string::npos) { make_and_throw<cgrame_error>([&](auto&& s) {
            s << "Ill-formatted C++ architecture option: " << opt_pair;
        });}
        auto key = opt_pair.substr(0, n);
        auto value = opt_pair.substr(n + 1, std::string::npos);
        if (arch_attrs->hasKey(key)) {
            std::cout << "[WARNING] duplicate key `" << key << "' in C++ architecture arguments. Overwriting with new value `" << value << "'\n";
        }
        (*arch_attrs).setString(std::move(key), std::move(value));
    }
    
    //Verify that architecture has correct arguments    
    if(!(arch_attrs->hasKey("rows") && arch_attrs->hasKey("cols"))) {
        throw cgrame_error("Rows and columns must be specified with --arch-opts.\n");
    }

    std::cout << "[INFO] Creating \"" << cpp_arch_id << "\" Architecture from C++..." << std::endl;
    arch_attrs->setInt("II", II);
    (*arch) = arch_generator(*arch_attrs);
    arch_attrs->setString("cpp_arch_id", cpp_arch_id);
    arch_attrs->setString("arch_opts", arch_opts);
    if (arch_attrs->hasKey("rows")) {
        (*arch)->setNumRows(arch_attrs->getInt("rows"));
    }
    if (arch_attrs->hasKey("cols")) {
        (*arch)->setNumCols(arch_attrs->getInt("cols"));
    }
}

void createTestbench(
    std::shared_ptr<CGRA> arch,
    Mapping mapping_result,
    int II
){
    std::ofstream tb_file("testbench.v");
    auto bitstream = arch->genBitStream(mapping_result);
    std::cout << bitstream;
    bitstream.printTestbench(tb_file, II);
}
