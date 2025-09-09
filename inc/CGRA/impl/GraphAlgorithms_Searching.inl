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
 * A single-threaded breadth-first visit over `graph`, starting at the
 * vertices in `initial_list`.
 */
template<typename Graph, typename InitialList, typename Visitor = DoNothingGraphVisitor<VertexID>>
auto breadthFirstVisit(
    Graph&& graph,
    const InitialList& initial_list,
    Visitor&& visitor = {}
) const {

    struct VertexData {
        bool put_in_queue = false;
    };

    auto data = makeVertexMap<VertexData>();

    std::list<VertexID> to_visit;
    for (const auto& vertex : initial_list) {
        to_visit.push_back(vertex);
        data[vertex].put_in_queue = true;
    }

    while (!to_visit.empty() && !visitor.shouldStop()) {
        const auto explore_curr = to_visit.front();
        to_visit.pop_front();

        if (visitor.shouldIgnore(explore_curr)) {
            visitor.onSkipped(explore_curr);
            continue;
        }

        visitor.onExamine(explore_curr);
        for (const auto& fanout : graph.fanout(explore_curr)) {
            auto& fanout_data = data[fanout];
            if (!fanout_data.put_in_queue  && !visitor.shouldIgnore(fanout)) {
                fanout_data.put_in_queue = true;
                to_visit.push_back(fanout);
                visitor.onExamineEdge(explore_curr, fanout);
            } else {
                visitor.onSkippedEdge(explore_curr, fanout);
            }
        }
    }

    return data;
}

/**
 * A single-threaded visit in dijkstra's algorithm-order over `graph`, starting at the
 * vertices in `initial_list`.
 */
template<typename Graph, typename InitialList, typename VertexCoster, typename Visitor = DoNothingGraphVisitor<VertexID>>
auto dijkstraVisit(
    Graph&& graph,
    const InitialList& initial_list,
    VertexCoster&& vertex_coster,
    Visitor&& visitor = {}
) const {
    using Cost = decltype(vertex_coster(std::declval<VertexID>()));

    struct VertexData {
        std::array<VertexID,1> fanin;
        Cost lowest_known_cost;
        bool operator==(const VertexData& rhs) const {
            return this->lowest_known_cost == rhs.lowest_known_cost && this->fanin == rhs.fanin; }
        // bool explored = false;
    };

    // store cost in priority queue so we can have duplicates
    struct VertexAndCost {
        VertexID vid;
        Cost cost_to_here;

        bool operator<(const VertexAndCost& rhs) const {
            return cost_to_here > rhs.cost_to_here;
        }
    };

    auto data = makeVertexMap<VertexData>();
    std::priority_queue<VertexAndCost> to_visit;

    for (const auto& vertex : initial_list) {
        if (visitor.shouldIgnore(vertex)) {
            visitor.onSkipped(vertex);
        } else {
            to_visit.push({vertex, 0});
            data.insert({vertex, {{vertex}, 0}});
        }
    }

    while (!to_visit.empty() && !visitor.shouldStop()) {
        const auto queue_top = to_visit.top();
        const auto& explore_curr = queue_top.vid;
        to_visit.pop();

        // nodes may be insereted in the queue more than once
        // but we don't care, because the cost will be higher?
        // auto& explored = data.at(explore_curr).explored;
        // if (explored) {
        if (visitor.shouldIgnore(explore_curr) || queue_top.cost_to_here > data.at(explore_curr).lowest_known_cost) {
            visitor.onSkipped(explore_curr);
            continue;
        }
        // explored = true;

        visitor.onExamine(explore_curr);
        for (const auto& fanout : graph.fanout(explore_curr)) {
            if (visitor.shouldIgnore(fanout) || visitor.shouldIgnoreEdge(explore_curr, fanout)) {
                visitor.onSkippedEdge(explore_curr, fanout);
                continue; // don't even add the vertex data
            }
            const auto cost = queue_top.cost_to_here + vertex_coster(fanout);
            auto fanout_data_insert = data.insert({fanout, {{explore_curr}, cost}});
            auto& fanout_data = fanout_data_insert.first->second;
            bool worse = not fanout_data_insert.second && cost >= fanout_data.lowest_known_cost;
            if (worse) {
                visitor.onSkippedEdge(explore_curr, fanout);
            } else {
                // always insert -- duplicates will get ignored
                fanout_data.lowest_known_cost = cost;
                fanout_data.fanin = {explore_curr};
                to_visit.push({fanout, cost});
                visitor.onExamineEdge(explore_curr, fanout);
            }
        }
    }

    return data;
}

/**
 * A (optionally) parallel breadth-first visit of `graph`, starting at the
 * vertices in `initial_list`, terminating if `is_target` evaluates to true on any vertex.
 * `is_target` should be a unary functor that accepts a vertex as its only argument
 *
 * Parallelism is achieved by splitting the current expansion wave into `num_threads`
 * segments, and expanding those in separate threads. No threads are created if `num_threads == 1`.
 * All visitor functions should be thread-safe when running with `num_threads > 1`
 *
 * This function returns a fanin graph that includes *all* edges discovered.
 * A fanin that is closer to the `begin` of the fanin was discovered at the same time or earlier in the search.
 * So, if you want a BFS spanning tree, follow `fanin.begin()`
 *
 * An example follows, and see the tests for more:
 * struct Graph {
 *     std::vector<std::vector<int>> fanout_lists;
 *     const std::vector<int>& fanout(int ndesc) const { return fanout_lists.at(ndesc); }
 * } g {
 *     {{1}, {2,0}, {3,0}, {1,0}}
 * };
 *
 * struct Visitor : DoNothingGraphVisitor<int> {
 *     void onExamine(int ndesc) { std::cout << ndesc << ' '; }
 * } v;
 *
 * GraphAlgorithms<int> g_algos;
 *
 * // this should print: 1 2 0 3
 * g_algos.wavedBreadthFirstVisit(g, {1}, [](auto&&) { return false; }, v);
 * // this should print: 1 2
 * g_algos.wavedBreadthFirstVisit(g, {1}, [](auto&& n) { return n == 2; }, v);
 */
template<typename Graph, typename IsTarget, typename Visitor = DoNothingGraphVisitor<VertexID>, typename InitialList = std::vector<VertexID>>
auto wavedBreadthFirstVisit(
    Graph&& graph,
    const InitialList& initial_list,
    IsTarget&& is_target,
    Visitor&& visitor = {}
) const {

    struct VertexData {
        std::vector<VertexID> fanin = {};
    };

    struct ExploreData {
        VertexID parent;
        VertexID fanout;
    };

    struct WaveData {
        std::vector<ExploreData> next_wave = {};
        void clear() {
            next_wave.clear();
        }
    };

    auto data = makeVertexMap<VertexData>();

    // the following are cleared and reused each outer loop
    std::vector<ExploreData> explorations_to_new_nodes;
    auto in_next_wave = makeVertexSet();
    std::vector<WaveData> waveData(num_threads);
    std::vector<VertexID> curr_wave = {};

    for (const auto& vertex : initial_list) {
        curr_wave.push_back(vertex);
        data[vertex];
    }

    while(true) {
        const auto expand_wave_code = [&](int ithread) {
            const auto& num_data_per_thread = 1 + ((curr_wave.size()-1)/num_threads); // rounds up
            const auto& my_curr_wave_begin_index = ithread*num_data_per_thread;
            const auto& my_curr_wave_end_index =   (ithread + 1)*num_data_per_thread + 1;
            const auto& my_curr_wave = make_iterator_range(
                std::next(begin(curr_wave), std::min(curr_wave.size(), my_curr_wave_begin_index)),
                std::next(begin(curr_wave), std::min(curr_wave.size(), my_curr_wave_end_index))
            );
            auto& my_next_wave = waveData.at(ithread).next_wave;

            for (const auto& id : my_curr_wave) {
                if (visitor.shouldIgnore(id)) {
                    visitor.onSkipped(id);
                    continue;
                }
                visitor.onExamine(id);

                for (const auto& fanout : graph.fanout(id)) {
                    if (not visitor.shouldIgnore(fanout) && not visitor.shouldIgnoreEdge(id, fanout)) {
                        my_next_wave.emplace_back(ExploreData{id, fanout});
                    } else {
                        visitor.onSkippedEdge(id, fanout);
                    }
                }
            }
        };

        if (num_threads == 1) {
            expand_wave_code(0);
        } else {
            std::vector<std::thread> threads;

            for (int ithread = 0; ithread < num_threads; ++ithread) {
                threads.emplace_back(expand_wave_code, ithread);
            }

            for (auto& thread : threads) {
                thread.join();
            }
        }

        bool found_target = false;
        explorations_to_new_nodes.clear();
        in_next_wave.clear();
        curr_wave.clear();
        for (auto& waveDatum : waveData) {
            for (auto& exploreData : waveDatum.next_wave) {
                // If not yet explored and not in next wave, add it
                // Node: edges & fanout are already ignored in `expand_wave_code`
                if (data.find(exploreData.fanout) == end(data)
                    && in_next_wave.emplace(exploreData.fanout).second
                ) {
                    curr_wave.push_back(exploreData.fanout);
                    visitor.onExamineEdge(exploreData.parent, exploreData.fanout);
                } else {
                    visitor.onSkippedEdge(exploreData.parent, exploreData.fanout);
                }

                if (is_target(exploreData.fanout)) {
                    found_target = true;
                }

            }
        }

        // Couldn't modify `data` in the above loop, so do it here.
        // Also, recall that this function returns a fanin graph with *all* discovered edges.
        for (auto& waveDatum : waveData) {
            for (auto& exploreData : waveDatum.next_wave) {
                data[exploreData.fanout].fanin.emplace_back(exploreData.parent);
            }
            waveDatum.clear();
        }

        visitor.onWaveEnd();
        if (found_target || curr_wave.empty() || visitor.shouldStop()) {
            break;
        }
    }

    return data;
}

/**
 * A single-threaded depth-first visit of `graph`, starting at the vertex `source`,
 * terminating if `is_target` evaluates to true on any vertex.
 * `is_target` should be a unary functor that accepts a vertex as its only argument
 *
 * An example follows, and see the tests for more:
 * struct Graph {
 *     std::vector<std::vector<int>> fanout_lists;
 *     const std::vector<int>& fanout(int ndesc) const { return fanout_lists.at(ndesc); }
 * } g {
 *     {{1}, {2,0}, {3,0}, {1,0}}
 * };
 *
 * struct Visitor : DoNothingGraphVisitor<int> {
 *     void onExamine(int ndesc) { std::cout << ndesc << ' '; }
 * } v;
 *
 * GraphAlgorithms<int> g_algos;
 *
 * // this should print: 1 2 3 0
 * g_algos.depthFirstVisit(g, {1}, [](auto&&) { return false; }, v);
 * // this should print: 1 2
 * g_algos.depthFirstVisit(g, {1}, [](auto&& n) { return n == 2; }, v);
 */
template<typename Graph, typename Source, typename IsTarget, typename Visitor = DoNothingGraphVisitor<VertexID>>
auto depthFirstVisit(
    Graph&& graph,
    const Source& source,
    IsTarget&& is_target,
    Visitor&& visitor = {}
) const {
    struct VertexData {
        std::array<VertexID,1> fanin;
        bool expanded = false;
    };

    auto data = makeVertexMap<VertexData>();

    struct ExploreData {
        VertexID driver;
        VertexID fanout;
    };

    std::vector<ExploreData> edges_to_visit;
    edges_to_visit.emplace_back(ExploreData{source, source});
    data.emplace(source, VertexData{{source}});

    while (not edges_to_visit.empty() && not visitor.shouldStop()) {
        const auto edata = std::move(edges_to_visit.back());
        edges_to_visit.pop_back();
        const auto& v = edata.fanout;
        auto& data_for_v = data.emplace(v, VertexData{{edata.driver}}).first->second;

        if (
               data_for_v.expanded
            || visitor.shouldIgnore(v)
            || visitor.shouldIgnoreEdge(edata.driver, v)
        ) {
            visitor.onSkipped(v);
            continue;
        }

        data_for_v.fanin.at(0) = edata.driver; // in case already exists
        data_for_v.expanded = true;
        visitor.onExamine(v);

        if (is_target(v)) {
            break;
        }

        const auto insertion_point = edges_to_visit.size();
        for (const auto& fanout : graph.fanout(v)) {
            const auto fanout_data_search = data.find(fanout);
            if (
                   not visitor.shouldIgnore(fanout)
                && not visitor.shouldIgnoreEdge(v, fanout)
                && ( fanout_data_search == end(data) || fanout_data_search->second.expanded )
            ) {
                visitor.onExamineEdge(v, fanout);
                edges_to_visit.push_back(ExploreData{v, fanout});
            } else {
                visitor.onSkippedEdge(v, fanout);
            }
        }

        // reverse the list, so that we visit in lexicographical order
        std::reverse(begin(edges_to_visit) + insertion_point, end(edges_to_visit));
    }

    return data;
}
