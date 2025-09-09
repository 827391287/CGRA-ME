/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef OPGRAPH_PROCEDURES_H___
#define OPGRAPH_PROCEDURES_H___

#include <CGRA/OpGraph.h>

// forward declarations
class ConfigGraph; // in <CGRA/utility/ConfigGraph.h>
struct TrailsToBalance;

/**
 * Determines a minimal set of pairs-of-trails and trails representing cycles that
 * must be balanced or have latency equal to II, respectively.
 *
 * # Algorithm
 * Until all Ops have been visited:
 *   Choose an Op, and walk up its fanin graph until you hit a cycle or Op without fanin.
 *   Construct a spanning tree (of edges), rooted at there.
 *   For every new non-tree edge discovered:
 *     Search up the tree & non-tree edges for an edge that is driven by the target of this edge
 *     If found, this is a cycle;
 *       If it is a new cycle, record it and continue to next non-tree edge
 *     Else (both cases), then this is re-convergence.
 *     Find the edge in the spanning tree that shares the same target as this edge,
 *       and find their nearest common ancestor. These trails are a non-cyclic pair.
 *     Mark the edge found as a non-tree edge already considered
 *
 * Note: Treats the graph as a directed multi-graph instead of a directed hyper-graph.
 * Note: Constraints from searches rooted at different Ops are de-duplicated.
 *
 * ## Intuition
 * The above algorithm was inspired by noticing that in a strongly connected graph,
 *   there is exactly one required constraint per non-tree edge.
 * Also, it tends to produce constraints that a manual examination would:
 *   - If there is a choice between two equivalent cycle constraints,
 *       this tends to choose the smaller, as the search used is breadth-first.
 *   - For re-convergence, paths of similar length are preferred,
 *       again, because of the BFS.
 *
 * # Behaviour and Output Uniqueness
 * Although the output is minimal, most non-trivial DFGs *do not* have a
 *   unique minimal set, thus the exact output could change with
 *   a change in algorithm, while still being correct.
 * Further, the exact result may depend on the order Ops are inserted into the graph,
 *   and in particular, the insertion order of edges, which will affect the spanning trees.
 * This arises from the fact that the return value is really just a set of equalities,
 *   and it is always possible to perform row-reductions to produce an equivalent,
 *   but different, set of equalities.
 *
 * A particular interesting result is that for every graph with 1 or more cycles,
 *   there is a minimal set of constraints that contains exactly 1 cycle constraint
 *   -- row reductions can be used to convert the others into re-convergence constraints.
 * Similarly, there also exists a minimal set that is comprised of *only* cycle constraints!
 *
 */
TrailsToBalance computeTrailsToBalance(const OpGraph& op_graph);

struct TrailsToBalance {
    // a sequence of *unique* (hyper) edges.
    using Trail = OpGraph::Walk;
    using TrailPair = std::pair<Trail,Trail>;

    // A set of pairs of trails that should have the same latency -- re-convergence
    std::set<TrailPair> noncyclic_trail_pairs = {};

    // A set of trails that must have latency of II -- cycles
    std::set<Trail> cyclic_trails = {};

    // Dump this to `os` in a human-readable format.
    void print_to(std::ostream& os) const; // (operator<< is defined later)
    bool operator==(const TrailsToBalance& rhs) const {
        return std::tie(noncyclic_trail_pairs, cyclic_trails)
            == std::tie(rhs.noncyclic_trail_pairs, rhs.cyclic_trails);
    }
};

/**
 * Create one from a description.
 * Each vertex will correspond to one Op, and attributes supported are:
 *   opcode=string, input=anything, output=anything
 * All edges from a particular vertex will form one Val/hyper-edge, and suported attributes are:
 *   operand=string
 * All other attributes will be ignored.
 */
OpGraph createOpGraphFromConfig(const ConfigGraph& config);

/**
 * Remove all BR ops, and any nodes that were only used to compute the condition.
 */
OpGraph removeBranchComputation(OpGraph opgraph);

/**
 * Remove nodes that perform casts
 */
OpGraph removeCastNodes(OpGraph og);

/**
 * Remove phi nodes from opgraph.
 * Since we only handle loops with a single basic block, every phi node will have
 *   exactly one edge coming from the rest of the graph (internal), and at least one edge coming from
 *   somewhere else (inputs or constants; external)
 * For each phi node, figure out which connections are which, drop the other inputs/constants (external inputs)
 *   hook up the internal inputs to the fanout(s), and remove the phi node.
 * Correctly handles phis that fan-out to phis, as they are treated as internal nodes.
 */
OpGraph removePhiNodes(OpGraph opgraph);

/**
 * Find and parts of the graph that must result in constant values, and evaluate them
 * Particularly useful for simplifying lowered GEP operations
 * Repeatedly runs internal passes until nothing more can be done; calling this function on its result should have no effect.
 *
 * Currently will only touch add and mul ops
 */
OpGraph propagateConstants(OpGraph og);

/**
 * Duplicate const ops to ensure that the maximum fanout of all consts is 1
 *
 * Most architectures do not support const sharing, and if they did,
 *  it would have to be based on some concept of 'nearness' in the DFG.
 */
OpGraph distributeConstants(OpGraph og);

inline std::ostream& operator<<(std::ostream& os, const TrailsToBalance& ptb) { ptb.print_to(os); return os; }

#endif /* OPGRAPH_PROCEDURES_H___ */
