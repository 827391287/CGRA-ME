#include <CGRA/HeuristicMappers.h>

#include <CGRA/ConstraintSet.h>
#include <CGRA/GraphAlgorithms.h>
#include <CGRA/Module.h>
#include <CGRA/MRRGProcedures.h>
#include <CGRA/OpGraphProcedures.h>
#include <CGRA/TupleHash.h>
#include <CGRA/utility/CodeProfiling.h>
#include <CGRA/utility/Functional.h>

#include <gurobi_c++.h>
#ifdef CGRAME_USE_GOOGLE_OR_TOOLS
#include <ortools/constraint_solver/constraint_solver.h>
#include <ortools/sat/cp_model_solver.h>
#include <ortools/util/time_limit.h>
#endif

#include <chrono>
#include <fstream>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef CGRAME_USE_GOOGLE_OR_TOOLS
namespace gor = operations_research;
namespace gorsat = operations_research::sat;
#endif

const NodeFilter allowAllNodes = [](const MRRG::NodeDescriptor&) { return true; };
const OpNodeFilter allowAllNodesForOps = [](const OpGraph::OpDescriptor&, const MRRG::NodeDescriptor&) { return true; };

struct OpAndFU : tuple_hash::TupleHashOps<OpAndFU> {
    OpGraph::OpDescriptor op;
    MRRG::NodeDescriptor fu;

    OpAndFU(OpGraph::OpDescriptor op, MRRG::NodeDescriptor fu)
        : op(op), fu(fu)
    { }

    auto asTuple() const { return std::make_tuple(op, fu); }
};

struct EdgeAndFU : tuple_hash::TupleHashOps<EdgeAndFU> {
    OpGraph::EdgeDescriptor edge;
    MRRG::NodeDescriptor fu;

    EdgeAndFU(OpGraph::EdgeDescriptor edge, MRRG::NodeDescriptor fu)
        : edge(edge), fu(fu)
    { }

    auto asTuple() const { return std::make_tuple(edge.val, edge.output_index, fu); }
};

struct MRRGNodePair : tuple_hash::TupleHashOps<MRRGNodePair> {
    MRRG::NodeDescriptor src_fu;
    MRRG::NodeDescriptor sink_fu;

    MRRGNodePair(MRRG::NodeDescriptor src_fu, MRRG::NodeDescriptor sink_fu)
        : src_fu(src_fu), sink_fu(sink_fu)
    { }

    auto asTuple() const { return std::make_tuple(src_fu, sink_fu); }
};

struct MRRGPathID : tuple_hash::TupleHashOps<MRRGPathID> {
    MRRG::NodeDescriptor src_fu;
    MRRG::NodeDescriptor sink_fu;
    int trip_count;
    int index;

    MRRGPathID(MRRG::NodeDescriptor src_fu, MRRG::NodeDescriptor sink_fu, int trip_count, int index)
        : src_fu(src_fu), sink_fu(sink_fu), trip_count(trip_count), index(index)
    { }

    auto asTuple() const { return std::make_tuple(src_fu, sink_fu, trip_count, index); }
};

struct GorSatCPVarID { int index; };

template<typename CFN>
auto unionOfCompatibleNodes(const OpGraph& opgraph, const CFN& compatible_fu_nodes) {
    std::unordered_set<MRRG::NodeDescriptor> all_compatible_nodes_set;
    for (auto& op : opgraph.opNodes()) {
        for (auto& compatible_node : compatible_fu_nodes.at(op)) {
            all_compatible_nodes_set.emplace(compatible_node);
        }
    }
    return all_compatible_nodes_set;
}

#ifdef CGRAME_USE_GOOGLE_OR_TOOLS
GorSatCPVarID makeBoolVar(gorsat::CpModelProto& cpmp, const std::string& name) {
    const int index = cpmp.variables_size();
    gorsat::IntegerVariableProto* const var = cpmp.add_variables();
    var->set_name(name);
    var->add_domain(0);
    var->add_domain(1);
    return GorSatCPVarID{index};
}

template<typename Literals>
auto addLogicalOr(gorsat::CpModelProto& cpmp, const Literals& literals) {
    gorsat::ConstraintProto* const ct = cpmp.add_constraints();
    gorsat::BoolArgumentProto* const bool_or = ct->mutable_bool_or();
    for (const auto& lit : literals) {
        bool_or->add_literals(lit.index);
    }
    return std::make_pair(ct, bool_or);
}

template<typename Literals>
auto addConstrainedSum(gorsat::CpModelProto& cpmp, const Literals& literals, const std::vector<std::pair<long long, long long>>& domains) {
    gorsat::ConstraintProto* const ct = cpmp.add_constraints();
    gorsat::LinearConstraintProto* const lin = ct->mutable_linear();
    for (const auto& lit : literals) {
        lin->add_vars(lit.index);
        lin->add_coeffs(1);
    }
    for (const auto& dom : domains) {
        lin->add_domain(dom.first);
        lin->add_domain(dom.second);
    }
    return std::make_pair(ct, lin);
}

template<typename Literals>
auto addMaxConstraint(gorsat::CpModelProto& cpmp, const Literals& literals, GorSatCPVarID equal_to) {
    // no idea why this doesn't work
    // gorsat::ConstraintProto* const ct = cpmp.add_constraints();
    // gorsat::IntegerArgumentProto* const maxCt = ct->mutable_int_max();
    // for (const auto& lit : literals) {
    //     maxCt->add_vars(lit.index);
    // }
    // maxCt->set_target(equal_to.index);
    // return std::make_pair(ct, maxCt);

    auto ct_and_lin = addConstrainedSum(cpmp, literals, {{0,kint64max}});
    ct_and_lin.second->add_vars(equal_to.index);
    ct_and_lin.second->add_coeffs(-1);
    return ct_and_lin;
}
#endif

std::string from_src_at_to_sink_at(std::string prefix, const OpGraph& opgraph, const MRRG& mrrg,
    OpGraph::OpDescriptor src_op, MRRG::NodeDescriptor src_compatible_node,
    OpGraph::OpDescriptor sink_op, MRRG::NodeDescriptor sink_compatible_node,
    bool make_name_solver_safe = false
) {
    prefix += ".from_src";
    if (src_op                 ) { prefix += '.' + opgraph.getNodeRef(src_op).getName(); }
    if (src_compatible_node    ) { prefix += ".at." + mrrg.getNodeRef(src_compatible_node).getFullName(); }
    prefix += ".to_sink";
    if (sink_op                ) { prefix += '.' + opgraph.getNodeRef(sink_op).getName(); }
    if (sink_compatible_node   ) { prefix += ".at." + mrrg.getNodeRef(sink_compatible_node).getFullName(); }

    return make_name_solver_safe ? makeNameSolverSafe(std::move(prefix)) : std::move(prefix);
}

std::string from_src_at_to_sink_at(std::string prefix, const OpGraph& opgraph, const MRRG& mrrg,
    OpGraph::OpDescriptor src_op, MRRG::NodeDescriptor src_compatible_node,
    OpGraph::EdgeDescriptor out_edge, MRRG::NodeDescriptor sink_compatible_node,
    bool make_name_solver_safe = false
) {
    auto result = from_src_at_to_sink_at(
        std::move(prefix), opgraph, mrrg, src_op, src_compatible_node,
        opgraph.targetOfEdge(out_edge), sink_compatible_node,
        false
    ) + ".out_edge" + std::to_string(out_edge.output_index);

    return make_name_solver_safe ? makeNameSolverSafe(std::move(result)) : std::move(result);
}

std::string from_src_at_to_sink_at_trip_count(std::string prefix, const OpGraph& opgraph, const MRRG& mrrg,
    OpGraph::OpDescriptor src_op, MRRG::NodeDescriptor src_compatible_node,
    OpGraph::OpDescriptor sink_op, MRRG::NodeDescriptor sink_compatible_node,
    int trip_count,
    bool make_name_solver_safe = false
) {
    auto result = from_src_at_to_sink_at(
        std::move(prefix), opgraph, mrrg,
        src_op, src_compatible_node, sink_op, sink_compatible_node
    ) + ".trip_count." + std::to_string(trip_count);
    return make_name_solver_safe ? makeNameSolverSafe(std::move(result)) : std::move(result);
}

std::string from_src_at_to_sink_at_trip_count(std::string prefix, const OpGraph& opgraph, const MRRG& mrrg,
    OpGraph::OpDescriptor src_op, MRRG::NodeDescriptor src_compatible_node,
    OpGraph::EdgeDescriptor out_edge, MRRG::NodeDescriptor sink_compatible_node,
    int trip_count,
    bool make_name_solver_safe = false
) {
    auto result = from_src_at_to_sink_at(
        std::move(prefix), opgraph, mrrg,
        src_op, src_compatible_node, out_edge, sink_compatible_node
    ) + ".trip_count." + std::to_string(trip_count);
    return make_name_solver_safe ? makeNameSolverSafe(std::move(result)) : std::move(result);
}

std::string from_src_at_to_sink_at_trip_count_number(std::string prefix, const OpGraph& opgraph, const MRRG& mrrg,
    OpGraph::OpDescriptor src_op, MRRG::NodeDescriptor src_compatible_node,
    OpGraph::OpDescriptor sink_op, MRRG::NodeDescriptor sink_compatible_node,
    int trip_count, int number,
    bool make_name_solver_safe = false
) {
    auto result = from_src_at_to_sink_at_trip_count(
        std::move(prefix), opgraph, mrrg,
        src_op, src_compatible_node, sink_op, sink_compatible_node, trip_count)
        + ".number." + std::to_string(number);
    return make_name_solver_safe ? makeNameSolverSafe(std::move(result)) : std::move(result);
}

std::string from_src_at_to_sink(std::string prefix, const OpGraph& opgraph, const MRRG& mrrg,
    OpGraph::OpDescriptor src_op, MRRG::NodeDescriptor src_compatible_node,
    OpGraph::OpDescriptor sink_op,
    bool make_name_solver_safe = false
) {
    return from_src_at_to_sink_at(std::move(prefix), opgraph, mrrg,
        src_op, src_compatible_node, sink_op, nullptr, make_name_solver_safe);
}

std::string from_src_to_sink_at(std::string prefix, const OpGraph& opgraph, const MRRG& mrrg,
    OpGraph::OpDescriptor src_op,
    OpGraph::EdgeDescriptor out_edge, MRRG::NodeDescriptor sink_compatible_node,
    bool make_name_solver_safe = false
) {
    return from_src_at_to_sink_at(std::move(prefix), opgraph, mrrg,
        src_op, nullptr, out_edge, sink_compatible_node, make_name_solver_safe);
}

std::string from_mrrg_node_to_mrrg_node(std::string prefix, const OpGraph& opgraph, const MRRG& mrrg,
    MRRG::NodeDescriptor src_compatible_node,
    MRRG::NodeDescriptor sink_compatible_node,
    bool make_name_solver_safe = false
) {
    return from_src_at_to_sink_at(std::move(prefix), opgraph, mrrg,
        nullptr, src_compatible_node, nullptr, sink_compatible_node, make_name_solver_safe);
}

std::string from_mrrg_node_to_mrrg_node_trip_count_number(std::string prefix, const OpGraph& opgraph, const MRRG& mrrg,
    MRRG::NodeDescriptor src_compatible_node,
    MRRG::NodeDescriptor sink_compatible_node,
    int trip_count, int number,
    bool make_name_solver_safe = false
) {
    auto result = from_mrrg_node_to_mrrg_node(std::move(prefix), opgraph, mrrg, src_compatible_node, sink_compatible_node)
        + ".trip_count." + std::to_string(trip_count) + ".number." + std::to_string(number);
    return make_name_solver_safe ? makeNameSolverSafe(std::move(result)) : std::move(result);
}

std::ostream& operator<<(std::ostream& os, const ILPHeuristicMapperOptions& opts) {
    os << "{\n"
        << "\tfind_all_neighbour_FUs = " << (opts.find_all_neighbour_FUs ? "set" : "unset") << '\n'
        << "\tsolution_selector = "      << (opts.solution_selector      ? "set" : "unset") << '\n'
        << "\tedge_coster = "            << (opts.edge_coster            ? "set" : "unset") << '\n'
        << "\tgeneral = " << opts.general << '\n'
    << '}';
    return os;
}

struct InternalILPHeuristicMapperOptions : ILPHeuristicMapperOptions {
    std::string solver_id;
    bool use_solver_gurobi;
    bool use_solver_gor_classic_cp;
    bool use_solver_gor_sat_cp;

    bool print_initial_mapping;
    bool print_intermediate_mappings;
    bool print_mapping;

    bool operand_nodes_are_normal;

    bool use_edge_vars_constrained_via_implication_network;
    bool split_edge_variables_by_trip_count;
    bool add_trip_count_balancing_constraints;
    bool add_trip_count_topological_constraints;

    InternalILPHeuristicMapperOptions(ILPHeuristicMapperOptions options_)
        : ILPHeuristicMapperOptions(std::move(ILPHeuristicMapperOptions::add_defaults(options_)))

        , solver_id( general.getString("solver") )
        , use_solver_gurobi(         solver_id == Solvers::Gurobi.id                  )
        , use_solver_gor_classic_cp( solver_id == Solvers::GoogleORTools_ClassicCP.id )
        , use_solver_gor_sat_cp(     solver_id == Solvers::GoogleORTools_SatCP.id     )

        , print_initial_mapping(       general.getBool("print_initial_mapping") )
        , print_intermediate_mappings( general.getBool("print_intermediate_mappings") )
        , print_mapping(               general.getBool("print_mapping") )

        , operand_nodes_are_normal(    general.getBool("operand_nodes_are_normal") )

        , use_edge_vars_constrained_via_implication_network(general.getBool("allow_multiple_op_mappings") || general.getBool("path_exclusivity_modelling"))
        , split_edge_variables_by_trip_count(false)
        , add_trip_count_balancing_constraints(false)
        , add_trip_count_topological_constraints(false)
    {
        override_all(ConfigStore(default_general_opts()), general); // check for typos

        if (not (use_solver_gurobi || use_solver_gor_classic_cp || use_solver_gor_sat_cp)) {
            throw make_from_stream<cgrame_mapper_error>([&](auto&& s) {
                s << "unhandled solver ID " << solver_id;
            });
        }

        if (general.getBool("path_exclusivity_modelling")) {
            if (not general.getBool("allow_unbalanced_latency")) {
                split_edge_variables_by_trip_count = true;
                if (general.getBool("allow_multiple_op_mappings")) {
                    add_trip_count_topological_constraints = true;
                } else {
                    add_trip_count_balancing_constraints = true;
                }
            }
        }

        is_node_allowed_for_op = [this, old_inafo=is_node_allowed_for_op](const auto& op, const auto& n) {
            return this->is_node_allowed ? this->is_node_allowed(n) : (old_inafo ? old_inafo(op,n) : true);
        };
    }

    friend std::ostream& operator<<(std::ostream& os, const InternalILPHeuristicMapperOptions& opts) {
        os << "{\n"
            << "\tBase Options = " << static_cast<const ILPHeuristicMapperOptions&>(opts) << '\n'
            << "\tsolver_id = " << opts.solver_id << '\n'
            << "\tuse_solver_gurobi = " << opts.use_solver_gurobi << '\n'
            << "\tuse_solver_gor_classic_cp = " << opts.use_solver_gor_classic_cp << '\n'
            << "\tuse_solver_gor_sat_cp = " << opts.use_solver_gor_sat_cp << '\n'
        << '}';
        return os;
    }
};

ILPHeuristicMapperOptions::Solvers::SolverID ILPHeuristicMapperOptions::Solvers::Gurobi { "gurobi" };
ILPHeuristicMapperOptions::Solvers::SolverID ILPHeuristicMapperOptions::Solvers::GoogleORTools_ClassicCP { "google-or-tools-classic-cp" };
ILPHeuristicMapperOptions::Solvers::SolverID ILPHeuristicMapperOptions::Solvers::GoogleORTools_SatCP { "google-or-tools-sat-cp" };

template<typename MappedType>
using OpAndFUMap = std::unordered_map<OpAndFU, MappedType, OpAndFU::Hash>;

template<typename MappedType>
using EdgeAndFUMap = std::unordered_map<EdgeAndFU, MappedType, EdgeAndFU::Hash>;

template<typename MappedType>
using FUMap = std::unordered_map<MRRG::NodeDescriptor, MappedType>;

Mapping mapViaConnectivityAndPathChoosing(std::unordered_map<std::string, std::string> fix_port,
    const OpGraph& opgraph, const MRRG& mrrg,
    const Mapping& initial_mapping,
    ILPHeuristicMapperOptions options_,
    ILPHeuristicMapperCaches caches
) try {
    const auto options = InternalILPHeuristicMapperOptions(std::move(options_));

    const auto solve_approach_name = options.general.getString("solve_approach_name");
    const bool print_solve_progress = options.general.getBool("print_solve_progress");
    const bool do_print_configuration_and_statistics = options.general.getBool("print_configuration_and_statistics");
    PrintOnDestructionChronoSequence timing_seq(solve_approach_name, print_solve_progress ? &std::cout : nullptr);
    ConfigStore problem_statistics;
    ConfigStore solve_statistics;

    if (do_print_configuration_and_statistics) {
        std::cout << "finding a mapping via " << solve_approach_name << ", with...\n "
            << "\t" << "Options = " << options << '\n'
        ;
    }
    if (options.print_initial_mapping) {
        initial_mapping.outputMapping(opgraph, std::cout);
    }
    timing_seq.tick("initial dumps", 0.01);

    #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
    gor::Solver gorCP_solver("mapOpsJustByConnectivity");

    gorsat::CpModelProto gorSatCP_model;
    gorSatCP_model.set_name("mapOpsJustByConnectivity");
    #endif

    const auto solver_seed = options.general.getInt("seed");
    const auto max_threads = options.general.getInt("max_threads");
    const auto timelimit = options.general.getInt("timelimit");
    const bool do_print_neighbours = options.general.getBool("print_neighbours");
    const bool do_print_paths = options.general.getBool("print_paths");
    const auto model_dump_filename = options.general.getString("model_dump_filename");
    const auto model_IIS_dump_filename = options.general.getString("model_IIS_dump_filename");

    const auto max_num_paths = options.general.getInt("max_num_paths_between_FUs");
    const auto allowable_routing_usage_multiplier = options.general.getInt("allowable_routing_usage_multiplier");

    const auto allowed_cycle_trip_count_min_max = [&]() -> std::pair<int,int> { // (immediately invoked)
        const auto really_big = std::numeric_limits<int>::max();
        if (options.general.getBool("allow_unbalanced_latency")) { return {0, really_big}; } // anything goes
        else { return {1, 1}; } // self-loops must be trip_count == 1;
    }();

    // TODO: Move this to functionality on class Mapping
    const auto mrrg_node_user_in_initial_mapping = [&]() {
        std::unordered_map<MRRG::NodeDescriptor, std::vector<OpGraph::OpDescriptor>> result;
        initial_mapping.forEachMapping([&](const auto& og_ndesc, const auto& mrrg_ndesc) {
            const auto op_desc = opgraph.asOp(og_ndesc);
            if (not op_desc) { return; }
            result[mrrg_ndesc].push_back(op_desc);
        });
        return result;
    }();
    timing_seq.tick("compute reverse initial mapping", 0.01);

    const auto mrrg_node_is_free_or_used_by_op_node = [&](
        const OpGraph::NodeDescriptor& op_node, const MRRG::NodeDescriptor& mrrg_node
    ) {
        const auto search_result = mrrg_node_user_in_initial_mapping.find(mrrg_node);
        if (search_result == mrrg_node_user_in_initial_mapping.end()) {
            return true; // not in use, go ahead
        } else {
            // if used, only okay if op_node is the initial mapping
            auto& ops_mapped = search_result->second;
            return std::find(ops_mapped.begin(), ops_mapped.end(), op_node) != ops_mapped.end();
        }
    };

    const auto op_node_is_allowed_to_map_to_mrrg = [&](
        const OpGraph::NodeDescriptor& op_node, const MRRG::NodeDescriptor& mrrg_node, std::unordered_map<std::string, std::string> fix_port
    ) {
        if(fix_port.empty()) {
            return true;
        }
        bool result = true;
        for (auto& node : fix_port) {
            if (op_node->getName() == node.first) {
                if (mrrg_node->getHierarchyQualifiedName().find(node.second) == node.second.npos) {
                    result = false;
                }
            }
        }
        return result;
    };

    // the true mrrg<->op filter
    const auto op_node_can_use_mrrg_node = [&](
        const OpGraph::OpDescriptor& op, const MRRG::NodeDescriptor& n
    ) {
        return options.is_node_allowed_for_op(op, n) && mrrg_node_is_free_or_used_by_op_node(op, n) && op_node_is_allowed_to_map_to_mrrg(op, n, fix_port);
    };

    const auto all_compatible_nodes = findAllCompatibleFUs(opgraph, mrrg);
    const auto all_compatible_nodes_set = unionOfCompatibleNodes(opgraph, all_compatible_nodes);
    const std::vector<MRRG::NodeDescriptor> all_compatible_nodes_list(begin(all_compatible_nodes_set), end(all_compatible_nodes_set));
    // TODO: filter the isReachableFrom by (src_op == sink_op || src_compatible_node != sink_compatible_node)
    auto fu_neighbours = options.find_all_neighbour_FUs ? options.find_all_neighbour_FUs(mrrg, all_compatible_nodes_list) : FunctionalUnitNeighbours{};
    if (options.general.getBool("add_self_as_neighbour_if_path")) {
        for (auto& fu_and_info : fu_neighbours) {
            const auto fu = fu_and_info.first;
            if (fu_neighbours.isReachableFrom(fu, fu)) { continue; }
            const auto& paths = findNRoutingPathsBetween(1, mrrg, fu, fu, allowed_cycle_trip_count_min_max, {}, caches.mrrg_proc_cache);
            if (paths.empty()) { continue; }
            fu_and_info.second.emplace(fu, FunctionalUnitNeighbours::NodeInfo{fu, (int)paths.at(0).size() - 1});
        }
    }

    const auto treatAsNeighbours = [&](const OpAndFU& src_op_and_fu, const OpAndFU& sink_op_and_fu) {
        // no point in having edges for DFG self-loops mapped to different FU nodes!
        if (src_op_and_fu.op == sink_op_and_fu.op && src_op_and_fu.fu != sink_op_and_fu.fu) { return false; }
        // if nothing else applies, just check if they are neighbours.
        return fu_neighbours.isReachableFrom(src_op_and_fu.fu, sink_op_and_fu.fu);
    };

    if (options.general.getBool("print_compatible_nodes")) {
        std::cout << "Compatible Nodes: " << fu_neighbours << std::endl;
        print_assoc_container(std::cout, all_compatible_nodes,
            [&](auto&& s, const auto& nodes) { print_container(s, nodes,
                [&](auto&& s, const auto& n) { s << mrrg.getNodeRef(n).getFullName(); }
            );},
            [&](auto&& s, const auto& op) { s << opgraph.getNodeRef(op).getName(); }
        );
        std::cout << std::endl;
    }

    if (do_print_neighbours) { std::cout << "FU Neighbours: " << fu_neighbours << std::endl; }

    std::unordered_map<OpGraph::OpDescriptor, std::vector<MRRG::NodeDescriptor>> compatible_fu_nodes;
    for (auto& op : opgraph.opNodes()) {
        const auto is_node_allowed_for_this_op = [&](auto&& n) { return op_node_can_use_mrrg_node(op, n); };

        auto& mappings_for_op = compatible_fu_nodes[op];
        if (initial_mapping.hasMapping(op)) {
            for (const auto& mapped_node : initial_mapping.getMappingList(op)) {
                if (not is_node_allowed_for_this_op(mapped_node)) {
                    if (not options.general.getBool("silence_warnings")) {
                    std::cout << "Warning: node filter(s) disallow the existing mapping of "
                        << opgraph.getNodeRef(op) << " to " << mrrg.getNodeRef(mapped_node) << std::endl;
                    }
                    continue;
                }
                mappings_for_op.push_back(mapped_node);
                // if a fanout is mapped too, make sure the fanout's FU is a neighbour
                for (const auto& sink_op : opgraph.outputOps(op)) {
                    if (not initial_mapping.hasMapping(sink_op)) { continue; }
                    for (const auto& sink_mapped_node : initial_mapping.getMappingList(sink_op)) {
                        if (not treatAsNeighbours({op, mapped_node}, {sink_op, sink_mapped_node})) {
                            fu_neighbours.neighbours[mapped_node].emplace(
                                sink_mapped_node,
                                FunctionalUnitNeighbours::NodeInfo{sink_mapped_node, -1}
                            );
                        }
                    }
                }
            }
        } else if (options.general.getBool("map_unmapped_ops")) {
            const auto& compat_nodes = all_compatible_nodes.at(op);
            std::copy_if(compat_nodes.begin(), compat_nodes.end(), std::back_inserter(mappings_for_op), is_node_allowed_for_this_op);
        }

        if (mappings_for_op.empty() && not options.general.getBool("silence_warnings")) {
            std::cout << "Warning: no compatible nodes for " << opgraph.getNodeRef(op) << std::endl;
        }
    }
    timing_seq.tick("compute compatible_fu_nodes", 0.01);

    const auto opfu_opfu_edge_counts = [&](){
        OpAndFUMap<OpAndFUMap<int>> result;
        for (const auto& src_op : opgraph.opNodes()) {
            for (const auto& src_compatible_node : compatible_fu_nodes.at(src_op)) {
                const auto src_op_and_fu = OpAndFU{src_op, src_compatible_node};
                for (const auto& sink_op : opgraph.outputOps(src_op)) {
                    for (const auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
                        const auto sink_op_and_fu = OpAndFU{sink_op, sink_compatible_node};
                        if (treatAsNeighbours(src_op_and_fu, sink_op_and_fu)) {
                            result[src_op_and_fu][sink_op_and_fu] += 1;
                        }
                    }
                }
            }
        }

        return result;
    }();

    const auto fu_fu_max_edge_counts = [&](){
        FUMap<FUMap<int>> result;
        for (const auto& src_opfu_and_sink_opfu_edge_counts : opfu_opfu_edge_counts) {
            for (const auto& sink_opfu_and_edge_count : src_opfu_and_sink_opfu_edge_counts.second) {
                auto& max_paths_needed = result[src_opfu_and_sink_opfu_edge_counts.first.fu][sink_opfu_and_edge_count.first.fu];
                max_paths_needed = std::max(max_paths_needed, sink_opfu_and_edge_count.second);
            }
        }

        return result;
    }();
    timing_seq.tick("compute opfu_opfu_edge_counts and fu_fu_max_edge_counts", 0.01);

    std::unordered_map<MRRGNodePair, std::map<int, std::vector<std::vector<MRRG::NodeDescriptor>>>, MRRGNodePair::Hash> mrrg_paths;
    if (options.general.getBool("path_exclusivity_modelling")) {
        // for each edge
        for (auto& src_op1 : opgraph.opNodes()) {
        for (auto& sink_op1 : opgraph.outputOps(src_op1)) {

            // for compatible node
            for (auto& src_compatible_node1 : compatible_fu_nodes.at(src_op1)) {
            for (auto& sink_compatible_node1 : compatible_fu_nodes.at(sink_op1)) {
                const auto src_op_and_fu1 = OpAndFU{src_op1, src_compatible_node1};
                const auto sink_op_and_fu1= OpAndFU {sink_op1, sink_compatible_node1};
                if (not treatAsNeighbours(src_op_and_fu1, sink_op_and_fu1)) { continue; }
                const auto emplace_result = mrrg_paths.insert({{src_compatible_node1, sink_compatible_node1}, {}});
                if (not emplace_result.second) { continue; } // avoid recomputation
                auto& mrrg_paths_item = emplace_result.first->second;
                const auto& paths = findNRoutingPathsBetween(max_num_paths, mrrg, src_compatible_node1, sink_compatible_node1,
                    allowed_cycle_trip_count_min_max, {}, caches.mrrg_proc_cache);
                for (auto& path : paths) {
                    auto trip_count = tripCountOfWalk(mrrg, path);
                    auto& mrrg_paths_for_trip_count = mrrg_paths_item[trip_count];
                    if (do_print_paths) {
                        std::cout << from_src_at_to_sink_at_trip_count_number("path_for", opgraph, mrrg, src_op1,
                            src_compatible_node1, sink_op1, sink_compatible_node1, trip_count, mrrg_paths_for_trip_count.size());
                        print_container(std::cout, path);
                        std::cout << '\n';
                    }
                    mrrg_paths_for_trip_count.emplace_back(std::move(path));
                }
                if (do_print_paths) { std::cout << '\n'; }
            }
            }
        }
        }
    } else {
        // There is one edge variable per value of trip_count;
        //   if the map is empty, then there won't be any edge variables.
        // Therefore, make sure there is one trip count value, even if it is bogus.
        // for each edge
        for (const auto& src_op1 : opgraph.opNodes()) {
        for (const auto& sink_op1 : opgraph.outputOps(src_op1)) {
            // for each compatible node
            for (const auto& src_compatible_node1 : compatible_fu_nodes.at(src_op1)) {
            for (const auto& sink_compatible_node1 : compatible_fu_nodes.at(sink_op1)) {
                const auto  src_op_and_fu1 = OpAndFU{ src_op1,  src_compatible_node1};
                const auto sink_op_and_fu1 = OpAndFU{sink_op1, sink_compatible_node1};
                if (not treatAsNeighbours(src_op_and_fu1, sink_op_and_fu1)) { continue; }
                mrrg_paths.insert({ {src_compatible_node1, sink_compatible_node1}, { {-1, {}} } });
            }
            }
        }
        }
    }
    timing_seq.tick("compute mrrg_paths", 0.02);

    auto trails_to_balance = lazy_pointer([&]{ return computeTrailsToBalance(opgraph); });
    auto dfg_edge_latency = lazy_pointer([&]{ return opgraph.edgeLatencies(); });

    problem_statistics.addReal("average neighbour count", std::accumulate(fu_neighbours.begin(), fu_neighbours.end(), 0L,
        [&](auto& v, auto& item) { return v + item.second.size(); }
    )/(double)fu_neighbours.neighbours.size());

    problem_statistics.addInt("DFG - number of OPs", opgraph.opNodes().size());

    if (do_print_configuration_and_statistics) {
        std::cout << "Problem Statistics = " << problem_statistics << '\n';
    }

    timing_seq.tick("compute and print statistics", 0.01);
    timing_seq.tick("setup");

    if (options.general.getBool("just_warm_up_caches")) {
        // resolve any lazy (but cacheable) data here
        timing_seq.tick("warm up caches");
        Mapping cpy = initial_mapping;
        cpy.clear();
        cpy.setStatus(MappingStatus::success); // successfully did nothing!
        return cpy;
    }

    GRBEnv env = GRBEnv(true);
    if (not print_solve_progress) {
        env.set(GRB_IntParam_OutputFlag, 0); // (nearly) silence Gurobi
    }
    env.start();
    if (timelimit != 0) {
        env.set(GRB_DoubleParam_TimeLimit, timelimit);
    }
    GRBModel model = GRBModel(env);

    const auto op_is_mapped_to_var_maps = [&]() {
        std::tuple<
            OpAndFUMap<GRBVar>
            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
            ,
            OpAndFUMap<gor::IntVar*>,
            OpAndFUMap<GorSatCPVarID>
            #endif
        > retval;

        auto& op_is_mapped_to_vars_result = std::get<0>(retval);
        #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
        auto& gorCP_op_is_mapped_to_vars_result = std::get<1>(retval);
        auto& gorSatCP_op_is_mapped_to_vars_result = std::get<2>(retval);
        #endif

        for (auto& op : opgraph.opNodes()) {
            for (auto& compatible_node : compatible_fu_nodes.at(op)) {
                const OpAndFU op_and_fu {op, compatible_node};
                auto var = std::unique_ptr<GRBVar[]>(model.addVars(1, GRB_BINARY))[0]; // free the returned pointer immediately
                const std::string name = "op." + opgraph.getNodeRef(op).name + ".is_mapped_to." + mrrg.getNodeRef(compatible_node).getFullName();
                var.set(GRB_StringAttr_VarName, name);
                op_is_mapped_to_vars_result.emplace(op_and_fu, var);
                #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
                gorCP_op_is_mapped_to_vars_result.emplace(op_and_fu, gorCP_solver.MakeBoolVar(name));
                gorSatCP_op_is_mapped_to_vars_result.emplace(op_and_fu, makeBoolVar(gorSatCP_model, name));
                #endif
                if (options.fu_coster) {
                    const auto fu_cost = options.fu_coster(opgraph, mrrg, op, compatible_node);
                    var.set(GRB_DoubleAttr_Obj, fu_cost);
                }
            }
        }
        return retval;
    }();

    const auto& op_is_mapped_to_vars = std::get<0>(op_is_mapped_to_var_maps);
    #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
    const auto& gorCP_op_is_mapped_to_vars = std::get<1>(op_is_mapped_to_var_maps);
    const auto& gorSatCP_op_is_mapped_to_vars = std::get<2>(op_is_mapped_to_var_maps);
    #endif
    solve_statistics.addInt("op_is_mapped_to_vars.size()",op_is_mapped_to_vars.size());

    const bool edge_vars_needed = (bool)options.edge_coster || options.use_edge_vars_constrained_via_implication_network;
    const auto edge_is_used_vars_maps = [&]() {
        std::tuple<
            OpAndFUMap<EdgeAndFUMap<std::map<int,GRBVar>>>
            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
            ,
            OpAndFUMap<EdgeAndFUMap<std::map<int,gor::IntVar*>>>,
            OpAndFUMap<EdgeAndFUMap<std::map<int,GorSatCPVarID>>>
            #endif
        > retval;

        auto& edge_is_used_vars_result = std::get<0>(retval);
        #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
        auto& gorCP_edge_is_used_vars_result = std::get<1>(retval);
        auto& gorSatCP_edge_is_used_vars_result = std::get<2>(retval);
        #endif

        if (not edge_vars_needed) {
            return retval;
        }

        for (const auto& src_op : opgraph.opNodes()) {
            for (const auto& out_edge : opgraph.outEdges(src_op)) {
                const auto sink_op = opgraph.targetOfEdge(out_edge);
                for (const auto& src_compatible_node : compatible_fu_nodes.at(src_op)) {
                    const auto src_op_and_fu = OpAndFU{src_op, src_compatible_node};
                    for (const auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
                        const auto sink_op_and_fu = OpAndFU{sink_op, sink_compatible_node};
                        const auto out_edge_and_fu = EdgeAndFU{out_edge, sink_compatible_node};
                        if (not treatAsNeighbours(src_op_and_fu, sink_op_and_fu)) { continue; }
                        if (edge_is_used_vars_result[src_op_and_fu].find(out_edge_and_fu) != edge_is_used_vars_result[src_op_and_fu].end()) {
                            continue; // multiple edges between ops is handled in the path variables
                        }
                        for (const auto& trip_count_and_paths : mrrg_paths.at({src_compatible_node,sink_compatible_node})) {
                            auto eu_var = std::unique_ptr<GRBVar[]>(model.addVars(1, GRB_BINARY))[0]; // free the returned pointer immediately
                            const auto var_name = from_src_at_to_sink_at_trip_count("edge_is_used", opgraph, mrrg,
                                src_op, src_compatible_node, out_edge, sink_compatible_node, trip_count_and_paths.first);
                            eu_var.set(GRB_StringAttr_VarName, var_name);
                            if (options.edge_coster) {
                                const auto edge_cost = options.edge_coster(fu_neighbours, *src_op, src_compatible_node, *sink_op, sink_compatible_node);
                                eu_var.set(GRB_DoubleAttr_Obj, edge_cost);
                            }
                            edge_is_used_vars_result[src_op_and_fu][out_edge_and_fu].emplace(trip_count_and_paths.first, eu_var);
                            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
                            gorCP_edge_is_used_vars_result[src_op_and_fu][out_edge_and_fu].emplace(
                                trip_count_and_paths.first, gorCP_solver.MakeBoolVar(var_name));
                            (void)gorSatCP_edge_is_used_vars_result;
                            // gorSatCP_edge_is_used_vars_result[src_op_and_fu][out_edge_and_fu].emplace(
                            //     trip_count_and_paths.first, makeBoolVar(gorSatCP_model, var_name));
                            #endif
                        }
                    }
                }
            }
        }

        return retval;
    }();

    const auto& edge_is_used_vars = std::get<0>(edge_is_used_vars_maps);
    #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
    const auto& gorCP_edge_is_used_vars = std::get<1>(edge_is_used_vars_maps);
    const auto& gorSatCP_edge_is_used_vars = std::get<2>(edge_is_used_vars_maps);
    #endif
    solve_statistics.addInt("num edge_is_used_vars",
        std::accumulate(edge_is_used_vars.begin(), edge_is_used_vars.end(), (std::ptrdiff_t)0, [](auto sum, auto&& elem) {
            return std::accumulate(elem.second.begin(), elem.second.end(), sum, [](auto sum2, auto&& elem2) {
                return sum2 + (std::ptrdiff_t)elem2.second.size();
            });
        })
    );

    const auto path_is_used_vars_maps = [&]() {
        std::tuple<
            std::unordered_map<MRRGPathID, GRBVar, MRRGPathID::Hash>
            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
            ,
            std::unordered_map<MRRGPathID, gor::IntVar*, MRRGPathID::Hash>,
            std::unordered_map<MRRGPathID, GorSatCPVarID, MRRGPathID::Hash>
            #endif
        > retval;

        auto& path_is_used_vars_result = std::get<0>(retval);
        #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
        auto& gorCP_path_is_used_vars_result = std::get<1>(retval);
        auto& gorSatCP_path_is_used_vars_result = std::get<2>(retval);
        #endif

        if (not options.general.getBool("path_exclusivity_modelling")) {
            return retval;
        }

        std::set<MRRGNodePair> covered_fu_pairs; // don't add the same paths more than once

        for (auto& src_op : opgraph.opNodes()) {
            for (auto& sink_op : opgraph.outputOps(src_op)) {

                for (auto& src_compatible_node : compatible_fu_nodes.at(src_op)) {
                    const auto src_op_and_fu = OpAndFU{src_op, src_compatible_node};
                    for (auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
                        const auto sink_op_and_fu= OpAndFU {sink_op, sink_compatible_node};
                        if (not treatAsNeighbours(src_op_and_fu, sink_op_and_fu)) { continue; }
                        if (not covered_fu_pairs.emplace(src_compatible_node, sink_compatible_node).second) { continue; }

                        for (auto& trip_count_and_paths : mrrg_paths.at({src_compatible_node, sink_compatible_node})) {
                        int path_num = 0;
                        for (auto& path : trip_count_and_paths.second) {
                            (void)path;
                            const auto mrrg_path_id = MRRGPathID {src_compatible_node, sink_compatible_node, trip_count_and_paths.first, path_num};
                            const auto var_name = from_mrrg_node_to_mrrg_node_trip_count_number("path_is_used", opgraph, mrrg,
                                src_compatible_node, sink_compatible_node, trip_count_and_paths.first, path_num);
                            auto var = std::unique_ptr<GRBVar[]>(model.addVars(1, GRB_BINARY))[0]; // free the returned pointer immediately
                            var.set(GRB_StringAttr_VarName, var_name);
                            path_is_used_vars_result.emplace(mrrg_path_id, var);
                            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
                            gorCP_path_is_used_vars_result.emplace(mrrg_path_id, gorCP_solver.MakeBoolVar(var_name));
                            (void)gorSatCP_path_is_used_vars_result;
                            // gorSatCP_path_is_used_vars_result.emplace(mrrg_path_id, makeBoolVar(gorSatCP_model, var_name));
                            #endif
                            path_num += 1;
                        }
                        }
                    }
                }
            }
        }
        return retval;
    }();

    const auto& path_is_used_vars = std::get<0>(path_is_used_vars_maps);
    #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
    const auto& gorCP_path_is_used_vars = std::get<1>(path_is_used_vars_maps);
    const auto& gorSatCP_path_is_used_vars = std::get<2>(path_is_used_vars_maps);
    #endif
    solve_statistics.addInt("path_is_used_vars.size()",path_is_used_vars.size());

    const auto& topological_trip_count_vars = lazy_pointer([&]() {
        std::tuple<
              std::map<MRRG::NodeDescriptor,GRBVar>
            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
            #endif
        > retval;

        for (const auto& node : all_compatible_nodes_set) {
            auto var = std::unique_ptr<GRBVar[]>(model.addVars(1, GRB_INTEGER))[0];
            const auto var_name = makeNameSolverSafe("topological_trip_count_for." + mrrg.getNodeRef(node).getFullName());
            var.set(GRB_StringAttr_VarName, var_name);
            std::get<0>(retval).emplace(node, var);
        }

        return retval;
    });

    timing_seq.tick("create variables");

    // constraints.registerGenerator(OpMustBeMapped, [&](ConstraintSet::ConstraintGeneratorParams params) {
        for (auto& op : opgraph.opNodes()) {
            GRBLinExpr sum_of_nodes_mapped_to_op;
            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
            std::vector<gor::IntVar*> gorCP_op_vars;
            std::vector<GorSatCPVarID> gorSatCP_op_vars;
            #endif
        OpGraphOp *opJ = op;
	    bool isIOorMem = (opJ->opcode == OpCode::LOAD) || (opJ->opcode == OpCode::STORE) || (opJ->opcode == OpCode::INPUT) || (opJ->opcode == OpCode::OUTPUT);
            for (auto& compatible_node : compatible_fu_nodes.at(op)) {
                const OpAndFU op_and_fu {op, compatible_node};
                sum_of_nodes_mapped_to_op += op_is_mapped_to_vars.at(op_and_fu);
                #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
                gorCP_op_vars.push_back(gorCP_op_is_mapped_to_vars.at(op_and_fu));
                gorSatCP_op_vars.push_back(gorSatCP_op_is_mapped_to_vars.at(op_and_fu));
                #endif
            }
	    if (isIOorMem) {
	      auto c = model.addConstr(sum_of_nodes_mapped_to_op == 1);
	      const auto cname = makeNameSolverSafe("this_op_must_be_mapped." + opgraph.getNodeRef(op).getName());
	      c.set(GRB_StringAttr_ConstrName, cname);
	      
	    }
	    else {
            auto c = [&](){ if (options.general.getBool("allow_multiple_op_mappings")) {
                return model.addConstr(sum_of_nodes_mapped_to_op >= 1);
            } else {
                return model.addConstr(sum_of_nodes_mapped_to_op == 1);
            } }();
            const auto cname = makeNameSolverSafe("this_op_must_be_mapped." + opgraph.getNodeRef(op).getName());
            c.set(GRB_StringAttr_ConstrName, cname);
        }
            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
            if (options.general.getBool("allow_multiple_op_mappings")) {
                gorCP_solver.AddConstraint(gorCP_solver.MakeEquality(gorCP_solver.MakeMax(gorCP_op_vars), 1));
                addLogicalOr(gorSatCP_model, gorSatCP_op_vars);
            } else {
                gorCP_solver.AddConstraint(gorCP_solver.MakeCount(gorCP_op_vars, 1, 1));
                addConstrainedSum(gorSatCP_model, gorSatCP_op_vars, {{1,1}});
            }
            #endif
        }
        timing_seq.tick("OpMustBeMapped constraints", 0.01);
    // });

    // constraints.registerGenerator(FunctionalUnitExclusivity, [&](ConstraintSet::ConstraintGeneratorParams params) {
        for (const auto& outer_compatible_node : all_compatible_nodes_set) {
            GRBLinExpr sum_of_ops_mapped_to_node;
            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
            std::vector<gor::IntVar*> gorCP_fu_vars;
            std::vector<GorSatCPVarID> gorSatCP_fu_vars;
            #endif
            for (const auto& op : opgraph.opNodes()) {
                for (const auto& compatible_node : compatible_fu_nodes.at(op)) {
                    if (outer_compatible_node == compatible_node) {
                        const OpAndFU op_and_fu {op, compatible_node};
                        sum_of_ops_mapped_to_node += op_is_mapped_to_vars.at(op_and_fu);
                        #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
                        gorCP_fu_vars.push_back(gorCP_op_is_mapped_to_vars.at(op_and_fu));
                        gorSatCP_fu_vars.push_back(gorSatCP_op_is_mapped_to_vars.at(op_and_fu));
                        #endif
                    }
                }
            }
            auto c = model.addConstr(sum_of_ops_mapped_to_node <= 1);
            const auto cname = makeNameSolverSafe("functional_unit_exclusivity_for." + mrrg.getNodeRef(outer_compatible_node).getFullName());
            c.set(GRB_StringAttr_ConstrName, cname);
            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
            gorCP_solver.AddConstraint(gorCP_solver.MakeAllDifferentExcept(gorCP_fu_vars, 0));
            addConstrainedSum(gorSatCP_model, gorSatCP_fu_vars, {{0,1}});
            #endif
        }
        timing_seq.tick("FunctionalUnitExclusivity constraints", 0.01);
    // });

    // constraints.registerGenerator(FunctionalUnitFanout, [&](ConstraintSet::ConstraintGeneratorParams params) {
    if (not options.use_edge_vars_constrained_via_implication_network) {
        for (auto& src_op : opgraph.opNodes()) {
            for (auto& sink_op : opgraph.outputOps(src_op)) {
                for (auto& src_compatible_node : compatible_fu_nodes.at(src_op)) {
                    const OpAndFU src_op_and_fu {src_op, src_compatible_node};

                    GRBLinExpr sum_of_sinks_used;
                    #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
                    std::vector<gor::IntVar*> gorCP_sinks_used;
                    std::vector<GorSatCPVarID> gorSatCP_sinks_used;
                    #endif
                    for (auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
                        const OpAndFU sink_op_and_fu {sink_op, sink_compatible_node};
                        if (treatAsNeighbours(src_op_and_fu, sink_op_and_fu)) {
                            sum_of_sinks_used += op_is_mapped_to_vars.at(sink_op_and_fu);
                            #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
                            gorCP_sinks_used.push_back(gorCP_op_is_mapped_to_vars.at(sink_op_and_fu));
                            gorSatCP_sinks_used.push_back(gorSatCP_op_is_mapped_to_vars.at(sink_op_and_fu));
                            #endif
                        }
                    }

                    auto& src_var = op_is_mapped_to_vars.at(src_op_and_fu);
                    auto c = model.addConstr(sum_of_sinks_used >= src_var);
                    const auto constr_name = from_src_at_to_sink("functional_unit_fanout", opgraph, mrrg,
                        src_op, src_compatible_node, sink_op, true);
                    c.set(GRB_StringAttr_ConstrName, constr_name);
                    #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
                    auto& gorCP_src_var = gorCP_op_is_mapped_to_vars.at(src_op_and_fu);
                    const auto gorSatCP_src_var = gorSatCP_op_is_mapped_to_vars.at(src_op_and_fu);
                    if (gorCP_sinks_used.empty()) {
                        gorCP_solver.AddConstraint(gorCP_solver.MakeEquality(gorCP_src_var, 0));
                        addConstrainedSum(gorSatCP_model, std::vector<GorSatCPVarID>{gorSatCP_src_var}, {{0,0}}); // not sure if better way to set to zero
                    } else {
                        gorCP_solver.AddConstraint(gorCP_solver.MakeGreaterOrEqual(gorCP_solver.MakeMax(gorCP_sinks_used), gorCP_src_var));
                        addMaxConstraint(gorSatCP_model, gorSatCP_sinks_used, gorSatCP_src_var);
                    }
                    #endif
                }
            }
        }
        timing_seq.tick("FunctionalUnitFanout constraints", 0.01);
    }
    // });

    // Using FU f implies that for {each fanin Op} there is {an edge that ends at f}
    // constraints.registerGenerator(FunctionalUnitFaninRequired, [&](ConstraintSet::ConstraintGeneratorParams params) {
    if (options.use_edge_vars_constrained_via_implication_network) {
        for (const auto& src_op : opgraph.opNodes()) {
            for (const auto& out_edge : opgraph.outEdges(src_op)) {
                const auto sink_op = opgraph.targetOfEdge(out_edge);
                bool has_fanin_edges = false;
                for (const auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
                    const auto sink_op_and_fu = OpAndFU{sink_op, sink_compatible_node};

                    GRBLinExpr sum_of_in_edges_used;
                    for (const auto& src_compatible_node : compatible_fu_nodes.at(src_op)) {
                        const auto src_op_and_fu = OpAndFU{src_op, src_compatible_node};
                        if (not treatAsNeighbours(src_op_and_fu, sink_op_and_fu)) { continue; }
                        const auto out_edge_and_fu = EdgeAndFU{out_edge, sink_compatible_node};
                        for (const auto& trip_count_and_paths : mrrg_paths.at({src_compatible_node, sink_compatible_node})) {
                            has_fanin_edges = true;
                            sum_of_in_edges_used += edge_is_used_vars.at(src_op_and_fu).at(out_edge_and_fu).at(trip_count_and_paths.first);
                        }
                    }

                    const auto& sink_var = op_is_mapped_to_vars.at(sink_op_and_fu);
                    auto c = model.addConstr(sink_var <= sum_of_in_edges_used);
                    const auto constr_name = from_src_to_sink_at("functional_unit_fanin_required", opgraph, mrrg,
                        src_op, out_edge, sink_compatible_node, true);
                    c.set(GRB_StringAttr_ConstrName, constr_name);
                }

                if (not has_fanin_edges && not options.general.getBool("silence_warnings")) {
                    std::cout << "Warning: no edges for " << opgraph.getNodeRef(src_op) << " -> " << opgraph.getNodeRef(sink_op) << std::endl;
                    std::cout << "src compatible nodes: ";
                    print_container(std::cout, compatible_fu_nodes.at(src_op));
                    std::cout << std::endl << "sink compatible nodes: ";
                    print_container(std::cout, compatible_fu_nodes.at(sink_op));
                    std::cout << std::endl;
                }
            }
        }
        timing_seq.tick("FunctionalUnitFaninRequired constraints", 0.01);
    }
    // });

    // Using an edge from a FU implies that that FU is in use
    // constraints.registerGenerator(FunctionalUnitFanoutImpliesUsage, [&](ConstraintSet::ConstraintGeneratorParams params) {
    if (options.use_edge_vars_constrained_via_implication_network) {
        for (const auto& src_op : opgraph.opNodes()) {
            for (const auto& out_edge : opgraph.outEdges(src_op)) {
                const auto sink_op = opgraph.targetOfEdge(out_edge);
                for (const auto& src_compatible_node : compatible_fu_nodes.at(src_op)) {
                    const auto src_op_and_fu = OpAndFU{src_op, src_compatible_node};
                    const auto& src_var = op_is_mapped_to_vars.at(src_op_and_fu);
                    for (const auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
                        const auto out_edge_and_fu = EdgeAndFU{out_edge, sink_compatible_node};
                        const auto sink_op_and_fu = OpAndFU{sink_op, sink_compatible_node};
                        if (not treatAsNeighbours(src_op_and_fu, sink_op_and_fu)) { continue; }
                        for (const auto& trip_count_and_paths : mrrg_paths.at({src_compatible_node, sink_compatible_node})) {
                            const auto& eu_var = edge_is_used_vars.at(src_op_and_fu).at(out_edge_and_fu).at(trip_count_and_paths.first);
                            auto c = model.addConstr(eu_var <= src_var);
                            const auto constr_name = from_src_at_to_sink_at_trip_count("functional_unit_fanout_implies_usage", opgraph, mrrg,
                                    src_op, src_compatible_node, out_edge, sink_compatible_node, trip_count_and_paths.first, true);
                            c.set(GRB_StringAttr_ConstrName, constr_name);

                            const auto sink_var = op_is_mapped_to_vars.at(sink_op_and_fu);
                            auto c2 = model.addConstr(eu_var <= sink_var);
                            const auto constr2_name = from_src_at_to_sink_at_trip_count("functional_unit_fanin_implies_usage", opgraph, mrrg,
                                    src_op, src_compatible_node, out_edge, sink_compatible_node, trip_count_and_paths.first, true);
                            c2.set(GRB_StringAttr_ConstrName, constr2_name);
                        }
                    }

                }
            }
        }
        timing_seq.tick("FunctionalUnitFanoutImpliesUsage constraints", 0.01);
    }
    // });

    // f_uv && fu_pv == e_opuv; needed if a cost function is given for a placement, and edges vars are not needed otherwise
    // constraints.registerGenerator(EdgeUsage, [&](ConstraintSet::ConstraintGeneratorParams params) {
    if (options.edge_coster && not options.use_edge_vars_constrained_via_implication_network) {
        for (auto& src_op : opgraph.opNodes()) {
            for (const auto& out_edge : opgraph.outEdges(src_op)) {
                const auto sink_op = opgraph.targetOfEdge(out_edge);
                for (auto& src_compatible_node : compatible_fu_nodes.at(src_op)) {
                    const auto src_op_and_fu = OpAndFU{src_op, src_compatible_node};
                    auto& src_var = op_is_mapped_to_vars.at(src_op_and_fu);

                    for (auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
                        const auto out_edge_and_fu = EdgeAndFU{out_edge, sink_compatible_node};
                        const auto sink_op_and_fu = OpAndFU{sink_op, sink_compatible_node};
                        if (treatAsNeighbours(src_op_and_fu, sink_op_and_fu)) { continue; }
                        auto& sink_var = op_is_mapped_to_vars.at(sink_op_and_fu);
                        for (const auto& trip_count_and_paths : mrrg_paths.at({src_compatible_node, sink_compatible_node})) {
                            auto eu_var = edge_is_used_vars.at(src_op_and_fu).at(out_edge_and_fu).at(trip_count_and_paths.first);
                            if (0 == options.edge_coster(fu_neighbours, *src_op, src_compatible_node, *sink_op, sink_compatible_node)) {
                                continue;
                            }

                            std::array<GRBVar, 2> r_values { src_var, sink_var };
                            const auto constr_name = from_src_at_to_sink_at_trip_count("edge_usage", opgraph, mrrg,
                                src_op, src_compatible_node, out_edge, sink_compatible_node, trip_count_and_paths.first);
                            model.addGenConstrAnd(
                                eu_var,
                                r_values.data(), r_values.size(),
                                constr_name
                            );
                        }
                    }

                }
            }
        }
        timing_seq.tick("EdgeUsage constraints", 0.01);
    }
    // });

    // constraints.registerGenerator(EdgeRequiresPath, [&](ConstraintSet::ConstraintGeneratorParams params) {
    if (options.general.getBool("path_exclusivity_modelling")) {
        for (const auto& src_op : opgraph.opNodes()) {
            const auto& all_output_ops = opgraph.outputOps(src_op);
            const auto unique_output_ops = std::set<OpGraph::OpDescriptor>(all_output_ops.begin(), all_output_ops.end());
            const auto& all_out_edges = opgraph.outEdges(src_op);
            for (const auto& sink_op : unique_output_ops) {
                for (const auto& src_compatible_node : compatible_fu_nodes.at(src_op)) {
                    const auto src_op_and_fu = OpAndFU{src_op, src_compatible_node};
                    for (const auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
                        const auto sink_op_and_fu = OpAndFU {sink_op, sink_compatible_node};
                        if (not treatAsNeighbours(src_op_and_fu, sink_op_and_fu)) { continue; }
                        for (const auto& trip_count_and_paths : mrrg_paths.at({src_compatible_node, sink_compatible_node})) {
                            GRBLinExpr number_of_paths_used;
                            GRBLinExpr number_of_edges_used;
                            std::set<int> added_paths;

                            for (const auto& out_edge : all_out_edges) {
                                if (opgraph.targetOfEdge(out_edge) != sink_op) { continue; }
                                const auto out_edge_and_fu = EdgeAndFU{out_edge, sink_compatible_node};
                                number_of_edges_used += edge_is_used_vars.at(src_op_and_fu).at(out_edge_and_fu).at(trip_count_and_paths.first);

                                int path_num = -1;
                                for (auto& path : trip_count_and_paths.second) {
                                    path_num += 1;
                                    if (not walkIsCompatibleWithOperand(mrrg, path, opgraph.getOperandTag(out_edge))) continue;
                                    const auto path_id = MRRGPathID{src_compatible_node, sink_compatible_node, trip_count_and_paths.first, path_num};
                                    if (added_paths.insert(path_num).second) {
                                        number_of_paths_used += path_is_used_vars.at(path_id);
                                    }
                                }
                            }

                            const auto constr_name = from_src_at_to_sink_at_trip_count("edges_require_paths", opgraph, mrrg,
                                src_op, src_compatible_node, sink_op, sink_compatible_node, trip_count_and_paths.first, true);
                            auto c = model.addConstr(number_of_edges_used <= number_of_paths_used);
                            c.set(GRB_StringAttr_ConstrName, constr_name);

                            for (const auto& out_edge : all_out_edges) {
                                if (opgraph.targetOfEdge(out_edge) != sink_op) { continue; }
                                const auto out_edge_and_fu = EdgeAndFU{out_edge, sink_compatible_node};
                                const auto eu_var = edge_is_used_vars.at(src_op_and_fu).at(out_edge_and_fu).at(trip_count_and_paths.first);

                                GRBLinExpr number_of_paths_used;
                                int path_num = -1;
                                for (auto& path : trip_count_and_paths.second) {
                                    path_num += 1;
                                    if (not walkIsCompatibleWithOperand(mrrg, path, opgraph.getOperandTag(out_edge))) continue;
                                    const auto path_id = MRRGPathID{src_compatible_node, sink_compatible_node, trip_count_and_paths.first, path_num};
                                    number_of_paths_used += path_is_used_vars.at(path_id);
                                }

                                const auto constr_name = from_src_at_to_sink_at_trip_count("edge_requires_path", opgraph, mrrg,
                                    src_op, src_compatible_node, out_edge, sink_compatible_node, trip_count_and_paths.first, true);
                                auto c = model.addConstr(eu_var <= number_of_paths_used);
                                c.set(GRB_StringAttr_ConstrName, constr_name);
                            }
                        }
                    }
                }
            }
        }
        timing_seq.tick("EdgeRequiresPath constraints", 0.01);
    }
    // });

    // of the paths that connect a given FU pair and go through the same FU input, at most one may be used
    // constraints.registerGenerator(FuInputsUsedByOnePath, [&](ConstraintSet::ConstraintGeneratorParams params) {
    if (options.general.getBool("path_exclusivity_modelling")) {
        for (const auto& fu_pair_and_paths : mrrg_paths) {
            const auto sink_fu = fu_pair_and_paths.first.sink_fu;
            const auto src_fu = fu_pair_and_paths.first.src_fu;
            std::map<MRRG::NodeDescriptor, GRBLinExpr> number_of_paths_used_for_input;
            for (const auto& trip_count_and_paths : mrrg_paths.at(fu_pair_and_paths.first)) {
                int path_num = 0;
                for (auto& path : trip_count_and_paths.second) {
                    const auto path_id = MRRGPathID{
                        src_fu,
                        sink_fu,
                        trip_count_and_paths.first,
                        path_num++,
                    };
                    const auto input_node = path.at(path.size() - 2); // second last
                    number_of_paths_used_for_input[input_node] += path_is_used_vars.at(path_id);
                }
            }
            for (const auto& input_and_number_of_paths_used : number_of_paths_used_for_input) {
                const auto constr_name = from_mrrg_node_to_mrrg_node(
                    "use_one_path_per_input", opgraph, mrrg,
                    src_fu, input_and_number_of_paths_used.first, true
                );
                auto c = model.addConstr(input_and_number_of_paths_used.second <= 1);
                c.set(GRB_StringAttr_ConstrName, constr_name);
            }
        }
        timing_seq.tick("FuInputsUsedByOnePath constraints", 0.01);
    }
    // });

    // constraints.registerGenerator(PathExclusivity, [&](ConstraintSet::ConstraintGeneratorParams params) {
    if (options.general.getBool("path_exclusivity_modelling")) {
    if (options.general.getBool("path_exclusivity_constraints_are_pairwise")) {
        std::set<std::array<MRRGPathID,2>> already_constrainted_to_only_use_one_path;
        std::set<MRRGNodePair> visited_fu_pairs1;
        std::set<std::pair<MRRGNodePair, MRRGNodePair>> visited_fu_pair_pairs;

        for (const auto& src_op1 : opgraph.opNodes()) {
        for (const auto& sink_op1 : opgraph.outputOps(src_op1)) {
            for (const auto& src_compatible_node1 : compatible_fu_nodes.at(src_op1)) {
            for (const auto& sink_compatible_node1 : compatible_fu_nodes.at(sink_op1)) {
                const auto src_op_and_fu1 = OpAndFU{src_op1,  src_compatible_node1};
                const auto sink_op_and_fu1= OpAndFU{sink_op1, sink_compatible_node1};
                const auto fu_pair1 = MRRGNodePair{src_compatible_node1, sink_compatible_node1};
                if (not treatAsNeighbours(src_op_and_fu1, sink_op_and_fu1)) { continue; }
                if (not visited_fu_pairs1.emplace(fu_pair1).second) { continue; }
                for (const auto& src_op2 : opgraph.opNodes()) {
                for (const auto& sink_op2 : opgraph.outputOps(src_op2)) {
                    for (const auto& src_compatible_node2 : compatible_fu_nodes.at(src_op2)) {
                    for (const auto& sink_compatible_node2 : compatible_fu_nodes.at(sink_op2)) {
                        const auto src_op_and_fu2 = OpAndFU{src_op2, src_compatible_node2};
                        const auto sink_op_and_fu2= OpAndFU {sink_op2, sink_compatible_node2};
                        const auto fu_pair2 = MRRGNodePair{src_compatible_node2, sink_compatible_node2};
                        if (not treatAsNeighbours(src_op_and_fu2, sink_op_and_fu2)) { continue; }
                        if (not visited_fu_pair_pairs.emplace(fu_pair1, fu_pair2).second) { continue; }

                        for (const auto& trip_count_and_paths1 : mrrg_paths.at({src_compatible_node1, sink_compatible_node1})) {
                        int path_num1 = -1;
                        for (const auto& path1 : trip_count_and_paths1.second) {
                            path_num1 += 1;

                            for (const auto& trip_count_and_paths2 : mrrg_paths.at({src_compatible_node2, sink_compatible_node2})) {
                            int path_num2 = -1;
                            for (const auto& path2 : trip_count_and_paths2.second) {
                                path_num2 += 1;

                                int path1_cumulative_latency = 0;
                                for (const auto& node_in_path1 : path1) {
                                path1_cumulative_latency += mrrg.getNodeRef(node_in_path1).getLatency();
                                int path2_cumulative_latency = 0;
                                for (const auto& node_in_path2 : path2) {
                                    path2_cumulative_latency += mrrg.getNodeRef(node_in_path2).getLatency();
                                    if (node_in_path2->type != MRRG_NODE_ROUTING) { continue; }
                                    // no overlap allowed
                                    if (node_in_path1 == node_in_path2) {
                                        // unless the paths start at the same FU
                                        if (path2.front() ==  path1.front()) {
                                            // and their latency matches
                                            if (path1_cumulative_latency == path2_cumulative_latency) { continue; }
                                        }
                                    } else {
                                        continue;
                                    }

                                    decltype(already_constrainted_to_only_use_one_path)::value_type incompatible_paths {{
                                        { src_compatible_node1, sink_compatible_node1, trip_count_and_paths1.first, path_num1 },
                                        { src_compatible_node2, sink_compatible_node2, trip_count_and_paths2.first, path_num2 },
                                    }};
                                    if (
                                        not already_constrainted_to_only_use_one_path.insert(incompatible_paths).second
                                    ) {
                                        continue;
                                    }
                                    const auto cname = makeNameSolverSafe(
                                        from_mrrg_node_to_mrrg_node_trip_count_number("path_incompatibility_between", opgraph, mrrg,
                                            src_compatible_node1, sink_compatible_node1, trip_count_and_paths1.first, path_num1
                                        ) + from_mrrg_node_to_mrrg_node_trip_count_number(".and", opgraph, mrrg,
                                            src_compatible_node2, sink_compatible_node2, trip_count_and_paths2.first, path_num2
                                        ) + ".concerning_node." + mrrg.getNodeRef(node_in_path1).getFullName()
                                    );
                                    GRBLinExpr number_of_times_node_is_used;
                                    for (const auto& path_id : incompatible_paths) {
                                        number_of_times_node_is_used += path_is_used_vars.at(path_id);
                                    }
                                    auto c = model.addConstr(number_of_times_node_is_used
                                        <= mrrg.getNodeRef(node_in_path1).capacity * allowable_routing_usage_multiplier);
                                    c.set(GRB_StringAttr_ConstrName, cname);
                                    break; // done with this path
                                }
                                }
                            }
                            }
                        }
                        }
                    }
                    }
                }
                }
            }
            }
        }
        }
    } else {
        std::set<std::set<MRRGPathID>> already_constrainted_to_only_use_one_path;

        // for each edge
        for (const auto& src_op1 : opgraph.opNodes()) {
        for (const auto& sink_op1 : opgraph.outputOps(src_op1)) {

            // for each path
            for (const auto& src_compatible_node1 : compatible_fu_nodes.at(src_op1)) {
            for (const auto& sink_compatible_node1 : compatible_fu_nodes.at(sink_op1)) {
            const auto src_op_and_fu1 = OpAndFU{src_op1, src_compatible_node1};
            const auto sink_op_and_fu1= OpAndFU {sink_op1, sink_compatible_node1};
            if (not treatAsNeighbours(src_op_and_fu1, sink_op_and_fu1)) { continue; }

            for (const auto& trip_count_and_paths1 : mrrg_paths.at({src_compatible_node1, sink_compatible_node1})) {
            int path_num1 = 0;
            for (const auto& path1 : trip_count_and_paths1.second) {

                // for each node in the path
                int path1_cumulative_latency = 0;
                for (const auto& node_in_path1 : path1) {
                    path1_cumulative_latency += mrrg.getNodeRef(node_in_path1).getLatency();

                    // for each edge
                    for (const auto& src_op2 : opgraph.opNodes()) {
                    for (const auto& sink_op2 : opgraph.outputOps(src_op2)) {

                        std::set<MRRGPathID> incompatible_paths;
                        incompatible_paths.emplace(src_compatible_node1, sink_compatible_node1, trip_count_and_paths1.first, path_num1);

                        // for each path
                        for (const auto& src_compatible_node2 : compatible_fu_nodes.at(src_op2)) {
                        for (const auto& sink_compatible_node2 : compatible_fu_nodes.at(sink_op2)) {
                        const auto src_op_and_fu2 = OpAndFU{src_op2, src_compatible_node2};
                        const auto sink_op_and_fu2= OpAndFU {sink_op2, sink_compatible_node2};
                        if (not treatAsNeighbours(src_op_and_fu2, sink_op_and_fu2)) { continue; }

                        for (const auto& trip_count_and_paths2 : mrrg_paths.at({src_compatible_node2, sink_compatible_node2})) {
                        int path_num2 = 0;
                        for (const auto& path2 : trip_count_and_paths2.second) {

                            // for each node in the path
                            int path2_cumulative_latency = 0;
                            for (const auto& node_in_path2 : path2) {
                                path2_cumulative_latency += mrrg.getNodeRef(node_in_path2).getLatency();
                                if (node_in_path2->type != MRRG_NODE_ROUTING) { continue; }
                                // no overlap allowed
                                if (node_in_path1 == node_in_path2) {
                                    // unless the paths start at the same FU
                                    if (path2.front() ==  path1.front()) {
                                        // and their latency matches
                                        if (path1_cumulative_latency == path2_cumulative_latency) { continue; }
                                    }
                                } else {
                                    continue;
                                }

                                incompatible_paths.emplace(src_compatible_node2, sink_compatible_node2, trip_count_and_paths2.first, path_num2);
                                break; // done with this path
                            }

                            path_num2 += 1;
                        }
                        }

                        if (
                               incompatible_paths.size() > 1
                            && already_constrainted_to_only_use_one_path.insert(incompatible_paths).second
                        ) {
                            GRBLinExpr number_of_times_node_is_used;
                            for (const auto& path_id : incompatible_paths) {
                                number_of_times_node_is_used += path_is_used_vars.at(path_id);
                            }
                            auto c = model.addConstr(number_of_times_node_is_used
                                <= mrrg.getNodeRef(node_in_path1).capacity * allowable_routing_usage_multiplier);
                            const auto cname = makeNameSolverSafe(
                                from_mrrg_node_to_mrrg_node_trip_count_number("path_incompatibility_with", opgraph, mrrg,
                                    src_compatible_node1, sink_compatible_node1, trip_count_and_paths1.first, path_num1
                                )
                                + ".concerning_node." + mrrg.getNodeRef(node_in_path1).getFullName()
                                + ".and_src." + src_op2->getName()
                                + ".to_sink." + sink_op2->getName()
                            );
                            c.set(GRB_StringAttr_ConstrName, cname);
                        }
                        }
                        }
                    }
                    }
                }
                path_num1 += 1;
            }
            }
            }
            }
        }
        }

    }
        timing_seq.tick("PathExclusivity constraints", 0.01);
    }
    // });

    // Sum trip counts for particular DFG edges: re-convergence must be balanced, and cycles must have latency of II
    // constraint_set.registerGenerator(ConstraintGroup::LatencyBalancing, [&](ConstraintSet::ConstraintGeneratorParams params) {
    if (options.add_trip_count_balancing_constraints) {
        const auto sum_of_edge_vars_times_trip_count = [&](const auto& path) {
            GRBLinExpr result;
            for (const auto& edge : path) {
                const auto src_op = opgraph.inputOp(edge.val);
                const auto sink_op = opgraph.targetOfEdge(edge);
                for (const auto&  src_compatible_node : compatible_fu_nodes.at( src_op)) {
                for (const auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
                const auto edge_and_sink_fu = EdgeAndFU{edge, sink_compatible_node};
                const auto  src_op_and_fu = OpAndFU{ src_op,  src_compatible_node};
                const auto sink_op_and_fu = OpAndFU{sink_op, sink_compatible_node};
                if (not treatAsNeighbours(src_op_and_fu, sink_op_and_fu)) { continue; }
                    for (const auto& trip_count_and_paths : mrrg_paths.at({src_compatible_node, sink_compatible_node})) {
                        if (trip_count_and_paths.first <= 0) { continue; } // cleaner model
                        const auto& eu_var = edge_is_used_vars.at(src_op_and_fu).at(edge_and_sink_fu).at(trip_count_and_paths.first);
                        result += trip_count_and_paths.first * eu_var;
                    }
                }
                }
            }
            return result;
        };

        for(const auto & path_pair : trails_to_balance->noncyclic_trail_pairs) {
            auto sum1 = sum_of_edge_vars_times_trip_count(path_pair.first);
            auto sum2 = sum_of_edge_vars_times_trip_count(path_pair.second);
            const auto constraint_name = makeNameSolverSafe(string_from_stream([&](auto&& s) {
                s << "line_balance." << opgraph.getNodeRef(opgraph.inputOp(path_pair.first.front().val)).getName()
                    << ".to." << opgraph.getNodeRef(opgraph.targetOfEdge(path_pair.first.back())).getName();
            }));
            auto c = model.addConstr(sum1 == sum2);
            c.set(GRB_StringAttr_ConstrName, constraint_name);
        }

        for(const auto & path : trails_to_balance->cyclic_trails) {
            auto sum = sum_of_edge_vars_times_trip_count(path);
            const auto constraint_name = makeNameSolverSafe(string_from_stream([&](auto&& s) {
                s << "cycle_balance.";
                print_container(s, path, "", "", ".");
            }));
            auto c = model.addConstr(sum == 1);
            c.set(GRB_StringAttr_ConstrName, constraint_name);
        }
        timing_seq.tick("LatencyBalancing constraints", 0.01);
    }
    // });

    // Assign an integer to each FU.
    // For each time a path goes from context N to 0 ("trip_count") the target must be trip_count more than the source
    // Edges in DFG with specified cycle latencies subtract from the required trip_count.
    // constraint_set.registerGenerator(ConstraintGroup::TopologicalTripCount, [&](ConstraintSet::ConstraintGeneratorParams params) {
    if (options.add_trip_count_topological_constraints) {
        for (const auto&  src_op : opgraph.opNodes()) {
        for (const auto& out_edge : opgraph.outEdges(src_op)) {
            const auto sink_op = opgraph.targetOfEdge(out_edge);

            // for mappable FU pair
            for (const auto&  src_compatible_node : compatible_fu_nodes.at( src_op)) {
            for (const auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
            const auto edge_and_sink_fu = EdgeAndFU{out_edge, sink_compatible_node};
            const auto  src_op_and_fu = OpAndFU{ src_op,  src_compatible_node};
            const auto sink_op_and_fu = OpAndFU{sink_op, sink_compatible_node};
            if (not treatAsNeighbours(src_op_and_fu, sink_op_and_fu)) { continue; }
                for (const auto& trip_count_and_paths : mrrg_paths.at({src_compatible_node, sink_compatible_node})) {
                    const auto constraint_name = makeNameSolverSafe(from_src_at_to_sink_at_trip_count("topo_trip_count", opgraph, mrrg,
                        src_op, src_compatible_node, out_edge, sink_compatible_node, trip_count_and_paths.first));
                    model.addGenConstrIndicator(
                        edge_is_used_vars.at(src_op_and_fu).at(edge_and_sink_fu).at(trip_count_and_paths.first), true,
                           std::get<0>(*topological_trip_count_vars).at(src_compatible_node) + trip_count_and_paths.first
                        == std::get<0>(*topological_trip_count_vars).at(sink_compatible_node) + dfg_edge_latency->at(out_edge),
                        constraint_name
                    );
                }
            }
            }
        }
        }
        timing_seq.tick("TopologicalTripCount constraints", 0.01);
    }
    // });

    Mapping result = initial_mapping;
    result.clear();
    result.setStatus(MappingStatus::failure);
    int cb_count = 0;

    const auto status_print_interval = options.general.getInt("status_print_interval");
    const auto evaluate_solution = [
        // maple/Ubuntu 16.04 g++ is old & buggy. must use explicit captures to prevent 'internal compiler error'
        &cb_count, &status_print_interval, &timing_seq, &initial_mapping, &opgraph,
        &compatible_fu_nodes, &mrrg_paths, &treatAsNeighbours, &mrrg, &options, &result, &print_solve_progress
    ](auto&& is_op_using_fu, auto&& is_path_used) {
        cb_count += 1;
        if (cb_count != 1 && cb_count % status_print_interval == 0) {
            timing_seq.tick("at " + std::to_string(cb_count) +" solutions", 0.0, print_solve_progress ? &std::cout : nullptr);
        }

        Mapping this_mapping = initial_mapping;
        this_mapping.clear();

        for (auto& op : opgraph.opNodes()) {
            for (auto& compatible_node : compatible_fu_nodes.at(op)) {
                const OpAndFU op_and_fu {op, compatible_node};
                const auto& is_used_var_value = is_op_using_fu(op_and_fu);
                if (is_used_var_value) {
                    this_mapping.mapMRRGNode(op, compatible_node);
                }
            }

            const auto& connecting_val = opgraph.outputVal(op);
            for (auto& src_compatible_node : compatible_fu_nodes.at(op)) {
                std::vector<MrrgNodeSpan> all_paths;
                for (auto& sink_op : opgraph.outputOps(op)) {
                    for (auto& sink_compatible_node : compatible_fu_nodes.at(sink_op)) {
                        const auto mrrg_paths_search = mrrg_paths.find({src_compatible_node, sink_compatible_node});
                        if (mrrg_paths_search == mrrg_paths.end()) { continue; }
                        for (const auto& trip_count_and_paths : mrrg_paths_search->second) {
                        int path_num = 0;
                        for (const auto& path : trip_count_and_paths.second) {
                            if (
                                   is_path_used( MRRGPathID{src_compatible_node, sink_compatible_node, trip_count_and_paths.first, path_num} )
                                && is_op_using_fu({op, src_compatible_node})
                                && is_op_using_fu({sink_op, sink_compatible_node})
                            ) {
                                all_paths.push_back({path.data(), path.size() - 1}); // leave-off the sink, so merging multi-edges works
                            }
                            path_num += 1;
                        }
                        }
                    }
                }
                const auto merged_paths = mergeMRRGWalks(mrrg, all_paths);
                std::set<MRRG::NodeDescriptor> already_added;
                for (auto& final_path : merged_paths) {
                    for (auto& node_in_path : final_path) {
                        if (not already_added.insert(node_in_path).second) { continue; }
                        if (node_in_path->type != MRRG_NODE_ROUTING) { continue; }
                        this_mapping.mapMRRGNode(connecting_val, node_in_path);
                    }
                }
            }
        }

        if (options.print_intermediate_mappings) {
            Mapping mapping_to_print = this_mapping;
            if (options.general.getBool("print_intermediate_mappings_placement_only")) {
                mapping_to_print.clear();
                for (auto& op : opgraph.opNodes()) {
                    for (auto& mapping : this_mapping.getMappingList(op)) {
                        mapping_to_print.mapMRRGNode(op, mapping);
                    }
                }
            }
            mapping_to_print.outputMapping(opgraph, std::cout);
        }

        auto suggested_mapping = options.solution_selector(this_mapping);

        this_mapping.setStatus(MappingStatus::success);
        result = suggested_mapping;

        return suggested_mapping.isMapped();
    };

    timing_seq.tick("solve setup", 0.0, print_solve_progress ? &std::cout : nullptr);

    if (false) {
        // do nothing
    } else if (options.use_solver_gurobi) {
        auto solution_selector_callback = makeOnMIPSolutionFunctorGRBCallback([&](auto& cb_obj) {
            {const auto& solution_dump_filename_base = options.general.getString("intermediate_solution_dump_filename_base");
            if (not solution_dump_filename_base.empty()) {
                const auto solution_dump_filename = solution_dump_filename_base + std::to_string(cb_count) + ".sol.txt";
                std::cout << "[INFO] " << solve_approach_name << " writing intermediate solution to " << solution_dump_filename << std::endl;

                std::ofstream sol(solution_dump_filename);
                const auto print_name_and_value = [&](const auto& var) {
                    sol << var.get(GRB_StringAttr_VarName) << " = " << std::llround(cb_obj.getSolution(var)) << '\n';
                };

                for (const auto& v1 : op_is_mapped_to_vars) {print_name_and_value(v1.second); }
                for (const auto& v1 : edge_is_used_vars) {
                    for (const auto& v2 : v1.second) {
                        for (const auto& v3 : v2.second) { print_name_and_value(v3.second); }
                    }
                }
                for (const auto& v1 : path_is_used_vars) { print_name_and_value(v1.second); }

                std::cout << "[INFO] " << solve_approach_name << " done writing intermediate solution to " << solution_dump_filename << std::endl;
            }}
            if (evaluate_solution([&](OpAndFU op_and_fu) -> bool {
                return std::llround(cb_obj.getSolution(op_is_mapped_to_vars.at(op_and_fu)));
            }, [&](MRRGPathID path_id) -> bool {
                return options.general.getBool("path_exclusivity_modelling")
                    && std::llround(cb_obj.getSolution(path_is_used_vars.at(path_id)));
            })) {
                cb_obj.abort(); // safely stop further searching
            }
        });
        model.setCallback(&solution_selector_callback);

        model.set(GRB_IntParam_Seed, solver_seed);
        model.set(GRB_IntParam_Threads, max_threads);
        model.set(GRB_IntParam_PoolSearchMode, 2); // finds up to ''PoolSolutions'' best solutions
        model.set(GRB_IntParam_PoolSolutions, options.general.getInt("max_solutions"));

        if (not model_dump_filename.empty()) {
            const auto dot_pos = model_dump_filename.find_last_of('.');
            const auto extension = dot_pos == model_dump_filename.npos ? ".lp" : model_dump_filename.substr(dot_pos);
            const auto model_dump_filename_with_extension = model_dump_filename.substr(0, dot_pos) + '.' + solve_approach_name + extension;
            std::cout << "[INFO] " << solve_approach_name << " writing model to " << model_dump_filename_with_extension << std::endl;
            model.write(model_dump_filename_with_extension);
            std::cout << "[INFO] " << solve_approach_name << " done writing model to " << model_dump_filename_with_extension << std::endl;
        }

        model.optimize();

    } else if (options.use_solver_gor_classic_cp) {
        #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
        std::vector<gor::IntVar*> allvars;
        for (const auto& name_and_var : gorCP_op_is_mapped_to_vars) {
            allvars.push_back(name_and_var.second);
        }
        // sort allvars - start at output node FUs, have all edge vars last.

        gor::DecisionBuilder* const db = gorCP_solver.MakePhase(
            allvars,
            gor::Solver::CHOOSE_FIRST_UNBOUND,
            gor::Solver::ASSIGN_MAX_VALUE
        );

        gorCP_solver.NewSearch(db);

        // gorCP_solver.Accept(gorCP_solver.MakePrintModelVisitor());
        while (gorCP_solver.NextSolution()) {
            if (evaluate_solution([&](OpAndFU op_and_fu) -> bool {
                return gorCP_op_is_mapped_to_vars.at(op_and_fu)->Value();
            },
                std::function<bool(MRRGPathID)>{ // placeholder
            })) {
                break;
            }
        }
        gorCP_solver.EndSearch();
        #else
        throw cgrame_mapper_error("Must be built to support Google Or-Tools based solvers");
        #endif

    } else if (options.use_solver_gor_sat_cp) {
        #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
        gorsat::Model model;

        gorsat::SatParameters parameters;
        parameters.set_enumerate_all_solutions(true);
        model.Add(NewSatParameters(parameters));

        // DecisionStrategyProto* const strategy = gorSatCP_model.add_search_strategy();
        // std::ofstream("gorSatCP-ops-model.txt") << gorSatCP_model.DebugString();

        std::atomic<bool> async_request_stop_solving(false);
        model.GetOrCreate<gor::TimeLimit>()->RegisterExternalBooleanAsLimit(&async_request_stop_solving);

        model.Add(gorsat::NewFeasibleSolutionObserver([&](const gorsat::CpSolverResponse& r) {
            if (evaluate_solution([&](OpAndFU op_and_fu) -> bool {
                return r.solution(gorSatCP_op_is_mapped_to_vars.at(op_and_fu).index);
            },
                std::function<bool(MRRGPathID)>{ // placeholder
            })) {
                async_request_stop_solving = true;
            }
        }));

        const gorsat::CpSolverResponse& response = SolveCpModel(gorSatCP_model, &model);
        (void)response;
        #else
        throw cgrame_mapper_error("Must be built to support Google Or-Tools based solvers");
        #endif

    } else {
        throw cgrame_mapper_error("Solver not available; Solver ID = " + options.solver_id);
    }

    timing_seq.tick("solve");

    const auto num_solutions = [&]() -> long long int {
        if (false) {
            // do nothing
        } else if (options.use_solver_gurobi) {
            return model.get(GRB_IntAttr_SolCount);
        #ifdef CGRAME_USE_GOOGLE_OR_TOOLS
        } else if (options.use_solver_gor_classic_cp) {
            return gorCP_solver.solutions();
        } else if (options.use_solver_gor_sat_cp) {
            return cb_count; // no way to check this?
        #endif
        } else {
            throw cgrame_mapper_error("Solver not available; Solver ID = " + options.solver_id);
        }
    }();
    solve_statistics.addInt("number of solutions evaluated", num_solutions);

    if (false) {
        // do nothing
    } else if (options.use_solver_gurobi) {
        const auto grb_status = model.get(GRB_IntAttr_Status);
        if (grb_status == GRB_INFEASIBLE) {
            if (not model_IIS_dump_filename.empty()) {
                const auto dot_pos = model_IIS_dump_filename.find_last_of('.');
                const auto extension = dot_pos == model_IIS_dump_filename.npos ? ".ilp" : model_IIS_dump_filename.substr(dot_pos);
                const auto iis_filename_with_extension = model_IIS_dump_filename.substr(0, dot_pos) + '.' + solve_approach_name + extension;
                std::cout << "[INFO] " << solve_approach_name << " computing IIS, will dump to " << iis_filename_with_extension << std::endl;
                model.computeIIS();
                std::cout << "[INFO] " << solve_approach_name << " writing IIS to " << iis_filename_with_extension << std::endl;
                model.write(iis_filename_with_extension);
                std::cout << "[INFO] " << solve_approach_name << " done writing IIS to " << iis_filename_with_extension << std::endl;
            }
        } else if (grb_status == GRB_TIME_LIMIT && num_solutions == 0) {
            result.setStatus(MappingStatus::timeout);
        }
    }

    if (options.print_mapping) {
        std::cout << "Mapping for " << solve_approach_name << ":\n";
        result.outputMapping(opgraph, std::cout);
        std::cout << '\n';
    }

    if (do_print_configuration_and_statistics) {
        std::cout << "Solve Statistics = " << solve_statistics << '\n';
    }

    return result;
} catch (...) {
    throwExceptionButWrapIfGurobiException(std::current_exception());
}

Mapping mapOpsJustByConnectivity(std::unordered_map<std::string, std::string> fix_port,
    const OpGraph& opgraph, const MRRG& mrrg,
    ILPHeuristicMapperOptions options,
    ILPHeuristicMapperCaches caches,
    Mapping initial_mapping
) {
    options.general.addString("solve_approach_name", "mapOpsJustByConnectivity");
    options.general.addBool("path_exclusivity_modelling", false); // Unless it's requested, disable
    return mapViaConnectivityAndPathChoosing(fix_port, opgraph, mrrg, initial_mapping, std::move(options), std::move(caches));
}

Mapping routeOpMappingByChoosingPaths(
    std::unordered_map<std::string, std::string> fix_port,
    const Mapping& op_mapping,
    const OpGraph& opgraph, const MRRG& mrrg,
    RouteOpMappingByChoosingPathsOptions options,
    ILPHeuristicMapperCaches caches
) {
    options.general.addString("solve_approach_name", "routeOpMappingByChoosingPaths");
    options.general.addBool("path_exclusivity_modelling", true); // Unless it's requested, enable
    options.general.addBool("map_unmapped_ops", false);
    return mapViaConnectivityAndPathChoosing(fix_port, opgraph, mrrg, op_mapping, std::move(options), std::move(caches));
}

static const std::map<std::string,FUCoster> fu_costers {
    {"", {}},
    {"one", [](
        const OpGraph& opgraph, const MRRG& mrrg,
        const OpGraph::OpDescriptor& op,  const MRRG::NodeDescriptor& node
    ) {
        (void)opgraph; (void)mrrg; (void)op; (void)node;
        return 1.0;
    }},
};

AutoRegisterMapper ILPHeuristicMapper_arm("ILPHeuristicMapper",
    [](std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args) {
        return std::make_unique<ILPHeuristicMapper>(cgra, timelimit, args);
    },
    false, // not a composite
    "Iteratively considers more flexibility.\n"
    "Initially presented in 'Generic Connectivity-Based CGRA Mapping via Integer Linear Programming' (FCCM 2019)",
    { // required args
        {"allow_recomputation", false, "Allow ops to map to more than one FU"},
        {"allow_unbalanced_latency", false, "Assume a dataflow-like architecture"},
        {"fu_coster", "", "Specify one of the available cost functions on functional units.\n"
            "Empty string for none. Other options include: `one': each FU costs 1"},
        {"do_cache_warmup", true, "Low-overhead option that gives a better estimate of what mapping time would be like if arch-specific data was saved to disk"},
        {"nneigh_start", "", "The number-of-neighbours to start solving at. Default depends on arch_id"},
        {"nneigh_stop", "", "The last number-of-neighbours to try. Default depends on arch_id"},
        {"nneigh_step", 2, "The amount number-of-neighbours is increased between attempts"},
        {"do_test_placement", true, "At each number-of-neighbours, try solving without modelling routing first"},
        {"do_final_combined_attempt", false, "At each number-of-neighbours, if unable to route any placements, try placing & routing at the same time"},
        {"max_threads", 1, ""},
        {"model_dump_filename", "", ""},
        {"model_IIS_dump_filename", "", ""},
        {"seed", 0, ""},
        {"verbosity", 0, ""},
        {"arch_id", "", ""},
    },
    { // optional arg regexes
        {"hm_test_placement_.*", "Pass options directly to test_placement"},
        {"hm_outer_.*", "Pass options directly to outer"},
        {"hm_inner_.*", "Pass options directly to inner"},
        {"hm_combined_.*", "Pass options directly to final combined P+R attempt"},
    }
);

ILPHeuristicMapper::ILPHeuristicMapper(std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args)
    : Mapper(cgra, timelimit)
    , general_options{args}
    , test_placement_options {
        {"solve_approach_name", "TestPlacement"},
        {"path_exclusivity_modelling", false},
        {"allow_unbalanced_latency", true}, // (not overridden by 'common option' code)
    }
    , outer_options {
        {"solve_approach_name", "OuterSolve"},
        {"path_exclusivity_modelling", "on"},
        {"max_num_paths_between_FUs", "5"},
        {"allowable_routing_usage_multiplier", "2"},
        {"allow_unbalanced_latency", true}, // (not overridden by 'common option' code)
        {"print_intermediate_mappings",             general_options.getInt("verbosity") > 0},
        {"print_intermediate_mappings_placement_only", "yes"},
        {"max_solutions", "100"},
    }
    , inner_options {
        {"solve_approach_name", "InnerSolve"},
    }
    , final_combined_attempt_options {
        {"solve_approach_name", "FinalCombinedAttempt"},
        {"path_exclusivity_modelling", "on"},
        {"max_num_paths_between_FUs", "20"},
        {"timelimit", "60"},
    }
{
    for (auto& prefix_and_cs : {
        std::make_pair("test_placement", std::ref(test_placement_options)),
        std::make_pair("outer", std::ref(outer_options)),
        std::make_pair("inner", std::ref(inner_options)),
        std::make_pair("combined", std::ref(final_combined_attempt_options)),
    }) {
        const auto& prefix = std::string("hm_") + prefix_and_cs.first + '_';
        auto& cs = prefix_and_cs.second;

        // common options. Use 'add' to not override any existing settings
        cs.addBool("allow_unbalanced_latency", args.getBool("allow_unbalanced_latency"));
        cs.addBool("print_configuration_and_statistics", args.getInt("verbosity") > 0);
        cs.addBool("allow_multiple_op_mappings", args.getBool("allow_recomputation"));
        cs.addBool("print_solve_progress", args.getInt("verbosity") > 0);
        cs.addBool("silence_warnings", args.getInt("verbosity") <= 0);
        cs.addInt("seed", args.getInt("seed"));
        cs.addInt("max_threads", args.getInt("max_threads"));
        cs.addString("model_dump_filename", args.getString("model_dump_filename"));
        cs.addString("model_IIS_dump_filename", args.getString("model_IIS_dump_filename"));

        // options passed in by the user
        for (auto& args_kv : args) {
            const auto mismatch_results = std::mismatch(prefix.begin(), prefix.end(), args_kv.first.begin(), args_kv.first.end());
            if (mismatch_results.first != prefix.end()) { // skip if not same prefix
                continue;
            }
            const auto param_name = std::string{mismatch_results.second, args_kv.first.end()}; // with the prefix removed
            // get our overridden value, or the global default. Also, checks if key is spelled correctly
            const auto& default_value = cs.getStringOr(param_name,
                ILPHeuristicMapperOptions::default_general_opts().getString(param_name)
            );
            if (args_kv.second != default_value && general_options.getInt("verbosity") > 0) {
                std::cout << "[INFO] " << cs.getStringOr("solve_approach_name", prefix) << "'s "
                    << param_name << " overridden from " << default_value << " to " << args_kv.second << std::endl;
            }
            cs.setString(param_name, args_kv.second);
        }
    }

    const auto& arch_id = args.getString("arch_id");
    const auto& arch_id_defaults = [&]() -> ConfigStore {
        if (false) { return {};
        } else if (arch_id == "adres" or arch_id == "2") {
            // ideal sched: 12, 14, 18, 21;
            // cusp at: 17
            return {
                {"nneigh_start", "12"},
                {"nneigh_stop",  "21"},
            };
        } else if (arch_id == "hycube" or arch_id == "3") {
            // ideal sched: 7, 9, 11, 16
            return {
                {"nneigh_start",  "7"},
                {"nneigh_stop",  "17"},
            };
        } else if (arch_id == "clustered" or arch_id == "1") {
            return {
                {"nneigh_start",  "8"},
                {"nneigh_stop",  "16"},
            };
        } else {
            if (args.getInt("verbosity") > 0) {
                std::cout << "Warning: uncharacterised architecture `" << arch_id << "'. Will use basic NN schedule\n";
            }
            return {
                {"nneigh_start",  "4"},
                {"nneigh_stop",  "24"},
            };
        }
    }();

    // set the per-arch options, but only if they're not user-specified (ie. left as empty strings)
    for (const auto& kv : arch_id_defaults) {
        if (general_options.getString(kv.first).empty()) {
            general_options.setString(kv.first, kv.second);
        }
    }
}

Mapping ILPHeuristicMapper::real_mapOpGraph(std::shared_ptr<OpGraph> opgraph_storage, int II, const MRRG& mrrg, std::unordered_map<std::string, std::string> fix_port) const {
    const auto& opgraph = *opgraph_storage;
    auto mrrg_cache_handle = std::make_unique<MRRGProcedureCacheHandle>();
    auto ilphm_cache = ILPHeuristicMapperCaches{ mrrg_cache_handle.get() };

    const auto mapping_algo = [&](bool cache_warmup) {
        const auto now = []{ return std::chrono::steady_clock::now(); };
        const auto start_time = now();
        const auto time_remaining = [&] {
            return std::max<int>(0, timelimit - std::chrono::duration_cast<std::chrono::seconds>(now() - start_time).count());
        };

        const auto computed_global_options = [&] {
            ConfigStore result;
            result.addInt("timelimit", std::max(1,time_remaining()));
            result.addBool("just_warm_up_caches", cache_warmup);
            return result;
        };

        const auto flow_name = std::string("ILP heuristic mapping") + (cache_warmup ? " cache warmup" : "");
        if (general_options.getInt("verbosity") > 0) {
            std::cout << "[INFO] Starting flow `" << flow_name << "'\n";
        }

        auto ostream_ptr = cache_warmup || general_options.getInt("verbosity") <= 0 ? nullptr : &std::cout;
        PrintOnDestructionChronoSequence timing_seq(flow_name, ostream_ptr);

        Mapping mapping(cgra, mrrg.initiationInterval(), opgraph_storage);

        auto nneigh_start = cache_warmup ? general_options.getInt("nneigh_stop") : general_options.getInt("nneigh_start"); // don't waste time
        const auto nneigh_step = general_options.getInt("nneigh_step");
        auto nneigh = nneigh_start - nneigh_step;
        while (nneigh < general_options.getInt("nneigh_stop") && not mapping.isMapped()) {
            nneigh += nneigh_step;

            // maybe try once with no path exclusivity
            if (general_options.getBool("do_test_placement")) {
                if (time_remaining() == 0) { mapping.clear(); break; }
                const auto test_placement = mapOpsJustByConnectivity(fix_port, opgraph, mrrg,
                    MapOpsJustByConnectivityOptions{
                        makeNClosestNeighbourFinder(nneigh),
                        acceptTheFirstSolution,
                        noEdgeCosting,
                        with_set(test_placement_options, computed_global_options()),
                    },
                    ilphm_cache, mapping
                );
                timing_seq.tick("test placement for NN = " + std::to_string(nneigh), 0.0, ostream_ptr);
                if (not test_placement.isMapped()) {
                    continue;
                }
            }

            // then try with the full options
            const auto inner_mapping_func = [&](auto& op_mapping) {
                if (time_remaining() == 0) { auto cpy = op_mapping; cpy.clear(); return cpy; }
                return routeOpMappingByChoosingPaths(fix_port, op_mapping, opgraph, mrrg,
                    RouteOpMappingByChoosingPathsOptions{
                        {},
                        acceptTheFirstSolution,
                        {},
                        with_set(inner_options, computed_global_options()),
                    },
                    ilphm_cache
                );
            };

            ConfigStore outer_option_overrides;
            if (cache_warmup) {
                outer_option_overrides.setInt("max_num_paths_between_FUs", 20); // enough for both phases
            }

            if (time_remaining() == 0) { mapping.clear(); break; }
            mapping = mapOpsJustByConnectivity(fix_port, opgraph, mrrg,
                MapOpsJustByConnectivityOptions{
                    makeNClosestNeighbourFinder(nneigh),
                    inner_mapping_func,
                    noEdgeCosting,
                    with_set(outer_options, outer_option_overrides, computed_global_options()),
                    allowAllNodes,
                    allowAllNodesForOps,
                    fu_costers.at(general_options.getString("fu_coster")),
                }, ilphm_cache, mapping
            );
            timing_seq.tick("tried NN = " + std::to_string(nneigh), 0.0, ostream_ptr);

            // and maybe try one final combined attempt
            if (not mapping.isMapped() && general_options.getBool("do_final_combined_attempt")) {
                mapping.clear();
                if (time_remaining() == 0) { mapping.clear(); break; }
                mapping = mapViaConnectivityAndPathChoosing(fix_port, opgraph, mrrg, mapping,
                    ILPHeuristicMapperOptions{
                        makeNClosestNeighbourFinder(nneigh), acceptTheFirstSolution,
                        noEdgeCosting, final_combined_attempt_options,
                    }, ilphm_cache
                );
                timing_seq.tick("FCA for NN = " + std::to_string(nneigh), 0.0, ostream_ptr);
            };
        }

        mapping.solve_time_in_seconds = timing_seq.secondsSinceStart();

        return mapping;
    };

    if (general_options.getBool("do_cache_warmup")) {
        mapping_algo(true);
    }
    return mapping_algo(false);
}
