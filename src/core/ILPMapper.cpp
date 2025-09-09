/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/ILPMapper.h>

#include <algorithm>

#include <CGRA/CGRA.h>
#include <CGRA/ConstraintSet.h>
#include <CGRA/Exception.h>
#include <CGRA/MRRGProcedures.h>
#include <CGRA/OpGraphProcedures.h>
#include <CGRA/utility/Collections.h>

#include <gurobi_c++.h>

enum class ILPMapper::ConstraintGroup : int {
    RouteExclusivity,
    FunctionUnitExclusivity,
    OpsMustBeMapped,
    FanoutRouting,
    MuxExclusivity,
    FunctionUnitFanout,
    OpSupported,
    LinePathBalance,
    CyclePathBalance,
    EdgeUsage,
    TopologicalOrdering,
    OperandRouting,
    FunctionUnitFanin,
};

static const std::map<ILPMapper::ConstraintGroup, std::string> lazy_constraint_names {
    std::make_pair(ILPMapper::ConstraintGroup::RouteExclusivity, "route_exclusivity"),
    std::make_pair(ILPMapper::ConstraintGroup::FunctionUnitExclusivity, "function_unit_exclusivity"),
    std::make_pair(ILPMapper::ConstraintGroup::OpsMustBeMapped, "ensure_all_ops_mapped"),
    std::make_pair(ILPMapper::ConstraintGroup::FanoutRouting, "fanout_routing"),
    std::make_pair(ILPMapper::ConstraintGroup::MuxExclusivity, "mux_exclusivity"),
    std::make_pair(ILPMapper::ConstraintGroup::FunctionUnitFanout, "function_unit_fanout"),
    std::make_pair(ILPMapper::ConstraintGroup::OpSupported, "op_support"),
    std::make_pair(ILPMapper::ConstraintGroup::LinePathBalance, "line_path_balance"),
    std::make_pair(ILPMapper::ConstraintGroup::CyclePathBalance, "cycle_path_balance"),
    std::make_pair(ILPMapper::ConstraintGroup::EdgeUsage, "fake_edge_used"), // "fake" while still using general constraints
    std::make_pair(ILPMapper::ConstraintGroup::TopologicalOrdering, "fake_ordering"), // "fake" while still using general constraints
    std::make_pair(ILPMapper::ConstraintGroup::OperandRouting, "ensure that operands are routed correctly"),
    std::make_pair(ILPMapper::ConstraintGroup::FunctionUnitFanin, "function_unit_fanin"),
};

std::ostream& operator<<(std::ostream& os, const ILPMapper::ConstraintGroup& lct) {
    os << lazy_constraint_names.at(lct);
    return os;
}

namespace {

std::vector<GRBVar> makeGRBVars(GRBModel& model, int count, char type) {
    std::unique_ptr<GRBVar[]> storage(model.addVars(count, type));
    return {storage.get(), storage.get() + count};
}

struct MRRGInfo {
    auto& nodeClasses() const { return node_classes; }
    const auto& mrrg() const { return *m_mrrg; }

    const MRRG* m_mrrg;
    MRRGNodeClassLists node_classes = computeNodeClassLists(*m_mrrg);
};

}

AutoRegisterMapper ILPMapper_arm("ILPMapper",
    [](std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args) {
        return std::make_unique<ILPMapper>(cgra, timelimit, args);
    },
    false, // not a composite
    "Fully encode the mapping problem in an ILP.\n"
    "Initially presented in 'An Architecture-Agnostic Integer Linear Programming Approach to CGRA Mapping' (DAC 2018)",
    { // required args
        {"enable_topological_order_costing_by_max", 0, ""},
        {"enable_topological_order_costing_by_sum", 0, ""},
        {"enable_topological_order_mode", 0, ""},
        {"lazy_constraint_add_mode", 0, ""},
        {"max_threads", 1, ""},
        {"mip_gap", 0.2, "Relative threshold on the difference between the primal and dual cost functions to consider a solution optimal"},
        {"per_value_topological_order_variables", 0, ""},
        {"seed", 0, ""},
        {"solution_limit", 1, "Maximum number of solutions that will be considered before giving up and taking the lowest cost one found. Larger values generally lead to more optimal solutions."},
        {"model_dump_filename", "", ""},
        {"model_IIS_dump_filename", "", ""},
        {"verbosity", 0, ""},
    },
    { // optional arg regexes
        {"enable_lazy_constraint_.*", "boolean, controls whether the constraint group that follows is added lazily"},
    }
);

ILPMapper::ILPMapper(std::shared_ptr<CGRA> cgra, int timelimit, const ConfigStore& args)
    : Mapper(cgra, timelimit)
    , flags(args)
    , lazy_constraints()
    , mipgap(args.getReal("mip_gap"))
    , solnlimit(args.getInt("solution_limit"))
{
    for (auto& type_and_name : lazy_constraint_names) {
        if (args.getBoolOr("enable_lazy_constraint_" + type_and_name.second, false)) {
            lazy_constraints.insert(type_and_name.first);
        }
    }
}

// This is the main mapping function
// true on success, false on failure
Mapping ILPMapper::mapOpGraph(std::shared_ptr<OpGraph> opgraph, int II, const MRRG& mrrg, std::unordered_map<std::string, std::string> fix_port)
{
    if (flags.getInt("verbosity") > 0) {
        std::cout << "[INFO] Mapping DFG Onto CGRA Architecture..." << std::endl;
        std::cout << "[INFO] ILP Mapper Config: " << flags << '\n';
        std::cout << "[INFO] ILP Mapper lazy constraints: "; print_container(std::cout, lazy_constraints); std::cout << '\n';
    }

    auto muxExInsertionResult = muxExNodeInsertion(mrrg);

    verifyAndPrintReport(muxExInsertionResult.transformed_mrrg, std::cout, true, true, {
        { "check_mux_exclusivity", "error" },
    });

    if (flags.getInt("verbosity") > 0) {
        std::cout << "[INFO] Inserted " << (muxExInsertionResult.transformed_mrrg.size() - mrrg.size())
            << " nodes for mux exclusivity invariant\n";
    }

    // Create result obj
    Mapping mapping_result(cgra, II, opgraph);

    ILPMapperStatus mapper_status = ILPMapperStatus::UNLISTED_STATUS;
    try {
        mapper_status = GurobiMap(*opgraph, II, &mapping_result, muxExInsertionResult.transformed_mrrg, fix_port);
    } catch (...) {
        throwExceptionButWrapIfGurobiException(std::current_exception());
    }

    switch (mapper_status) {
        case ILPMapperStatus::OPTIMAL_FOUND: [[fallthrough]];
        case ILPMapperStatus::SUBOPTIMAL_FOUND:
            mapping_result.setStatus(MappingStatus::success);
            break;
        case ILPMapperStatus::TIMEOUT:
            mapping_result.setStatus(MappingStatus::timeout);
            break;
        case ILPMapperStatus::UNLISTED_STATUS:
            std::cout << "[ERROR] CGRA Mapping Results in Unlisted Status" << std::endl;
            std::cout << "[INFO] Please Report this Bug to Xander Chin(xan@ece.utoronto.ca)" << std::endl;
            [[fallthrough]];
        case ILPMapperStatus::INTERRUPTED:  [[fallthrough]];
        case ILPMapperStatus::INFEASIBLE: [[fallthrough]];
        default:
            mapping_result.setStatus(MappingStatus::failure);
            break;
    }

    return transformToOriginalMRRG(mapping_result, muxExInsertionResult);
}

ILPMapperStatus ILPMapper::GurobiMap(const OpGraph& opgraph, int II, Mapping* mapping_result, const MRRG& mrrg, std::unordered_map<std::string, std::string> fix_port)
{
    const auto& trails_to_balance = computeTrailsToBalance(opgraph);

    // Create gurobi instance
    GRBEnv env = GRBEnv(true);
    env.set(GRB_IntParam_OutputFlag, flags.getInt("verbosity") > 0);
    env.start();
    env.set(GRB_DoubleParam_MIPGap, mipgap);
    if (timelimit != 0) { env.set(GRB_DoubleParam_TimeLimit, timelimit); }
    if (solnlimit != 0) { env.set(GRB_IntParam_SolutionLimit, solnlimit); }

    GRBModel model = GRBModel(env);
    model.set(GRB_IntParam_Seed, flags.getInt("seed"));
    model.set(GRB_IntParam_Threads, flags.getInt("max_threads"));

    MRRGInfo mrrg_info {
        &mrrg,
    };

    const int num_dfg_vals  = opgraph.valNodes().size();
    const int num_dfg_ops   = opgraph.opNodes().size();
    const int num_mrrg_r    = mrrg_info.nodeClasses().routing_nodes.size();
    const int num_mrrg_f    = mrrg_info.nodeClasses().function_nodes.size();

    const auto R = makeGRBVars(model, num_dfg_vals * num_mrrg_r, GRB_BINARY);
    const auto F = makeGRBVars(model, num_dfg_ops * num_mrrg_f, GRB_BINARY);

    std::map<OpGraph::ValDescriptor, std::map<MRRG::NodeDescriptor, GRBVar>> R_var_map; // routing node in use
    std::map<OpGraph::ValDescriptor, std::map<MRRG::NodeDescriptor, std::vector<GRBVar>>> S_var_map; // routing node in use by signal
    std::map<OpGraph::ValDescriptor, std::map<MRRG::NodeDescriptor, GRBVar>> T_var_map; // ordering values
    std::map<OpGraph::OpDescriptor,  std::map<MRRG::NodeDescriptor, GRBVar>> F_var_map; // function node in use

    // secondary variables
    std::map<OpGraph::ValDescriptor, std::map<MRRG::NodeDescriptor, std::map<MRRG::NodeDescriptor, GRBVar>>> edge_used_var_map;

    // Populate maps
    int constr_count = 0;
    int j = 0;
    for (auto& val : opgraph.valNodes()) {
        const auto num_val_fanouts = (std::ptrdiff_t)opgraph.outputOps(val).size();
        int i = 0;
        for (auto& r : mrrg_info.nodeClasses().routing_nodes) {
            const auto& node_ref = mrrg.getNodeRef(r);
            const auto base_name = "R_" + node_ref.getFullName() + "_" + std::to_string(i);
            R_var_map[val][r] = R[j * num_mrrg_r + i];
            R_var_map.at(val).at(r).set(GRB_StringAttr_VarName, makeNameSolverSafe(base_name));
            auto& s_vars_for_val_and_node = S_var_map[val][r];
            if (num_val_fanouts > 1) {
                s_vars_for_val_and_node = makeGRBVars(model, num_val_fanouts, GRB_BINARY);
                for (int k = 0; k < num_val_fanouts; k++) {
                    auto& fanout_s_var = s_vars_for_val_and_node.at(k);
                    fanout_s_var.set(GRB_StringAttr_VarName, makeNameSolverSafe(base_name + "_" + std::to_string(k)));
                    model.addConstr((R_var_map.at(val).at(r)) >= fanout_s_var, "sub_val" + std::to_string(constr_count++));
                }
            } else {
                s_vars_for_val_and_node = {R_var_map.at(val).at(r)};
            }
            i++;
        }
        j++;
    }

    int p = 0;
    for (auto& op : opgraph.opNodes()) {
        int q = 0;
        for (auto& f : mrrg_info.nodeClasses().function_nodes) {
            F_var_map[op][f] = F[p * num_mrrg_f + q];
            F_var_map.at(op).at(f).set(GRB_StringAttr_VarName, "F_" + std::to_string(p) + "_" + std::to_string(q));
            q++;
        }
        p++;
    }

    // needed for the GRBVar::get calls below
    model.update();

    std::vector<GRBVar> all_T_vars;
    std::map<MRRG::NodeDescriptor, GRBVar> T_var_by_node;
    for (const auto& val_and_node_to_var_map : R_var_map) {
        for (const auto& node_and_var : val_and_node_to_var_map.second) {
            auto& val = val_and_node_to_var_map.first;
            auto& r = node_and_var.first;
            const auto& tvar = [&]() {
                if (not flags.getBool("per_value_topological_order_variables")) {
                    const auto search_result = T_var_by_node.find(r);
                    if (search_result == end(T_var_by_node)) {
                        auto res = std::unique_ptr<GRBVar[]>(model.addVars(1, GRB_INTEGER))[0]; // free the returned pointer immediately (this is the easiest way to make one var?)
                        T_var_by_node.emplace(r, res);
                        res.set(GRB_StringAttr_VarName,  "T_" + R_var_map.at(val).at(r).get(GRB_StringAttr_VarName));
                        all_T_vars.emplace_back(res);
                        return res;
                    } else {
                        return search_result->second;
                    }
                } else {
                    auto res = std::unique_ptr<GRBVar[]>(model.addVars(1, GRB_INTEGER))[0]; // free the returned pointer immediately (this is the easiest way to make one var?)
                    res.set(GRB_StringAttr_VarName,  "T_" + R_var_map.at(val).at(r).get(GRB_StringAttr_VarName));
                    all_T_vars.emplace_back(res);
                    return res;
                }
            }();
            T_var_map[val][r] = tvar;
        }
    }

    for (auto& val : opgraph.valNodes()) {
        for (auto &r: mrrg_info.nodeClasses().routing_nodes) {
            for (auto &mrrg_fanout : r->fanout) {
                if (mrrg_fanout->type != MRRG_NODE_ROUTING) {
                    continue;
                }
                const auto& rvar = R_var_map.at(val).at(r);
                const auto& rfanoutvar = R_var_map.at(val).at(mrrg_fanout);
                const auto RvalString = rvar.get(GRB_StringAttr_VarName) + "_to_" + rfanoutvar.get(GRB_StringAttr_VarName);
                auto evar = std::unique_ptr<GRBVar[]>(model.addVars(1, GRB_BINARY))[0]; // free the returned pointer immediately (this is the easiest way to make one var?)
                evar.set(GRB_StringAttr_VarName,  "Edge_used_" + RvalString);
                edge_used_var_map[val][r][mrrg_fanout] = evar;
            }
        }
    }

    const auto dot_product_of_S_and_F_vars_with_latency_for_opgraph_path = [&](const auto& path) {
        GRBLinExpr sum;
        for (const auto& edge : path) {
            for (const auto& r : mrrg_info.nodeClasses().routing_nodes) {
                sum += mrrg.getNodeRef(r).getLatency() * S_var_map.at(edge.val).at(r).at(edge.output_index);
            }
            for (const auto& f : mrrg_info.nodeClasses().function_nodes) {
                sum += mrrg.getNodeRef(f).getLatency() * F_var_map.at(opgraph.inputOp(edge.val)).at(f);
            }
        }
        return sum;
    };

    // Create and Set objective
    GRBLinExpr objective;
    for (const auto& r : R) {
        objective += r;
    }

    if (flags.getBool("enable_topological_order_costing_by_max") && flags.getBool("enable_topological_order_costing_by_sum")) {
        throw cgrame_mapper_error("can't set both enable_topological_order_costing_by_max and enable_topological_order_costing_by_sum");
    }

    if (flags.getBool("enable_topological_order_costing_by_max")) {
        auto max_tvar = std::unique_ptr<GRBVar[]>(model.addVars(1, GRB_INTEGER))[0];
        model.addGenConstrMax(max_tvar, all_T_vars.data(), all_T_vars.size(), 0, "max_order_var");
        objective += max_tvar;
    }

    if (flags.getBool("enable_topological_order_costing_by_sum")) {
        for (const auto& t : all_T_vars) {
            objective += t;
        }
    }

    model.setObjective(objective, GRB_MINIMIZE);

    ConstraintSet<ConstraintGroup> constraint_set;
    constraint_set.setVerbosity(flags.getInt("verbosity"));

    // Constraint Group - Route Exclusivity
    constraint_set.registerGenerator(ConstraintGroup::RouteExclusivity, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        for (int i = 0; i < num_mrrg_r; i++) {
            GRBLinExpr constraint;
            long num_node_usages = 0;

            for (int j = 0; j < num_dfg_vals; j++) {
                auto& r_val = R[j * num_mrrg_r + i];
                constraint += r_val;
                num_node_usages += params.inIncrementalMode() ? std::lround(params.callback->getSolution(r_val)) : 0;
            }

            if (not params.inIncrementalMode() or not ( num_node_usages <= 1 )) {
                result.constraints.emplace_back(makeNewStandardConstraintID(i), constraint <= 1);
            }
        }

        return result;
    });

    // Constraint Group - Function Unit Exclusivity
    constraint_set.registerGenerator(ConstraintGroup::FunctionUnitExclusivity, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        for (int p = 0; p < num_mrrg_f; p++) {
            GRBLinExpr constraint;
            auto coeffs = std::make_unique<double[]>(num_dfg_ops);
            auto vars = std::make_unique<GRBVar[]>(num_dfg_ops);

            for (int q = 0; q < num_dfg_ops; q++) {
                coeffs[q] = 1.0;
                vars[q] = F[q * num_mrrg_f + p];
            }

            constraint.addTerms(coeffs.get(), vars.get(), num_dfg_ops);

            result.constraints.emplace_back(makeNewStandardConstraintID(p), constraint <= 1);
        }

        return result;
    });

    // Constraint Group - All Ops must be mapped
    constraint_set.registerGenerator(ConstraintGroup::OpsMustBeMapped, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        for (int q = 0; q < num_dfg_ops; q++) {
            GRBLinExpr constraint;
            auto coeffs = std::make_unique<double[]>(num_mrrg_f);
            auto vars = std::make_unique<GRBVar[]>(num_mrrg_f);

            for (int p = 0; p < num_mrrg_f; p++) {
                coeffs[p] = 1.0;
                vars[p] = F[q * num_mrrg_f + p];
            }

            constraint.addTerms(coeffs.get(), vars.get(), num_mrrg_f);

            result.constraints.emplace_back(makeNewStandardConstraintID(q), constraint == 1);
        }

        return result;
    });

    // Constraint Group - Fanout Routing
    constraint_set.registerGenerator(ConstraintGroup::FanoutRouting, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        for (auto &val: opgraph.valNodes()) {
            for (auto &r: mrrg_info.nodeClasses().routing_nodes) {
                int val_fanouts = val->output.size();
                for (int i = 0; i < val_fanouts; i++) {
                    GRBLinExpr sum_of_fanouts;
                    long num_fanouts_used = 0.0;
                    for (auto &mrrg_fanout : r->fanout) {
                        if (mrrg_fanout->type == MRRG_NODE_ROUTING) {
                            const auto& s_val = S_var_map.at(val).at(mrrg_fanout).at(i);
                            sum_of_fanouts += s_val;
                            num_fanouts_used += params.inIncrementalMode() ? std::lround(params.callback->getSolution(s_val)) : 0;
                        } else if (mrrg_fanout->type == MRRG_NODE_FUNCTION) {
                            auto &op =  val->output[i];
                            for (auto& fanin : mrrg_fanout->fanin) {
                                if (fanin == r) {
                                    const auto& f_val = F_var_map.at(op).at(mrrg_fanout);
                                    sum_of_fanouts += f_val;
                                    num_fanouts_used += params.inIncrementalMode() ? std::lround(params.callback->getSolution(f_val)) : 0;
                                    break;
                                }
                            }
                        } else {
                            make_and_throw<cgrame_mapper_error>([&](auto&& s) {
                                s << "unsupported MRRG node type for " << ConstraintGroup::FanoutRouting << " constraints: " << mrrg_fanout->type;
                            });
                        }
                    }

                    const auto& src_s_val = S_var_map.at(val).at(r).at(i);
                    const auto constraint_is_obeyed = [&]() {
                        return num_fanouts_used >= std::lround(params.callback->getSolution(src_s_val));
                    };

                    if (not params.inIncrementalMode() || not constraint_is_obeyed()) {
                        result.constraints.emplace_back(makeNewTupleConstraintID(val, r), sum_of_fanouts >= src_s_val);
                    }
                }
            }
        }

        return result;
    });

    // Constraint Group - Mux Exclusivity
    constraint_set.registerGenerator(ConstraintGroup::MuxExclusivity, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        for (auto &val: opgraph.valNodes()) {
            for (auto &r: mrrg_info.nodeClasses().routing_nodes) {
                GRBLinExpr sum_of_fanins;
                long num_fanin_used = 0;
                int fanin_count = r->fanin.size();
                if (fanin_count > 1) {
                    for (auto &fanin : r->fanin) {
                        if (fanin->type != MRRG_NODE_ROUTING) {
                            throw make_from_stream<cgrame_mapper_error>([&](auto&& s) {
                                s << "node has more than one fanin, and one of these fanin is not a routing node";
                            });
                        }

                        if (fanin->fanout.size() != 1) {
                            throw make_from_stream<cgrame_mapper_error>([&](auto&& s) {
                                s << "Mux Exclusivity Constraint Invariant is violated\n";
                                s << "Candidate MUX node is: " << mrrg.getNodeRef(r) << '\n';
                                for (const auto& r_fanin : mrrg.fanin(r)) {
                                    s << mrrg.getNodeRef(r_fanin) << "->" << mrrg.getNodeRef(r) << '\n';
                                }

                                s << "Problem fanin is: " << mrrg.getNodeRef(fanin) << '\n';
                                for (const auto& fanin_fanout : mrrg.fanout(fanin)) {
                                    s << mrrg.getNodeRef(fanin) << "->" << mrrg.getNodeRef(fanin_fanout) << '\n';
                                }
                            });
                        }

                        auto& fanin_r_val = R_var_map.at(val).at(fanin);
                        sum_of_fanins += fanin_r_val;
                        num_fanin_used += params.inIncrementalMode() ? std::lround(params.callback->getSolution(fanin_r_val)) : 0.0;
                    }

                    auto& src_r_val = R_var_map.at(val).at(r);
                    const auto& constraint_is_obeyed = [&]() {
                        return num_fanin_used == std::lround(params.callback->getSolution(src_r_val));
                    };

                    if (not params.inIncrementalMode() or not constraint_is_obeyed()) {
                        auto cid = makeNewTupleConstraintID(val, r);
                        result.constraints.emplace_back(std::move(cid), sum_of_fanins == src_r_val);
                    }
                }
            }
        }

        return result;
    });

    // Constraint Group - FU Fanout
    // XXX: this assumes single output nodes in both OpGraph and MRRG
    constraint_set.registerGenerator(ConstraintGroup::FunctionUnitFanout, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        for (auto &op: opgraph.opNodes()) {
            for (auto &f: mrrg_info.nodeClasses().function_nodes) {
                if (op->output) {
                    OpGraphVal* val = op->output;
                    if (f->fanout.size() != 1) {
                        throw make_from_stream<cgrame_mapper_error>([&](auto&& s) {
                            s << "FU does not have exactly one fanout. " << mrrg.getNodeRef(f).name << " has " << mrrg.fanout(f).size();
                        });
                    }
                    for (auto& r : f->fanout) {
                        int val_fanouts = val->output.size();
                        for (int i = 0; i < val_fanouts; i++) {
                            result.constraints.emplace_back(makeNewTupleConstraintID(op, f), (F_var_map.at(op).at(f)) == S_var_map.at(val).at(r).at(i));
                        }
                    }
                }
            }
        }

        return result;
    });

    // For multi-edges in the opgraph, make sure FU input nodes are used by at most one val output
    // Only needed for multi-edges.
    // Electrically, it's okay for S vars to use the same node *if they are the same val*.
    // However, we need to make sure that a FU receives the right number of inputs!
    constraint_set.registerGenerator(ConstraintGroup::FunctionUnitFanin, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        for (const auto& val : opgraph.valNodes()) {
            const auto& val_fanout = opgraph.outputOps(val);

            // Collect the output indexes that go to each op. Indexes are needed for getting S vars
            std::map<OpGraph::OpDescriptor, std::vector<int>> outputs_to_op;
            for (int i = 0; i < val_fanout.size(); ++i) {
                const auto op = val_fanout.at(i);
                outputs_to_op[op].push_back(i);
            }

            // Make a constraint for each multi-edge for each input of each compatible FU.
            for (const auto& op_and_output_list : outputs_to_op) {
                const auto& op = op_and_output_list.first;
                const auto& output_list = op_and_output_list.second;
                if (output_list.size() == 1) continue; // the multi-edge check

                for (const auto& f : mrrg_info.nodeClasses().function_nodes) {
                    if (not mrrg.getNodeRef(f).canMapOp(op)) continue; // the compatible check
                    for (const auto& fanin_mrrg : mrrg.fanin(f)) {
                        // make a constraint from the collected indices
                        GRBLinExpr sum_of_s_at_input;
                        for (const auto i : output_list) {
                            sum_of_s_at_input += S_var_map.at(val).at(fanin_mrrg).at(i);
                        }
                        result.constraints.emplace_back(
                            makeNewTupleConstraintID(op, val, f, fanin_mrrg),
                            sum_of_s_at_input <= 1
                        );
                    }
                }
            }
        }

        return result;
    });

    // Constraint Group - FU supported Op legality
    constraint_set.registerGenerator(ConstraintGroup::OpSupported, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();
        bool fix = false;
        std::string fixNode;
        //std::cout << "fix_port size:" << fix_port.size() << std::endl;
        for (auto &op: opgraph.opNodes()) {
            fix = false;
            for (auto& node : fix_port) {
                if (op->getName() == node.first) {
                    fix = true;
                    fixNode = node.second;
                }
            }
            for (auto &f: mrrg_info.nodeClasses().function_nodes) {
                if (fix) {
                    if(f->getHierarchyQualifiedName().find(fixNode) == fixNode.npos || !f->canMapOp(op)) {
          //              std::cout << op->getName() << " " << fixNode << std::endl;
            //            std::cout << f->getHierarchyQualifiedName() << std::endl;
                        result.constraints.emplace_back(makeNewTupleConstraintID(op, f), (F_var_map.at(op).at(f)) == 0);
                    }
                } else if (!f->canMapOp(op)) {
                    result.constraints.emplace_back(makeNewTupleConstraintID(op, f), (F_var_map.at(op).at(f)) == 0);
                }
            }
        }

        /*for (auto &op: opgraph.opNodes()) {
            std::cout << op->getName() << " ";
        }
        std::cout << std::endl;
        for (auto &f: mrrg_info.nodeClasses().function_nodes) {
            std::cout << f->getFullName() << " ";
        }
        std::cout << std::endl;*/
        return result;
    });

    // Constraint Group - Line Path Balance
    constraint_set.registerGenerator(ConstraintGroup::LinePathBalance, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        for (const auto& path_pair : trails_to_balance.noncyclic_trail_pairs) {
            auto sum1 = dot_product_of_S_and_F_vars_with_latency_for_opgraph_path(path_pair.first);
            auto sum2 = dot_product_of_S_and_F_vars_with_latency_for_opgraph_path(path_pair.second);
            result.constraints.emplace_back(makeNewTupleConstraintID(&path_pair), sum1 == sum2);
        }

        return result;
    });

    // Constraint - Cycle Path Balance
    constraint_set.registerGenerator(ConstraintGroup::CyclePathBalance, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        for (const auto& path : trails_to_balance.cyclic_trails) {
            auto sum = dot_product_of_S_and_F_vars_with_latency_for_opgraph_path(path);
            result.constraints.emplace_back(makeNewTupleConstraintID(&path), sum == mrrg.initiationInterval());
        }

        return result;
    });

    // Constraint Group - keep track of edge usage
    constraint_set.registerGenerator(ConstraintGroup::EdgeUsage, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        if (params.inIncrementalMode()) {
            std::cerr << "Cannot add edge use constraints lazily, as they are general constraints" << std::endl;
            params.callback->abort();
        }

        int edge_used_constr_count = 0;
        for (auto& val : opgraph.valNodes()) {
            for (auto &r: mrrg_info.nodeClasses().routing_nodes) {
                for (auto &mrrg_fanout : r->fanout) {
                    if (mrrg_fanout->type != MRRG_NODE_ROUTING) {
                        continue;
                    }
                    const auto& rvar = R_var_map.at(val).at(r);
                    const auto& rfanoutvar = R_var_map.at(val).at(mrrg_fanout);
                    const auto& evar = edge_used_var_map.at(val).at(r).at(mrrg_fanout);

                    // The edge is used if the R values on both sides are used
                    std::array<GRBVar, 2> r_values { rvar, rfanoutvar };
                    model.addGenConstrAnd(
                        evar,
                        r_values.data(), r_values.size(),
                        "edge_used_" + std::to_string(edge_used_constr_count++)
                    );
                }
            }
        }

        if (flags.getInt("verbosity") > 0) {
            std::cout << "actually added " << edge_used_constr_count << " edge_used constraints\n";
        }
        return result;
    });

    // Constraint Group - to enforce the existence of a topological ordering
    constraint_set.registerGenerator(ConstraintGroup::TopologicalOrdering, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        if (params.inIncrementalMode()) {
            std::cerr << "Cannot add topological constraints lazily, as they are general constraints" << std::endl;
            params.callback->abort();
        }

        int ordering_constr_count = 0;
        for (const auto& val_and_node_to_var_map : R_var_map) {
            for (const auto& node_and_var : val_and_node_to_var_map.second) {
                auto& r = node_and_var.first;
                auto& val = val_and_node_to_var_map.first;
                for (auto& mrrg_fanout : r->fanout) {
                    if (mrrg_fanout->type != MRRG_NODE_ROUTING) {
                        continue;
                    }
                    model.addGenConstrIndicator(
                        edge_used_var_map.at(val).at(r).at(mrrg_fanout), true,
                        T_var_map.at(val).at(mrrg_fanout) - T_var_map.at(val).at(r), GRB_GREATER_EQUAL, 1,
                        "ordering_" + std::to_string(ordering_constr_count++)
                    );
                }
            }
        }

        if (flags.getInt("verbosity") > 0) {
            std::cout << "actually added " << ordering_constr_count << " ordering constraints\n";
        }
        return result;
    });

    // Constraint Group - Operands must be mapped to viable mrrg nodes
    constraint_set.registerGenerator(ConstraintGroup::OperandRouting, [&](ConstraintGeneratorParams params) {
        auto result = params.makeReturnValue();

        for (auto &val: opgraph.valNodes()) {
            for (auto &r: mrrg_info.nodeClasses().routing_nodes) {
                if (r->supported_operand_tags.empty()) continue;
                for (int k = 0; k < (ptrdiff_t)val->output.size(); k++) {
                    if (not nodeIsCompatibleWithOperand(mrrg, r, opgraph.getOperandTag(OpGraph::EdgeDescriptor{val,k}))) {
                        result.constraints.emplace_back(makeNewTupleConstraintID(val, r, k), S_var_map.at(val).at(r).at(k) == 0);
                    }
                }
            }
        }
        return result;
    });

    const auto& all_cgroups = constraint_set.getAllConstraintGroupsRegistered();
    const auto filtered_cgroups = filter_collection(all_cgroups, [&](auto&& e) {
        if (flags.getBool("enable_topological_order_mode")) {
            if (e == ConstraintGroup::MuxExclusivity) {
                return false;
            }
        } else {
            if (e == ConstraintGroup::TopologicalOrdering || e == ConstraintGroup::EdgeUsage) {
                return false;
            }
        }
        return true;
    });

    const auto non_lazy_cgroups = filter_collection(filtered_cgroups, [&](auto&& e) {
        return lazy_constraints.find(e) == end(lazy_constraints);
    });

    const auto lazy_add_mode = [&]() { switch (flags.getInt("lazy_constraint_add_mode")) {
        default: return ConstraintAddMode::LazyViaCallback;
        case  1: return ConstraintAddMode::LazyToModel1;
        case  2: return ConstraintAddMode::LazyToModel2;
        case  3: return ConstraintAddMode::LazyToModel3;
    }}();

    auto constraint_add_conf = ConstraintAddConf::make<ConstraintGroup>({
        { lazy_add_mode, lazy_constraints },
        { ConstraintAddMode::NotLazyToModel, non_lazy_cgroups },
    });

    if (flags.getInt("verbosity") > 0) {
        std::cout << "Adding constraints to model...\n";
    }
    auto constraint_cache = constraint_set.addConstraintsToModel(ConstraintCache{}, constraint_add_conf, model);

    auto callback = makeOnMIPSolutionFunctorGRBCallback([&](auto& cb_obj) {
        if (flags.getInt("verbosity") > 0) {
            std::cout << "CGRAME_main_ILP_callback is adding constraints...\n";
        }
        constraint_cache = constraint_set.addConstraintsViaCallback(std::move(constraint_cache), constraint_add_conf, cb_obj);
    });

    if (constraint_add_conf.willRequireLazyModelAttribute()) {
        model.set(GRB_IntParam_LazyConstraints, 1);
    }
    model.update();

    model.setCallback(&callback);

    {const auto model_dump_filename = flags.getString("model_dump_filename");
    if (not model_dump_filename.empty()) {
        const auto model_dump_filename_with_extension = model_dump_filename
            + (model_dump_filename.find('.') == model_dump_filename.npos ? ".lp" : "");
        std::cout << "[INFO] Writing model to " << model_dump_filename_with_extension << std::endl;
        model.write(model_dump_filename_with_extension);
        std::cout << "[INFO] Done writing model to " << model_dump_filename_with_extension << std::endl;
    }}

    // Optimize model
    model.optimize();

    mapping_result->solve_time_in_seconds = model.get(GRB_DoubleAttr_Runtime);

    int status = model.get(GRB_IntAttr_Status);
    switch (status) {
    case GRB_INFEASIBLE:
        {const auto model_IIS_dump_filename = flags.getString("model_IIS_dump_filename");
        if (not model_IIS_dump_filename.empty()) {
            const auto iis_filename_with_extension = model_IIS_dump_filename
                + (model_IIS_dump_filename.find('.') == model_IIS_dump_filename.npos ? ".ilp" : "");
            std::cout << "[INFO] Computing IIS, will dump to " << iis_filename_with_extension << std::endl;
            model.computeIIS();
            std::cout << "[INFO] Writing IIS to " << iis_filename_with_extension << std::endl;
            model.write(iis_filename_with_extension);
            std::cout << "[INFO] Done writing IIS to " << iis_filename_with_extension << std::endl;
        }}
        return ILPMapperStatus::INFEASIBLE;
    case GRB_TIME_LIMIT: return ILPMapperStatus::TIMEOUT;
    case GRB_INTERRUPTED: return ILPMapperStatus::INTERRUPTED;
    case GRB_OPTIMAL: [[fallthrough]];
    case GRB_SUBOPTIMAL: [[fallthrough]];
    case GRB_SOLUTION_LIMIT:
        if (flags.getInt("verbosity") > 0) {
            std::cout << "FinalILPObjective: " << model.get(GRB_DoubleAttr_ObjVal) << '\n';
        }

        for (auto& val : opgraph.valNodes()) {
            for (auto& r : mrrg_info.nodeClasses().routing_nodes) {
                if (std::llround(R_var_map.at(val).at(r).get(GRB_DoubleAttr_X))) {
                    mapping_result->mapMRRGNode(val, r);
                }
            }
        }

        for (auto& op : opgraph.opNodes()) {
            for (auto& f : mrrg_info.nodeClasses().function_nodes) {
                if (std::llround(F_var_map.at(op).at(f).get(GRB_DoubleAttr_X))) {
                    mapping_result->mapMRRGNode(op, f);
                }
            }
        }

        return (status == GRB_OPTIMAL) ? ILPMapperStatus::OPTIMAL_FOUND : ILPMapperStatus::SUBOPTIMAL_FOUND;
    default:
        return ILPMapperStatus::UNLISTED_STATUS;
    }
}
