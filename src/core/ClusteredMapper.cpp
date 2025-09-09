/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/ClusteredMapper.h>
#include <CGRA/Exception.h>
#include <CGRA/GraphAlgorithms.h>
#include <CGRA/MRRGProcedures.h>
#include <CGRA/utility/CodeProfiling.h>
#include <CGRA/user-inc/UserModules.h>
#include <CGRA/dotparse.h>

#include <algorithm>
#include <utility>
#include <functional>
#include <cmath>
#include <fstream>

using std::cout;
using std::endl;

AutoRegisterMapper ClusteredMapper_arm("ClusteredMapper",
    [](std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args) {
        return std::make_unique<ClusteredMapperDriver>(cgra, timelimit, args);},
    false,  // not a composite
    "Clustered mapper.\n", {  // required args
        {"arch_id", "", ""},
        {"initial_hfactor", 0.2, "History factor"},
        {"initial_pfactor", 0.01, "Initial FU and routing overuse penalty"},
        {"hfactor_factor", 1.01, "history factor increment"},
        {"pfactor_factor", 1.07, "Multiplicative factor for overuse penalty, per iteration"},
        {"initial_temperature_method", "", "Temperature that the anneal starts at"},
        {"progress_dump_filename", "", "File to dump csv data status of each iteration. Empty disables."},
        {"seed",  10, ""},
        {"cost_function",  1, "Placement cost function could be 0 (BoundaryBox cost) or 1 (Scheduling cost)"},
        {"swap_factor",  10, "Swaps per op node, per iteration"},
        {"cluster", true, "Flag to set if the benchmark gets clustered or no"},
        {"fanout_threshold", 5, "Sets the threshold of acceptable fanout"},
        {"ram_ports", 0, "Set the number of ram ports"},
        {"verbosity", 0, ""},
    },
    {}  // optional arg regexes
);

ClusteredMapperDriver::ClusteredMapperDriver
    (std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args)
    : Mapper(cgra, timelimit)
    , l_arch_name(args.getString("arch_id"))
    , l_rand_seed(args.getInt("seed"))
    , l_initial_pfactor(args.getReal("initial_pfactor"))
    , l_initial_hfactor(args.getReal("initial_hfactor"))
    , l_p_factor_factor(args.getReal("pfactor_factor"))
    , l_h_factor_factor(args.getReal("hfactor_factor"))
    , l_swap_factor(args.getInt("swap_factor"))
    , l_verbosity(args.getInt("verbosity"))
    , l_mapper_args(args)
    , l_cost_func(args.getInt("cost_function"))
    , l_empty_mapping_result(cgra, 0, nullptr)
    , l_print(0)
    , l_cluster(args.getBool("cluster"))
    , l_fanout_threshold(args.getInt("fanout_threshold"))
    , l_ram_ports(args.getInt("ram_ports"))
{ }

Mapping ClusteredMapperDriver::mapOpGraph (std::shared_ptr<OpGraph> opgraph,
    int II, const MRRG& mrrg,
    std::unordered_map<std::string, std::string> fix_port) {
        std::cout<< "SEED: " << l_rand_seed << std::endl;
        std::cout <<"Num of ops: " << opgraph->opNodes().size();
    PrintOnDestructionChronoSequence timing_seq("PCKM:Packing Mapper", &std::cout );
    // Setting up the mapping
    l_empty_mapping_result = Mapping(l_empty_mapping_result.getCGRA_shared(),
            mrrg.initiationInterval(), opgraph);
    l_iteration_interval = mrrg.initiationInterval();
    const Module& top_mod = cgra->getTopLevelModule();
    l_isElastic = top_mod.isElastic;

    if(!isAllOpsSupported(*opgraph, mrrg)) {
        throw cgrame_mapper_error("Not all ops are supported by the architecture");
        return l_empty_mapping_result;
    }
    // Running either the elastic or the static version of the code
    Mapping mapping_result = l_empty_mapping_result;
    if (l_isElastic) {
        mapping_result = mapElastic(opgraph, II, mrrg, fix_port, timing_seq);
    } else {
        mapping_result = mapStatic(opgraph, II, mrrg, fix_port, timing_seq);
    }
    mapping_result.solve_time_in_seconds = timing_seq.secondsSinceStart();
    return mapping_result;
    
}


Mapping ClusteredMapperDriver::mapStatic (std::shared_ptr<OpGraph> opgraph,
    int II,
    const MRRG& mrrg,
    std::unordered_map<std::string, std::string> fix_port,
    PrintOnDestructionChronoSequence& timing_seq) {
    l_op_sched = new Scheduler(opgraph.get(), l_arch_name);
    // Run the clustring algorthim to create clusters for the DFG nodes
    TMPack am_clust(*this, &cgra->getTopLevelModule(), opgraph, mrrg, cgra->getNumRows(), cgra->getNumCols(), l_iteration_interval, l_isElastic);
    
    bool reschedule = true;


    while (reschedule) {
        l_schedule = l_op_sched->asap_schedule;
        timing_seq.tick("Schedule", 0.0);
        for (auto& op_schedule : l_schedule) {
            std::cout << op_schedule.first->getName() << "  " << op_schedule.second << std::endl;
        }
        am_clust.unsetReschedule();
        am_clust.setSchedule(l_schedule);
        reschedule = am_clust.clusterPEs(l_cluster);
        timing_seq.tick("TMPACK", 0.0);
        // l_op_sched->extended_sched_const = am_clust.extended_sched_const;
    }
    l_clusters = am_clust.getClusters();
    l_ops_to_node_names = am_clust.getOpsNodeNames();
    l_op_cluster_index = am_clust.getOpClusterIndex();

    std::cout << "TMPack Result" << std::endl;
    int clusters_greater_one = 0;
    for (int i = 0; i < l_clusters.size(); i++) {
        std::cout << "cluster " << i << std::endl;
        if (l_clusters[i].size() > 1) clusters_greater_one++;
        for (auto& op : l_clusters[i]) {
            std::cout << *op << " ";
            std::cout << l_op_cluster_index[op] << std::endl;
        }
    }

    AnnealPlacer am_placer(*this, *opgraph, mrrg, fix_port, cgra->getNumRows(), cgra->getNumCols(), l_isElastic);
    PathFinder am_router(*this, *opgraph, mrrg, l_isElastic);
    
    am_placer.clearPlacement();
    am_placer.setInitialPlacement();
    float temprature = am_placer.determineTemperature(25);
    Mapping mapping_result = l_empty_mapping_result;
    int i = 0;
    while (mapping_result.getStatus() != MappingStatus::success && i < 1000) {
        std::cout << "ITERATION: " << i << std::endl;
        if (timelimit != 0 && timing_seq.secondsSinceStart() >= timelimit) {
            mapping_result.setStatus(MappingStatus::timeout);
            break;
        }
        l_empty_mapping_result.clear();
        l_empty_mapping_result = am_placer.placeOpGraph(temprature);
        timing_seq.tick("Placer iteration" + std::to_string(i), 0.01);
        if (i < 10) {
         //   printPlacementNeato(i, *opgraph, l_empty_mapping_result, l_schedule);
        }
        mapping_result = am_router.routeOpGraph(std::move(l_empty_mapping_result));
        timing_seq.tick("Router iteration" + std::to_string(i), 0.01);
        temprature = am_placer.determineTemperature(10);
        i++;
    }
    timing_seq.tick("Place and route", 0.01);
    //am_router.printCriticalPath();
    std::cout << "[INFO] clusters greater than 1: " << clusters_greater_one << std::endl;
    return mapping_result;
}


Mapping ClusteredMapperDriver::mapElastic (std::shared_ptr<OpGraph> opgraph,
    int II,
    const MRRG& mrrg,
    std::unordered_map<std::string, std::string> fix_port,
    PrintOnDestructionChronoSequence timing_seq) {
    // Run the clustring algorthim to create clusters for the DFG nodes
    TMPack am_clust(*this, &cgra->getTopLevelModule(), opgraph, mrrg, cgra->getNumRows(), cgra->getNumCols(), II, l_isElastic);
    am_clust.clusterPEs(l_cluster);
    l_clusters = am_clust.getClusters();
    l_ops_to_node_names = am_clust.getOpsNodeNames();
    l_op_cluster_index = am_clust.getOpClusterIndex();
    std::cout << "Final Result" << std::endl;
    for (auto& cluster : l_clusters) {
        std::cout << "cluster" << std::endl;
        for (auto& op : cluster) {
            std::cout << *op << std::endl;
        }
    }

    AnnealPlacer am_placer(*this, *opgraph, mrrg, fix_port, cgra->getNumRows(), cgra->getNumCols(), l_isElastic);
    PathFinder am_router(*this, *opgraph, mrrg, l_isElastic);

    am_placer.clearPlacement();
    am_placer.setInitialPlacement();
    float temprature = am_placer.determineTemperature(20);
    Mapping mapping_result = l_empty_mapping_result;
    int i = 0;
    while (mapping_result.getStatus() != MappingStatus::success && i < 1000) {
        std::cout << "ITERATION: " << i << std::endl;
        if (timelimit != 0 && timing_seq.secondsSinceStart() >= timelimit) {
            mapping_result.setStatus(MappingStatus::timeout);
            break;
        }
        l_empty_mapping_result.clear();
        l_empty_mapping_result = am_placer.placeOpGraph(temprature);
        if (i < 10) {
	  //	  printPlacementNeato(i, *opgraph, base.empty_mapping_result, base.schedule);
        }
        mapping_result = am_router.routeOpGraph(std::move(l_empty_mapping_result));
        temprature = am_placer.determineTemperature(0.1);
        i++;
    }
    return mapping_result;
}

const Latency ClusteredMapperDriver::getCyclesToSink(OpGraphOpDesc source, OpGraphOpDesc sink) {
    if (l_isElastic) return kUndefLatency;
    Latency cycles_to_sink = l_schedule.at(sink) - l_schedule.at(source);
    if (cycles_to_sink < 0) {
        cycles_to_sink = l_iteration_interval - (std::abs(cycles_to_sink)%l_iteration_interval);
    }
    // If backedge and the cycles is 0
    if (sink->getOpCode() == OpCode::PHI &&
        source->getOpCode() != OpCode::INPUT &&
        source->getOpCode() != OpCode::CONST &&
        source->getOpCode() != OpCode::INPUT_PRED &&
        cycles_to_sink == 0) {

        cycles_to_sink = l_iteration_interval - 1 + l_op_sched->getLowerBound(source->getOpCode(), sink->getOpCode());
    }
    if (sink == source) cycles_to_sink = l_iteration_interval;
    if (l_isElastic) cycles_to_sink = kUndefLatency;
    return cycles_to_sink;
}

bool ClusteredMapperDriver::isAllOpsSupported(const OpGraph& opgraph, const MRRG& mrrg) {
    const auto classes = computeNodeClassLists(mrrg);
    for (auto op : opgraph.opNodes()) {
        bool op_supported = false;
        for (auto mrrg_node : classes.function_nodes) {
            if (mrrg_node->canMapOp(op)) {
                op_supported = true;
            }
        }
        if (!op_supported) {
            throw cgrame_error(op->getName() + " Could not be mapped");
            return false;
        }
    }
    return true;
}

void ClusteredMapperDriver::printPlacementNeato(int i, const OpGraph& opgraph, Mapping placed,  std::unordered_map<OpGraphOpDesc, int> sched) {
    std::vector<std::string> colors = {"antiquewhite4", "aquamarine3", "azure3", "blue", "red", "blueviolet", "brown4", "burlywood4",
                                        "cadetblue4", "chartreuse4", "chocolate4", "coral4", "cyan2", "darkgoldenrod", "crimson"};
    int color_index = 0;
    std::map<OpGraphOpDesc, std::string> ops_to_modules;
    std::ofstream os{"placement_" + std::to_string(i) + ".dot"};
    Module& topModule = this->cgra->getTopLevelModule();
    os << "digraph " << topModule.getName() << " {\n";
    bool set_color = false;
    for (auto& module : topModule.submodules) {
        if (module.first.find("pred") != std::string::npos) continue;
         os << module.first << "[ height=.5,width=.5,fontsize=8, ";
         os << " pos = \""  << module.second->loc.x_coord << "," << "-" << module.second->loc.y_coord << "!\"";
         os << " ,label = \"";
         for (auto& op : opgraph.opNodes()) {
            MRRG::NodeDescriptor mrrg_node = placed.getSingleMapping(op);
            Module* par = mrrg_node->parent;
            if (par->loc.x_coord == module.second->loc.x_coord
                && par->loc.y_coord == module.second->loc.y_coord ) {
                set_color = true;
                os << " \\n" << op->getName();
                ops_to_modules.emplace(op, module.first);
            }
        }
        if (set_color) {
            os << "\"color=" << colors[color_index%colors.size()] << "]" << "\n";
            color_index++;
        } else {
            os << module.first <<"\"]" << "\n";
        }
        set_color = false;
    }

    for (auto& val : opgraph.valNodes()) {
        auto src = val->input;
        MRRG::NodeDescriptor src_mrrg_node = placed.getSingleMapping(src);
        Module* src_par = src_mrrg_node->parent;
        int src_x = src_par->loc.x_coord;
        int src_y = src_par->loc.y_coord;
        for (auto& snk : val->output) {
            int cycles_to_sink = sched[snk] - sched[src];
            if (cycles_to_sink < 0) {
                cycles_to_sink = 0;  // (std::abs(cycles_to_sink)%II);
            }
            if (src == snk) cycles_to_sink = 1;

            MRRG::NodeDescriptor snk_mrrg_node = placed.getSingleMapping(snk);
            Module* snk_par = snk_mrrg_node->parent;
            int snk_x = snk_par->loc.x_coord;
            int snk_y = snk_par->loc.y_coord;
            if (src_x != snk_x || src_y != snk_y) {
                os << ops_to_modules[src] << "->" << ops_to_modules[snk] << "[";
                os << "label = \"" << cycles_to_sink;
                os << "\"color=" << colors[color_index%colors.size()] << ",fontsize=8]" << "\n";
                color_index++;
            }
        }
    }
    os << "}\n";
    return;
}
