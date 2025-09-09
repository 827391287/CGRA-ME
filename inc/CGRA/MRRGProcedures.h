/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

/**
 * A file for functions and procedures that operate on a MRRG.
 * Intended to be only included in source files, if possible.
 */

#ifndef MRRG_PROCEDURES__H_
#define MRRG_PROCEDURES__H_

#include <CGRA/MRRG.h>
#include <CGRA/MRRGProcedureCache.h>
#include <CGRA/utility/Collections.h>
#include <CGRA/impl/GraphAlgorithmHelpers.h>
#include <CGRA/Mapping.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>

/*******************
 * Begin forward declarations and simple/small types
 *******************/

struct InterMRRGMap;
struct MRRGTransformationResult;
struct FindNeighbourFUsVisitor;
struct NodeExclusions;

struct MRRGNodeClassLists {
    std::vector<MRRG::NodeDescriptor> function_nodes = {};
    std::vector<MRRG::NodeDescriptor> routing_nodes = {};
};

struct MRRGNodeClassListsByCycle {
    std::vector<std::vector<MRRG::NodeDescriptor>> function_nodes = {};
    std::vector<std::vector<MRRG::NodeDescriptor>> routing_nodes = {};
};

struct FunctionalUnitNeighbours {
    struct NodeInfo {
        MRRG::NodeDescriptor node;
        int distance;

        operator MRRG::NodeDescriptor() const { return node; }
        bool operator==(const MRRG::NodeDescriptor& ndesc) const { return node == ndesc; }
    };
    using NodeInfoList = std::unordered_map<MRRG::NodeDescriptor, NodeInfo>;

    bool isReachableFrom(MRRG::NodeDescriptor src, MRRG::NodeDescriptor dest) const;
    const NodeInfo& getInfoFor(MRRG::NodeDescriptor src, MRRG::NodeDescriptor dest) const { return neighbours.at(src).at(dest); }
    friend std::ostream& operator<<(std::ostream& os, const FunctionalUnitNeighbours& fu_neighbours);

    std::unordered_map<MRRG::NodeDescriptor, NodeInfoList> neighbours = {};

    auto begin() const { return neighbours.begin(); }
    auto end() const { return neighbours.end(); }
    auto begin()       { return neighbours.begin(); }
    auto end()         { return neighbours.end(); }
};

struct MRRGTransformFlags {
};

/*******************
 * Begin function prototypes
 *******************/

/**
 * Finds certain useful node classes, puts them in lists
 */
MRRGNodeClassLists computeNodeClassLists(const MRRG& mrrg);

/**
 * Finds certain useful node classes, puts them in lists
 */
MRRGNodeClassListsByCycle computeNodeClassListsByCycle(const MRRG& mrrg);

/**
 * Examines the nodes in `mrrg` to find compatible nodes for each op in `opgraph`
 */
std::unordered_map<OpGraph::OpDescriptor, std::vector<MRRG::NodeDescriptor>> findAllCompatibleFUs(const OpGraph& opgraph, const MRRG& base_mrrg);

/**
 * Finds functional units that are reachable from the given node `source`. An breath-first fanout
 * search of `mrrg` is performed (starting at the fanout of `source`) that does not pass through any FU nodes.
 * Give a non-negative number for `min_neighbours_to_find` to limit number of FUs found -
 * after finding at least this many FUs, the search will stop. This is a minimum because
 * this function will return all FUs in `mrrg` that are at most as far as the farthest FU returned.
 * Distance is measured as the number of vertices on the shortest path starting from `source`
 */
FunctionalUnitNeighbours::NodeInfoList findNeighbourFUs(const MRRG& mrrg, MRRG::NodeDescriptor source, int min_neighbours_to_find);

template<typename NodeList = std::vector<MRRG::NodeDescriptor>, typename NEIGHBOUR_FINDER = FunctionalUnitNeighbours::NodeInfoList(const MRRG&, MRRG::NodeDescriptor,int)>
FunctionalUnitNeighbours findAllNeighbourFUs(const MRRG& mrrg, const NodeList& sources, int min_neighbours_to_find = -1, NEIGHBOUR_FINDER&& neighbour_finder = findNeighbourFUs) {
    FunctionalUnitNeighbours result;
    for(auto& n : sources) result.neighbours.emplace(n, std::forward<NEIGHBOUR_FINDER>(neighbour_finder)(mrrg, n, min_neighbours_to_find));
    return result;
}

inline auto makeNClosestNeighbourFinder(int min_neighbours) {
    return [=](const MRRG& mrrg, const std::vector<MRRG::NodeDescriptor>& all_compatible_nodes_list) -> FunctionalUnitNeighbours {
        auto result = findAllNeighbourFUs(mrrg, all_compatible_nodes_list, min_neighbours);
        // std::cout << result << std::endl;
        return result;
    };
}

/**
 * Search @p mrrg for up to N paths between @p source and @p sink
 *  that do not include any FU nodes other than @p source or @p sink,
 *  and are compatible with @p operand_tag .
 * A negative number will result in all paths being found.
 * `sink == source` is allowed, and it will return only paths that include a fanout of `source`,
 *    ie. the length of the paths will always be at least 2.
 * Also, for `sink == source`, paths returned will have a trip count
 *    in the closed interval @p cycle_trip_count_min_max .
 * All returned paths will satisfy `.front() == source` and `.back() == sink`,
 *    and be at least 2 nodes.
 */
std::vector<std::vector<MRRG::NodeDescriptor>> findNRoutingPathsBetween(
    int num_paths, const MRRG& mrrg, MRRG::NodeDescriptor source, MRRG::NodeDescriptor sink,
    std::pair<int,int> cycle_trip_count_min_max, const OperandTag& operand_tag,
    MRRGProcedureCacheHandle* cache_handle
);

/**
 * Automatic merging of paths through an MRRG. Usual use case is resolving the results of
 *   routing the destinations of a val independently.
 * Note that this function ignores latency, so, for the merged result to be functionally
 *   equivalent to the input, all re-convergences should have matching iteration latency.
 *
 * Takes a list of edge sequences ("walks", as a lists of vertices), and from the superset
 *   of all edges in the walks,produces a list of edge lists (as lists of of vertices) that
 *   do not overlap, other than at their terminal nodes (ie. all edges are unique).
 * Additionally, if the returned edge lists are merged, they form a polytree -- a directed
 *   acyclic graph that if converted to an undirected graph, would be a tree (or forest).
 * That is, in addition to there being no cycles, there are also no re-convergences.
 */
std::vector<std::vector<MRRG::NodeDescriptor>> mergeMRRGWalks(
    const MRRG& mrrg, const std::vector<MrrgNodeSpan>& walks);

/**
 * For convenience. See above.
 */
inline auto mergeMRRGWalks(
    const MRRG& mrrg, const std::vector<std::vector<MRRG::NodeDescriptor>>& walks
) {
    std::vector<MrrgNodeSpan> with_pointers;
    with_pointers.reserve(walks.size());
    auto to_span = [](auto& e) -> MrrgNodeSpan { return {e.data(), e.size()}; };
    std::transform(walks.begin(), walks.end(), std::back_inserter(with_pointers), to_span);
    return mergeMRRGWalks(mrrg, with_pointers);
}

/**
 * Count the number of times this walk goes from context N to context 0.
 * It includes delay from all nodes except the last;
 *   delay is defined as from the input of the front() to the input of the back()
 * This definition is used because a walk is a a sequence of connected edges,
 *   but we are using a sequence of vertices to describe it.
 * The following additive identity holds:
 *   `tripCountOfWalk({a,..,b}) + tripCountOfWalk({b,..,c}) = tripCountOfWalk({a,..,b,..,c})
 * Will throw if `walk` does not have at least 2 elements.
 */
int tripCountOfWalk(const MRRG& mrrg, const std::vector<MRRG::NodeDescriptor>& walk);

/**
 * Is this node compatible with the operand tag?
 * Handles null tags and nodes with no supplied tags.
 */
bool nodeIsCompatibleWithOperand(const MRRG& mrrg, MRRG::NodeDescriptor node, const OperandTag& operand_tag);

/**
 * Does this walk consist soley of either nodes that have the operand, or nodes with no operand set?
 * Uses the usual definition of a walk (ie. the last node is not part of the walk).
 * Invariant: concatenated walks behave like logical AND, ie. f(a) && f(b) == f(a+b) always holds.
 */
bool walkIsCompatibleWithOperand(const MRRG& mrrg, const std::vector<MRRG::NodeDescriptor>& walk, const OperandTag& operand_tag);

/**
 * Adds a mapping to `transformation_result.mapping`, and erases the `from` node from `transformation_result.transformed_mrrg`
 * Call this after the properties & connections have already been transferred to `into`.
 */
void recordNodeMerge(
    MRRGTransformationResult& transformation_result, const MRRG& src_mrrg,
    MRRG::NodeDescriptor into, MRRG::NodeDescriptor from
);

/**
 * Make a simplified copy of the given MRRG, in a way that looses no information.
 * A reverse mapping is guaranteed to exist.
 */
MRRGTransformationResult reduceLosslessly(const MRRG& src_mrrg, const MRRGTransformFlags& flags);

/**
 * Adding nodes to satisfy constraint where a node with multiple fan-out cannot
 * fan-out to a node with multiple fan-in
 */
MRRGTransformationResult muxExNodeInsertion(const MRRG& src_mrrg);

/*******************
 * Simple node/graph functions
 *******************/

bool isNodeFanoutOf(const MRRG& mrrg, MRRG::NodeDescriptor driver_ndesc, MRRG::NodeDescriptor test_ndesc);
bool isNodeFaninOf(const MRRG& mrrg, MRRG::NodeDescriptor driven_ndesc, MRRG::NodeDescriptor test_ndesc);
void assertNodeIsFanoutOf(const MRRG& mrrg, MRRG::NodeDescriptor driver_ndesc, MRRG::NodeDescriptor test_ndesc, const char* action_name = " -- ");
void assertNodeIsFaninOf(const MRRG& mrrg, MRRG::NodeDescriptor driven_ndesc, MRRG::NodeDescriptor test_ndesc, const char* action_name = " -- ");
void assertNodeIsExcusiveFanout(const MRRG& mrrg, MRRG::NodeDescriptor driver_ndesc, MRRG::NodeDescriptor test_ndesc, const char* action_name = " -- ");
void assertNodeIsExcusiveFanin(const MRRG& mrrg, MRRG::NodeDescriptor driven_ndesc, MRRG::NodeDescriptor test_ndesc, const char* action_name = " -- ");
void assertHasNoOps(const MRRGNode& n, const char* action_name = "");

void assertHasSingleFanout(const MRRGNode& n, const char* action_name);

/**
 * Add the fanin and fanout of `from_ndesc` to `into_ndesc`
 */
void mergeNodeConnections(MRRG& mrrg, MRRG::NodeDescriptor into_ndesc, MRRG::NodeDescriptor from_ndesc);

/**
 * Given a node `from_ndesc` that is the only fanout of `into_ndesc`, and where `into_ndesc`
 * is the only fanin of `from_ndesc`, add the properties of `from_ndesc` into `into_ndesc`
 */
void mergeNodePropertiesOfExclusiveFanoutToExclusiveFanin(MRRG& mrrg, MRRG::NodeDescriptor into_ndesc, MRRG::NodeDescriptor from_ndesc);

/**
 * Given a node 'from_ndesc' that has only a single fanin 'into_ndesc' and a
 * single fanout 'fanout_ndesc', add the properties of from_ndesc into
 * into_ndesc
 */
void collapseSingleFaninFanoutNodeIntoFanin(MRRG& mrrg, MRRG::NodeDescriptor into_ndesc, MRRG::NodeDescriptor from_ndesc);

/*******************
 * Begin type definitions
 *******************/

struct InterMRRGMap {
    using NodeSet = std::unordered_set<MRRG::NodeDescriptor>;

    // Get the nodes in the tranformed MRRG that correspond to node `n` in the src MRRG
    const NodeSet& newNodesForOldNode(MRRG::NodeDescriptor n) const { return value_for_key_or(forward_map, n, empty_node_list); }

    // Get the nodes in the src MRRG that correspond to node `n` in the transformed MRRG
    const NodeSet& oldNodesForNewNode(MRRG::NodeDescriptor n) const { return value_for_key_or(reverse_map, n, empty_node_list); }

    void addMapping(MRRG::NodeDescriptor old_node, MRRG::NodeDescriptor new_node) {
        forward_map[old_node].emplace(new_node);
        reverse_map[new_node].emplace(old_node);
    }

    template<typename NodeList = std::vector<MRRG::NodeDescriptor>>
    void addMappingMulti(MRRG::NodeDescriptor old_node, const NodeList& new_nodes) { for (auto& nn : new_nodes) { addMapping(old_node, nn); } }
    template<typename NodeList = std::vector<MRRG::NodeDescriptor>>
    void addMappingMulti(const NodeList& old_nodes, MRRG::NodeDescriptor new_node) { for (auto& on : old_nodes) { addMapping(on, new_node); } }

    // Adds mapping from node in src MRRG to its copy in the transformed MRRG.
    void addAllByMatchingProperties(const MRRG& src_mrrg, const MRRG& transformed_mrrg);

    void mergeDescriptors(MRRG::NodeDescriptor from, MRRG::NodeDescriptor into);

    bool operator==(const InterMRRGMap& rhs) const { return forward_map == rhs.forward_map && reverse_map == rhs.reverse_map; }
    friend std::ostream& operator<<(std::ostream& os, const InterMRRGMap& imm);
private:
    std::unordered_map<MRRG::NodeDescriptor, NodeSet> forward_map = {};
    std::unordered_map<MRRG::NodeDescriptor, NodeSet> reverse_map = {};
    NodeSet empty_node_list = {};
};

struct MRRGTransformationResult {
    MRRG transformed_mrrg;
    InterMRRGMap mapping = {};
};

struct FindNeighbourFUsVisitor : DoNothingGraphVisitor<MRRG::NodeDescriptor> {
    std::unordered_set<MRRG::NodeDescriptor> discovered_fu_nodes = {};

    bool shouldIgnore(const MRRG::NodeDescriptor& v) {
        return discovered_fu_nodes.find(v) != end(discovered_fu_nodes);
    }

    void onExamineEdge(const MRRG::NodeDescriptor&, const MRRG::NodeDescriptor& fanout) {
        if (fanout->type == MRRG_NODE_FUNCTION || fanout->type == MRRG_NODE_ROUTING_FUNCTION) {
            discovered_fu_nodes.insert(fanout);
        }
    }
};

/**
 * Assuming the mapping operated on a transformed (reduced mrrg), undo
 * the transformation.
 */
Mapping transformToOriginalMRRG(const Mapping & map, const MRRGTransformationResult & transformation);

Mapping removeMuxExNodeInsertion(const Mapping & map, const MRRGTransformationResult & transformation);

#endif /* MRRG_PROCEDURES__H_ */
