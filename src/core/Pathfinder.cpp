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

PathFinder::PathFinder(ClusteredMapperDriver driver,
    const OpGraph& opgraph,
    const MRRG& mrrg,
    bool isElastic,
    const std::string placement_filename)
    : ClusteredMapperDriver(driver)
    , l_mrrg(mrrg)
    , l_isElastic(isElastic)
    , l_placement_filename(placement_filename)
    , l_opgraph(opgraph)
    , l_routing_nodes() {
    const auto classes = computeNodeClassLists(mrrg);
    l_p_factor = l_initial_pfactor;
    l_h_factor = l_initial_hfactor;

    for (const auto& mrrg_node : classes.routing_nodes) {
        // Assinging a base cost of 1.0 to reg based connections
        // and 2.0 to combinatational based connections
        double baseCost = 1.0;
        l_routing_nodes.emplace(mrrg_node, NodeAttributes());
        l_routing_nodes[mrrg_node].baseCost = baseCost;
    }
}

Mapping PathFinder::routeOpGraph(Mapping placed) {
    Mapping mapping_result = placed;
    //if (l_verbosity > 0) std::cout << "Options = " << l_mapper_args << '\n';

    // Read in the placement file
    if (l_placement_filename.empty()) {
        l_used_rounting_function_nodes.clear();
        for (auto& op : l_opgraph.opNodes()) {
            MRRG::NodeDescriptor mrrg_node = placed.getSingleMapping(op);
            l_placement[op] = mrrg_node;
            if (mrrg_node->type == MRRGNode_Type::MRRG_NODE_ROUTING_FUNCTION) {
                l_used_rounting_function_nodes.emplace(mrrg_node);
            }
        }
    } else {
        parsePlacementFile();
    }

    // store cost in priority queue so we can have duplicates
    struct NetInfo {
        OpGraph::ValDescriptor val_node;
        bool overuse = false;
        Latency max_cycles;
        unsigned fanout = 0;

        bool operator<(const NetInfo& rhs) const {
            return fanout > rhs.fanout && max_cycles > rhs.max_cycles;
        }
    };
    bool routed = false;
    std::vector<NetInfo> nets;
    // Do initial routing for every value node
    // If the node can't be routed then give up.
    for (OpGraph::ValDescriptor val_node :  l_opgraph.valNodes()) {
        routed = routeVal(val_node);
        if (!routed) break;

        Latency max_cycles = 0;
        auto& src_op = val_node->input;
        int II = l_mrrg.initiationInterval();
        for (auto& sink_op : val_node->output) {
            Latency cycles_to_sink = getCyclesToSink(src_op, sink_op);
        }
        nets.push_back({val_node, false, max_cycles, (unsigned)val_node->output.size()});
    }
    std::sort(nets.begin(), nets.end());
    // Check if the opgraph mapped
    bool mapped = isOpgraphCovered() && checkOveruse();
    int i = 0;
    while (!mapped && i < 70 && routed) {
        i++;
        // If the mapping does not occur keep ripping out
        // values and re-routing them.
        for (auto& net : nets) {
            auto& val_node = net.val_node;
            if (l_verbosity > 0) std::cout << "Ripping out node: " << val_node->getName() << std::endl;

            ripUpOpVal(val_node);

            if (l_verbosity > 0) std::cout << "Routing node: " << val_node->getName() << std::endl;

            routed = routeVal(val_node);

             if (!routed && i > 1) {
                throw cgrame_error("ERR: net: " + val_node->getName() + " could not be routed");
            } else 
            if (!routed) {
               break;
            }

            bool mrrg_overuse = checkOveruse();
            bool opgraph_covered  = isOpgraphCovered();
            mapped = mrrg_overuse && opgraph_covered;
            std::cout << "overuse_ok=" << mrrg_overuse;
            std::cout << " opgraph_covered=" << opgraph_covered << std::endl;
            if (mapped) break;
        }
        if (!routed) break;
        l_p_factor = l_p_factor * l_p_factor_factor;
        l_h_factor = l_h_factor * l_h_factor_factor;
    }
    std::cout<< "GRAPH COVERED: " << isOpgraphCovered() << "   " << checkOveruse() <<std::endl;
    if (isOpgraphCovered() && checkOveruse()) {
        printNumberOfResourcesUsed();
        setMappingInto(mapping_result);
        mapping_result.setStatus(MappingStatus::success);
    } else {
        for (auto& valNode : l_opgraph.valNodes()) {
            ripUpOpVal(valNode);
        }
        for (auto& mrrg_node : l_routing_nodes) {
            mrrg_node.second.hCost = 0;
        }
        l_used_rounting_function_nodes.clear();
        l_p_factor = l_initial_pfactor;
        l_h_factor = l_initial_hfactor;
    }

    return mapping_result;
}

bool PathFinder::isOpgraphCovered()  {
    bool is_covered = true;
    for (const auto& val : l_opgraph.valNodes()) {
        auto lookup = l_mapping.find(val);
        if (lookup == l_mapping.end() || lookup->second.empty()) {
            is_covered = false;
        }
    }
    return is_covered;
}

void PathFinder::printNumberOfResourcesUsed() {
    int num_of_resources_used = 0;
    for (const auto& nodelist : l_routing_nodes) {
        int occupancy = nodelist.second.occupancy;
        const MRRGNode* node  = nodelist.first;
        if (occupancy > 0) {
            num_of_resources_used++;
        }
    }
    std::cout << "Number of routing resoucres used: " << num_of_resources_used << std::endl;
    return;
}

bool PathFinder::checkOveruse() {
    bool result = true;
    int num_of_overuse = 0;
    for (const auto& nodelist : l_routing_nodes) {
        int occupancy = nodelist.second.occupancy;
        const MRRGNode* node  = nodelist.first;
        bool overuse = occupancy > node->capacity;
        if (overuse && l_verbosity > 1) {
            //std::cout << node << " is overused by: ";
            num_of_overuse++;
            for (auto& value : nodelist.second.mapped_values) {
                //std::cout << "    " << value->getName();
            }
            //std::cout << std::endl;
        }
        result &= !overuse;
    }
    //std::cout << "Number of overuse: "  << num_of_overuse << " PFACTOR: " << l_p_factor << " HFACTOR: " << l_h_factor << std::endl;
    return result;
}



void PathFinder::ripUpOpVal(const OpGraphVal* val) {
    // unmap all nodes
    for (auto& n : l_mapping[val]) {
        // update history cost by adding the number of extra occupacny that
        // occured on the node
        l_routing_nodes[n].hCost += (l_routing_nodes[n].occupancy > n->capacity) ?
            (l_routing_nodes[n].occupancy - n->capacity) : 0;

        // TODO(raghebom): do we still need to keep track of the mapped values
        auto it = find(l_routing_nodes[n].mapped_values.begin(), l_routing_nodes[n].mapped_values.end(), val);
        if (it == l_routing_nodes[n].mapped_values.end()) {
            throw cgrame_error("Cannot find value (" + val->getName() + ") mapped to the mrrg node (" + n->getFullName() + ")\n");
        }

        // Updating the number of values mapped to this node
        l_routing_nodes[n].mapped_values.erase(it);
        if (l_routing_nodes[n].occupancy-- <= 0) {
            throw cgrame_error("occupancy was already zero or less" + std::to_string(l_routing_nodes[n].occupancy) + "    " + n->getFullName());
        }
    }

    l_mapping[val].clear();

    return;
}

double PathFinder::getCost(const MRRGNode* n) {
    int oc = l_routing_nodes[n].occupancy;
    int cap = n->capacity;
    double history_cost = l_routing_nodes[n].hCost;
    double base_cost = l_routing_nodes[n].baseCost;
    // return (base_cost+ history_cost) * (oc <= cap ? 1 : (oc - cap));
    double h_cost = (l_h_factor*history_cost);
    double oc_cost = (l_p_factor*(oc <= cap ? 1 : (oc - cap)));
    return base_cost + h_cost + oc_cost;
}

void PathFinder::mapMRRGNode(const OpGraphVal* val, const MRRGNode* node) {
    l_mapping[val].push_back(node);
    l_routing_nodes[node].occupancy++;
    l_routing_nodes[node].mapped_values.push_back(val);
}

bool PathFinder::routeVal(const OpGraphVal* val) {
    if (l_mapping[val].size()) return true;  // already routed!

    // store cost in priority queue so we can have duplicates
    struct SinkAndLatency {
        const MRRGNode* sink_node;
        const OpGraphOp* sink_op;
        Latency cycles;

        bool operator<(const SinkAndLatency& rhs) const {
            if (cycles == rhs.cycles) {
                return sink_op < rhs.sink_op;
            }
            return cycles > rhs.cycles;  // && cycles > rhs.cycles;
        }
    };
    int II = l_mrrg.initiationInterval();
    // Assuming each val has a single op node
    // that is mapped to a single mrrgnode
    const OpGraphOp* src_op = val->input;
    const auto& src = l_placement.at(val->input);
    int latency_of_src = (l_isElastic) ? kUndefLatency : src->latency;
    const auto& the_single_fanout = l_mrrg.fanout(src).at(0);

   // collect routing destinations with the operand tags wanted for each sink
    std::priority_queue<SinkAndLatency> sinks;
    std::map<SinkAndLatency, std::vector<OperandTag>> sink_tags;
    for (int isink_op = 0; isink_op != val->output.size(); ++isink_op) {
        const auto& sink_op = val->output.at(isink_op);
        Latency cycles_to_sink = getCyclesToSink(src_op, sink_op);
        const auto& op_tag = l_opgraph.getOperandTag(OpGraph::EdgeDescriptor{val, isink_op});
        SinkAndLatency sl = {l_placement.at(sink_op), sink_op, cycles_to_sink};
        if (sink_tags.find(sl) == sink_tags.end()) {
            sinks.push(sl);
        }
        if (std::find(sink_tags[sl].begin(), sink_tags[sl].end(), op_tag) != sink_tags[sl].end() && op_tag != Operands::BINARY_ANY) {
            throw cgrame_error("Operation: " + sink_op->getName() + " have two similar tags: " + op_tag +
                " from operation: " + src_op->getName());
        }
        sink_tags[sl].push_back(op_tag);
    }

    std::cout << "MAPPING From: "<< val->input->getName() << " ";
    for (auto op : val->output) {
        std::cout << op->getName() << " ";
    }
    std::cout << sinks.size() << "  " << sink_tags.size() << std::endl;
    std::set<const MRRGNode*> routes;
    // Do the routing. Find one sink at a time
    while (!sinks.empty()) {
        auto sink = sinks.top();
        sinks.pop();
        std::cout << *sink.sink_node << std::endl;
        for (auto tag : sink_tags[sink]) {
            std::cout << src_op->getName() << " " << sink.sink_op->getName() << "   " << sink.cycles << tag << std::endl;
            std::vector<const MRRGNode*> route =  dijkstraVisit(the_single_fanout, sink.sink_node, tag,
                val, sink.sink_op, routes, sink.cycles, latency_of_src);
            if (route.empty()) return false;
            std::copy(route.begin(), route.end(), std::inserter(routes, routes.end()));
        }
    }

    for (const auto& node : routes) {
        if (node->type == MRRG_NODE_ROUTING || node->type == MRRG_NODE_ROUTING_FUNCTION) {
            mapMRRGNode(val, node);
        }
    }
    return true;
}

/**
 * A single-threaded visit in dijkstra's algorithm-order over `graph`, starting at the
 * vertices in `initial_list`.
 */

std::vector<const MRRGNode*> PathFinder::dijkstraVisit(const MRRGNode* source,
    const MRRGNode* sink, OperandTag tag_to_find, const OpGraphVal* val, const OpGraphOp* sink_op,
    std::set<const MRRGNode*> routes, int cycles_to_sink, int latency_of_src) {
    using Cost = double;

    // Stores the current node cost and the path to this node
    struct VertexData {
        std::vector<const MRRGNode*> fanin;
        Cost lowest_known_cost;
        Latency num_of_cycles;
        bool tag_found;
        bool operator==(const VertexData& rhs) const {
            return this->lowest_known_cost == rhs.lowest_known_cost && this->fanin == rhs.fanin
                && num_of_cycles == rhs.num_of_cycles && tag_found == rhs.tag_found;
        }
    };

    // store cost in priority queue so we can have duplicates
    struct VertexAndCost {
        const MRRGNode* vid;
        Cost cost_to_here;
        Latency cycles;

        bool operator<(const VertexAndCost& rhs) const {
            return cost_to_here > rhs.cost_to_here;  // && cycles > rhs.cycles;
        }
    };

    // Data stores the current mrrgnode and its vertex data
    auto data = std::unordered_map<std::pair<MRRGNodeDesc, Latency>, VertexData, pair_hash>();
    // Stores the nodes and the visited ones
    std::priority_queue<VertexAndCost> to_visit;
    // Stores the node and if it is visited
    std::set<std::pair<MRRGNodeDesc, Latency>> visited;

    // Initialize the visit and data to the source
    to_visit.push({source, 0, latency_of_src});
    data.insert({{source, latency_of_src}, {{source}, 0, latency_of_src, false}});
    MRRGNodeDesc found = NULL;
    bool tag_found = false;
    while (!to_visit.empty() && !found) {
        // Get the first element from the queue
        const auto queue_top = to_visit.top();
        MRRGNodeDesc explore_curr = queue_top.vid;
        Latency cycles_to_curr = queue_top.cycles;
        to_visit.pop();

        // Check if this node has been explored and check if it is the lowest known cost
        if (visited.find({explore_curr, cycles_to_curr}) != visited.end()) continue;

        // Reached the sink need to check if the tag is correct otherwise go to the
        // next element
        if (!found && sink == explore_curr) {
            bool tag_ok = true;
            if(tag_to_find != Operands::UNTAGGED) {
                if (sink->type == MRRG_NODE_ROUTING_FUNCTION && tag_to_find == Operands::PREDICATE) {
                    tag_ok = data[{explore_curr, cycles_to_curr}].tag_found;
                } else if (sink->type == MRRG_NODE_FUNCTION) {
                    tag_ok = data[{explore_curr, cycles_to_curr}].tag_found;
                } 
            } 
            if ((cycles_to_sink == cycles_to_curr || cycles_to_sink == kUndefLatency) && tag_ok) {
                found = explore_curr;
            } else {
                //visited.emplace(explore_curr, cycles_to_curr);
                data.erase({explore_curr, cycles_to_curr});
            }
            continue;
        }

        for (const auto fanout : l_mrrg.fanout(explore_curr)) {
            tag_found = data[{explore_curr, cycles_to_curr}].tag_found;  // Tricky variable so double checking that it is always is set to false
            // Ignore if the node is the source node
            if (fanout == source) continue;

            // if this is an mrrg_node_function that is not the sink we are looking for then skip it
            if (fanout->type == MRRGNode_Type::MRRG_NODE_FUNCTION && fanout != sink) continue;
            if (l_routing_nodes[explore_curr].mapped_values.size() != 0 && fanout == sink) continue;

            // If this mrrg_node_routing_function and is used as a function node then skip it
            // if it is not the sink
            if (fanout->type == MRRGNode_Type::MRRG_NODE_ROUTING_FUNCTION) {
                if (l_used_rounting_function_nodes.count(fanout) != 0 && fanout != sink) continue;
            }
            
            int sink_op_int = -1;
            for (int isink_op = 0; isink_op != val->output.size(); ++isink_op) {
                const auto& op = val->output.at(isink_op);
                if (op == sink_op) {
                    sink_op_int = isink_op;
                    break;
                }
            }
            if (fanout->bitwidth < l_opgraph.getBitwidth(OpGraph::EdgeDescriptor{val, sink_op_int})) continue;

            // Since we are mapping multiple sinks we do not want the path
            // of different nodes creating shorts
            if (!routes.count(explore_curr) && routes.count(fanout)) continue;

            // Checking if the current data is better than the next data
            auto data_curr = data[{explore_curr, cycles_to_curr}];
            // if we found the tag that we are looking for but the sink this operand_pin
            // is not the sink we are looking for then skip it
            if (data_curr.tag_found && fanout != sink) continue;

            // get the cost and the cycles
            Cost cost = queue_top.cost_to_here + getCost(fanout);
            Latency cycles = data_curr.num_of_cycles;
            // Avoiding loop back
            if (std::find(data_curr.fanin.begin(), data_curr.fanin.end(), fanout)
                        != data_curr.fanin.end()) continue;

            // Set the num of cycles for this path
            if (fanout->type != MRRGNode_Type::MRRG_NODE_FUNCTION) {
                cycles = (l_isElastic) ? kUndefLatency : cycles + explore_curr->latency;  // et == HW_REG ? 1 : 0;
            }

            // checking if the cost and cycles is better for this route or no
            if (cycles > cycles_to_sink && cycles_to_sink != kUndefLatency) continue;

            // Looking for the correct tag
            if (!fanout->supported_operand_tags.empty() && tag_to_find != Operands::UNTAGGED) {
                if (fanout->type != MRRG_NODE_ROUTING_FUNCTION) {
                    if (fanout->supported_operand_tags.find(tag_to_find) == fanout->supported_operand_tags.end()) {
                        continue;
                    } else {
                        if (l_routing_nodes[fanout].mapped_values.size() == 0 && routes.count(fanout) == 0)
                            tag_found = true;
                    }
                } else if (tag_to_find == Operands::PREDICATE) {
                    if (fanout->supported_operand_tags.find(tag_to_find) == fanout->supported_operand_tags.end()) {
                        continue;
                    } else {
                        if (l_routing_nodes[fanout].mapped_values.size() == 0 && routes.count(fanout) == 0)
                            tag_found = true;
                    }
                }
                if (!tag_found) continue;
            }
            
            if (data.find({fanout, cycles}) != data.end()) {
                if (!data[{fanout, cycles}].tag_found && data_curr.tag_found) {
                    data.erase({fanout, cycles});
                } else if (data[{fanout, cycles}].tag_found && !data_curr.tag_found) {
                    continue;
                } else if (data[{fanout, cycles}].lowest_known_cost <= cost) {
                    continue;
                } else {
                    data.erase({fanout, cycles});
                }
            }

            // Update data and queue
            data[{fanout, cycles}] = {data[{explore_curr, cycles_to_curr}].fanin, cost, cycles};
            data[{fanout, cycles}].fanin.push_back(explore_curr);
            data[{fanout, cycles}].tag_found = tag_found;
            to_visit.push({fanout, cost, cycles});
            tag_found = false;
        }
        // Update the visited and clean up the data such
        // that we do not save all the paths
        visited.emplace(explore_curr, cycles_to_curr);
        data.erase({explore_curr, cycles_to_curr});
    }
    // TODO(raghebom)   : remove currently there for debugging purposes
    if (!found) {
        std::cout << to_visit.empty() << std::endl;
        std::cout << "NOT FOUND" << std::endl;
        std::cout << source << "    " << sink->getFullName() << "   " << data.size() << std::endl;
    } 
    std::cout << "cost " << data[{sink, cycles_to_sink}].lowest_known_cost << std::endl;
    for (auto data_i : data[{sink, cycles_to_sink}].fanin) {
        std::cout << data_i->getFullName() << std::endl;
    }
    return data[{sink, cycles_to_sink}].fanin;
}


void PathFinder::parsePlacementFile() {
    ConfigGraph parsed_placement_dot;
    std::cout << "[INFO] Getting placement information from DOT file: " << l_placement_filename<< std::endl;
    auto placement_ifstream = std::ifstream(l_placement_filename);
    parsed_placement_dot = parseDot(placement_ifstream, l_placement_filename);

    // Parse opnodes and the mrrg nodes that they corrsponed to
    // Then place them in a map from op to mrrg node
    for (const auto& v : parsed_placement_dot.allNodes()) {
        const auto& v_attrs = parsed_placement_dot.attributes(v);
        const bool isOp = v_attrs.hasKey("maps_to_op");

        if (!isOp) {
            throw make_from_stream<cgrame_error>([&](auto&& s) {
                s << "mrrg node: " << parsed_placement_dot.name(v) << " is not mapped to anything";
            });
        }

        std::string mrrg_node_name = parsed_placement_dot.name(v);
        std::string op_name;
        op_name = v_attrs.getString("maps_to_op");

        const MRRGNode* mrrg_ndesc = l_mrrg.getNode(v_attrs.getInt("cycle"), v_attrs.getString("name"));
        if (mrrg_ndesc == nullptr) {
            throw make_from_stream<cgrame_error>([&](auto&& s) {
                s << "[ERROR]: mrrg node: " << v_attrs.getInt("cycle") << ":" <<
                    v_attrs.getString("name") << " is not part of specified architecture";
            });
        }

        const OpGraphNode* op_ndesc = l_opgraph.getOp(op_name);
        if (op_ndesc == nullptr) {
            throw make_from_stream<cgrame_error>([&](auto&& s) {
                s << "[ERROR]: op node: "<< op_name << " is not part of specified dfg graph";
            });
        }
        l_placement.emplace(op_ndesc, mrrg_ndesc);
    }
    return;
}
