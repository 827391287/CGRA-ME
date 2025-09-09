/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/


#ifndef __IMPL_GRAPH_ALGORITHM_HELPERS_H__
#define __IMPL_GRAPH_ALGORITHM_HELPERS_H__

#include <CGRA/utility/Functional.h>

#include <set>
#include <map>
#include <iostream>

template<typename VertexID>
auto findSingleVertex(VertexID v) {
    return returnsIfEqualTo(std::move(v));
}

template<typename VertexSet, typename VertexID>
auto findVertexInSet(const VertexSet& v_set, VertexID& vertex_found) {
    return [&](const auto& v) {
        if (v_set.find(v) != v_set.end()) {
            vertex_found = v;
            return true;
        } else {
            return false;
        }
    };
}

inline auto visitAllVertices() { return AlwaysFalse{}; }
inline auto acceptAllPaths() { return AlwaysTrue{}; }

/**
 * A visitor that... does nothing! Mostly just a useful base class to inherit
 * your own visitors from - it implements all required callbacks, but they do nothing
 * and don't affect the graph visit in any way. This way, you only have to implement
 * the callbacks that you want to implement. Also, this is where all the callback
 * documentation is.
 */
template<typename VertexID>
class DoNothingGraphVisitor {
public:
    /**
     * The premier visitation function. Is called once per vertex in the visitation
     * order specified by the algorithm.
     */
    void onExamine(const VertexID&) { }

    /**
     * Called every time an edge is examined.
     */
    void onExamineEdge(const VertexID&, const VertexID&) { }

    /**
     * Should this vertex be treated as if it doesn't exist?
     */
    bool shouldIgnore(const VertexID&) { return false; }

    /**
     * Should this edge be treated as if it doesn't exist?
     */
    bool shouldIgnoreEdge(const VertexID&, const VertexID&) { return false; }

    /**
     * If a vertex is discovered but is ignored or already examined, this function is called.
     * Distinguishing between either case can be done by calling your shouldIgnore function
     * Between this function and `onExamine`, all vertices that an algorithm discovers
     *   will be called by one function or the other.
     */
    void onSkipped(const VertexID&) { }

    /**
     * If an edge is discovered, but it was ignored, or the fanout was already examined,
     *   or the fanout itself was ignored, this function is called.
     * Distinguishing between these cases can be done by
     *   calling your shouldIgnore and/or shouldIgnoreEdge functions
     * Between this function and `onExamineEdge`, all edges that an algorithm discovers
     *   will be called by one function or the other.
     */
    void onSkippedEdge(const VertexID&, const VertexID&) { }

    /**
     * If implemented by the algorithm, is called at the end of every expansion wave.
     */
    void onWaveEnd() { }

    /**
     * Asynchronously stop the algorithm.
     * Called periodically, with no specific guarantees
     *  -- other callbacks may be called after this would evaluate to true.
     * So, you should disable them with a boolean flag, or something.
     */
    bool shouldStop() { return false; }
};

/**
 * A class for help with composing visitors together. Given a pointer to a
 * visitor, it (by default) delegates all callbacks to that visitor, allowing
 * the composing code to only override/change certain callbacks.
 */
template<typename Visitor>
class ForwardingVisitor {
public:
    using VisitorPtr = std::remove_reference_t<Visitor>*;
    VisitorPtr ref;

    ForwardingVisitor(VisitorPtr visitor) : ref(visitor) { }

    ForwardingVisitor(const ForwardingVisitor&) = default;
    ForwardingVisitor& operator=(const ForwardingVisitor&) = default;
    ForwardingVisitor(ForwardingVisitor&&) = default;
    ForwardingVisitor& operator=(ForwardingVisitor&&) = default;

    template<typename VertexID>
    void onExamine(const VertexID& v) { ref->onExamine(v); }

    template<typename VertexID1, typename VertexID2>
    void onExamineEdge(const VertexID1& v1, const VertexID2& v2) { ref->onExamineEdge(v1, v2); }

    template<typename VertexID>
    bool shouldIgnore(const VertexID& v) { return ref->shouldIgnore(v); }

    template<typename VertexID1, typename VertexID2>
    bool shouldIgnoreEdge(const VertexID1& v1, const VertexID2& v2) { return ref->shouldIgnoreEdge(v1, v2); }

    template<typename VertexID>
    void onSkipped(const VertexID& v) { ref->onSkipped(v); }

    template<typename VertexID1, typename VertexID2>
    void onSkippedEdge(const VertexID1& v1, const VertexID2& v2) { ref->onSkippedEdge(v1, v2); }
};

template<
    typename VertexID,
    typename Base = DoNothingGraphVisitor<VertexID>,
    typename NodeSet = std::set<VertexID>
>
struct IgnoreVertexSetVisitor : Base {
    NodeSet nodes_to_ignore;

    template<typename ARG1 = NodeSet, typename... Args>
    IgnoreVertexSetVisitor(ARG1&& arg1, Args&&... args)
        : nodes_to_ignore(std::forward<ARG1>(arg1), std::forward<Args>(args)...)
    { }

    bool shouldIgnore(const VertexID& v) {
        return nodes_to_ignore.find(v) != end(nodes_to_ignore);
    }
};

template<
    typename VertexID,
    typename Base = DoNothingGraphVisitor<VertexID>,
    typename EdgeSet = std::set<std::pair<VertexID, VertexID>>
>
struct IgnoreEdgeSetVisitor : Base {
    EdgeSet edges_to_ignore;

    template<typename ARG1 = EdgeSet, typename... Args>
    IgnoreEdgeSetVisitor(ARG1&& arg1, Args&&... args)
        : edges_to_ignore(std::forward<ARG1>(arg1), std::forward<Args>(args)...)
    { }

    bool shouldIgnoreEdge(const VertexID& v1, const VertexID& v2) {
        return edges_to_ignore.find(std::make_pair(v1, v2)) != end(edges_to_ignore);
    }
};

/**
 * Optionally prints the callbacks as they happen
 */
template<typename VertexID, typename Base = DoNothingGraphVisitor<VertexID>, typename OStream = std::ostream>
struct DebugVisitor : Base {
    OStream* debug_stream;
    DebugVisitor(OStream* debug_stream = nullptr) : debug_stream(debug_stream) {}

    void onExamine(const VertexID& v) {
        Base::onExamine(v);
        if (debug_stream) { *debug_stream << " explore: " << v << '\n'; }
    }

    void onSkipped(const VertexID& v) {
        Base::onSkipped(v);
        if (debug_stream) { *debug_stream << " skipped: " << v << '\n'; }
    }

    void onExamineEdge(const VertexID& source, const VertexID& target) {
        Base::onExamineEdge(source, target);
        if (debug_stream) { *debug_stream << "  edge: " << source << " -> " << target << '\n'; }
    }

    void onSkippedEdge(const VertexID& source, const VertexID& target) {
        Base::onSkippedEdge(source, target);
        if (debug_stream) { *debug_stream << "  skipped edge: " << source << " -> " << target << '\n'; }
    }

    void onWaveEnd() {
        Base::onWaveEnd();
        if (debug_stream) { *debug_stream << "--- wave ended ---\n"; }
    }
};

/**
 * Implements all callbacks, and records their order, along with the IDs passed.
 */
template<typename VertexID, typename Base = DoNothingGraphVisitor<VertexID>>
struct EventRecordingVisitor : Base {
    struct Event {
        enum Type {
            onExamine,
            onSkipped,
            onExamineEdge,
            onSkippedEdge,
            onWaveEnd,
        };
        Type type;
        VertexID v1, v2;

        friend std::ostream& operator<<(std::ostream& os, const Type& et) {
            switch (et) {
                case onExamine: return os << "onExamine";
                case onSkipped: return os << "onSkipped";
                case onExamineEdge: return os << "onExamineEdge";
                case onSkippedEdge: return os << "onSkippedEdge";
                case onWaveEnd: return os << "onWaveEnd";
                default: return os << "Unimplemented Event::Type";
            }
        }

        friend std::ostream& operator<<(std::ostream& os, const Event& e) {
            os << "{ " << e.type;
            switch (e.type) {
                case onExamine:
                case onSkipped: return os << ": " << e.v1 << " }";
                case onExamineEdge:
                case onSkippedEdge: return os << ": " << e.v1 << " -> " << e.v2 << " }";
                case onWaveEnd:
                default: return os << " }";
            }
        }
    };

    std::vector<Event> examine_order = {};

    EventRecordingVisitor() = default;
    EventRecordingVisitor(Base b) : Base(std::move(b)) {}

    void onExamine(const VertexID& v) {
        Base::onExamine(v);
        examine_order.push_back({Event::onExamine, v, {}});
    }

    void onSkipped(const VertexID& v) {
        Base::onSkipped(v);
        examine_order.push_back({Event::onSkipped, v, {}});
    }

    void onExamineEdge(const VertexID& source, const VertexID& target) {
        Base::onExamineEdge(source, target);
        examine_order.push_back({Event::onExamineEdge, source, target});
    }

    void onSkippedEdge(const VertexID& source, const VertexID& target) {
        Base::onSkippedEdge(source, target);
        examine_order.push_back({Event::onSkippedEdge, source, target});
    }

    void onWaveEnd() {
        Base::onWaveEnd();
        examine_order.push_back({Event::onWaveEnd, {} , {}});
    }
};

/**
 * A super simple graph, where the VertexID is just std::string
 * Useful for writing tests, mostly.
 */
struct StringIDGraph {
    using VertexID = std::string;
    std::map<std::string, std::vector<VertexID>> fanout_lists = {};

    VertexID insert(std::string name) {
        fanout_lists[name]; return name;
    }

    VertexID insert(VertexID fanin, std::string name) {
        auto vid = insert(name);
        link(fanin, vid);
        return vid;
    }

    template<typename VertexIDList = std::initializer_list<VertexID>>
    VertexID insertMultiFanin(const VertexIDList& fanins, std::string name) {
        auto vid = insert(name);
        for (const auto& fanin : fanins) {
            link(fanin, vid);
        }
        return vid;
    }

    void link(const VertexID& source, const VertexID& target) {
        fanout_lists.at(source).push_back(target);
    }

    auto& fanout(const VertexID& ndesc) const {
        return fanout_lists.at(ndesc);
    }
};

/**
 * Make a Graph from a map to list of fanins.
 * ex.:
 * struct VData { std::vector<VertexID> fanin; };
 * std::unordered_map<VertexID, VData> fanin_map = foo();
 * auto reverse_graph = makeReversedGraphFromFaninLists<VertexID>(fanin_map)
 * // now do a BFS or something
 */
template<typename VertexID, typename Data>
struct ReversedGraphFromFaninLists {
    Data* data;

    using VertexList = std::decay_t<decltype(std::declval<Data>().at(std::declval<VertexID>()).fanin)>;
    VertexList empty_list = {};

    ReversedGraphFromFaninLists(const ReversedGraphFromFaninLists&) = default;
    ReversedGraphFromFaninLists& operator=(const ReversedGraphFromFaninLists&) = default;
    ReversedGraphFromFaninLists(ReversedGraphFromFaninLists&&) = default;
    ReversedGraphFromFaninLists& operator=(ReversedGraphFromFaninLists&&) = default;

    const auto& fanout(const VertexID& v) {
        const auto search_result = data->find(v);
        if (search_result == end(*data)) return empty_list;
            else return search_result->second.fanin;
    }
};

template<typename VertexID, typename Data>
auto makeReversedGraphFromFaninLists(Data* data) {
    return ReversedGraphFromFaninLists<VertexID, Data>{data};
}

#endif /* __IMPL_GRAPH_ALGORITHM_HELPERS_H__ */
