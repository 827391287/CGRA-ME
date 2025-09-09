/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/OpGraphProcedures.h>

#include <CGRA/GraphAlgorithms.h>
#include <CGRA/utility/ConfigGraph.h>

namespace {

std::pair<OpGraph::Walk, OpGraph::Walk> reverse_and_canonicalize
    (std::pair<OpGraph::Walk, OpGraph::Walk>&& line_trail_pair) {
    std::reverse(line_trail_pair.first.begin(), line_trail_pair.first.end());
    std::reverse(line_trail_pair.second.begin(), line_trail_pair.second.end());
    if (line_trail_pair.second < line_trail_pair.first) {
        std::swap(line_trail_pair.first, line_trail_pair.second);
    }
    return line_trail_pair;
}

OpGraph::Walk reverse_and_canonicalize(OpGraph::Walk&& cyclic_trail, const OpGraph& opgraph) {
    std::reverse(cyclic_trail.begin(), cyclic_trail.end());
    auto new_begin = std::min_element(cyclic_trail.begin(), cyclic_trail.end(), [&](const auto& lhs, const auto& rhs) {
        return opgraph.getNodeRef(lhs.val).getName() < opgraph.getNodeRef(rhs.val).getName();
    });
    std::rotate(cyclic_trail.begin(), new_begin, cyclic_trail.end());
    return cyclic_trail;
}

}  // end anon namespace

// see comment in header for algorithm overview
TrailsToBalance computeTrailsToBalance(const OpGraph& opgraph) {
    using EdgeDesc = OpGraph::EdgeDescriptor;
    using OpDesc = OpGraph::OpDescriptor;

    // return value
    TrailsToBalance result;

    // ops that have been visited by previous iterations
    std::set<OpDesc> ops_visited;

    // non-tree and re-convergence edges that have been visited by previous iterations
    // note: re-convergence edges are symmetric with non-tree ones, so we treat them the same here
    std::set<EdgeDesc> nte_and_convergence_edges_found;

    // use (ordered) sets -- no hash defined
    // also, we'll be treating hyper-edge output pins as vertices; as-if it were a multi-graph
    const auto galgos = GraphAlgorithms<EdgeDesc,
        StandardMapMaker<std::map, EdgeDesc>, StandardSetMaker<std::set, EdgeDesc>
    >{};

    for (OpDesc op : opgraph.opNodes()) {
        if (ops_visited.find(op) != ops_visited.end()) { continue; }

        // start as far up as possible, to make for fewer iterations.
        // note: for an unvisited Op, all Ops reachable by fanin edges must have also not been visited.
        {std::set<OpDesc> ancestors;
        while (true) {
            const auto& fanin = opgraph.inputVals(op);
            if (fanin.empty() || not ancestors.insert(op).second) { break; }
            op = opgraph.inputOp(*fanin.begin());
        }}

        struct FindOpNonTreeEdgesVisitor : DoNothingGraphVisitor<EdgeDesc> {
            const OpGraph* opgraph = nullptr;
            std::set<OpDesc> examined_op_targets = {};
            std::set<EdgeDesc> non_tree_edges = {};

            void onExamine(const EdgeDesc& e) {
                // if this hyper-edge's target was already visited, save it
                if (not examined_op_targets.insert(opgraph->targetOfEdge(e)).second) {
                    non_tree_edges.insert(e);
                }
            }
        } visitor;
        visitor.opgraph = &opgraph;
        visitor.examined_op_targets.insert(op);  // handle edges that loop around to op

        // a search with with the hyper-edge output pins as vertices
        // returns a fanin graph representing the search, where fanin.front() defines a spanning tree
        auto search_tree = galgos.wavedBreadthFirstVisit(opgraph, opgraph.outEdges(op), visitAllVertices(), visitor);

        // ancestors (including e), if you walk up the search tree. halts at a cycle.
        const auto determineSearchTreeAncestors = [&](const auto& e) {
            auto traceback = galgos.singleTraceback(std::set<EdgeDesc>(), e, search_tree);
            std::reverse(traceback.path.begin(), traceback.path.end());
            return std::move(traceback.path);
        };

        // finds the single search tree edge that drives the target of `e`
        const auto determineSearchTreeEdgeWithSameTarget = [&](const auto& e) {
            for (const auto& edge_to_target : opgraph.inEdges(opgraph.targetOfEdge(e))) {
                if (visitor.non_tree_edges.find(edge_to_target) != visitor.non_tree_edges.end()) { continue; }
                if (search_tree.find(edge_to_target) == search_tree.end()) { continue; }
                return edge_to_target;
            }
            throw make_from_stream<cgrame_error>([&](auto&& s) {
                s << "couldn't find the search tree edge with same target as " << e;
            });
        };

        const auto same_driver = [&](auto&& e1, auto&& e2) { return opgraph.inputOp(e1.val) == opgraph.inputOp(e2.val); };

        // each non-tree edge represents one constraint.
        // examine the spanning tree around the target and driver to determine type and members
        for (const auto nte : visitor.non_tree_edges) {
            // only one constraint per nte or convergence edge!
            if (not nte_and_convergence_edges_found.insert(nte).second) { continue; }

            // if there is any tree or non-tree edge that is driven by the target of nte, then it's a cycle
            EdgeDesc cycle_edge;
            const auto reverse_search_tree = galgos.wavedBreadthFirstVisit(
                makeReversedGraphFromFaninLists<EdgeDesc>(&search_tree), {nte},
                // input_is(opgraph.targetOfEdge(nte))
                [&](auto& e) {
                    const bool found = opgraph.targetOfEdge(nte) == opgraph.inputOp(e.val);
                    if (found) { cycle_edge = e; }
                    return found;
                }
            );
            if (cycle_edge) {
                auto traceback = galgos.singleTraceback(singleItemSet(nte), cycle_edge, reverse_search_tree);
                if (not traceback.success) { throw make_from_stream<cgrame_error>([fname=__func__](auto&& s) {
                    s << fname << " couldn't find original non-tree edge when searching a reverse search tree";
                });}
                if (nte == cycle_edge) {
                    traceback.path.pop_back(); // remove duplicate
                }
                // if already inserted, then it's a re-convergence edge
                if (result.cyclic_trails.insert(reverse_and_canonicalize(std::move(traceback.path), opgraph)).second) {
                    continue;
                }
            }

            // else, it's a re-convergence -- find the common ancestor
            const auto convergence_edge = determineSearchTreeEdgeWithSameTarget(nte); // ce for short
            // every re-convergence edge represents symmetry in the graph, so treat it the same as a non-tree edge
            if (not nte_and_convergence_edges_found.insert(convergence_edge).second) { continue; }
            const auto nte_ancestors = determineSearchTreeAncestors(nte);
            const auto ce_ancestors = determineSearchTreeAncestors(convergence_edge);
            const auto same_driver_search = find_first_of_and_matching(nte_ancestors, ce_ancestors, same_driver);
            if (same_driver_search.first != nte_ancestors.end()) {
                result.noncyclic_trail_pairs.insert(reverse_and_canonicalize({
                    {nte_ancestors.begin(), std::next(same_driver_search.first)},
                    {ce_ancestors.begin(), std::next(same_driver_search.second)},
                }));
                continue;
            }

            throw make_from_stream<cgrame_error>([nte,fname=__func__](auto&& s) {
                s << fname << " couldn't construct a constraint from the non-tree edge " << nte
                    << ". It is neither a cycle or a re-convergence!";
            });
        }

        // synchronize the global visited set with the one for this iteration
        std::copy(visitor.examined_op_targets.begin(), visitor.examined_op_targets.end(), std::inserter(ops_visited, ops_visited.end()));
    }

    return result;
}

void TrailsToBalance::print_to(std::ostream& os) const {
    os << "Trails to balance: {\n";
    os << "  Line trail pairs to be balanced: ";
    print_container(os, noncyclic_trail_pairs, ",\n    ", "{ ", " }", [](auto&& os, const auto& trail_pair) {
        print_container(os, trail_pair.first);
        os << " == ";
        print_container(os, trail_pair.second);
    });

    os << "\n  Cycle trails to be balanced: ";
    print_container(os, cyclic_trails, ",\n    ", "{ ", " }", [](auto&& os, const auto& trail) {
        print_container(os, trail);
    });
    os << "\n}";
}

OpGraph createOpGraphFromConfig(const ConfigGraph& config) {
    OpGraph result;
    auto cg_vert_to_op = std::map<ConfigGraph::VertexID, OpGraph::OpDescriptor>{};
    for (const auto& v : config.allNodes()) {
        const auto& v_attrs = config.attributes(v);

        const bool has_input = v_attrs.hasKey("input");
        const bool has_output = v_attrs.hasKey("output");
        const bool has_opcode = v_attrs.hasKey("opcode");
        const int bitwidth = v_attrs.getIntOr("bitwidth", 32);
        const std::string node_config = v_attrs.getStringOr("config", "");
        if ((int)has_input + (int)has_output + (int)has_opcode > 1) {
            throw make_from_stream<cgrame_error>([&](auto&& s) {
                s << "ambiguous: op " << config.name(v)
                     << " specifies more than one of 'input', 'output', and 'opcode'";
            });
        }

        const auto opcode =
              has_opcode ? [&] { return opcode_from_string(v_attrs.getString("opcode")); }()
            : has_input ? OpCode::INPUT
            : has_output ? OpCode::OUTPUT
            : throw make_from_stream<cgrame_error>([&](auto&& s) {
                s << "op: " << config.name(v) << " does not specify an opcode!";
            });

        if (opcode == OpCode::CONST) {
            const auto src = cg_vert_to_op[v] = result.insert({
                config.name(v),
                bitwidth,
                opcode,
                v_attrs.getInt("constVal")
            });
        } else if (opcode == OpCode::CMP) {
            const auto src = cg_vert_to_op[v] = result.insert({
                config.name(v),
                bitwidth,
                opcode,
                v_attrs.getString("cmpMode"),
                true
            });
        } else if ((opcode == OpCode::LOAD || opcode == OpCode::STORE) & (!config.empty())) {
            BitConfig* bitConfig = new BitConfig(1);
            std::vector<BitSetting> bitSet;
            for (char bit : node_config) {
                bitSet.push_back(from_bool<BitSetting>(int(bit) - 48));
            }
            bitConfig->add(bitSet, 0);
            const auto src = cg_vert_to_op[v] = result.insert({
                config.name(v),
                bitwidth,
                opcode,
                bitConfig
            });
        } else if (opcode == OpCode::LOAD || opcode == OpCode::STORE) {
            const auto src = cg_vert_to_op[v] = result.insert({
                config.name(v),
                bitwidth,
                opcode,
                v_attrs.getStringOr("memName", ""),
                false
            });
        } else {
            const auto src = cg_vert_to_op[v] = result.insert({
                config.name(v),
                bitwidth,
                opcode
            });
        }
    }

    for (const auto& v : config.allNodes()) {
        for (const auto& out_edge : config.outEdges(v)) {
            const auto& e_attrs = config.attributes(out_edge);
            result.link(
                cg_vert_to_op.at(v), cg_vert_to_op.at(config.target(out_edge)),
                e_attrs.getStringOr("operand", ""),
                e_attrs.getIntOr("bitwidth", 32),
                e_attrs.getIntOr("dist", 0),
                ((e_attrs.getStringOr("kind", "").compare("alias") == 0) ? EdgeKind::kAlias : EdgeKind::kDataFlow),
                e_attrs.getBoolOr("predicate", false));
        }
    }
    return result;
}


namespace {

/**
 * remove the given nodes, and any fanin that are now unused, and any fanin-of-fanin that are now unused, etc.
 */
OpGraph removeTheseNodesAndAnyNowUnusedFaninRecursively(OpGraph opgraph, std::vector<OpGraph::OpDescriptor> ops_to_erase) {
    // don't erase the same thing twice
    std::set<OpGraph::OpDescriptor> will_be_or_is_erased{ops_to_erase.begin(), ops_to_erase.end()};

    // while there are nodes to be erased
    while (not ops_to_erase.empty()) {
        // pop one
        const auto op = ops_to_erase.back();
        ops_to_erase.pop_back();
        // save the fanins (undefined after erase)
        const auto op_inputs = opgraph.inputOps(op);
        // erase it
        opgraph.erase(op);
        // add any fanin that now have no fanout to the erase stack
        for (const auto& input_op : op_inputs) {
            if (not opgraph.outputOps(input_op).empty()) { continue; }
            if (not will_be_or_is_erased.insert(input_op).second) { continue; }
            ops_to_erase.push_back(input_op);
        }
    }

    return opgraph;
}

} // end anon namespace

OpGraph removeBranchComputation(OpGraph opgraph) {
    // start with the br nodes
    auto ops_to_erase = filter_collection(std::vector<OpGraph::OpDescriptor>{opgraph.opNodes().begin(), opgraph.opNodes().end()},
        [&](const auto& opdesc) { return opgraph.getNodeRef(opdesc).getOpCode() == OpCode::BR; });

    return removeTheseNodesAndAnyNowUnusedFaninRecursively(std::move(opgraph), ops_to_erase);
}

OpGraph removeCastNodes(OpGraph og) {
    const auto orig_casts = filter_collection(og.opNodes(), [&og](auto&& op) {
        const auto opcode = og.getNodeRef(op).getOpCode();
        return og.inputVals(op).size() == 1 &&
            (opcode == OpGraphOpCode::NOP
            || opcode == OpGraphOpCode::SEXT
            || opcode == OpGraphOpCode::ZEXT
            || opcode == OpGraphOpCode::TRUNC);
    });

    for (const auto& cast_op : orig_casts) {
        const auto input_op = og.inputOp(og.inputVals(cast_op).front());
        for (const auto& edge : og.outEdges(cast_op)) {
            og.link_like(input_op, og.targetOfEdge(edge), edge);
        }
        og.erase(cast_op);
    }

    return og;
}

OpGraph removePhiNodes(OpGraph opgraph) {
    const auto initial_op_nodes = opgraph.opNodes();

    auto erased_opnodes = std::set<OpGraph::OpDescriptor>{};
    const auto erase_op = [&](OpGraph::OpDescriptor op_desc) {
        opgraph.erase(op_desc);
        erased_opnodes.insert(op_desc);
    };

    for (const auto& op_desc : initial_op_nodes) {
        const auto& opnode = opgraph.getNodeRef(op_desc);
        if (erased_opnodes.find(op_desc) != erased_opnodes.end()) { continue; }  // skip if removed
        if (opnode.getOpCode() != OpCode::PHI) { continue; }  // only care about PHIs

        // const or input nodes
        const auto is_external_edge = [&](const auto& in_edge) {
            const auto& in_opcode = opgraph.getNodeRef(opgraph.inputOp(in_edge.val)).getOpCode();
            return in_opcode == OpCode::CONST || in_opcode == OpCode::INPUT;
        };
        const auto external_in_edges = filter_collection(opgraph.inEdges(op_desc), is_external_edge);
        const auto internal_in_edges = filter_collection(opgraph.inEdges(op_desc), [&](auto& e) { return !is_external_edge(e); });
        if (external_in_edges.empty()) { throw std::logic_error("could not find external input edges to phi node '" + opnode.getName() + "'!"); }
        if (internal_in_edges.empty()) { throw std::logic_error("could not find internal input edges to phi node '" + opnode.getName() + "'!"); }
        if (internal_in_edges.size() > 1) { throw std::logic_error("multiple internal input edges to phi node '" + opnode.getName() + "'... does this loop have multiple basic blocks?"); }

        // make links to output. internal_in_edges is asserted to have size() == 1 above
        for (const auto& int_in_edge : internal_in_edges) {
            for (const auto& out_edge : opgraph.outEdges(op_desc)) {
                opgraph.link_like(opgraph.inputOp(int_in_edge.val),opgraph.targetOfEdge(out_edge), out_edge);
            }
        }

        // erase phi & any now unused nodes
        opgraph = removeTheseNodesAndAnyNowUnusedFaninRecursively(std::move(opgraph), {op_desc});
    }

    return opgraph;
}

OpGraph propagateConstants(OpGraph og) {
    bool made_changes = true;
    while (made_changes) {
        made_changes = false;
        for (const auto& op : og.opNodes()) {
            const auto& op_ref = og.getNodeRef(op);

            if (op_ref.getOpCode() == OpGraphOpCode::MUL) {
                std::int64_t product_of_constants = 1;
                int num_const_inputs = 0;

                // count const inputs and compute their product
                for (const auto& in_edge : og.inEdges(op)) {
                    const auto& in_op_ref = og.getNodeRef(og.inputOp(in_edge.val));
                    if (in_op_ref.getOpCode() == OpGraphOpCode::CONST) {
                        product_of_constants *= in_op_ref.getConstValue();
                        num_const_inputs += 1;
                    }
                }

                if (num_const_inputs == (std::ptrdiff_t)og.inEdges(op).size() || product_of_constants == 0) {
                    // if all inputs are consts, or one or more constant inputs are 0, replace the whole op.
                    const auto new_const = og.emplace(op_ref.getName() + "_as_const",
                        op_ref.bitwidth, OpGraphOpCode::CONST, product_of_constants);
                    for (const auto& out_edge : og.outEdges(op)) {
                        og.link_like(new_const, og.targetOfEdge(out_edge), out_edge);
                    }
                    og = removeTheseNodesAndAnyNowUnusedFaninRecursively(std::move(og), {op});
                    made_changes = true;
                // } else if (num_const_inputs > 1) {
                    // if there are more than one constant, we could fold them
                    // but what about, say, a 3-input multiply, with 2 constant inputs?
                    // can that be reduced to just a two-input one?
                    // Or, what would the operand of the merged constant be?
                }
            }

            if (op_ref.getOpCode() == OpGraphOpCode::ADD) {
                int64_t sum_of_constants = 0;
                int num_const_inputs = 0;
                OpGraph::OpDescriptor a_non_const_input = {};

                // count const inputs and compute their sum
                for (const auto& in_edge : og.inEdges(op)) {
                    const auto& in_op_ref = og.getNodeRef(og.inputOp(in_edge.val));
                    if (in_op_ref.getOpCode() == OpGraphOpCode::CONST) {
                        sum_of_constants += in_op_ref.getConstValue();
                        num_const_inputs += 1;
                    } else {
                        a_non_const_input = og.inputOp(in_edge.val);
                    }
                }

                OpGraph::OpDescriptor replacement_op = {};

                const auto num_op_inputs = (std::ptrdiff_t)og.inEdges(op).size();
                if (num_const_inputs == num_op_inputs) {
                    // if all inputs are consts, replace the whole op
                    replacement_op = og.emplace(op_ref.getName() + "_as_const",
                        op_ref.bitwidth, OpGraphOpCode::CONST, sum_of_constants);
                } else if (num_const_inputs + 1 == num_op_inputs && sum_of_constants == 0) {
                    // if adding to 0, and only 1 non-const input, replace the op with the input
                    replacement_op = a_non_const_input;  // the only one
                }

                if (replacement_op) {
                    for (const auto& out_edge : og.outEdges(op)) {
                        og.link_like(replacement_op, og.targetOfEdge(out_edge), out_edge);
                    }
                    og = removeTheseNodesAndAnyNowUnusedFaninRecursively(std::move(og), {op});
                    made_changes = true;
                }
            }

            // if any changes were made, start over to refresh the opNodes for-loop
            if (made_changes) { break; }
        }
    }

    return og;
}

OpGraph distributeConstants(OpGraph og) {
    for (const auto& op : og.opNodes()) {
        const auto& op_ref = og.getNodeRef(op);
        if (op_ref.getOpCode() != OpGraphOpCode::CONST) { continue; }

        int dup_number = -1;
        for (;;) {
            const auto out_edges = og.outEdges(op);
            // don't touch consts with fanout-of-1, and leave original op attached to one fanout
            if (out_edges.size() <= 1) { break; }

            const auto& out_edge = out_edges.back();
            dup_number += 1;
            auto new_name = op_ref.getName() + "_dup" + std::to_string(dup_number);
            og.link_like(og.insert(op_ref.propertyClone(std::move(new_name))), og.targetOfEdge(out_edge), out_edge);
            og.unLink(out_edge.val, og.targetOfEdge(out_edge));
        }
    }
    return og;
}
