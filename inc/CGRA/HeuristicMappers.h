/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef ___HEURISTICMAPPERS_H__
#define ___HEURISTICMAPPERS_H__

#include <CGRA/ConfigStore.h>
#include <CGRA/Mapper.h>
#include <CGRA/Mapping.h>
#include <CGRA/MRRG.h>
#include <CGRA/MRRGProcedureCache.h>
#include <CGRA/OpGraph.h>

#include <functional>
#include <iosfwd>

class FunctionalUnitNeighbours;

using NeighbourFinder = std::function<FunctionalUnitNeighbours(const MRRG&, const std::vector<MRRG::NodeDescriptor>&)>;

using SolutionSelector = std::function<Mapping(const Mapping&)>;

using EdgeCoster = std::function<double(
	const FunctionalUnitNeighbours& fu_neighbours,
    const OpGraphOp& src_op,  const MRRG::NodeDescriptor& src_node,
    const OpGraphOp& sink_op, const MRRG::NodeDescriptor& sink_node
)>;

using FUCoster = std::function<double(
	const OpGraph& opgraph, const MRRG& mrrg,
    const OpGraph::OpDescriptor& op,  const MRRG::NodeDescriptor& node
)>;

const SolutionSelector acceptTheFirstSolution = [](const Mapping& m){ Mapping copy = m; copy.setStatus(MappingStatus::success); return copy; };
const EdgeCoster noEdgeCosting = {};

using NodeFilter = std::function<bool(
	const MRRG::NodeDescriptor& node
)>;

extern const NodeFilter allowAllNodes;

using OpNodeFilter = std::function<bool(
	const OpGraph::OpDescriptor& op,
	const MRRG::NodeDescriptor& node
)>;

extern const OpNodeFilter allowAllNodesForOps;

struct ILPHeuristicMapperOptions {
	ILPHeuristicMapperOptions() = delete;

	NeighbourFinder find_all_neighbour_FUs;
	SolutionSelector solution_selector;
	EdgeCoster edge_coster;
	ConfigStore general = {};
	NodeFilter is_node_allowed = allowAllNodes;
	OpNodeFilter is_node_allowed_for_op = allowAllNodesForOps;
	FUCoster fu_coster = {};

	struct Solvers {
		struct SolverID { const char* const id; };

		static SolverID Gurobi;
		static SolverID GoogleORTools_ClassicCP;
		static SolverID GoogleORTools_SatCP;
	};

	/**
	 * Sets everything that is unset, but needs to be set, to a default value.
	 */
	static ILPHeuristicMapperOptions& add_defaults(ILPHeuristicMapperOptions& options) {
		add_all(options.general, default_general_opts());
		add_all(options.general, {
		});
		return options;
	}

	static const ConfigStore& default_general_opts() {
		static const ConfigStore general_defaults {{
			{"solver", Solvers::Gurobi.id},
			// {"solver", Solvers::GoogleORTools_ClassicCP.id},
			// {"solver", Solvers::GoogleORTools_SatCP.id},
			{"seed", 0},
			{"max_threads", 1},
			{"allow_multiple_op_mappings", false}, // allow mapping of the same op to different FUs, ie. re-computation
			{"allow_unbalanced_latency", false}, // latency matching of re-convergent and cyclic paths in the DFG
			{"add_self_as_neighbour_if_path", true}, // make sure all FUs support self-loops
			{"path_exclusivity_modelling", true}, // "Turns on the routing part" -- if false, will assume unlimited overlap is allowed by not modelling it
			{"path_exclusivity_constraints_are_pairwise", true}, // more flexible, but way more constraints
			{"max_num_paths_between_FUs", 20}, // maximum number of paths to consider between connected FU nodes -- main scaling parameter for routing/path modelling variables & constraints
			{"map_unmapped_ops", true}, // if false, will only consider ops that are already present in the initial mapping parameter
			{"solve_approach_name", "ILP-Heuristic-Mapper"}, // printed out in some messages, notably the profiling ones
			{"status_print_interval", 100}, // print a message every this many solutions found
			{"allowable_routing_usage_multiplier", 1}, // number of paths that can use the same routing node.
			{"operand_nodes_are_normal", false}, // ignore operand sets on MRRG nodes; everything can route everywhere

			{"max_solutions", "2000000000"}, // \approx\inf
			{"timelimit", 0}, // in seconds. 0 means no limit.

			{"print_configuration_and_statistics", false}, // a single initial print at the beginning
			{"print_initial_mapping", false}, // the argument
			{"print_intermediate_mappings", false}, // each mapping sent to the selector
			{"print_intermediate_mappings_placement_only", false}, // same as above, but don't print value mappings
			{"print_mapping", false}, // the final result
			{"print_solve_progress", false}, // control prints from the ILP solver
			{"print_compatible_nodes", false}, // print compatibility mapping from OpGraph node to a list of MRRG nodes
			{"print_neighbours", false}, // print determined FU neighbours for each FU node
			{"print_paths", false}, // print paths between FUs
			{"silence_warnings", false}, // warnings almost certainly imply there is no solution
			{"model_dump_filename", ""}, // non-empty will cause dump
			{"model_IIS_dump_filename", ""}, // non-empty will cause computation & dump
			{"intermediate_solution_dump_filename_base", ""}, // non-empty will cause dump of every solution found to a unique file

			{"just_warm_up_caches", false}, // warm-up caches, but don't solve
		}};
		return general_defaults;
	}

	friend std::ostream& operator<<(std::ostream& os, const ILPHeuristicMapperOptions& opts);
};

struct ILPHeuristicMapperCaches {
	MRRGProcedureCacheHandle* mrrg_proc_cache = nullptr;
};

using MapOpsJustByConnectivityOptions = ILPHeuristicMapperOptions;

Mapping mapViaConnectivityAndPathChoosing(
	std::unordered_map<std::string, std::string> fix_port,
    const OpGraph& opgraph, const MRRG& mrrg,
    const Mapping& initial_mapping,
    ILPHeuristicMapperOptions options,
    ILPHeuristicMapperCaches caches = {}
);

/**
 * Finds compatible FU nodes in `mrrg` for the ops in `opgraph`, and uses
 * just the connectivity of these FUs to come up with a op mapping.
 */
Mapping mapOpsJustByConnectivity(std::unordered_map <std::string, std::string> fix_port, const OpGraph& opgraph, const MRRG& mrrg,
    ILPHeuristicMapperOptions options,
    ILPHeuristicMapperCaches caches = {},
    Mapping initial_mapping = Mapping{std::make_shared<CGRA>(), -1, std::make_shared<OpGraph>()}
);

using RouteOpMappingByChoosingPathsOptions = ILPHeuristicMapperOptions;
/**
 * Queries `op_mapping` for the FUs that the ops in `opgraph` are mapped to,
 * finds candidate paths between those FUs, and tries to find a legal set of paths.
 */
Mapping routeOpMappingByChoosingPaths(
	std::unordered_map<std::string, std::string> fix_port,
	const Mapping& op_mapping,
	const OpGraph& opgraph, const MRRG& mrrg,
	RouteOpMappingByChoosingPathsOptions options = { {}, acceptTheFirstSolution, {} },
    ILPHeuristicMapperCaches caches = {}
);

class ILPHeuristicMapper : public Mapper {
    public:
        ILPHeuristicMapper(std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args);
        Mapping mapOpGraph(std::shared_ptr<OpGraph> opgraph, int II, const MRRG& mrrg, std::unordered_map<std::string, std::string>fix_port) override 
			{ return real_mapOpGraph(opgraph, II, mrrg, fix_port); }
    private:
        Mapping real_mapOpGraph(std::shared_ptr<OpGraph> opgraph, int II, const MRRG& mrrg, std::unordered_map<std::string, std::string>fix_port) const;
        ConfigStore general_options;
        ConfigStore test_placement_options;
        ConfigStore outer_options;
        ConfigStore inner_options;
        ConfigStore final_combined_attempt_options;
};

#endif /* ___HEURISTICMAPPERS_H__ */
