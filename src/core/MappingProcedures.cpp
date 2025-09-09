/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/MappingProcedures.h>
#include <CGRA/user-inc/UserArchs.h>

CreateMappingGraphResult createMappingGraphFromConfig(Mapping & m, const ConfigGraph & config, const OpGraph & opgraph, const MRRG & mrrg) {
    auto cg_vert_to_mrrg = std::map<ConfigGraph::VertexID, MRRG::NodeDescriptor>{};

    // Map nodes
    for (const auto& v : config.allNodes()) {
        const auto& v_attrs = config.attributes(v);
        const bool isVal = v_attrs.hasKey("maps_to_val");
        const bool isOp = v_attrs.hasKey("maps_to_op");

        if (!(isVal || isOp)) {
            throw make_from_stream<cgrame_error>([&](auto&& s) {
                s << "mrrg node: " << config.name(v) << " is not mapped to anything";
            });
        }

        std::string mrrg_node_name = config.name(v);
        std::string op_name;
        if (isVal){
            op_name = v_attrs.getString("maps_to_val");
        }
        else {
            op_name = v_attrs.getString("maps_to_op");
        }
        auto mrrg_ndesc = mrrg.getNode(v_attrs.getInt("cycle"), v_attrs.getString("name"));
        if (mrrg_ndesc == nullptr) {
            throw make_from_stream<cgrame_error>([&](auto&& s) {
                s << "[ERROR]: mrrg node: " << v_attrs.getInt("cycle") << ":" << v_attrs.getString("name") << " is not part of specified architecture";
            });
        }

        // Map the mrrg node and op node
        if (isVal) {
            // Case of op node being a ValNode
            m.mapMRRGNode(opgraph.getVal(op_name), mrrg_ndesc);
        }
        else {
            // Case of op node being an OpNode
            m.mapMRRGNode(opgraph.getOp(op_name), mrrg_ndesc);
        }

        cg_vert_to_mrrg[v] = mrrg_ndesc;
    }

    m.setStatus(MappingStatus::success);

    // Create mapping graph
    const auto& mapping = m.getMapping();
    MappingGraph mg;
    std::unordered_map<MRRG::NodeDescriptor, MappingGraph::NodeDescriptor> fromMRRG;
    std::unordered_map<MappingGraph::NodeDescriptor, MRRG::NodeDescriptor, MappingGraph::NodeDescriptorHash> toMRRG;

    // Add nodes to mapping graph
    for (auto & opToMRRGNodeList : mapping) {
        for (auto & mrrgNode : opToMRRGNodeList.second) {
            auto mgNode = mg.insert({opToMRRGNodeList.first}).first;
            toMRRG.emplace(mgNode, mrrgNode);
            fromMRRG.emplace(mrrgNode, mgNode);
        }
    }

    // Connect nodes together
    for (const auto& v : config.allNodes()) {
        for (const auto& out_edge : config.outEdges(v)) {
            mg.link(fromMRRG[cg_vert_to_mrrg.at(v)], fromMRRG[cg_vert_to_mrrg.at(config.target(out_edge))]);
        }
    }
    return {mg, fromMRRG, toMRRG};
}
