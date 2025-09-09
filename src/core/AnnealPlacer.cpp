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

AnnealPlacer::AnnealPlacer(
    ClusteredMapperDriver driver,
    const OpGraph& opgraph,
    const MRRG& mrrg,
    std::unordered_map<std::string, std::string> fix_ports,
    int rows, int cols, bool isElastic)
    : ClusteredMapperDriver(driver)
    , l_mrrg(mrrg)
    , l_opgraph(opgraph)
    , l_function_nodes()
    , l_fix_ports(fix_ports)
    , l_rows(rows + 2)
    , l_cols(cols + 2)
    , l_isElastic(isElastic) {

    l_II = mrrg.initiationInterval();
    l_randomInt.seed(l_rand_seed);

    const auto classes = computeNodeClassListsByCycle(mrrg);

    for (auto fix_port : fix_ports) {
        MRRGNodeDesc fixed_mrrg_node = mrrg.getNode(0, fix_port.second);
        if (!fixed_mrrg_node) throw cgrame_error("Cannot find mrrg node for fixed operation " + fix_port.first + " locked to node " + fix_port.second);
        l_fixed_mrrg_nodes.emplace(fixed_mrrg_node);
    }

    for (auto& vector_mrrg_node : classes.function_nodes) {
        for (const auto& mrrg_node : vector_mrrg_node) {
            l_function_nodes.emplace(mrrg_node, NodeAttributes());
        }
    }

    for (auto op : opgraph.opNodes()) {
        for (int i = 0; i < mrrg.initiationInterval(); i++) {
            if (l_op_to_nodes.find({op->getOpCode(), i}) != l_op_to_nodes.end()) continue;
            for (auto mrrg_node : classes.function_nodes[i]) {
                if (mrrg_node->canMapOp(op)) {
                    l_op_to_nodes[{op->getOpCode(), i}].push_back(mrrg_node);
                }
            }
        }
    }

}

double AnnealPlacer::getTotalCost() {
    double final_cost = 0.0;
    double wire_cost = 0.0;
    float cong_cost = 0.0;
    float sum = 0;
    float sos = 0;
    int k = 0;
    float size = 0;
    double q = 0;
    double cycles_to_sink;
    float max_cycles_to_sink;
    // Congestion matrix to mark the overlapping bounding boxes

    double latency_cost = 0;
    // Looping over the different nets to get the bounding boxes
    for (auto& val : l_opgraph.valNodes()) {
        size = val->output.size();
        q = 2.79 + 0.33 * (size - 3);
        max_cycles_to_sink = 0;
        OpGraphOpDesc source_op = val->input;
        MRRGNodeDesc node_in = getMappedMRRGNode(source_op);
        for (auto& sink_op : val->output) {
            cycles_to_sink = getCyclesToSink (source_op, sink_op);
            if (cycles_to_sink == kUndefLatency) cycles_to_sink = 1;
            MRRGNodeDesc node_out = getMappedMRRGNode(sink_op);

            int source_x = node_in->parent->loc.x_coord;
            int source_y = node_in->parent->loc.y_coord;
            int source_z = node_in->getContextNum();

            int sink_x = node_out->parent->loc.x_coord;
            int sink_y = node_out->parent->loc.y_coord;
            int sink_z = node_out->getContextNum();
            double manhattan_dist = std::abs(source_x - sink_x) + std::abs(source_y - sink_y) + std::abs(source_z - sink_z);
            double operation_dist = 1;
            if (!l_isElastic){
                operation_dist = l_op_sched->getLowerBound(source_op->getOpCode(), sink_op->getOpCode());
                if (operation_dist == 0) 
                    operation_dist++;
            }
            double dist = manhattan_dist;
            double cost = std::abs(dist * operation_dist - cycles_to_sink) * cycles_to_sink;
            latency_cost += cost;
        }

        BoundingBox bbxyz = getBB(val);
        wire_cost += q * ((bbxyz.x_max- bbxyz.x_min) + (bbxyz.y_max - bbxyz.y_min));
    }
    if (l_cost_func == 0 || l_isElastic) {
        final_cost = wire_cost;
    } if (l_cost_func == 1) {
        final_cost =  0.5 * wire_cost + 0.5 * latency_cost;
    }
    return final_cost;
}

BoundingBox AnnealPlacer::getBB(OpGraph::ValDescriptor val) {
    MRRGNodeDesc node_in = getMappedMRRGNode(val->input);
    int x_min = node_in->parent->loc.x_coord;
    int x_max = node_in->parent->loc.x_coord;
    int y_min = node_in->parent->loc.y_coord;
    int y_max = node_in->parent->loc.y_coord;
    int z_max = node_in->getContextNum();
    int z_min = node_in->getContextNum();

    for (OpGraph::OpDescriptor op : val->output) {
        MRRGNodeDesc node_out = getMappedMRRGNode(op);
        if (node_out->parent->loc.x_coord < x_min) {
            x_min = node_out->parent->loc.x_coord;
        }
        if (node_out->parent->loc.x_coord > x_max) {
            x_max = node_out->parent->loc.x_coord;
        }

        if (node_out->parent->loc.y_coord < y_min) {
            y_min = node_out->parent->loc.y_coord;
        }
        if (node_out->parent->loc.y_coord > y_max) {
            y_max = node_out->parent->loc.y_coord;
        }

        if (node_out->getContextNum() < z_min) {
            z_min = node_out->getContextNum();
        }
        if (node_out->getContextNum() > z_max) {
            z_max = node_out->getContextNum();
        }
    }

    return {x_max, x_min, y_max, y_min, z_max, z_min};
}

// Checks if there are any node is overused
bool AnnealPlacer::checkOveruse() {
    bool result = true;
    for (const auto& node : l_function_nodes) {
        const auto& nref = node.second;
        const auto& mrrg_nref = node.first;
        const bool overuse = l_function_nodes[mrrg_nref].occupancy > mrrg_nref->capacity;
        if (overuse && l_verbosity > 0) {
            int size_vector = l_opgraph.opNodes().size();
            OpGraph::OpDescriptor return_ptr = nullptr;
            OpGraph::NodeDescriptor node_ptr = nullptr;
            for (int i = 0; i < size_vector; i++) {
                const auto current_node = getMappedMRRGNode(l_opgraph.opNodes()[i]);
                if (current_node == node.first) {
                    return_ptr = l_opgraph.opNodes()[i];
                }
            }
        }
        result &= !overuse;
    }
    return result;
}

// Get the op that is mapped at the passed mrrg node
OpGraph::OpDescriptor AnnealPlacer::getOpMapedAt(MRRGNodeDesc n) {
    int size_vector = l_opgraph.opNodes().size();

    OpGraph::OpDescriptor return_ptr = nullptr;

    // loop through all op nodes to find same MRRG node as 'this'
    // may be very slow
    for (int i = 0; i < size_vector; i++) {
        const auto current_node = getMappedMRRGNode(l_opgraph.opNodes()[i]);
        if (current_node == n) {
            return_ptr = l_opgraph.opNodes()[i];
            break;
        }
    }

    return return_ptr;
}

// Return the MRRG node that the op is assigned to 
MRRGNodeDesc AnnealPlacer::getMappedMRRGNode(OpGraph::OpDescriptor op) {
    std::vector<MRRGNodeDesc>& mapped_nodes = l_mapping[op];
    if (mapped_nodes.size() != 0) {
        if (mapped_nodes.size() > 1) throw cgrame_error("Op should only be mapped to a single node");
        return mapped_nodes[0];
    }

    return {};  // return null if unmapped
}


// Rips up Op as well as routes in and out (if they exist) and records the cost of whatever was ripped up
// if a pointer to cost is given, the cost is also returned
void AnnealPlacer::ripUpOp(OpGraph::OpDescriptor op) {
    // unmap all nodes
    for (auto& n : l_mapping[op]) {
        if (l_function_nodes[n].occupancy-- <= 0) {
            throw cgrame_error("occupancy was already zero or less");
        }
    }
    l_mapping[op].clear();
    return;
}

// Place the following op into the given mrrg node
bool AnnealPlacer::placeOp(OpGraph::OpDescriptor op, MRRGNodeDesc n) {
    if (n->type != MRRG_NODE_FUNCTION && n->type != MRRG_NODE_ROUTING_FUNCTION) {
        throw cgrame_error("trying to place op at non-function node");
    }
    if (l_mapping[op].size() != 0)  // Make sure this op is not placed anywhere else
        return false;

    l_mapping[op].push_back(n);
    l_function_nodes[n].occupancy++;

    return true;
}

// Accept the following swap depending on the temprature and the delta cost
bool AnnealPlacer::accept(float delta_cost, float temperature) const {
    if (delta_cost < 0)
        return true;

    float probability = std::exp(-(delta_cost) / temperature);
    return probability > randomRealBetween0And1();
}

// Check if one of the ops of the cluster is fixed or not 
bool AnnealPlacer::isClusterFixed(std::vector<OpGraph::OpDescriptor> cluster) {
    for (auto& op : cluster) {
        if (l_fix_ports.find(op->getName()) == l_fix_ports.end()) continue;
        return true;
    }
    return false;
}


std::pair<bool, float> AnnealPlacer::inner_place_and_route_loop(float temperature) {
    int num_swaps = l_opgraph.opNodes().size() * l_swap_factor;
    int total_accepted = 0;
    int total_tries = 0;

    for (int i = 0; i < num_swaps; i++) {
        int vector_size = l_clusters.size();
        int index = randomUintBelow(vector_size);
        auto ops = l_clusters.at(index);
        if (isClusterFixed(ops)) continue;
        const auto fus = getRandomFUs(ops, true);
        if (fus.at(0) == getMappedMRRGNode(ops.at(0)))  // make sure that it is a different FU
            continue;
        // This is an actual attempt to swap
        total_tries++;
        // find the cost of this selected op
        double old_cost = getTotalCost();
        std::map<const OpGraphOp*, const MRRGNode*> old_placement;
        for (int j = 0; j < ops.size(); j++) {
            // check if occupied
            if (l_function_nodes[fus[j]].occupancy < fus[j]->capacity) {
                // move there
                old_placement.emplace(ops[j], getMappedMRRGNode(ops[j]));
                ripUpOp(ops[j]);
                placeOp(ops[j], fus[j]);

                
            } else {  // there must only be one unit mapped here
                assertOccupancyLimit(fus[j]);
                // swap, rip off two nodes?
                // first find the op node corresponding to this mrrg node
                // keep track of op's MRRG Node
                swap(ops[j], fus[j], old_placement);
            }
        }

        // get new cost
        double new_cost = getTotalCost();
        float delta_cost = new_cost - old_cost;

        if (accept(delta_cost, temperature)) {
            if (delta_cost > 0) {
                total_accepted++;
            }
        } else {  // restore changes
            for (auto op_node_pair : old_placement) {
                ripUpOp(op_node_pair.first);
                placeOp(op_node_pair.first, op_node_pair.second);
            }
        }
    }

    bool mrrg_overuse = checkOveruse();
    bool opgraph_covered  = isOpgraphCovered();
    bool mapped = mrrg_overuse && opgraph_covered;
    const auto accept_rate = total_accepted / std::max<float>(total_tries, 1);

    if (l_verbosity > 1) {
        //std::cout << "overuse_ok=" << mrrg_overuse << " total_cost=" << getTotalCost();
        //std::cout << " opgraph_covered=" << opgraph_covered << " opgraph_cost=" << getOpGraphCost();
        //std::cout << " accept_rate=" << total_accepted << '/' << total_tries << '=' << accept_rate;
        //std::cout << std::endl;
    }

    return std::make_pair(mapped, accept_rate);
}

void AnnealPlacer::assertOccupancyLimit(MRRGNodeDesc fu) {
    auto& fu_ref = l_function_nodes[fu];
    if (l_function_nodes[fu].occupancy > 1) {
        throw make_from_stream<std::runtime_error>([&](auto&& s) {
            s << "occupancy for functional unit node " << fu
                << " is more than 1, and multiple placement is not allowed";
        });
    }
}


bool AnnealPlacer::isOpgraphCovered() const {
    // having any nodes mapped is sufficient:
    // re-computation not supported, and we never partially route a value
    for (const auto& op : l_opgraph.opNodes()) {
        auto lookup = l_mapping.find(op);
        if (lookup == l_mapping.end() || lookup->second.empty()) {
            return false;
        }
    }
    return true;
}

// get a random unoccupied  FU
MRRGNodeDesc AnnealPlacer::getRandomUnoccupiedFU(OpGraph::OpDescriptor op) {
    std::vector<MRRGNodeDesc> candidates;
    std::vector<std::string> names_of_nodes;
    if (l_ops_to_node_names.find(op) != l_ops_to_node_names.end()) {
        names_of_nodes = l_ops_to_node_names[op];
    }

    for (const auto& fu : l_op_to_nodes[{op->getOpCode(), l_schedule[op] % l_II}]) {
        if (fu->canMapOp(op) && l_function_nodes[fu].occupancy < fu->capacity && l_fixed_mrrg_nodes.count(fu) == 0) {
            if (names_of_nodes.size() != 0) {
                for (auto& name : names_of_nodes) {
                    if (fu->getHierarchyQualifiedName().find(name) != std::string::npos) {
                        candidates.push_back(fu);
                    }
                }
            } else {
                candidates.push_back(fu);
            }
        }
    }

    if (candidates.size() < 1) {
        throw cgrame_mapper_error("The device cannot fit the design\n");
    }

    return candidates[randomUintBelow(candidates.size())];
}

MRRGNodeDesc AnnealPlacer::getFUForOpAtLoc(OpGraph::OpDescriptor op, Location loc) {
    std::vector<MRRGNodeDesc> candidates;  // = grid[op->getOpCode()][loc.x_coord][loc.y_coord];
    std::vector<std::string> names_of_nodes;

    std::vector<MRRGNodeDesc> mrrg_nodes = l_op_to_nodes[{op->getOpCode(), l_schedule[op] % l_II}];
    if (l_ops_to_node_names.find(op) != l_ops_to_node_names.end())
        names_of_nodes = l_ops_to_node_names[op];
    for (auto& node : mrrg_nodes) {
        Module* parent = node->parent;
        if (node->canMapOp(op) && parent->loc.x_coord == loc.x_coord && parent->loc.y_coord == loc.y_coord && (l_fixed_mrrg_nodes.count(node) == 0 || l_fix_ports.find(op->getName()) != l_fix_ports.end())) {
            if (names_of_nodes.size() != 0) {
                for (auto& name : names_of_nodes) {
                    if (node->getHierarchyQualifiedName().find(name) != std::string::npos) {
                        candidates.push_back(node);
                        break;
                    }
                }
            } else {
                candidates.push_back(node);
            }
        }
    }
    if (candidates.size() == 1) {
        return candidates[0];
    } else if (candidates.size() == 0) {
        return NULL;
    }
    return candidates[randomUintBelow(candidates.size())];
}

// generate a random FU
MRRGNodeDesc AnnealPlacer::getRandomFU(OpGraph::OpDescriptor op) {
    std::vector<MRRGNodeDesc> op_candidates = l_op_to_nodes[{op->getOpCode(), l_schedule[op] % l_II}];
    std::vector<std::string> names_of_nodes;
    std::vector<MRRGNodeDesc>candidates;

    if (l_ops_to_node_names.find(op) != l_ops_to_node_names.end()) {
        names_of_nodes = l_ops_to_node_names[op];
    }

    if (names_of_nodes.size() != 0) {
        for (auto& fu : op_candidates) {
                for (auto& name : names_of_nodes) {
                    if (fu->getHierarchyQualifiedName().find(name) != std::string::npos) {
                        candidates.push_back(fu);
                    }
                }
        }
    } else {
        candidates = op_candidates;
    }

    if (candidates.size() < 1) {
        throw make_from_stream<AnnealPlacer::UnmappableException>([&](auto&& s) {
            s << "Impossible to place op " << l_opgraph.getNodeRef(op);
        });
    }

    bool fixed = true;
    MRRGNodeDesc new_node;
    while (fixed) {
        new_node = candidates[randomUintBelow(candidates.size())];
        if (l_fixed_mrrg_nodes.find(new_node) == l_fixed_mrrg_nodes.end()) break;
    }

    return new_node;
}

float AnnealPlacer::determineTemperature(float scale) {
    const auto& method = l_mapper_args.getString("initial_temperature_method");
    if (method == "high_constant") {
        return 1000000.0;
    }

    if (l_verbosity > 0) {
        cout << "Finding delta Costs:" << endl;
        cout << "Total Cost: " << getTotalCost() << endl;
    }

    float max_delta_cost = 0;
    double old_cost = getTotalCost();
    std::vector<float> mean_of_runs;
    // Do 100x of perturbation
    std::map<const OpGraphOp*, const MRRGNode*> old_placement;
    for (int j = 0; j < 20; j++) {
        float mean = 0.0;
        for (int i = 0; i < l_clusters.size(); i++) {
            // choose a random op
            int vector_size = l_clusters.size();
            int index = randomUintBelow(vector_size);
            auto ops = l_clusters.at(index);
            if (isClusterFixed(ops)) continue;
            const auto fus = getRandomFUs(ops, true);
            for (int k = 0; k < ops.size(); k++) {
                if (l_function_nodes[fus[k]].occupancy < fus[k]->capacity) {
                    // if unoccupied, move there
                    ripUpOp(ops[k]);
                    placeOp(ops[k], fus[k]);
                } else {
                    // if occupied, swap
                    assertOccupancyLimit(fus[k]);
                    swap(ops[k], fus[k], old_placement);
                }
            }

            // see how the cost changed
            float change_cost = std::abs(getTotalCost() - old_cost);
            mean += change_cost;
            max_delta_cost = std::max(max_delta_cost, change_cost);
        }
        mean /= l_clusters.size();
        mean_of_runs.push_back(mean);
    }
    float sum = 0.0;
    float mean = 0.0;
    float standard_dev = 0.0;
    for (int i = 0; i < mean_of_runs.size(); i++) {
        sum += mean_of_runs[i];
    }
    mean = sum / mean_of_runs.size();
    for (int i = 0; i < mean_of_runs.size(); i++) {
         standard_dev += pow(mean_of_runs[i] - mean, 2.0);
    }
    standard_dev = sqrt(standard_dev/mean_of_runs.size());

    return standard_dev * scale;
}

void AnnealPlacer::swap(OpGraphOpDesc op, MRRGNodeDesc node,
    std::map<const OpGraphOp*, const MRRGNode*>& old_placement) {
    // swap, rip off two nodes?
    // first find the op node corresponding to this mrrg node
    // keep track of op's MRRG Node
    // check if the second op is the same as the current op
    const auto second_op = getOpMapedAt(node);
    if (second_op == op) return;
    
    const auto first_MRRGNode = getMappedMRRGNode(op);
    // If there is a cluster then swap all the nodes of the cluster
    int cluster_index = l_op_cluster_index[second_op];
    if (l_clusters[cluster_index].size() > 1) {
        std::vector<MRRGNodeDesc> nodes;
        for (int i = 0; i < l_clusters[cluster_index].size(); i++) {
            const auto op_mrrg_node = getFUForOpAtLoc(l_clusters[cluster_index][i], first_MRRGNode->parent->loc);
            if (!op_mrrg_node) throw cgrame_error("Clustering does not support hetrogenous yet");
            // check if occupied
            if (l_function_nodes[op_mrrg_node].occupancy < op_mrrg_node->capacity) {
                // move there
                old_placement.emplace(l_clusters[cluster_index][i], getMappedMRRGNode(l_clusters[cluster_index][i]));
                ripUpOp(l_clusters[cluster_index][i]);
                placeOp(l_clusters[cluster_index][i], op_mrrg_node);

                
            } else {  // there must only be one unit mapped here
                const auto s_op = getOpMapedAt(op_mrrg_node);
                const auto f_MRRGNode = getMappedMRRGNode(l_clusters[cluster_index][i]);
                old_placement.emplace(l_clusters[cluster_index][i], f_MRRGNode);
                old_placement.emplace(s_op, op_mrrg_node);

                ripUpOp(l_clusters[cluster_index][i]);
                ripUpOp(s_op);
                placeOp(l_clusters[cluster_index][i], op_mrrg_node);
                placeOp(s_op, f_MRRGNode);
            }
        }

    } else {    
        old_placement.emplace(op, first_MRRGNode);
        old_placement.emplace(second_op, node);

        ripUpOp(op);
        ripUpOp(second_op);
        placeOp(op, node);
        placeOp(second_op, first_MRRGNode);
    }
    return;
}

float AnnealPlacer::nextTemperature(float t, float accept_rate) const {
    if (accept_rate > 0.96) {
        return t * 0.5;
    } else if (accept_rate > 0.8) {
        return t * 0.9;
    } else if (accept_rate > 0.15) {
        return t * 0.95;
    } else {
        return t * 0.8;
    }
}

std::vector<MRRG::NodeDescriptor> AnnealPlacer::getRandomFUs
        (std::vector<const OpGraphOp*> cluster, bool occupied) {
    std::vector<MRRGNodeDesc> nodes;
    const MRRGNode* fixed_mrrg_node;
    bool has_fixed_port = false;
    bool satisfied = false;

    for (auto& op : cluster) {
        if (l_fix_ports.find(op->getName()) == l_fix_ports.end()) continue;
        fixed_mrrg_node = l_mrrg.getNode(0, l_fix_ports[op->getName()]);
        has_fixed_port = true;
        break;
    }

    // Loops break once all op nodes in a cluster could be placed to the
    // same place
    while (!satisfied) {
        MRRG::NodeDescriptor fu;
        if (has_fixed_port) {
            fu = fixed_mrrg_node;
        } else {
            // Getting a random FU for a random operation in the cluster
            OpGraphOpDesc clustered_op = cluster[randomUintBelow(cluster.size())];
            fu  = occupied ? getRandomFU(clustered_op) : getRandomUnoccupiedFU(clustered_op);
        }
        // For each node in the cluster gather the mrrg nodes that relates to 
        // the location
        for (auto op : cluster) {
            const auto op_mrrg_node = getFUForOpAtLoc(op, fu->parent->loc);
            if (!op_mrrg_node) break;
            if (l_function_nodes[op_mrrg_node].occupancy != 0 && !occupied) break;
            nodes.push_back(op_mrrg_node);
        }
        if (nodes.size() == cluster.size()) {
            satisfied = true;
        } else if (has_fixed_port) {
            throw cgrame_error ("MRRG Node: " + fixed_mrrg_node->name + " Cannot be fixed" );
        } else {
            nodes.clear();
        }
    }
    return nodes;
}

void AnnealPlacer::setInitialPlacement() {
    // randomized initial placement
    for (auto& cluster : l_clusters) {
        auto fus = getRandomFUs(cluster, false);
        for (int i = 0; i < cluster.size(); i++) {
            bool placed = placeOp(cluster[i], fus[i]);
            if (!placed) {
                throw cgrame_error("problem placing op   " + cluster[i]->getName());
            }
        }
    }
}

void AnnealPlacer::clearPlacement() {
    for (auto& mrrg_node : l_function_nodes) {
        mrrg_node.second.occupancy = 0;
    }
    l_mapping.clear();
}

Mapping AnnealPlacer::placeOpGraph(float temp) {
    Mapping mapping_result = l_empty_mapping_result;  // Create result obj
    std::ofstream anneal_debug;

    // Set radius and temprature
    // const float initial_temperature = determineInitialTemperature();
    float temperature = temp;

    if (l_verbosity > 1) {
        cout << "Initial Temperature is " << temperature <<endl;
        cout << "initial mapping: ";
        printMapping();
        cout << "Begin annealing" << endl;
    }

    // Getting intial cost
    double current_cost = getTotalCost();
    bool mapped = false;

    for (int iteration_num = 1;; ++iteration_num) {
        float previous_cost = current_cost;
        if (l_verbosity > 1) {
            std::cout << "Iteration start: temp=" << temperature << std::endl;
        }

        // do a bunch of swaps at the current temperature
        float accept_rate;
        std::tie(mapped, accept_rate) = inner_place_and_route_loop(temperature);

        // Get the new cost after swaps
        current_cost = getTotalCost();
        if (anneal_debug) {
            anneal_debug << temperature << "," << current_cost  << std::endl;
        }

        // Exit if there is a problem with mapping an operation
        if (!mapped) {
             throw cgrame_error("There is a op that could not be mapped");
        }
        current_cost = current_cost == 0 ? 1 : current_cost;
        // Break once the temprature becomes cold
        if ((temperature < (0.05 * (current_cost/l_clusters.size())))) break;

        // post-iteration updates
        temperature = nextTemperature(temperature, accept_rate);
    }

    printMapping();
    if (mapped) setMappingInto(mapping_result);
    return mapping_result;
}

