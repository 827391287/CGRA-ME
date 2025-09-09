/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __MAPPING_H___
#define __MAPPING_H___

#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

class Mapping;

#include <CGRA/CGRA.h>
#include <CGRA/ConfigStore.h>
#include <CGRA/MRRG.h>
#include <CGRA/OpGraph.h>

enum class MappingStatus : int {
  success, failure, timeout,
};
std::ostream& operator<<(std::ostream& os, MappingStatus ms);

class Mapping
{
    public:
        Mapping(std::shared_ptr<CGRA> cgra, int II, std::shared_ptr<OpGraph> opgraph);
        ~Mapping();

        bool isMapped() const { return status == MappingStatus::success; }
        bool isTimeout() const { return status == MappingStatus::timeout; }
        MappingStatus getStatus() const { return status; }
        void setStatus(MappingStatus new_status) { status = new_status; }

        CGRA * getCGRA() const;
        auto getCGRA_shared() const { return cgra; }
        int getII() const;

        // accessor functions to the mapping
              std::map<OpGraph::NodeDescriptor,std::vector<MRRG::NodeDescriptor>>& getMapping()       { return mapping; }
        const std::map<OpGraph::NodeDescriptor,std::vector<MRRG::NodeDescriptor>>& getMapping() const { return mapping; }

        bool hasMapping(OpGraph::NodeDescriptor key) const { return mapping.find(key) != end(mapping); }
        void assertHasMapping(OpGraph::NodeDescriptor key) const { if (not hasMapping(key)) { make_and_throw<cgrame_error>([&](auto&&s) { s << "op " << key << " not mapped"; }); } }

        bool hasSingleMapping(OpGraph::NodeDescriptor key) const { hasMapping(key); return mapping.at(key).size() == 1; }
        void assertHasSingleMapping(OpGraph::NodeDescriptor key) const { assertHasMapping(key); if (not hasSingleMapping(key)) { make_and_throw<cgrame_error>([&](auto&&s) { s << "op " << key << " has multiple mappings"; }); } }

        MRRG::NodeDescriptor getSingleMapping(OpGraph::NodeDescriptor key) const { assertHasSingleMapping(key); return mapping.at(key).at(0); }
        const std::vector<MRRG::NodeDescriptor>& getMappingList(OpGraph::NodeDescriptor key) const { return mapping.at(key); }

        void setMapping(std::map<OpGraph::NodeDescriptor,std::vector<MRRG::NodeDescriptor>> mapping);
        void setNodeMapping(OpGraph::NodeDescriptor, std::vector<MRRG::NodeDescriptor> mapping);

        void  mapMRRGNode(OpGraph::NodeDescriptor, MRRG::NodeDescriptor node);
        void  unmapMRRGNode(OpGraph::NodeDescriptor, MRRG::NodeDescriptor node);

        /*
         * Restore this object to the just-constructed state.
         * No mappings, and not mapped, but other data is left alone.
         */
        void clear();

        // Result printing function
        void outputMapping(std::ostream & o = std::cout) const { outputMapping(*opgraph, o); }
        void outputMapping(const OpGraph& opgraph, std::ostream & o) const;
        void outputDetailedMapping(std::ostream & o = std::cout) const;
        void outputValMapping(OpGraphVal* val, std::ostream & o = std::cout) const;

        void check() const;

        // mapping verification
        bool verifyOpGraphMappingConnectivity();

              OpGraph& getOpGraph()       { return *opgraph; }
        const OpGraph& getOpGraph() const { return *opgraph; }

        template<typename F>
        void forEachMappingList(F&& f) const {
            for (const auto& og_node_and_mappings : mapping) {
                std::forward<F>(f)(og_node_and_mappings.first, og_node_and_mappings.second);
            }
        }

        template<typename F>
        void forEachMapping(F&& f) const {
            return forEachMappingList([&](auto&& og_node, auto&& mappings) {
                for (auto& mrrg_node : mappings) {
                    std::forward<F>(f)(og_node, mrrg_node);
                }
            });
        }

    private:
        std::map<OpGraph::NodeDescriptor,std::vector<MRRG::NodeDescriptor>> mapping = {};
        // TODO: add fanout-details
        std::shared_ptr<CGRA>       cgra;
        int                         II;
        std::shared_ptr<OpGraph>    opgraph;

        MappingStatus status = MappingStatus::failure;
    public:
        double solve_time_in_seconds = -1;
};

/**
 * Returns a near copy of `src`, but for each key-value of `forward_mappings`,
 *  the nodes that were mapped to key are now mapped to value,
 *  and key is not present.
 */
Mapping withRemappedOps(
    const Mapping& src,
    const std::unordered_map<OpGraph::NodeDescriptor, OpGraph::NodeDescriptor>& forward_mappings
);


/******** Graph of the Mapping ************/
struct MappingGraphNode {
    OpGraph::NodeDescriptor op_node_desc;
};

class MappingGraph {
public:
    struct NodeDescriptor { std::ptrdiff_t id = 0;
        bool operator==(const NodeDescriptor& rhs) const {
            return this->id == rhs.id;
        }
        bool operator< (const NodeDescriptor& rhs) const {
            return this->id < rhs.id;
        }
    };

    struct NodeDescriptorHash {
        std::size_t operator()(const NodeDescriptor& ndesc) const {
            return std::hash<decltype(NodeDescriptor::id)> {}(ndesc.id);
        }
    };

    using ToMRRGVertexMap = std::unordered_map<NodeDescriptor, MRRG::NodeDescriptor, NodeDescriptorHash>;

    MappingGraph() = default;
    MappingGraph(const MappingGraph& src) = default;
    MappingGraph(MappingGraph&&) = default;

    MappingGraph& operator=(const MappingGraph& rhs) = default;
    MappingGraph& operator=(MappingGraph&&) = default;

    /**
     * Add `node` to this MappingGraph by `std::move`ing it to the heap.
     * Overloads with a `fanin(s)` argument also add link(s) from the node(s) in that argument.
     */
    std::pair<NodeDescriptor,bool> insert(MappingGraphNode node);
    std::pair<NodeDescriptor,bool> insert(NodeDescriptor fanin, MappingGraphNode node) { return insertMultiFanin(std::array<NodeDescriptor,1>{{fanin}}, std::move(node)); }

    bool contains(NodeDescriptor node) {
        return nodes.find(node) != nodes.end();
    }
    bool contains(std::ptrdiff_t id) {
        return nodes.find({id}) != nodes.end();
    }

    template<typename NodeDescList = std::vector<NodeDescriptor>>
    std::pair<NodeDescriptor,bool> insertMultiFanin(NodeDescList fanins, MappingGraphNode node) {
        auto ir = insert(std::move(node));
        if (ir.second) for (auto& fanin : fanins) link(fanin, ir.first);
        return ir;
    }

    /**
     * For creating and destroying edges between nodes in an MappingGraph
     * All may perform some checks (especially when unlinking) on the validity of the graph,
     * throwing if something is wrong. Overall, better than manipulating the
     * fanout & fanin lists directly.
     */
    void link(MappingGraph::NodeDescriptor driver, MappingGraph::NodeDescriptor fanout);
    void unlink(MappingGraph::NodeDescriptor driver, MappingGraph::NodeDescriptor fanout);

    /**
     * Remove the node from the graph. Disconnects all fanout & fanin
     */
    void erase(MappingGraph::NodeDescriptor n);

    /**
     * Get an actual node object from your descriptor.
     * Undefined Behaviour if this node is not in this MappingGraph.
     */
          MappingGraphNode& getNodeRef(NodeDescriptor ndesc);
    const MappingGraphNode& getNodeRef(NodeDescriptor ndesc) const;

    /**
     * Gives a list of elements that fan-out from the given descriptor.
     * Allows a MappingGraph to be used with most graph algorithms.
     */
    auto& fanout(NodeDescriptor ndesc) const { return fanout_lists.at(ndesc); }

    /**
     * Gives a list of elements that fan-out from the given descriptor.
     * Allows a MappingGraph to be used with most graph algorithms.
     */
    auto& fanin(const NodeDescriptor ndesc) const { return fanin_lists.at(ndesc); }

    /**
     * Analyses this MappingGraph to find violations of various invariants, such as:
     *  - Connectivity
     *  = Latency
     */
    struct VerifyMessage {
        enum class Type : char { Info, Warning, Error, };
        Type type;
        std::string message;
        VerifyMessage(Type t, std::string s) : type(t), message(s) {}
    };

    std::vector<VerifyMessage> verify(const MRRG& mrrg, const ToMRRGVertexMap& toMRRG) const;

    /**
     * Print a machine readable text form of this graph.
     * Uses the classic .dot format, openable with xdot and other programs.
     */
    void printDot(std::ostream& os, const MRRG& mrrg, const OpGraph& opgraph, const ToMRRGVertexMap& toMRRG, const ConfigStore& archAttributes) const;

    /**
     * The number of nodes. No guarantee on time complexity.
     */
    std::ptrdiff_t size() const;

    auto& allNodes() const { return nodes; }

private:
    NodeDescriptor nextId = {1};
    std::unordered_map<NodeDescriptor, MappingGraphNode, NodeDescriptorHash> nodes = {};
    std::unordered_set<int> deletedIds = {};

    std::unordered_map<NodeDescriptor, std::vector<NodeDescriptor>, NodeDescriptorHash> fanout_lists = {};
    std::unordered_map<NodeDescriptor, std::vector<NodeDescriptor>, NodeDescriptorHash> fanin_lists = {};

};

struct CreateMappingGraphResult {
    using ToMRRG = MappingGraph::ToMRRGVertexMap;
    using FromMRRG = std::unordered_map<MRRG::NodeDescriptor, MappingGraph::NodeDescriptor>;

    MappingGraph mg;
    FromMRRG fromMRRG;
    ToMRRG toMRRG;
};

/**
 * Generates a mapping graph and the mapping to and from mapping graph nodes to mrrg nodes.
 */
CreateMappingGraphResult createMappingGraph(const MRRG& mg, const Mapping& m);

/**
 * Fixes the mapping by removing clusters of nodes that do not contain
 * functional units
 */
MappingGraph removeIsolatedRoutingNodes(const MappingGraph& mapping, const MRRG& mrrg, const MappingGraph::ToMRRGVertexMap& toMRRG);

using MappingGraphCycleAssignment = std::unordered_map<MappingGraph::NodeDescriptor, int, MappingGraph::NodeDescriptorHash>;
/**
 * Checks the mapping to make sure that the latencies are balanced.
 */
std::pair<bool, MappingGraphCycleAssignment> latencyCheck(
    const MappingGraph& mapping, const MRRG& mrrg,
    const CreateMappingGraphResult::ToMRRG& toMRRG
);

#endif
