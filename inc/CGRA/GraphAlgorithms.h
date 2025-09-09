/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __GRAPH_ALGORITHMS_H__
#define __GRAPH_ALGORITHMS_H__

#include <CGRA/utility/Collections.h>
#include <CGRA/impl/GraphAlgorithmHelpers.h>

#include <algorithm>
#include <array>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <set>
#include <stack>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

/**
 * A generic graph algorithms library.
 * Works with any object - the only required interface is a unary function
 * called `fanout` that takes a vertex id, and returns a range-like
 * object (ie. has .begin() and .end()) of objects that can be converted to vertex ids.
 *
 * First, a small example using a depth first visit:
 *
 * struct Graph {
 *     std::vector<std::vector<int>> fanout_lists;
 *     const std::vector<int>& fanout(int ndesc) const { return fanout_lists.at(ndesc); }
 * } g {
 *     {{1}, {2,0}, {3,0}, {1,0}}
 * };
 *
 * struct Visitor : DoNothingGraphVisitor<int> {
 *     void onExplore(int ndesc) { std::cout << ndesc << ' '; }
 * } v;
 *
 * GraphAlgorithms<int> g_algos;
 *
 * // this should print: 1 2 3 0
 * g_algos.depthFirstVisit(g, 1, [](auto&&) { return false; }, v);
 *
 * // (end example)
 *
 * The GraphAlgorithms class itself store no information about any graphs
 * or algorithms that are called. It only serves as a common configuration
 * object for meta-parameters, such as the number of threads used, and
 * what sort of map is used to store data indexed by vertices when running algorithms.
 *
 * The default constructor is sufficient for most purposes. If configuration
 * of the meta-parameters is desired, then other constructors can be used,
 * or the builder-style member functions can be called.
 */
template<
    typename VertexID,
    typename MapGen = StandardMapMaker<std::unordered_map, VertexID>,
    typename SetGen = StandardSetMaker<std::unordered_set, VertexID>
>
class GraphAlgorithms {
private:
    int num_threads = 1; //< Max number of concurrent threads (may not be used by all algorithms)

    /**
     * The map generator object used.
     * This object may be called in a concurrent manner, and therefore
     * must be thread-safe. A nullary template `operator()` must be supplied,
     * which will be passed template parameters corresponding to the mapped type.
     * See `makeVertexMap` for exact details.
     */
    MapGen vertex_map_gen = {};

    /**
     * The set generator object used.
     * This object may be called in a concurrent manner, and therefore
     * must be thread-safe. A nullary template `operator()` must be supplied,
     * See `makeVertexSet` for exact details.
     */
    SetGen vertex_set_gen = {};

public:
    GraphAlgorithms() { }

    template<typename NewMapGen, typename NewSetGen>
    GraphAlgorithms(
        int num_threads,
        NewMapGen&& vertex_map_gen,
        NewSetGen&& vertex_set_gen
    )
        : num_threads(num_threads)
        , vertex_map_gen(std::forward<NewMapGen>(vertex_map_gen))
        , vertex_set_gen(std::forward<NewSetGen>(vertex_set_gen))
    { }

    /**
     * A builder-style function for specifying the number of threads.
     * This function returns (by value) a GraphAlgorithms object that only
     * differs in the maximum number of threads.
     */
    auto withThreads(int num_threads) const {
        return GraphAlgorithms<
            VertexID,
            MapGen
        >(
            num_threads,
            vertex_map_gen,
            vertex_set_gen
        );
    }

    /**
     * A builder-style function for specifying the vertex map maker
     * This function returns (by value) a GraphAlgorithms object that only
     * differs in what object is used to make a vertex map.
     */
    template<typename NewMapGen>
    auto withMapGen(NewMapGen&& newVertexMapGen) const {
        return GraphAlgorithms<
            VertexID,
            std::decay_t<NewMapGen>,
            SetGen
        >(
            num_threads,
            std::forward<NewMapGen>(newVertexMapGen),
            vertex_set_gen
        );
    }

    /**
     * A builder-style function for specifying the vertex set maker
     * This function returns (by value) a GraphAlgorithms object that only
     * differs in what object is used to make a vertex set.
     */
    template<typename NewSetGen>
    auto withSetGen(NewSetGen&& newVertexSetGen) const {
        return GraphAlgorithms<
            VertexID,
            MapGen,
            std::decay_t<NewSetGen>
        >(
            num_threads,
            vertex_map_gen,
            std::forward<NewSetGen>(newVertexSetGen)
        );
    }

    template<typename... Params>
    auto makeVertexMap() const {
        return this->vertex_map_gen.template operator()<Params...>();
    }

    template<typename... Params>
    auto makeVertexSet() const {
        return this->vertex_set_gen.template operator()<Params...>();
    }

    template<typename... Params>
    using VertexMap = std::decay_t<decltype(std::declval<MapGen>().template operator()<Params...>())>;
    using VertexList = std::vector<VertexID>;
    using VertexPath = VertexList;
    template<typename... Params>
    using VertexSet = std::decay_t<decltype(std::declval<SetGen>().template operator()<Params...>())>;
    using PathList = std::vector<VertexPath>;
    using EdgeSet = std::set<std::pair<VertexID, VertexID>>;

    struct TracebackResult {
        VertexPath path;
        bool success;

        template<typename STREAM, typename GRAPH>
        void print(STREAM&& os, const GRAPH& g) const {
            os << "{success = " << success << "; ";
            print_container(os, path, [&](auto&& os, auto& v) { os << g.getNodeRef(v); });
            os << '}';
        }

        template<typename STREAM>
        friend STREAM& operator<<(STREAM& os, const TracebackResult& tbr) { tbr.print(os); return os; }
    };

    // this file has algorithms related to searching,
    // eg. depth- and breadth-first visits
    #include <CGRA/impl/GraphAlgorithms_Searching.inl>

    // this file has algorithms related to finding paths,
    // eg. tracing back through a search, and finding n shortest paths
    #include <CGRA/impl/GraphAlgorithms_Paths.inl>

}; // end class GraphAlgorithms

#endif // __GRAPH_ALGORITHMS_H__
