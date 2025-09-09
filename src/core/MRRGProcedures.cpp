/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/MRRGProcedures.h>

#include <CGRA/GraphAlgorithms.h>

#include <queue>

namespace {
    template<typename WALK_TYPE>
    void assertWalkLength(const MRRG&, const WALK_TYPE& walk) {
        if (std::next(walk.begin()) == walk.end() ) {
            throw make_from_stream<std::logic_error>([&,f=__func__](auto&& s) {
                s << f << " called with a walk that has fewer than 2 vertices";
            });
        }
    }
} // end anon namespace

bool FunctionalUnitNeighbours::isReachableFrom(MRRG::NodeDescriptor src, MRRG::NodeDescriptor dest) const {
    auto neigh_search_result = neighbours.find(src);
    if (neigh_search_result == neighbours.end()) {
        return false;
    } else {
        auto& neighs = neigh_search_result->second;
        return neighs.end() != neighs.find(dest);
    }
}

std::ostream& operator<<(std::ostream& os, const FunctionalUnitNeighbours& fu_neighbours) {
    os << "{\n";
    for (auto& node_and_infos : fu_neighbours.neighbours) {
        os << '\t' << node_and_infos.first << " -> \n";
        for (auto& node_info : node_and_infos.second) {
            os << "\t\t" << node_info.second.node << '@' << node_info.second.distance << '\n';
        }
        os << '\n';
    }
    os << '}';
    return os;
}

void InterMRRGMap::addAllByMatchingProperties(const MRRG& src_mrrg, const MRRG& transformed_mrrg) {
    for (const auto& nodes_for_cycle : src_mrrg.allNodesByCycle()) {
        for (const auto& name_and_nodeUPtr : nodes_for_cycle) {
            const auto& ndesc = name_and_nodeUPtr.second.get();
            const auto ndesc_in_transformed_graph = transformed_mrrg.getNodeByPropertiesOf(src_mrrg.getNodeRef(ndesc));
            if (ndesc_in_transformed_graph) {
                addMapping(ndesc, ndesc_in_transformed_graph);
            }
        }
    }

    for (const auto& nodes_for_cycle : transformed_mrrg.allNodesByCycle()) {
        for (const auto& name_and_nodeUPtr : nodes_for_cycle) {
            const auto& ndesc = name_and_nodeUPtr.second.get();
            const auto ndesc_in_src_graph = src_mrrg.getNodeByPropertiesOf(transformed_mrrg.getNodeRef(ndesc));
            if (ndesc_in_src_graph) {
                addMapping(ndesc_in_src_graph, ndesc);
            }
        }
    }
}

void InterMRRGMap::mergeDescriptors(MRRG::NodeDescriptor from, MRRG::NodeDescriptor into) {
    for (auto& map_ptr : {&forward_map, &reverse_map}) {
        auto& map = *map_ptr;

        // rename references to `from`
        for (auto& node_and_set : map) {
            auto& set = node_and_set.second;
            const auto search_for_from = set.find(from);
            if (search_for_from != end(set)) {
                set.erase(search_for_from);
                set.insert(into);
            }
        }

        // merge/create sets that map from `from`
        const auto search_for_from = map.find(from);
        if (search_for_from != end(map)) {
            auto src = std::move(search_for_from->second);
            map.erase(search_for_from);
            auto& dest = map[into];
            std::move(begin(src), end(src), std::inserter(dest, end(dest)));
        }
    }
}

std::ostream& operator<<(std::ostream& os, const InterMRRGMap& imm) {
    const auto mapping_printer = [=](auto&& s, auto&& mapping_set) {
        print_container(s, mapping_set);
    };
    os << "}\n forward_map = ";
    print_assoc_container(os, imm.forward_map, mapping_printer);
    os << "\n reverse_map = ";
    print_assoc_container(os, imm.reverse_map, mapping_printer);
    os << '}';
    return os;
}

class MRRGProcedureCache {
public:
    using MRRGNDesc = MRRG::NodeDescriptor;
    struct Paths {
        std::vector<std::vector<MRRGNDesc>> paths = {};
        bool all_paths_found = false;
    };

    std::unordered_map<MRRGNDesc, std::unordered_map<MRRGNDesc, std::map<OperandTag,
        Paths
    >>> shortest_paths = {};
};

MRRGProcedureCacheHandle::MRRGProcedureCacheHandle() : impl_ptr() { }
MRRGProcedureCacheHandle::MRRGProcedureCacheHandle(const MRRGProcedureCacheHandle&) = default;
MRRGProcedureCacheHandle::MRRGProcedureCacheHandle(MRRGProcedureCacheHandle&&) = default;
MRRGProcedureCacheHandle::~MRRGProcedureCacheHandle() = default;

MRRGProcedureCacheHandle& MRRGProcedureCacheHandle::operator=(const MRRGProcedureCacheHandle&) = default;
MRRGProcedureCacheHandle& MRRGProcedureCacheHandle::operator=(MRRGProcedureCacheHandle&&) = default;

MRRGProcedureCache& MRRGProcedureCacheHandle::getOrCreate() {
    if (not impl_ptr) {
        impl_ptr = std::make_shared<MRRGProcedureCache>();
    }
    return *impl_ptr;
}

MRRGNodeClassListsByCycle computeNodeClassListsByCycle(const MRRG& mrrg) {
    MRRGNodeClassListsByCycle result;

    int i = 0;
    for (const auto& nodes_for_cycle : mrrg.allNodesByCycle())
    {
        std::vector<MRRG::NodeDescriptor> func_nodes;
        std::vector<MRRG::NodeDescriptor> rout_nodes;
        for (const auto& node : nodes_for_cycle)
        {
            if(node.second->type == MRRG_NODE_FUNCTION)
                func_nodes.push_back(node.second.get());
            else if(node.second->type == MRRG_NODE_ROUTING)
                rout_nodes.push_back(node.second.get());
            else if(node.second->type == MRRG_NODE_ROUTING_FUNCTION){
                rout_nodes.push_back(node.second.get());
                func_nodes.push_back(node.second.get());
            }
             else
                make_and_throw<cgrame_error>([&](auto&& s) {
                    s << "unhanded MRRG node type " << node.second->type;
                });
        }

        result.function_nodes.push_back(func_nodes);
        result.routing_nodes.push_back(rout_nodes);
    }

    return result;
}

MRRGNodeClassLists computeNodeClassLists(const MRRG& mrrg) {
    MRRGNodeClassLists result;

    for (const auto& nodes_for_cycle : mrrg.allNodesByCycle())
    {
        for (const auto& node : nodes_for_cycle)
        {
            if(node.second->type == MRRG_NODE_FUNCTION)
                result.function_nodes.push_back(node.second.get());
            else if(node.second->type == MRRG_NODE_ROUTING)
                result.routing_nodes.push_back(node.second.get());
            else if(node.second->type == MRRG_NODE_ROUTING_FUNCTION){
                result.routing_nodes.push_back(node.second.get());
                result.function_nodes.push_back(node.second.get());
            }
             else
                make_and_throw<cgrame_error>([&](auto&& s) {
                    s << "unhanded MRRG node type " << node.second->type;
                });
        }
    }

    return result;
}

FunctionalUnitNeighbours::NodeInfoList findNeighbourFUs(const MRRG& mrrg, MRRG::NodeDescriptor source, int min_neighbours_to_find) {
    const auto g_algos = GraphAlgorithms<MRRG::NodeDescriptor>();

    // start at the fanout of source, so we don't immediately "find it" (only want paths with length > 1)
    const std::unordered_set<MRRG::NodeDescriptor> source_fanout_set(mrrg.fanout(source).begin(), mrrg.fanout(source).end());
    const int path_length_adjustment = 1;

    FindNeighbourFUsVisitor visitor;

    const auto should_stop_searching = [&](auto& discovered_node) {
        (void)discovered_node;
        if (min_neighbours_to_find < 0) {
            return false;
        } else {
            std::ptrdiff_t num_found = visitor.discovered_fu_nodes.size();
            if (visitor.discovered_fu_nodes.find(source) != visitor.discovered_fu_nodes.end()) {
                num_found -= 1;
            }
            return num_found >= min_neighbours_to_find;
        }
    };

    const auto vertex_data = g_algos.wavedBreadthFirstVisit(mrrg, source_fanout_set, should_stop_searching, visitor);

    FunctionalUnitNeighbours::NodeInfoList result;

    for (auto& sink : visitor.discovered_fu_nodes) {
        auto path_and_success = g_algos.singleTraceback(source_fanout_set, sink, vertex_data);
        if (path_and_success.success) {
            result.emplace(sink, FunctionalUnitNeighbours::NodeInfo{ sink, (int)path_and_success.path.size() + path_length_adjustment});
        }
    }

    return result;
}

std::unordered_map<OpGraph::OpDescriptor, std::vector<MRRG::NodeDescriptor>> findAllCompatibleFUs(const OpGraph& opgraph, const MRRG& base_mrrg) {
    std::unordered_map<OpGraph::OpDescriptor, std::vector<MRRG::NodeDescriptor>> compatible_fu_nodes;

    for (auto& op : opgraph.opNodes()) {
        compatible_fu_nodes.emplace(op, decltype(compatible_fu_nodes)::mapped_type{});
        for (int cycle = 0; cycle < base_mrrg.initiationInterval(); ++cycle) {
            for (auto& name_and_node : base_mrrg.allNodesByCycle().at(cycle)) {
                auto ndesc = name_and_node.second.get();
                if (base_mrrg.getNodeRef(ndesc).canMapOp(op)) {
                    compatible_fu_nodes[op].emplace_back(ndesc);
                }
            }
        }
    }

    return compatible_fu_nodes;
}

std::vector<std::vector<MRRG::NodeDescriptor>> findNRoutingPathsBetween(
    int num_paths, const MRRG& mrrg, MRRG::NodeDescriptor source, MRRG::NodeDescriptor sink,
    std::pair<int,int> cycle_trip_count_min_max, const OperandTag& operand_tag,
    MRRGProcedureCacheHandle* cache_handle
) {
    if (cache_handle) {
        auto& datum = cache_handle->getOrCreate().shortest_paths[source][sink][operand_tag];
        auto& existing_paths = datum.paths;
        if ((std::ptrdiff_t)existing_paths.size() >= num_paths || datum.all_paths_found) {
            return {existing_paths.begin(), std::next(existing_paths.begin(), std::min(num_paths, (int)existing_paths.size()))};
        }
    }

    const auto g_algos = GraphAlgorithms<MRRG::NodeDescriptor>();

    struct V : DoNothingGraphVisitor<MRRG::NodeDescriptor> {
        const MRRG* mrrg;
        MRRG::NodeDescriptor sink, source;
        const OperandTag* operand_tag;

        V(const MRRG* mrrg, MRRG::NodeDescriptor sink, MRRG::NodeDescriptor source, const OperandTag* operand_tag)
            : mrrg(mrrg), sink(sink), source(source), operand_tag(operand_tag) { }

        bool shouldIgnore(MRRG::NodeDescriptor v) {
            if (not nodeIsCompatibleWithOperand(*mrrg, v, *operand_tag)) return true;
            return v != sink && v != source && (mrrg->getNodeRef(v).type == MRRG_NODE_FUNCTION);
        }
    } visitor { &mrrg, sink, source, &operand_tag };

    // find the paths, ignoring some loops
    auto result = g_algos.findNShortestPaths(num_paths, mrrg, {source}, findSingleVertex(sink), visitor,
        [&](const auto& path) {
            // if it's not a loop, then anything is fine
            if (source != sink) { return true; }
            // else only allow trip_count in the interval specified
            const auto tc = tripCountOfWalk(mrrg, path);
            return cycle_trip_count_min_max.first <= tc && tc <= cycle_trip_count_min_max.second;
        }
    );

    if (cache_handle) {
        auto& datum = cache_handle->getOrCreate().shortest_paths[source][sink][operand_tag];
        datum.paths = result;
        datum.all_paths_found = (std::ptrdiff_t)result.size() < num_paths;
    }
    return result;
}

std::vector<std::vector<MRRG::NodeDescriptor>> mergeMRRGWalks(const MRRG& mrrg, const std::vector<MrrgNodeSpan>& walks) {
    const auto g_algos = GraphAlgorithms<MRRG::NodeDescriptor>();
    struct Graph {
        std::map<MRRG::NodeDescriptor, std::set<MRRG::NodeDescriptor>> edges;
        auto fanout(const MRRG::NodeDescriptor& v) const { return edges.at(v); }
    } g;
    std::set<MRRG::NodeDescriptor> sources;
    std::set<MRRG::NodeDescriptor> sinks;

    // construct graph, and source & sink sets
    for (const auto& walk : walks) {
        sources.insert(walk.front());
        sinks.insert(walk.back());
        auto prev = walk.begin();
        for (auto curr = std::next(prev); curr != walk.end(); prev=curr,++curr) {
            g.edges[*prev].insert(*curr);
        }
        g.edges[*prev]; // just make sure it exists
    }

    // keep searching until we find all the sinks, and remember their order
    struct SinkRecordingVisitor : DoNothingGraphVisitor<MRRG::NodeDescriptor> {
        const std::set<MRRG::NodeDescriptor>* sinks;
        std::set<MRRG::NodeDescriptor> found_sinks = {};
        std::vector<MRRG::NodeDescriptor> sink_order = {};

        SinkRecordingVisitor(const std::set<MRRG::NodeDescriptor>* sinks) : sinks(sinks) { }

        void onExamine(const MRRG::NodeDescriptor& v) {
            if (sinks->find(v) == sinks->end()) { return; }
            if (not found_sinks.insert(v).second) { return; }
            sink_order.push_back(v);
        }
    } sink_recorder {&sinks};

    // if this is changed to Dijkstra and all costs are positive, then this function will return the optimal merge
    auto search_tree = g_algos.wavedBreadthFirstVisit(g, sources, visitAllVertices(), sink_recorder);

    std::vector<std::vector<MRRG::NodeDescriptor>> result;

    std::set<MRRG::NodeDescriptor> sources_not_found = sources;
    std::set<MRRG::NodeDescriptor> nodes_in_result = sources;

    const auto trace_back_and_add = [&](const auto& search_set, const auto& tb_start, const auto& fanin_map) {
        auto path_and_success = [&]() { // (immediately invoked)
            if (search_set.find(tb_start) == search_set.end()) {
                // not a loop, do a normal trace-back
                return g_algos.singleTraceback(search_set, tb_start, fanin_map);
            } else {
                // is a loop, try tracing back from each fanin, because there are no loops in a search *tree*
                for (const auto& src_and_fanout_set : g.edges) {
                    if (src_and_fanout_set.second.find(tb_start) == src_and_fanout_set.second.end()) { continue; }
                    auto result = g_algos.singleTraceback(search_set, src_and_fanout_set.first, fanin_map);
                    if (result.success) {
                        result.path.push_back(tb_start); // remember, we started at the fanin
                        return std::move(result);
                    }
                }
                throw std::logic_error("loop trace-back failed");
            }
        }();
        if (not path_and_success.success) { throw std::logic_error("trace-back failed"); }
        sources_not_found.erase(path_and_success.path.front());
        std::copy(path_and_success.path.begin(), path_and_success.path.end(), std::inserter(nodes_in_result, nodes_in_result.end()));
        result.emplace_back(std::move(path_and_success.path));
    };

    for (const auto& sink : sink_recorder.sink_order) {
        trace_back_and_add(nodes_in_result, sink, search_tree);
    }

    for (const auto& walk : walks) {
        const auto source = walk.front();
        if (not sources_not_found.erase(source)) { continue; }
        MRRG::NodeDescriptor the_node_found;
        auto fixup_search_tree = g_algos.wavedBreadthFirstVisit(g, singleItemSet(source), findVertexInSet(nodes_in_result, the_node_found));
        if (not the_node_found) { throw std::logic_error("fix-up search failed"); }
        trace_back_and_add(singleItemSet(source), the_node_found, fixup_search_tree);
    }

    return result;
}

int tripCountOfWalk(const MRRG& mrrg, const std::vector<MRRG::NodeDescriptor>& walk) {
    if (walk.empty()) { return 0; }
    assertWalkLength(mrrg, walk);

    return std::accumulate(walk.begin(), std::prev(walk.end()), 0, [&](auto sum, const auto& ndesc) {
        const auto& node = mrrg.getNodeRef(ndesc);
        const auto hypothetical_cycle = node.getContextNum() + node.getLatency();
        // trip count is how many IIs in the future that the output's cycle would be
        return sum + hypothetical_cycle/mrrg.initiationInterval();
    });
}

bool nodeIsCompatibleWithOperand(const MRRG& mrrg, MRRG::NodeDescriptor node, const OperandTag& operand_tag) {
    const auto& supported_ops = mrrg.getNodeRef(node).supported_operand_tags;
    return operand_tag.empty() || supported_ops.empty() || supported_ops.find(operand_tag) != supported_ops.end();
}

bool walkIsCompatibleWithOperand(const MRRG& mrrg, const std::vector<MRRG::NodeDescriptor>& walk, const OperandTag& operand_tag) {
    if (walk.empty()) { return true; }
    if (operand_tag.empty()) return true;
    assertWalkLength(mrrg, walk);

    const auto node_allows_tag = [&mrrg,&operand_tag](const auto& node) { return nodeIsCompatibleWithOperand(mrrg, node, operand_tag); };
    return std::all_of(walk.begin(), std::prev(walk.end()), node_allows_tag);
}

static void recordNodeMerge(
    const MRRG& src_mrrg, MRRGTransformationResult& transformation_result,
    MRRG::NodeDescriptor into_ndesc, MRRG::NodeDescriptor from_ndesc
) {
    auto& new_mrrg = transformation_result.transformed_mrrg;
    auto& from = new_mrrg.getNodeRef(from_ndesc);

    // add it to the mappings
    transformation_result.mapping.mergeDescriptors(from_ndesc, into_ndesc);
    transformation_result.mapping.addMapping(src_mrrg.getNodeByPropertiesOf(from), into_ndesc);

    // erase the node, remove all links
    new_mrrg.erase(from_ndesc);
}

MRRGTransformationResult reduceLosslessly(const MRRG& src_mrrg, const MRRGTransformFlags& flags)
{
    MRRGTransformationResult result{src_mrrg};
    auto& new_mrrg = result.transformed_mrrg;
    (void)flags;

    const auto& node_classes = computeNodeClassLists(new_mrrg);
    std::unordered_set<MRRG::NodeDescriptor> removed_nodes;

    // remove routing nodes that have exactly one fanin where that fanin only fans-out to this node
    for (auto& ndesc : node_classes.routing_nodes)
    {
        // make sure we haven't already deleted this node
        if (removed_nodes.find(ndesc) != end(removed_nodes)) { continue; }

        const auto& node = src_mrrg.getNodeRef(ndesc);

        // Remove nodes that have an exclusive fanin and are a exclusive fanout of that fanin
        if (node.fanin.size() == 1) {
            const auto the_fanin_ndesc = new_mrrg.getNodeRef(ndesc).fanin.at(0); // if statement guarantees there is only one
                  auto& the_fanin_node = new_mrrg.getNodeRef(the_fanin_ndesc);
            if (
                   (the_fanin_node.type == MRRG_NODE_ROUTING)
                && the_fanin_node.fanout.size() == 1
            ) {
                // std::cout << "reduceLosslessly merging: " << node << " into " << the_fanin_node << '\n';
                mergeNodePropertiesOfExclusiveFanoutToExclusiveFanin(new_mrrg, the_fanin_ndesc, ndesc);
                recordNodeMerge(src_mrrg, result, the_fanin_ndesc, ndesc);
                removed_nodes.emplace(ndesc);
            }
        }
    }

    result.mapping.addAllByMatchingProperties(src_mrrg, new_mrrg);

    return result;
}

MRRGTransformationResult muxExNodeInsertion(const MRRG& src_mrrg) {
    MRRGTransformationResult result {src_mrrg};
    auto& new_mrrg = result.transformed_mrrg;

    std::vector<std::pair<MRRG::NodeDescriptor, MRRG::NodeDescriptor>> insertionEdges;
    for (const auto & nodesForCycle: src_mrrg.allNodesByCycle()) {
        for (const auto & node: nodesForCycle) {
            /**
             * Make sure that node with multiple fanout does not fanout to a
             * node with multiple fanin
             */
            if (node.second->fanout.size() > 1) {
                for (const auto & fanout: node.second->fanout) {
                    if (fanout->fanin.size() > 1) {
                        insertionEdges.emplace_back(node.second.get(), fanout);
                    }
                }
            }
        }
    }

    // Insert the dummy nodes
    int i = 0;
    for (auto & edge: insertionEdges) {
        const auto & firstNode = src_mrrg.getNodeRef(edge.first);
        const auto & secondNode = src_mrrg.getNodeRef(edge.second);

        new_mrrg.unlink(new_mrrg.getNodeByPropertiesOf(firstNode), new_mrrg.getNodeByPropertiesOf(secondNode));
        auto newNode = new_mrrg.insert(new_mrrg.getNodeByPropertiesOf(firstNode), MRRGNode::make_routing(new_mrrg.getNodeByPropertiesOf(firstNode)->parent, firstNode.bitwidth, (firstNode.cycle + firstNode.latency) % new_mrrg.initiationInterval(), "mux_ex_insert_" + std::to_string(i++), 0)).first;

        new_mrrg.link(newNode, new_mrrg.getNodeByPropertiesOf(secondNode));
    }
    result.mapping.addAllByMatchingProperties(src_mrrg, new_mrrg);

    return result;
}

bool isNodeFanoutOf(const MRRG& mrrg, MRRG::NodeDescriptor driver_ndesc, MRRG::NodeDescriptor test_ndesc) {
    const auto& driver = mrrg.getNodeRef(driver_ndesc);
    const auto& test = mrrg.getNodeRef(test_ndesc);

    const bool found_test_in_fanout_of_driver = end(driver.fanout) != std::find(begin(driver.fanout), end(driver.fanout), test_ndesc);
    const bool found_driver_in_fanin_of_test  = end(test.fanin)  != std::find(begin(test.fanin),  end(test.fanin),  driver_ndesc);

    if (found_driver_in_fanin_of_test != found_test_in_fanout_of_driver) {
        make_and_throw<cgrame_error>([&](auto&& s) {
            s << "non-reflective MRRG link discovered";
        });
    }

    return found_test_in_fanout_of_driver;
}

bool isNodeFaninOf(const MRRG& mrrg, MRRG::NodeDescriptor driven_ndesc, MRRG::NodeDescriptor test_ndesc) {
    const auto& driver = mrrg.getNodeRef(driven_ndesc);
    const auto& test = mrrg.getNodeRef(test_ndesc);

    const bool found_test_in_fanin_of_driver = end(driver.fanin) != std::find(begin(driver.fanin), end(driver.fanin), test_ndesc);
    const bool found_driver_in_fanout_of_test  = end(test.fanout)  != std::find(begin(test.fanout),  end(test.fanout),  driven_ndesc);

    if (found_driver_in_fanout_of_test != found_test_in_fanin_of_driver) {
        make_and_throw<cgrame_error>([&](auto&& s) {
            s << "non-reflective MRRG link discovered";
        });
    }

    return found_test_in_fanin_of_driver;
}

void assertNodeIsFanoutOf(const MRRG& mrrg, MRRG::NodeDescriptor driver_ndesc, MRRG::NodeDescriptor test_ndesc, const char* action_name) {
    if (not isNodeFanoutOf(mrrg, driver_ndesc, test_ndesc)) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "attempted to " << action_name << " a non-fanout node";
    });}
}

void assertNodeIsFaninOf(const MRRG& mrrg, MRRG::NodeDescriptor driven_ndesc, MRRG::NodeDescriptor test_ndesc, const char* action_name) {
    if (not isNodeFaninOf(mrrg, driven_ndesc, test_ndesc)) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "attempted to " << action_name << " a non-fanin node";
    });}
}

void assertNodeIsExcusiveFanout(const MRRG& mrrg, MRRG::NodeDescriptor driver_ndesc, MRRG::NodeDescriptor test_ndesc, const char* action_name) {
    assertNodeIsFanoutOf(mrrg, driver_ndesc, test_ndesc, action_name);

    if (mrrg.getNodeRef(driver_ndesc).fanout.size() != 1) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "attempted to " << action_name << " a non-exclusive-fanout node";
    });}
}

void assertNodeIsExcusiveFanin(const MRRG& mrrg, MRRG::NodeDescriptor driven_ndesc, MRRG::NodeDescriptor test_ndesc, const char* action_name) {
    assertNodeIsFaninOf(mrrg, driven_ndesc, test_ndesc, action_name);

    if (mrrg.getNodeRef(driven_ndesc).fanin.size() != 1) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "attempted to " << action_name << " a non-exclusive-fanin node";
    });}
}

void assertHasNoOps(const MRRGNode& n, const char* action_name) {
    if (not n.supported_ops.empty()) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "attempted to " << action_name << " in a node that supports ops";
    });}
}

void assertHasSingleFanout(const MRRGNode& n, const char* action_name) {
    if (n.fanout.size() != 1) { make_and_throw<cgrame_error>([&](auto&& s) {
        s << "attempted to " << action_name << " in a node that has multiple fanouts";
    });}
}

void mergeNodeConnections(MRRG& mrrg, MRRG::NodeDescriptor into_ndesc, MRRG::NodeDescriptor from_ndesc) {
    auto& from = mrrg.getNodeRef(from_ndesc);

    for(auto& fanout_ndesc : from.fanout) {
        if (fanout_ndesc != into_ndesc) {
            mrrg.link(into_ndesc, fanout_ndesc);
        }
    }

    for(auto& fanin_ndesc : from.fanin) {
        if (fanin_ndesc != into_ndesc) {
            mrrg.link(into_ndesc, fanin_ndesc);
        }
    }
}

void mergeNodePropertiesOfExclusiveFanoutToExclusiveFanin(MRRG& mrrg, MRRG::NodeDescriptor into_ndesc, MRRG::NodeDescriptor from_ndesc) {
    auto& into = mrrg.getNodeRef(into_ndesc);
    const auto& from = mrrg.getNodeRef(from_ndesc);

    assertNodeIsExcusiveFanout(mrrg, into_ndesc, from_ndesc, "merge");
    assertNodeIsExcusiveFanin(mrrg, from_ndesc, into_ndesc, "merge");
    assertHasNoOps(from, "merge");

    mergeNodeConnections(mrrg, into_ndesc, from_ndesc);

    into.latency += from.latency;
    into.delay += from.delay;
    if (into.supported_operand_tags.empty()) {
        into.supported_operand_tags = from.supported_operand_tags;
    }
    else {
        if (into.supported_operand_tags != from.supported_operand_tags) {
            make_and_throw<cgrame_error>([&](auto&& s) {
                s << "Attempting to merge nodes that support different operands";
            });
        }
    }
}

Mapping transformToOriginalMRRG(const Mapping & map, const MRRGTransformationResult & transformation) {
    Mapping reversedMap(map);
    reversedMap.clear();    // Start with an empty mapping
    reversedMap.setStatus(map.getStatus());
    reversedMap.solve_time_in_seconds = map.solve_time_in_seconds;

    /**
     * Iterate through the mapping and replace each node withs its respective
     * reverse mapping. All nodes should have a reverse mapping, although it
     * may just be itself but in the original MRRG.
     */
    for (auto & opToMRRG : map.getMapping()) {
        for (auto & node_trans : opToMRRG.second) {
            // Get the nodes that were transformed from the original MRRG
            for (auto & node_orig: transformation.mapping.oldNodesForNewNode(node_trans)) {
                reversedMap.mapMRRGNode(opToMRRG.first, node_orig);
            }
        }
    }

    return reversedMap;
}
