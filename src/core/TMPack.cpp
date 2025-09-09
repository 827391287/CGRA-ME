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

TMPack::TMPack(
    ClusteredMapperDriver& driver,
    Module* topLevelModule,
    std::shared_ptr<OpGraph> opgraph,
    const MRRG& mrrg,
    int rows, int cols,
    int II, bool isElastic)
    : ClusteredMapperDriver(driver)
    , l_opgraph(opgraph)
    , l_mrrg(mrrg)
    , l_rows(rows+2)
    , l_cols(cols+2)
    , l_reschedule(false)
    , l_II(II)
    , l_isElastic(isElastic) {
    localMRRG = topLevelModule->createMRRG(1);

    std::vector<std::vector<std::vector<MRRGNodeDesc>>> l_routing_nodes_3d (l_cols, std::vector<std::vector<MRRGNodeDesc>>(l_rows, std::vector<MRRGNodeDesc>{} ));
    std::vector<std::vector<std::vector<MRRGNodeDesc>>> l_function_nodes_3d(l_cols, std::vector<std::vector<MRRGNodeDesc>>(l_rows, std::vector<MRRGNodeDesc>{}));
    const auto classes = computeNodeClassListsByCycle(*localMRRG);

    for (const auto& mrrg_node : classes.routing_nodes[0]) {
        uint x_loc = mrrg_node->parent->loc.x_coord;
        uint y_loc = mrrg_node->parent->loc.y_coord;

        if (x_loc == UINT_MAX && y_loc != UINT_MAX) {
            for (int i = 1; i < l_cols - 1; i++) {
                l_routing_nodes_3d[i][y_loc].push_back(mrrg_node);
            }
        } else if (x_loc != UINT_MAX && y_loc == UINT_MAX) {
            for (int i = 1; i < l_rows - 1; i++) {
                l_routing_nodes_3d[x_loc][i].push_back(mrrg_node);
            }

        } else if (x_loc != UINT_MAX && y_loc != UINT_MAX) {
            l_routing_nodes_3d[x_loc][y_loc].push_back(mrrg_node);
        } else {
            throw cgrame_error("Clusterting: x & y locations are both assigned to uint max for module " + mrrg_node->parent->getName());
        } 
    }

    for (const auto& mrrg_node : classes.function_nodes[0]) {
        uint x_loc = mrrg_node->parent->loc.x_coord;
        uint y_loc = mrrg_node->parent->loc.y_coord;
        if (x_loc == UINT_MAX && y_loc != UINT_MAX) {
            for (int i = 1; i < l_cols - 1; i++) {
                l_function_nodes_3d[i][y_loc].push_back(mrrg_node);
            }
        } else if (x_loc != UINT_MAX && y_loc == UINT_MAX) {
            for (int i = 1; i < l_rows - 1; i++) {
                l_function_nodes_3d[x_loc][i].push_back(mrrg_node);
            }
        } else if (x_loc != UINT_MAX && y_loc != UINT_MAX) {
            l_function_nodes_3d[x_loc][y_loc].push_back(mrrg_node);
        } else {
            throw cgrame_error("Clusterting: x & y locations are both assigned to uint max for module " + mrrg_node->parent->getName());
        }
     }

    for (int i = 0; i < l_cols; i++) {
        for (int j = 0; j < l_rows; j++) {
            int function_node_size = l_function_nodes_3d[i][j].size();
            int routing_node_size = l_routing_nodes_3d[i][j].size();
            if (function_node_size == 0 && routing_node_size == 0) continue;
            std::string genericName = "func_" + std::to_string(function_node_size) + "_route_" + std::to_string(routing_node_size);
            if (l_function_nodes.find(genericName) != l_function_nodes.end()) {
                bool same_functions = true;
                // Check if not homogenous 
                continue;
            } else {
                l_function_nodes[genericName] = l_function_nodes_3d[i][j];
                l_routing_nodes[genericName] = l_routing_nodes_3d[i][j];
                setPortsForSubmodule(genericName);
            }
        }
    }
   
}

void TMPack::clusterMemoryOperations(std::set<OpGraphOpDesc>* visited) {
    // Check if the memory ports are limited. If it is then
    // set certain number of memories to be clustered
    if (l_ram_ports <=  0)  return;
    static int runs = 0;
    std::map<std::string, std::vector<OpGraphOp*>> l_mem_name_ops;

    // Getting all the load and store operations and placing them with the similar
    // named operations
    for (auto& op : l_opgraph->opNodes()) {
        if (op->getOpCode() == OpCode::LOAD || op->getOpCode() == OpCode::STORE) {
            if (!op->getName().empty())
                l_mem_name_ops[op->getMemName()].push_back(op);
        }
    }

    for (auto mem_name_op : l_mem_name_ops) {
        // Checking if the size of the differnt operations could fit
        // the CGRA
        if (mem_name_op.second.size() > (l_ram_ports * l_II)) {
            throw cgrame_error("Err: Memory: " + mem_name_op.first + " has "
                + std::to_string(mem_name_op.second.size()) +
                " load/store operations. That could not fit within a CGRA connected to a RAM with "
                + std::to_string(l_ram_ports) + " ports and II of " + std::to_string(l_II) );
        }
        if (mem_name_op.second.size() == l_ram_ports) continue;

        int num_of_elements_per_port =
            ceil(static_cast<double>(mem_name_op.second.size())/static_cast<double>(l_ram_ports));

        for (int i = 0; i < mem_name_op.second.size(); i+= num_of_elements_per_port) {
            std::vector<OpGraphOpDesc> ops_v;
            // std::set<int> used_contexts;
            std::map<int, OpGraphOp*> used_contexts;
            for (int j = 0; j < num_of_elements_per_port; j++) {
                int context = l_schedule[mem_name_op.second[i+j]]%l_II;
                // if (used_contexts.count(schedule[mem_name_op.second[i+j]]%l_II) != 0) {
                if (used_contexts.find(context) != used_contexts.end()) {
                    if (runs > 5) {
                        throw cgrame_error
                            ("ERR: Two memory operations that are scheduled at the same context are scheduled to be mapped together");  // NOLINT
                    }
                    OpGraphOp* op_mem_1 = mem_name_op.second[i+j];
                    OpGraphOp* op_mem_2 = used_contexts[context];
                    std::cout << "Adding a new contraint " << op_mem_1->getName() << "  " << op_mem_2->getName() << std::endl;
                    opPair oper_pair = {op_mem_2, op_mem_1};
                    extended_sched_const.emplace(oper_pair,
                        std::abs(l_schedule[used_contexts[context]] - l_schedule[mem_name_op.second[i+j]]) + 1);
                    l_reschedule = true;
                    //    std::abs(schedule[used_contexts[context]] - schedule[mem_name_op.second[i+j]]));
                }
                ops_v.push_back(mem_name_op.second[i+j]);
                visited->emplace(mem_name_op.second[i+j]);
                l_clustered_ops.emplace(mem_name_op.second[i+j]);
                used_contexts.emplace(l_schedule[mem_name_op.second[i+j]]%l_II, mem_name_op.second[i+j]);
            }
            l_clusters.push_back(ops_v);
        }
    }
    runs++;
    return;
}

void TMPack::setPortsForSubmodule(std::string submodule) {
    std::set<MRRGNodeDesc> input_ports;
    std::set<MRRGNodeDesc> output_ports;
    for (auto& nlist : {&l_function_nodes[submodule], &l_routing_nodes[submodule]}) {
        for (auto& node : *nlist) {
            for (auto& fanout : node->fanout) {
                if (std::find(l_function_nodes[submodule].begin(), l_function_nodes[submodule].end(), fanout) == l_function_nodes[submodule].end() && 
                    std::find(l_routing_nodes[submodule].begin(), l_routing_nodes[submodule].end(), fanout) == l_routing_nodes[submodule].end()) {
                        output_ports.emplace(node);
                        continue;
                    }
            }
            for (auto& fanin : node->fanin) {
                if (std::find(l_function_nodes[submodule].begin(), l_function_nodes[submodule].end(), fanin) == l_function_nodes[submodule].end() && 
                    std::find(l_routing_nodes[submodule].begin(), l_routing_nodes[submodule].end(), fanin) == l_routing_nodes[submodule].end()) {
                        input_ports.emplace(node);
                        continue;
                    }
            }
        }
    }
    l_submodule_input_ports.emplace(submodule, input_ports);
    l_submodule_output_ports.emplace(submodule, output_ports);
}

void TMPack::getFirstOps(std::queue<OpGraphOpDesc>& operations) {
    // Get the first operations with no input operations
    for (auto &op : l_opgraph->opNodes()) {
        bool hasInput = false;
        for (auto input_val : op->input) {
            if (input_val->input && (input_val->input != op)) {
                hasInput = true;
                break;
            }
        }
        if (!hasInput) operations.emplace(op);
    }
    return;
}

void TMPack::getModulesToMapOp(OpGraphOpDesc op, std::map<std::string, MRRGNodeDesc>& modules_could_map_op) {
    for (auto& subModule : l_function_nodes) {
        for (auto& function_node : subModule.second) {
            if (function_node->canMapOp(op)) {
                modules_could_map_op.emplace(subModule.first, function_node);
                break;
            }
       }
    }
    return;
}

void TMPack::opsCouldBeClustered(
    std::vector<OpGraphOpDesc> ops, std::vector<MRRGNodeDesc> &used_nodes,
    std::vector<OpGraphOpDesc> &used_ops, std::set<OpGraphOpDesc> &visited_ops, std::string submodule) {
        std::vector<OpGraphOpDesc> next_ops;

        for (auto& next_op : ops) {
            if (std::find(used_ops.begin(), used_ops.end(), next_op) != used_ops.end()) continue;
            if (visited_ops.count(next_op) != 0) continue;

            for (auto& function_node : l_function_nodes[submodule]) {
                if (std::find(used_nodes.begin(), used_nodes.end(), function_node) != used_nodes.end()) continue;
                if (!function_node->canMapOp(next_op)) continue;
                addNextOps(&next_ops, next_op);

                used_nodes.push_back(function_node);
                used_ops.push_back(next_op);
                break;
            }
        }

        if (next_ops.empty()) return;
        opsCouldBeClustered(next_ops, used_nodes, used_ops, visited_ops, submodule);

        return;
}

bool TMPack::checkIfOutputPortsNeeded(OpGraphValDesc val, std::vector<OpGraphOpDesc> &ops) {
    // Check if there are any output ports needed for this operation
    for (auto& sink_op : val->output) {
        if (std::find(ops.begin(), ops.end(), sink_op) == ops.end()) {
            return true;
        }
    }
    return false;
}

bool TMPack::checkIfInputPortsNeeded(OpGraphOpDesc source_op, std::vector<OpGraphOpDesc> &ops) {
    for (auto& in_val : source_op->input) {
        OpGraphOpDesc connected_op = in_val->input;
        if (std::find(ops.begin(), ops.end(), connected_op) == ops.end()) {
            return true;
        }
    }
    return false;
}

void TMPack::setSourceOpNodes(std::vector<MRRGNodeDesc>& op_mrrg_nodes, 
        OpGraphOpDesc op, std::string submodule) {
    op_mrrg_nodes.clear();
    // Get mrrg nodes that could map this operation
    for (auto& function_node : l_function_nodes[submodule]) {
        if (function_node->canMapOp(op)) {
            op_mrrg_nodes.push_back(function_node);
        }
    }
}

void TMPack::setReachesOutputPorts(std::map<MRRGNodeDesc, std::set<MRRGNodeDesc>>& connected_nodes,
                std::vector<MRRGNodeDesc>& op_mrrg_nodes, std::string submodule) {
    // Check if any of the source mrrg nodes reaches an output port
    for (auto& op_node : op_mrrg_nodes) {
        for (auto& output_node : l_submodule_output_ports[submodule]) {
            if (output_node->bitwidth < op_node->bitwidth) continue;
            bool reached_output_port = isReachable(op_node, output_node, submodule);
            if (reached_output_port) {
                connected_nodes[op_node].emplace(output_node);
            }
        }
    }
    return;
}

void TMPack::setReachesInputPorts(std::map<MRRGNodeDesc, std::set<MRRGNodeDesc>>& connected_nodes,
                std::vector<MRRGNodeDesc>& op_mrrg_nodes, std::string submodule) {
    // Check if any of the source mrrg nodes reaches an output port
    for (auto& op_mrrg_node : op_mrrg_nodes) {
        for (auto& input_node : l_submodule_input_ports[submodule]) {
            bool reached_input_port = isReachable(input_node, op_mrrg_node, submodule);
            if (reached_input_port) {
                connected_nodes[op_mrrg_node].emplace(input_node);
            }
        }
    }
    return;
}

bool TMPack::checkPortConn(std::vector<std::pair<OpGraphOpDesc, OpGraphOpDesc>>& global_port_connection, int i,
        std::set<MRRGNodeDesc> ports_used, 
        std::set<OpGraphOpDesc> out_port_found,
        std::map<OpGraphOpDesc, std::set<MRRGNodeDesc>>& op_to_port_nodes, 
        std::string submodule, bool isInput) {
    if (global_port_connection.size() == i) {
        return true;
    }   
    if ((ports_used == l_submodule_input_ports[submodule] & isInput) | (ports_used == l_submodule_output_ports[submodule] & !isInput)) {
        return false;
    }
    std::set<MRRGNodeDesc>& ports_of_op = op_to_port_nodes[global_port_connection[i].first];
    // Since outputs could have multi-fanout signals we need to check if the op connects to a single output 
    // While input needs a unique input for each signal
    if (out_port_found.count(global_port_connection[i].first) != 0) {
        return checkPortConn(global_port_connection, ++i, ports_used, out_port_found, op_to_port_nodes, submodule, isInput);
    }
    for (auto& mrrg_node : ports_of_op) {
        if (ports_used.count(mrrg_node) == 0) {
            ports_used.emplace(mrrg_node);
            if (!isInput) out_port_found.emplace(global_port_connection[i].first);
            if (checkPortConn(global_port_connection, ++i, ports_used, out_port_found, op_to_port_nodes, submodule, isInput)) {
                return true;
            } else {
                ports_used.erase(mrrg_node);
            }
        }
    }
    return false;
}

bool TMPack::checkPortsOfOperation(std::vector<std::pair<OpGraphOpDesc, OpGraphOpDesc>> global_port_connection, 
        std::map<OpGraphOpDesc, std::vector<MRRGNodeDesc>>& ops_mrrg_nodes,
        std::set<OpGraphOpDesc> exclusive_port_operation, std::string submodule, bool isInput) {
    
    std::map<MRRGNodeDesc, std::set<MRRGNodeDesc>> connected_nodes;
    std::map<OpGraphOpDesc, std::set<MRRGNodeDesc>> op_to_port_nodes;
    for (const auto& op_pair : global_port_connection) {
        if (isInput) {
            setReachesInputPorts(connected_nodes, ops_mrrg_nodes[op_pair.first], submodule);
        } else {
            setReachesOutputPorts(connected_nodes, ops_mrrg_nodes[op_pair.first], submodule);
        }
        // Check if there a connection exist for each of the mrrg nodes of the operation
        // otherwise remove the node from the list
        std::vector<MRRGNodeDesc> nodes_to_remove;
        for (auto& mrrg_node : ops_mrrg_nodes[op_pair.first]) {
            // Remove node if it cannout be found within the connection list
            if (connected_nodes.find(mrrg_node) == connected_nodes.end()) {
                nodes_to_remove.push_back(mrrg_node);
                continue;
            }
            // For the connected nodes set the opgraphop of the node
            if (connected_nodes[mrrg_node].size() != 0) {
                for (auto& input_node : connected_nodes[mrrg_node]) {
                    op_to_port_nodes[op_pair.first].emplace(input_node);
                }
            }
        } 
        // remove the nodes that could not be connected
        for (auto& mrrg_node : nodes_to_remove) {
            ops_mrrg_nodes[op_pair.first].erase(remove(ops_mrrg_nodes[op_pair.first].begin(), ops_mrrg_nodes[op_pair.first].end(), mrrg_node), ops_mrrg_nodes[op_pair.first].end());
        }
        // If the operation has an input/output from within and outside of the cluster and cannot reach any of the nodes
        // then apply re-computation. 
        if (op_to_port_nodes[op_pair.first].size() == 0 && exclusive_port_operation.count(op_pair.first) == 0) {
            if (isInput) {
                resolveReachability(op_pair.second, op_pair.first, op_pair.second->output);
            } else {
                resolveReachability(op_pair.first, op_pair.second, op_pair.first->output);
            }
            // if we added an operation we need to break and repeat
            if (l_reschedule) return false;
        } else if (op_to_port_nodes[op_pair.first].size() == 0){
            if (isInput) {
                throw cgrame_error("Input ERR: cannot connect operation: " + op_pair.second->getName() +
                     " to operation: " + op_pair.first->getName() + ". \n");
            } else {
                throw cgrame_error("Output ERR: cannot connect operation: " + op_pair.first->getName() +
                     " to operation: " + op_pair.second->getName() + ". \n");
            }
        }
    }
    if (op_to_port_nodes.size() == 1 ) return true;
    // Making sure that there is not more ports than ops and that for each port there exist an input/output
    bool reachable = checkPortConn(global_port_connection, 0, {}, {}, op_to_port_nodes, submodule, isInput);
    if (!reachable) {
        //Place operations in different clusters
        if (op_to_port_nodes.size() < 2) {
            throw cgrame_error("ERR: operation: could not connect all output/input ports. \n");
        }
        // Find the port with the minimum nodes to connect to then set it to be clustered alone
        OpGraphOpDesc op_to_cluster_alone = NULL;
        int min_inputs = INT_MAX;
        for (auto& op_port_nodes : op_to_port_nodes) {
            if (min_inputs >= op_port_nodes.second.size()) {
                min_inputs = op_port_nodes.second.size();
                op_to_cluster_alone = op_port_nodes.first;
            }
        }
            auto& val = op_to_cluster_alone->output;
            if (!val) {
                throw cgrame_error("ERR: operation: " + op_to_cluster_alone->name +
                     " could not not be placed. \n");
            }
            for (auto& output_op : val->output){
                l_ops_not_clustered.insert({op_to_cluster_alone, output_op});
            }
            for (auto& in_val : op_to_cluster_alone->input) {
              OpGraphOpDesc connected_op = in_val->input;
              l_ops_not_clustered.insert({connected_op, op_to_cluster_alone});
            }
    }
    return reachable;
}

bool TMPack::checkConnectedOperations(std::pair<OpGraphOpDesc, OpGraphOpDesc> op_pair, std::map<OpGraphOpDesc, std::vector<MRRGNodeDesc>>& ops_mrrg_nodes,
        std::string submodule) {
    std::vector<MRRGNodeDesc> nodes_to_remove;
    Latency cycles_to_sink = getCyclesToSink(op_pair.first, op_pair.second);
    for (auto source_mrrg_node : ops_mrrg_nodes[op_pair.first]) {
        bool reached_sink_port = false;
        for (auto& sink_mrrg_node : ops_mrrg_nodes[op_pair.second]) {
            reached_sink_port = isReachable(source_mrrg_node, sink_mrrg_node, submodule, cycles_to_sink, op_pair.first->output);
            if (reached_sink_port) {
                break;
            }
        }
        if (!reached_sink_port) {
            nodes_to_remove.push_back(source_mrrg_node);
        }
    }
    
    // remove the nodes that could not be connected
    for (auto& mrrg_node : nodes_to_remove) {
        ops_mrrg_nodes[op_pair.first].erase(remove(ops_mrrg_nodes[op_pair.first].begin(), ops_mrrg_nodes[op_pair.first].end(), mrrg_node), ops_mrrg_nodes[op_pair.first].end());
    }
    if (ops_mrrg_nodes[op_pair.first].empty()) return false;
    
    return true;

    // Now I need to check the conditions at which I cannot cluster together operations
}

bool TMPack::checkReachbitlity(std::vector<OpGraphOpDesc> &ops, std::string submodule) {
    if (ops.size() == 1) return true;
    std::map<OpGraphOpDesc, std::vector<MRRGNodeDesc>> ops_mrrg_nodes;
    bool is_input_reachable = false;
    bool is_output_reachable = false;
    bool is_connected_reachable = false;
    std::set<OpGraphOpDesc> exclusive_input_operation;
    std::set<OpGraphOpDesc> exclusive_output_operation;
    std::set<OpGraphOpDesc> non_exclusive_port_operation;
    std::vector<std::pair<OpGraphOpDesc, OpGraphOpDesc>> global_input_connection;
    std::vector<std::pair<OpGraphOpDesc, OpGraphOpDesc>> connected_ops;
    std::vector<std::pair<OpGraphOpDesc, OpGraphOpDesc>> global_output_connection;
    for (auto& operation: ops) {
        // Get the mrrg nodes that could map this op
        std::vector<MRRGNodeDesc> mrrg_nodes_to_map_op;
        setSourceOpNodes(mrrg_nodes_to_map_op, operation, submodule);
        ops_mrrg_nodes[operation] = mrrg_nodes_to_map_op;
    
        // check if the operations has inputs outside of the interconnect
        for (auto& in_val : operation->input) {
            OpGraphOpDesc connected_op = in_val->input;
            if (std::find(ops.begin(), ops.end(), connected_op) == ops.end()) {
                global_input_connection.push_back({operation, connected_op});
                exclusive_input_operation.emplace(operation);
            }
        }

        // Getting the output
        auto& val = operation->output;
        if (!val) return true;
        // Get output of the operation and check if it needs to connect to the global interconnect or 
        // the connection is within the cluster
        for (auto& sink_op : val->output) {
            if (std::find(ops.begin(), ops.end(), sink_op) == ops.end()) {
                global_output_connection.push_back({operation, sink_op});
                exclusive_output_operation.emplace(operation);
            } 
        }
        for (auto& sink_op: val->output) {
            if (operation == sink_op) continue;
            if (std::find(ops.begin(), ops.end(), sink_op) != ops.end()) {
                connected_ops.push_back({operation, sink_op});
                if (exclusive_input_operation.count(sink_op) != 0) {
                    exclusive_input_operation.erase(sink_op);
                    non_exclusive_port_operation.emplace(sink_op);
                }
                if (exclusive_output_operation.count(operation) != 0) {
                    exclusive_output_operation.erase(operation);
                    non_exclusive_port_operation.emplace(operation);
                }
            }
        }

    }
    is_input_reachable = checkPortsOfOperation(global_input_connection, ops_mrrg_nodes, exclusive_input_operation, submodule, true /*isInput*/);
    if (!is_input_reachable) return false;
    is_output_reachable = checkPortsOfOperation(global_output_connection, ops_mrrg_nodes, exclusive_output_operation, submodule, false /*isInput*/);
    if (!is_output_reachable) return false;
    for (auto& op_pair : connected_ops) {
        is_connected_reachable = checkConnectedOperations(op_pair, ops_mrrg_nodes, submodule);
        if (!is_connected_reachable && non_exclusive_port_operation.count(op_pair.first) == 0 && non_exclusive_port_operation.count(op_pair.second) == 0) {
            throw cgrame_error("ERR: cannot connect operation: " + op_pair.first->getName() +
                 " to operation: " + op_pair.second->getName() + ". \n");
        } 
        else if (!is_connected_reachable) {
            l_ops_not_clustered.insert({op_pair.first, op_pair.second});
            l_ops_not_clustered.insert({op_pair.second, op_pair.first});

            return false;
        }
    }
    for (auto& op_mrrg_nodes : ops_mrrg_nodes) {
        std::vector<MRRGNodeDesc> mrrg_nodes_to_map_op;
        setSourceOpNodes(mrrg_nodes_to_map_op, op_mrrg_nodes.first, submodule);
        
        if (mrrg_nodes_to_map_op.size() != op_mrrg_nodes.second.size()) {
            for (auto& node : op_mrrg_nodes.second) {
                if (node->type != MRRG_NODE_FUNCTION && node->type != MRRG_NODE_ROUTING_FUNCTION) throw cgrame_error ("Trying to tie an operation " + op_mrrg_nodes.first->name + " to none function node " + node->name);
                std::string name = node->name;
                std::vector<std::string> v;
                int start, end;
                start = end = 0;
                // defining the delimitation character
                char dl = '.';
    
                while ((start = name.find_first_not_of(dl, end))
                   != std::string::npos) {
                    end = name.find(dl, start);
                    v.push_back(name.substr(start, end - start));
                }
                int vec_size = v.size();
                std::string module_operation = v[vec_size -2] + '.' + v[vec_size -1];
                l_ops_to_node_names[op_mrrg_nodes.first].push_back(module_operation);
            }
        }
    }
    return true;
}

void TMPack::resolveReachability(OpGraphOpDesc source, OpGraphOpDesc sink, OpGraphVal* val) {
    // If this is the only output of the value and the sink and source are not clustered together then error out.
    if (val->output.size() <= 1) {
         throw cgrame_error("Op: " + source->getName() + " and Op: " + sink->getName() + " could not be clustered together or routed to one another."); //NOLINT
    }

    // Create a new operation with the correct connections
    l_opgraph->unLink(val, sink);
    auto new_op = l_opgraph->insert(source->propertyClone(source->name + "_new"));
    l_opgraph->link(new_op, sink, new_op->getName() + "_out", val->bitwidth, 0, EdgeKind::kDataFlow);
    for (auto& source_inputs : source->input) {
        l_opgraph->link(source_inputs, new_op, source_inputs->getOperandForOutput(source));
    }
    for (auto val : l_opgraph->valNodes()) {
        std::cout << val->getName() << std::endl;
        std::cout << "OUTPUTS" << std::endl;
        for (auto output : val->output) {
            std::cout << output->getName() << std::endl;
        }
    }
    // if this point is reached we added a new operation and we need to repeat the
    // clustering process from the start
    l_reschedule = true;
    return;
}

void TMPack::addNextOps(std::vector<OpGraphOpDesc>* next_ops, OpGraphOpDesc op) {
    if (op->output) {
        for (auto& nxt_op : op->output->output) {
            if (l_ops_not_clustered.count({op, nxt_op}) != 0) continue;
            next_ops->push_back(nxt_op);
        }
    }

    for (auto& input : op->input) {
        if (!input->input) continue;
        if (l_ops_not_clustered.count({input->input, op}) != 0) continue;
        next_ops->push_back(input->input);
    }
    return;
}

bool TMPack::isReachable(MRRGNodeDesc source, MRRGNodeDesc sink, std::string submodule, Latency cycles, OpGraphValDesc val) {
    if (source == sink) return false;
    // Stores the current node cost and the path to this node
    struct VertexData {
        std::vector<const MRRGNode*> fanin;
        Latency num_of_cycles;
        bool operator==(const VertexData& rhs) const {
            return this->fanin == rhs.fanin && num_of_cycles == rhs.num_of_cycles;
        }
    };
    using Vertex = std::pair<MRRGNodeDesc, Latency>;
    // Stores the nodes and the visited ones
    std::queue<Vertex> to_visit;
    // Stores the node and if it is visited
    std::unordered_map<Vertex, bool, pair_hash> visited;
    // Data stores the current mrrgnode and its vertex data
    auto data = std::unordered_map<Vertex, VertexData, pair_hash>();

    Latency c = (cycles == kUndefLatency)? kUndefLatency : 0;
    // Initialize the visit and data to the source
    to_visit.push({source, c});
    data.insert({{source, 0}, {{source}, 0}});
    bool found = false;
    while (!to_visit.empty() && !found) {
        // Get the first element from the queue
        const auto explore_curr = to_visit.front().first;
        Latency cycles_curr = to_visit.front().second;
        to_visit.pop();

        // Check if this node has been explored and check if it is the lowest known cost
        if (visited[{explore_curr, cycles_curr}]) continue;

        // Reached the sink need to check if the tag is correct otherwise go to the
        // next element
        if (!found && sink == explore_curr) {
            if (cycles_curr == kUndefLatency || cycles_curr == cycles) {
                 auto data_curr = data[{explore_curr, cycles_curr}];

                //printNodes(data_curr.fanin);
                found = true;
                continue;
            }
        }

        for (const auto& fanout : explore_curr->fanout) {
            if (std::find(l_function_nodes[submodule].begin(), l_function_nodes[submodule].end(), fanout) == l_function_nodes[submodule].end() && 
                    std::find(l_routing_nodes[submodule].begin(), l_routing_nodes[submodule].end(), fanout) == l_routing_nodes[submodule].end()) {
                
                continue;
                
            }
            //std::cout << "FANOUT " << *fanout << std::endl;
            // Ignore if the node is the source node or is a function node other than the sink
            if (fanout == source) continue;
            if (fanout->type == MRRGNode_Type::MRRG_NODE_FUNCTION && fanout != sink) continue;
            if (val) {
                if (val->bitwidth > fanout->bitwidth) continue;
            } else if (fanout->bitwidth < sink->bitwidth && fanout->bitwidth < source->bitwidth) continue;

            auto data_curr = data[{explore_curr, cycles_curr}];
            // Avoiding loop back
            if (std::find(data_curr.fanin.begin(), data_curr.fanin.end(), fanout)
                        != data_curr.fanin.end()) continue;

            Latency cycles_fanout = (cycles == kUndefLatency)? kUndefLatency : cycles_curr + explore_curr->latency;
            if (fanout->parent->loc.x_coord != explore_curr->parent->loc.x_coord && fanout->parent->loc.y_coord != explore_curr->parent->loc.y_coord) continue;
            if (visited[{fanout, cycles_fanout}]) continue;
            data[{fanout, cycles_fanout}] = {data[{explore_curr, cycles_curr}].fanin, cycles_fanout};
            data[{fanout, cycles_fanout}].fanin.push_back(explore_curr);
            to_visit.push({fanout, cycles_fanout});
        }
        // Update the visited and clean up the data such
        // that we do not save all the paths
        visited[{explore_curr, cycles_curr}] = true;
        data.erase({explore_curr, cycles_curr});
    }
    return found;
}

void TMPack::createSingleOpClusters () {
    int i = 0;
    for (auto &op : l_opgraph->opNodes()) {
        std::vector<OpGraphOpDesc> clust;
        l_op_cluster_index[op] = i;
        i++;
        clust.push_back(op);
        l_clusters.push_back(clust);
    }
    return;
}

bool TMPack::clusterPEs(bool cluster) {
    // If no clustering is set then set each operation
    // as it's own cluster
    if (!cluster) {
        createSingleOpClusters();
        return false;
    }

    std::queue<OpGraphOpDesc> operations;
    std::set<OpGraphOpDesc> visited;
    std::map<std::string, MRRGNodeDesc> modules_to_map_op;

    int latency = 0;
    // Get Operations wiht no inputs
    getFirstOps(operations);
    // Cluster operations depending on the  number of ram ports assigned
    clusterMemoryOperations(&visited);
    // Set the operations that are already clustered as first operations
    for (auto op : visited) {
        if (op->output) {
            for (auto& next_op : op->output->output) {
                operations.emplace(next_op);
            }
        }
    }
    
    // Check the modules that could map this operation
    while (!operations.empty()) {
        // get the first op and check if it is visited
        auto op = operations.front();
        operations.pop();
        if (visited.count(op) != 0) continue;
        // Get all modules that could map this op
        getModulesToMapOp(op, modules_to_map_op);

        if (modules_to_map_op.empty()) {
            throw cgrame_mapper_error("No submodule could map op: " + op->getName());
        }

        // Get all connected ops to this module
        std::vector<OpGraphOpDesc> next_ops;
        addNextOps(&next_ops, op);

        int max = 0;
        std::string submodule_max_ops = "";
        std::vector<OpGraphOpDesc> max_used_ops;

        // From all the submodules that could map op get the one with the largest cluster
        // TODO (raghebom) : save the different clusters and submodules to run reachibilty 
        // check on them from largest generated cluster to the smallest
        for (auto& op_submodules : modules_to_map_op) {
            std::vector<MRRGNodeDesc> used_nodes;
            std::vector<OpGraphOpDesc> used_ops;
            used_nodes.push_back(op_submodules.second);
            used_ops.push_back(op);
            opsCouldBeClustered(next_ops, used_nodes, used_ops,
                     visited, op_submodules.first);
            if (max < used_ops.size()) {
                max = used_ops.size();
                max_used_ops = used_ops;
                submodule_max_ops = op_submodules.first;
            } else if (max == used_ops.size()) {
                if (l_routing_nodes[op_submodules.first] < l_routing_nodes[submodule_max_ops]) {
                    max_used_ops = used_ops;
                    submodule_max_ops = op_submodules.first;
                }
            }
        }
        modules_to_map_op.clear();
        // make clusters for each op to place clusters not ops
        bool reachable = checkReachbitlity(max_used_ops, submodule_max_ops);
        if (l_reschedule) break;
        if (!reachable) {
            operations.push(op);
            continue;
        }

        // update the visited list and add the next operations to be clustered
        visited.emplace(op);
        if (op->output) {
            for (auto& next_op : op->output->output) {
                operations.emplace(next_op);
            }
        }
        for (auto& visited_op : max_used_ops) {
            visited.emplace(visited_op);
            if (!visited_op->output) continue;
            for (auto& next_op : visited_op->output->output) {
                operations.emplace(next_op);
            }
        }
        // Add cluster
        l_clusters.push_back(max_used_ops);

        // for sanity check will remove later
        for (auto& clustered_op : max_used_ops) {
            l_op_cluster_index[clustered_op] = l_clusters.size() - 1;
            l_clustered_ops.emplace(clustered_op);
        }
        
        modules_to_map_op.clear();
    }
    if (!l_reschedule) {
        // Sanity check
        checkIfAllOpsAreClustered();
    } else {
        // restart the clustering process
        l_clustered_ops.clear();
        for (auto& cluster : l_clusters) {
            cluster.clear();
        }
        l_clusters.clear();
        while (!operations.empty())
            operations.pop();
        visited.clear();
        modules_to_map_op.clear();
    }

    return l_reschedule;
}

void TMPack::checkIfAllOpsAreClustered() {
    for (const auto& op : l_opgraph->opNodes()) {
        if (l_clustered_ops.count(op) == 0) {
            std::cout << "Final Result" << std::endl;
            for (auto& cluster : l_clusters) {
                std::cout << "cluster" << std::endl;
                for (auto& op : cluster) {
                    std::cout << *op << std::endl;
                }
            }
            throw cgrame_mapper_error("Not all ops are included in the clusters missing: " + op->getName());
        }
    }
}

void TMPack::printOps (std::vector<OpGraphOpDesc>& ops) {
    std::cout << "Ops packed together:    " << std::endl;
    for (auto& op : ops) { 
        std::cout << op->getName() << " ";
    }
    std::cout << std::endl;
}
void TMPack::printNodes (std::vector<MRRGNodeDesc>& mrrg_nodes) {
    std::cout << "MRRG Nodes:    " << std::endl;
    for (auto& node : mrrg_nodes) { 
        std::cout << node->getHierarchyQualifiedName() << " ";
    }
    std::cout << std::endl;
}
