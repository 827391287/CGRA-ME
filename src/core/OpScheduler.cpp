/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/
#include <CGRA/OpScheduler.h>
#include <CGRA/Exception.h>
#include <CGRA/GraphAlgorithms.h>
#include <CGRA/utility/ConfigGraph.h>
#include <CGRA/MRRGProcedures.h>
#include <CGRA/dotparse.h>

#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <limits>
#include <stack>
#include <fstream>

#include <gurobi_c++.h> //NOLINT



// Op scheduler class to create a schedule to the operations within the opgraph
OpScheduler::OpScheduler(const OpGraph& opgraph,
    const MRRG& mrrg,
    std::string arch_name,
    std::string supported_ops_file_name)
    : op_graph(opgraph)
    , mrrg(mrrg) {
    // If no directory is given to the scheduler look within the
    // sched directory to find a supported ops file
    std::string network_latency_filename = supported_ops_file_name;
    if (supported_ops_file_name.empty()) {
        std::string cgrame_rootdir = "CGRA_ME_ROOTDIR";
        std::string dir = std::getenv(cgrame_rootdir.c_str());
        dir = dir + "/src/sched/" + arch_name + "_supported_ops.dot";
        network_latency_filename = dir;
    }
    parseNetworkSchedule(network_latency_filename);

    // Set up the function nodes from the MRRG file
    const auto classes = computeNodeClassLists(mrrg);
    for (const auto& mrrg_node : classes.function_nodes) {
        function_nodes.push_back(mrrg_node);
    }
}

void OpScheduler::parseNetworkSchedule(std::string network_latency_filename) {
    // Parsing the network schedule file
    ConfigGraph parsed_network_latency_dot;
    std::cout << "[INFO] Getting network latency from DOT file: " << network_latency_filename<< std::endl;
    auto network_latency_ifstream = std::ifstream(network_latency_filename);
    parsed_network_latency_dot = parseDot(network_latency_ifstream, network_latency_filename);

    for (const auto& v : parsed_network_latency_dot.allNodes()) {
        // Setting up the operation latency
        const auto& v_attrs = parsed_network_latency_dot.attributes(v);
        op_latency.emplace(opcode_from_string(parsed_network_latency_dot.name(v)), v_attrs.getInt("OP_LATENCY"));

        for (const auto& out_edge : parsed_network_latency_dot.outEdges(v)) {
            // Setting up the edges upper and lower latencies
            const auto& e_attrs = parsed_network_latency_dot.attributes(out_edge);
            std::string out_name = parsed_network_latency_dot.name(v);
            std::string in_name =  parsed_network_latency_dot.name(parsed_network_latency_dot.target(out_edge));
            OpGraphOpCode out_op_code = opcode_from_string(out_name);
            OpGraphOpCode in_op_code = opcode_from_string(in_name);
            OpCodeEdge op_code_edge = {out_op_code, in_op_code};
            lower_bound_op_code_edge.emplace(op_code_edge, e_attrs.getInt("LOWER_BOUND_NETWORK_LATENCY"));
            upper_bound_op_code_edge.emplace(op_code_edge, e_attrs.getInt("UPPER_BOUND_NETWORK_LATENCY"));
        }
    }
    return;
}

bool OpScheduler::isBackEdge(OpGraphOp* op) {
    if (op->getOpCode() == OpCode::CONST
        || op->getOpCode() == OpCode::PHI
        || op->getOpCode() == OpCode::INPUT
        || op->getOpCode() == OpCode::INPUT_PRED)
        return false;

    OpGraphVal* val = op->output;
    if (!val) return false;
    for (auto& sink_op : val->output) {
        if (sink_op->getOpCode() == OpCode::PHI)
            return true;
        if (sink_op == op)
            return true;
    }
    return false;
}

int OpScheduler::calcLowerBound(OpGraphOp* op) {
    if (op_latency.find(op->getOpCode()) == op_latency.end())
        throw cgrame_error("ERROR: Cannot find Op Code: " + to_string(op->getOpCode()) + " within the supported ops");
    if (op->getOpCode() != OpCode::PHI) {
        return 0;
    }
    int r = 0;

    for (auto& edge : op->input) {
        if (edge->input->getOpCode() == OpCode::CONST) {
            r = std::max(r , op_latency[OpCode::CONST] + lower_bound_op_code_edge[{OpCode::CONST, OpCode::PHI}]);
        } else if (edge->input->getOpCode() == OpCode::INPUT) {
            r = std::max(r , op_latency[OpCode::INPUT] + lower_bound_op_code_edge[{OpCode::INPUT, OpCode::PHI}]);
        }
    }
    return op_latency[op->getOpCode()];
}

int OpScheduler::calcEdgeDist(OpGraphVal* val, OpGraphOp* sink_op, unsigned II, bool ignore_backedge) {
    OpGraphOp* source_op = val->input;

    if (source_op == sink_op) return -1;
    if (ignore_backedge) {
        if (sink_op->getOpCode() == OpCode::PHI
            && source_op->getOpCode() != OpCode::INPUT
            && source_op->getOpCode() != OpCode::CONST
            && source_op->getOpCode() != OpCode::INPUT_PRED) {
                 return -1;
            }
    }

    int latency = 0;
    switch (val->getKind()) {
        case EdgeKind::kDataFlow :
            return op_latency[source_op->getOpCode()] +
            lower_bound_op_code_edge[{source_op->getOpCode(), sink_op->getOpCode()}];
        case EdgeKind::kAlias :
            if (source_op->getOpCode() == OpCode::LOAD) {
                if (sink_op->getOpCode() != OpCode::STORE)
                    throw cgrame_error("ERR: Alias edge: " + val->getName() + " does not connect a load to a store");
                latency = 1 + lower_bound_op_code_edge[{source_op->getOpCode(), sink_op->getOpCode()}];
            } else if (source_op->getOpCode() == OpCode::STORE) {
                if (sink_op->getOpCode() == OpCode::STORE) {
                    latency = 1;
                } else if (sink_op->getOpCode() == OpCode::LOAD) {
                    latency = op_latency[source_op->getOpCode()]
                        + lower_bound_op_code_edge[{source_op->getOpCode(), sink_op->getOpCode()}];
                } else {
                    throw cgrame_error("ERR: Alias edge: " + val->getName()
                        + " does not connect store to another store/load");
                }
            } else {
                throw cgrame_error("ERR: Alias edge: " + val->getName() + " does not connect to a store or a load");
            }
            return -latency + (II * val->dist);
        default :
            assert(0 && "ERROR: edge does not have a kind");
            return 0;
    }
    return 0;
}

int OpScheduler::addOpConstr(GRBmodel* model, unsigned II, int max_step, bool ignore_backedge, int exit_id) {
    std::vector<bool> is_exit_pred(op_graph.opNodes().size(), true);

    // Constraint format s_a - s_b <= cnst
    const int diff_constr_var_count = 2;

    int     ind[diff_constr_var_count];
    double  value[diff_constr_var_count];
    for (auto op_pair : extended_sched_const) {
        ind[0] = op_graph.getOpIndex(op_pair.first.first);
        ind[1] = op_graph.getOpIndex(op_pair.first.second);
        value[0] = +1.0;
        value[1] = -1.0;
        double fd = -(static_cast<double>(op_pair.second));
        std::string val_name = "m_" + std::to_string(op_graph.getOpIndex(op_pair.first.first));
        val_name = val_name + "_" + std::to_string(op_graph.getOpIndex(op_pair.first.second));
        int err = GRBaddconstr(model, diff_constr_var_count, ind, value, GRB_LESS_EQUAL, fd, val_name.c_str());
    }

    for (auto& edge_v : {&op_graph.valNodes(), &op_graph.aliasNodes()}) {
        for (const auto& val : *edge_v) {
            OpGraphOp* source_op = val->input;
            for (const auto& sink_op : val->output) {
                int dist = calcEdgeDist(val, sink_op, II, ignore_backedge);
                if (dist == -1) continue;
                ind[0] = op_graph.getOpIndex(source_op);
                ind[1] = op_graph.getOpIndex(sink_op);
                value[0] = +1.0;
                value[1] = -1.0;

                if (sink_op->getOpCode() == OpCode::PHI &&
                    source_op->getOpCode() != OpCode::INPUT &&
                    source_op->getOpCode() != OpCode::CONST &&
                    source_op->getOpCode() != OpCode::INPUT_PRED)
                    dist = max_step;  // II*dist - op_latency[source_op->getOpCode()] - 1;

                double fd = -(static_cast<double>(dist));
                std::string val_name;
                if (val->getKind() == EdgeKind::kDataFlow) {
                    val_name = "v_" + std::to_string(op_graph.getValIndex(val));
                    val_name = val_name + "_" + std::to_string(op_graph.getOpIndex(sink_op));
                } else if (val->getKind() == EdgeKind::kAlias) {
                    val_name = "a_" + std::to_string(op_graph.getAliasIndex(val));
                    val_name = val_name + "_" + std::to_string(op_graph.getOpIndex(sink_op));
                } else {
                    throw cgrame_error("Edge kind is not recognised");
                }
                int err = GRBaddconstr(model, diff_constr_var_count, ind, value, GRB_LESS_EQUAL, fd, val_name.c_str());

                // TODO(raghebom) :: REMOVE THIS HARD CODE::
                if (source_op->getOpCode() == OpCode::CONST)  {
                    val_name = val_name + "_1";
                    int err =
                        GRBaddconstr(model, diff_constr_var_count, ind, value, GRB_GREATER_EQUAL, fd, val_name.c_str());
                }
                
                if (source_op->getOpCode() == OpCode::PHI && source_op->output->output.size() == 1)  {
                    val_name = val_name + "_1";
                    int err =
                        GRBaddconstr(model, diff_constr_var_count, ind, value, GRB_GREATER_EQUAL, fd, val_name.c_str());
                }

                if ( err ) return err;

                if (exit_id != -1 && val->getKind() != EdgeKind::kAlias)
                    is_exit_pred[ind[0]] = false;
            }
        }
    }


    // exit dependencies ??
    if (exit_id != -1) {
        for (auto& op : op_graph.opNodes()) {
            if (is_exit_pred[op_graph.getOpIndex(op)]) {
                ind[0] = op_graph.getOpIndex(op);
                ind[1] = exit_id;
                value[0] = +1.0;
                value[1] = -1.0;
                int d = isBackEdge(op) ? (op_latency[op->getOpCode()] + 1) : 0;

                double fd = -(static_cast<double>(d));
                std::string nm = "x_" + std::to_string(ind[0]);
                int err = GRBaddconstr(model, diff_constr_var_count, ind, value, GRB_LESS_EQUAL, fd, nm.c_str());
                if (err) return err;
            }
        }
    }
    return 0;
}

int OpScheduler::schedASLAP(unsigned II, unsigned max_cycles, SchedType algo) {
    //if (algo == SchedType::ALAP) setBackEdges();
    if (algo != SchedType::ASAP && algo != SchedType::ALAP) return -1;
    std::vector<OpGraphOp*> ops = op_graph.opNodes();
    int node_count = ops.size();
    int ext_node_count = node_count;
    int exit_id = ext_node_count++;

    GRBenv* env = NULL;
    GRBmodel* model = NULL;
    int err = 0;

    double* sol = new double[ext_node_count];
    double* obj = new double[ext_node_count];
    double* lower_bound = new double[ext_node_count];
    double* upper_bound = NULL;
    int opt_status;
    double obj_val;
    int result_max_step = -1;

    const char* log_file_nm = (algo == SchedType::ASAP) ? "asap.log" : "alap.log";
    const char*  model_nm = (algo == SchedType::ASAP) ? "asap" :"alap";
    const char* lp_file_nm = (algo == SchedType::ASAP) ? "asap.lp" :"alap.lp";
    const char* iis_file_nm = (algo == SchedType::ASAP) ? "asap.ilp" :"alap.ilp";

    // Create envirtoment and set the output to a logfile
    err = GRBemptyenv(&env);
    if (err) goto QUIT;
    err = GRBsetstrparam(env, "LogFile", log_file_nm);
    if (err) goto QUIT;
    err = GRBsetintparam(env, "LogToConsole", 0);
    if (err) goto QUIT;
    err = GRBstartenv(env);
    if (err) goto QUIT;

    // Create an empty model
    err = GRBnewmodel(env, &model, model_nm, 0, NULL, NULL, NULL, NULL, NULL);
    if (err) goto QUIT;

    // set variable lower bounds
    memset(lower_bound, 0, ext_node_count * sizeof(double));
    for (int i = 0; i < node_count; i++) {
        lower_bound[i] = 0;
    }
    lower_bound[exit_id] = 0;

    if (algo == SchedType::ALAP) {
        upper_bound = new double[ext_node_count];
        for (int i = 0; i < node_count; i++) {
            upper_bound[i] = max_cycles;
        }
        upper_bound[exit_id] = max_cycles;
    }

    for (int i = 0; i < node_count; i++) {
        obj[i] = 1.0;
    }
    obj[exit_id] = 1.0;

    err = GRBaddvars(model, ext_node_count, 0, NULL, NULL, NULL, obj, lower_bound, upper_bound, NULL, NULL);
    if (err) goto QUIT;

    err = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, (algo == SchedType::ASAP) ? GRB_MINIMIZE : GRB_MAXIMIZE);
    if (err) goto QUIT;

    err = addOpConstr(model, II, (algo == SchedType::ASAP) ? -1 : max_cycles, (algo == SchedType::ASAP), exit_id);
    if (err) goto QUIT;

    err = GRBoptimize(model);
    if (err) goto QUIT;

    err = GRBwrite(model, lp_file_nm);
    if (err) goto QUIT;

    err = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &opt_status);
    if (err) goto QUIT;

    if (opt_status == GRB_INFEASIBLE) {
        for (int i = 0; i < ops.size(); i++) {
            std::cout << i << "  " << ops[i]->getName() << std::endl;
        }
        GRBcomputeIIS(model);
        GRBwrite(model, iis_file_nm);
        return -1;
    }
    assert(opt_status == GRB_OPTIMAL);

    err = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &obj_val);
    if (err) goto QUIT;

    err = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, ext_node_count, sol);
    if (err) goto QUIT;

    for (int i = 0; i < node_count; i++) {
        if (SchedType::ALAP == algo) {
            alap_schedule[op_graph.getOpByIndex(i)] = static_cast<int>(sol[i]);
        } else if (SchedType::ASAP == algo) {
            asap_schedule[op_graph.getOpByIndex(i)] = static_cast<int>(sol[i]);
        }
    }

    result_max_step = static_cast<int>(sol[exit_id]);

QUIT:

    if ( err ) {
        std::string err_msg(GRBgeterrormsg(env));
        throw cgrame_error("ERR: " + err_msg);
    }

    // Free model
    GRBfreemodel(model);

    // Free enviroment
    GRBfreeenv(env);

    delete [] sol;
    delete [] obj;
    delete [] lower_bound;
    delete [] upper_bound;

    return result_max_step;
}

int OpScheduler::calcNodePrio(OpGraphOp* op, int step) {
    assert(asap_schedule[op] <= step);
    return 1024 * (step - asap_schedule[op]) + asap_schedule[op];
}

void OpScheduler::buildLastUseTable(int& ext_node_count, int exit_id, std::map<OpGraphOp*, int>& last_use) {
    for (auto& op : op_graph.opNodes()) {
        if (isBackEdge(op)) {
            last_use.emplace(op, exit_id);
            continue;
        }

        OpGraphVal* val = op->output;
        if (!val) {
            last_use.emplace(op, exit_id);
            continue;
        }

        if (val->output.size() == 1) {
            last_use.emplace(op, op_graph.getOpIndex(val->output[0]));
        } else if (val->output.size() > 1) {
            int lu_id = ext_node_count++;
            last_use.emplace(op, lu_id);
        }
    }
}

int OpScheduler::addLastUseConstr(GRBmodel* model , std::map<OpGraphOp*, int> last_use) {
    for (auto& op : op_graph.opNodes()) {
        if (isBackEdge(op)) continue;
        OpGraphVal* val = op->output;

        if (!val) continue;
        if (val->output.size() == 1) continue;
        for (auto& sink_op : val->output) {
            int Kdiff_constr_var_count = 2;
            int ind[Kdiff_constr_var_count];
            double value[Kdiff_constr_var_count];

            int lu_id = last_use[op];
            ind[0] = op_graph.getOpIndex(sink_op);
            ind[1] = lu_id;
            value[0] = +1.0;
            value[1] = -1.0;

            double fd = 0.0;
            std::string nm = "lu" + std::to_string(ind[0]) + "_" + std::to_string(lu_id);
            int err = GRBaddconstr(model, Kdiff_constr_var_count, ind, value, GRB_LESS_EQUAL, fd, nm.c_str());
            if (err) return err;
        }
    }
    return 0;
}

int OpScheduler::schedSDCMod(unsigned II, SchedType algo) {
    if (algo != SchedType::SDCLIST && algo != SchedType::SDCLRO) return -1;

    std::vector<OpGraphOp*> ops = op_graph.opNodes();
    int node_count = ops.size();
    int ext_node_count = node_count;
    int exit_id = ext_node_count++;


    std::map<OpGraphOp*, int> last_use;
    if (algo == SchedType::SDCLRO)
        buildLastUseTable(ext_node_count, exit_id, last_use);

    GRBenv* env = NULL;
    GRBmodel* model = NULL;
    int err = 0;
    double* sol = new double[ext_node_count];
    double* obj = new double[ext_node_count];
    double* lower_bound = new double[ext_node_count];
    int opt_status;
    double obj_val;
    int result_max_step = -1;

    const char* log_file_nm = (algo == SchedType::SDCLRO) ? "lro.log" : "list.log";
    const char*  model_nm   = (algo == SchedType::SDCLRO) ? "lro" : "list";
    const char* lp_file_nm  = (algo == SchedType::SDCLRO) ? "lro.lp" : "list.lp";

    // Create envirtoment and set the output to a logfile
    err = GRBemptyenv(&env);
    if (err) goto QUIT;
    err = GRBsetstrparam(env, "LogFile", log_file_nm);
    if (err) goto QUIT;
    err = GRBsetintparam(env, "LogToConsole", 0);
    if (err) goto QUIT;
    err = GRBstartenv(env);
    if (err) goto QUIT;

    // Create an empty model
    err = GRBnewmodel(env, &model, model_nm, 0, NULL, NULL, NULL, NULL, NULL);
    if (err) goto QUIT;

    switch (algo) {
    case SchedType::SDCLIST:
        for (int i = 0; i < node_count; i++) {
            obj[i] = 1.0;
        }
        obj[exit_id] = 1.0;
        break;
    case SchedType::SDCLRO:
        for (int i = 0; i < ext_node_count; i++) {
            obj[i] = 0.0;
        }
        for (auto& op : op_graph.opNodes()) {
            if (last_use.find(op) == last_use.end()) {
                throw cgrame_error("ERR: LRO operation: " + op->getName() + " has no entry to last use table");
            }
            int op_index = op_graph.getOpIndex(op);
            obj[last_use[op]] = obj[last_use[op]] + 1.0;
            obj[op_index] = obj[op_index] - 1.0;
        }
        break;
    default:
        break;
    }
    // set variable lower bounds
    memset(lower_bound, 0, ext_node_count * sizeof(double));
    for (int i = 0; i < ext_node_count; i++) {
        lower_bound[i] = 0;
    }
    lower_bound[exit_id] = 0;
    err = GRBaddvars(model, ext_node_count, 0, NULL, NULL, NULL, obj, lower_bound, NULL, NULL, NULL);
    if (err) goto QUIT;

    err = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
    if (err) goto QUIT;

    err = addOpConstr(model, II, -1, true, exit_id);
    if (err) goto QUIT;

    if (algo == SchedType::SDCLRO) {
        err = addLastUseConstr(model, last_use);
        if (err) goto QUIT;
    }

    err = GRBoptimize(model);
    if (err) goto QUIT;

    err = GRBwrite(model, lp_file_nm);
    if (err) goto QUIT;

    err = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &opt_status);
    if (err) goto QUIT;

    if (opt_status == GRB_INFEASIBLE)
        return -1;
    assert(opt_status == GRB_OPTIMAL);

    err = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &obj_val);
    if (err) goto QUIT;

    err = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, ext_node_count, sol);
    if (err) goto QUIT;

    {
        // resource constraint
        using PN = std::pair<int, OpGraphOp*>;
        using MinPrioQueue = std::priority_queue<PN, std::vector<PN>, std::greater<PN>>;
        MinPrioQueue p_queue;

        auto mrrgNodesByCycle = computeNodeClassListsByCycle(mrrg);
        std::vector<std::vector<bool>> rsrv_station
            (II, std::vector<bool>(mrrgNodesByCycle.function_nodes[II-1].size(), false));

        int scheduled_node_count = 0;
        int step = 0;

        while (scheduled_node_count < node_count) {
            for (auto& op : op_graph.opNodes()) {
                if (sol[op_graph.getOpIndex(op)] == step) {
                    p_queue.push({calcNodePrio(op, step), op});
                }
            }
            while (!p_queue.empty()) {
                PN temp = p_queue.top();
                p_queue.pop();

                bool back_search_fail = true;
                if (temp.second->getOpCode() == OpCode::CONST || temp.second->getOpCode() == OpCode::PHI) {
                    scheduled_node_count++;
                    continue;
                }
                for (int s = step; s >= asap_schedule[temp.second]; s--) {
                    for (int i = 0; i < mrrgNodesByCycle.function_nodes[step%II].size(); i++) {
                        if (rsrv_station[s%II][i]) continue;
                        if (!mrrgNodesByCycle.function_nodes[step%II][i]->canMapOp(temp.second)) continue;
                        int ind = op_graph.getOpIndex(temp.second);
                        double val = +1.0;
                        double fd = static_cast<double>(step);
                        std::string e_nm = "rc" + std::to_string(ind);
                        int err = GRBaddconstr(model, 1, &ind, &val, GRB_EQUAL, fd, e_nm.c_str());
                        if (err) goto QUIT;
                        err = GRBwrite(model, lp_file_nm);
                        if (err) goto QUIT;
                        err = GRBoptimize(model);
                        if (err) goto QUIT;
                        err = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &opt_status);
                        if (err) goto QUIT;
                        assert(opt_status == GRB_OPTIMAL);
                        err = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, ext_node_count, sol);
                        assert(!err);
                        rsrv_station[s%II][i] = true;
                        scheduled_node_count++;
                        back_search_fail = false;
                        break;
                    }
                }
                if (back_search_fail) {
                    int ind = op_graph.getOpIndex(temp.second);
                    double val = 1.0;
                    double fd = static_cast<double>(step + 1);
                    std::string e_nm = "bf" + std::to_string(ind) + "_" + std::to_string(step);
                    int err = GRBaddconstr(model, 1, &ind, &val, GRB_GREATER_EQUAL, fd, e_nm.c_str());
                    assert(!err);
                    err = GRBoptimize(model);
                    assert(!err);
                    err = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &opt_status);
                    assert(!err);
                    if (opt_status == GRB_INFEASIBLE) {
                        goto GIVE_UP;
                    } else {
                        assert(opt_status == GRB_OPTIMAL);
                        err = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, ext_node_count, sol);
                        if (err) goto QUIT;
                    }
                }
            }
            step = step + 1;
            if (step > 10 * node_count) {
                cgrame_error("ERR: unable to schedule application!");
            }
        }

        for (int i = 0; i < node_count; i++) {
            if (algo == SchedType::SDCLIST) {
                sdc_mod_schedule[op_graph.getOpByIndex(i)] = static_cast<int>(sol[i]);
            } else if (algo == SchedType::SDCLRO) {
                sdc_lro_schedule[op_graph.getOpByIndex(i)] = static_cast<int>(sol[i]);
            }
        }
        result_max_step = static_cast<int>(sol[exit_id]);
    }

    QUIT:

    if ( err ) {
        std::string err_msg(GRBgeterrormsg(env));
        throw cgrame_error("ERR: " + err_msg);
    }

    GIVE_UP:

    // Free model
    GRBfreemodel(model);

    // Free enviroment
    GRBfreeenv(env);

    delete [] sol;
    delete [] obj;

    return result_max_step;
}

std::unordered_map<const OpGraphOp*, int> OpScheduler::getSchedule(SchedType type) {
        int II = mrrg.initiationInterval();
        int r = -1;
        r = schedASLAP(II, -1, SchedType::ASAP);
        if (r == -1)
          throw cgrame_error("ASAP FAILED");
        if (type == SchedType::ASAP) {
            return asap_schedule;
        }

        
        if (r == -1)
            throw cgrame_error("ALAP FAILED");
        if (type == SchedType::ALAP) {
            r = schedASLAP(II, r, SchedType::ALAP);
            return alap_schedule;
        }
        if (type == SchedType::SDCLIST) {
            r = schedSDCMod(II, SchedType::SDCLIST);
            if (r == -1)
                throw cgrame_error("LIST FAILED");
            return sdc_mod_schedule;
        }
        r = schedSDCMod(II, SchedType::SDCLRO);
        if (r == -1)
            throw cgrame_error("LRO FAILED");
        if (type == SchedType::SDCLRO) {
            return sdc_lro_schedule;
        }

        return alap_schedule;
}
