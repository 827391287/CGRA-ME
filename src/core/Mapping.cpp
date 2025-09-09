/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <algorithm>
#include <vector>
#include <set>
#include <stack>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <deque>

#include <CGRA/utility/ConfigGraph.h>
#include <CGRA/Exception.h>
#include <CGRA/Mapping.h>
#include <CGRA/Module.h>
#include <CGRA/MRRGProcedures.h>
#include <CGRA/utility/Collections.h>

std::ostream& operator<<(std::ostream& os, MappingStatus ms) {
    switch (ms) {
        case MappingStatus::success: return os << "success";
        case MappingStatus::failure: return os << "failure";
        case MappingStatus::timeout: return os << "timeout";
    }
    return os << "MappingStatus{" << static_cast<int>(ms) << '}';
}

Mapping::Mapping(std::shared_ptr<CGRA> cgra, int II, std::shared_ptr<OpGraph> opgraph)
    : cgra(cgra)
    , II(II)
    , opgraph(opgraph)
{
}

Mapping::~Mapping()
{
}

CGRA * Mapping::getCGRA() const
{
    return cgra.get();
}

int Mapping::getII() const
{
    return II;
}

void Mapping::mapMRRGNode(OpGraph::NodeDescriptor opnode, MRRG::NodeDescriptor node)
{
    try
    {
        mapping[opnode].push_back(node);
    }
    catch(const std::exception & e)
    {
        throw cgrame_error(std::string("Mapping Exception Thrown by: [") + e.what() + "] at File: " + std::string(__FILE__) + " Line: " + std::to_string(__LINE__));
    }
}

void Mapping::unmapMRRGNode(OpGraph::NodeDescriptor opnode, MRRG::NodeDescriptor node)
{
    try
    {
        auto iter = find(mapping[opnode].begin(), mapping[opnode].end(), node);
        if(iter != mapping[opnode].end())
        {
            mapping[opnode].erase(iter);
        }
    }
    catch(const std::exception & e)
    {
        throw cgrame_error(std::string("Mapping Exception Thrown by: [") + e.what() + "] at File: " + std::string(__FILE__) + " Line: " + std::to_string(__LINE__));
    }
}

void Mapping::setMapping(std::map<OpGraph::NodeDescriptor,std::vector<MRRG::NodeDescriptor>> mapping)
{
    this->mapping = mapping;
}

void Mapping::setNodeMapping(OpGraph::NodeDescriptor node, std::vector<MRRG::NodeDescriptor> mapping) {
    this->mapping[node] = std::move(mapping);
}

void Mapping::clear() {
    *this = Mapping(cgra, II, opgraph);
}

static bool find_sink(const std::vector<MRRG::NodeDescriptor> & val_map, std::map<MRRG::NodeDescriptor,bool> & visited, MRRG::NodeDescriptor src, MRRG::NodeDescriptor sink)
{
    // mark src visited
    visited[src] = true;

    // check each fanout
    for(auto & fo : src->fanout)
    {
        if(fo == sink && std::find(sink->fanin.begin(), sink->fanin.end(), src) != sink->fanin.end())
        {
            return true;
        }

        if(std::find(val_map.begin(), val_map.end(), fo) != val_map.end()) // if the node in the mapping
            if(visited.find(fo) == visited.end()) // and we haven't visited before
            {
                if(find_sink(val_map, visited, fo, sink))
                    return true;
            }
    }
    return false;
}

bool Mapping::verifyOpGraphMappingConnectivity() // Should be const member function, but the [] operator might modify the mapping
{
    bool result = true;
    // 1. check that every op has one MRRG node mapped. also check that the FU can support the OP
    for(auto & op : opgraph->opNodes())
    {
        if(mapping[op].size() > 1)
        {
            std::cout << "Verify FAILED: Op mapped to more than FU. Op=\'" << *op << "\'" << std::endl;
            return false;
        }
        else if(mapping[op].size() == 0)
        {
            std::cout << "Verify FAILED: Op mapped to no FU. Op=\'" << *op << "\'" << std::endl;
            return false;
        }
        else
        {
            if(!mapping[op][0]->canMapOp(op))
            {
                std::cout << "Verify FAILED: Op mapped to more illegal FU. Op=\'" << *op << "\'" << std::endl;
                return false;
            }
        }
    }
    // 2. Check that there are no shorts between values
    for(auto & val1 : opgraph->valNodes())
    {
        for(auto & val2 : opgraph->valNodes())
        {
            if(val1 != val2)
            {
                for(auto & node1 : mapping[val1])
                {
                    for(auto & node2 : mapping[val2])
                    {
                        if(node1 == node2)
                        {
                            std::cout << "Verify FAILED: Different vals mapped to same route. Val1=\'" << *val1 << "\' Val2=\'" << *val2 << "\'" << std::endl;
                            return false;
                        }
                    }
                }
            }
        }
    }
    // 3. for every val, verify source to sink connections for each fanout
    for(auto & val : opgraph->valNodes())
    {
        std::cout << "Verifying Value: " << *val << std::endl;
        for(int i = 0; i < (std::ptrdiff_t)val->output.size(); i++)
        {
            MRRG::NodeDescriptor srcfu = mapping[val->input][0];
            MRRG::NodeDescriptor sinkfu = mapping[val->output[i]][0];
            std::cout << "Finding sink \'" << *sinkfu << "(operand=" << val->output_operand[i] << ")" << std::endl;
            std::map<MRRG::NodeDescriptor,bool> visited;
            if(!find_sink(mapping[val], visited, srcfu, sinkfu))
            {
                std::cout << "Verify FAILED: Disconnect between " << *(val->input) << "/" << *srcfu << " -> " << *(val->output[i]) << "/" << *sinkfu << "(operand=" <<val->output_operand[i] << ")" << std::endl;
                std::cout << "Could not find sink \'" << *sinkfu << "(operand=" << val->output_operand[i] << ")" << std::endl;

                outputValMapping(val);
                result &= false;
            }
            else
            {
                std::cout << "Found sink \'" << *sinkfu << "(operand=" << val->output_operand[i] << ")" << std::endl;
            }
        }
    }
    return result;
}

namespace {
    template<typename STREAM, typename MAPPING, typename KEY>
    void if_found_print_single_mapping_else_print_message(STREAM& o, MAPPING&& m, KEY&& k) {
        auto find_result = m.find(k);
        if (find_result == end(m)) {
            o << "__NOT_MAPPED__";
        } else {
            auto& mapping_list = m.at(k);
            if (mapping_list.size() == 1) {
                o << **begin(mapping_list);
            } else {
                o << "__MULTIPLE__";
            }
        }
    }

    template<typename STREAM, typename MAPPING, typename KEY>
    void if_found_print_mapping_else_print_message(STREAM& o, MAPPING&& m, KEY&& k) {
        auto find_result = m.find(k);
        if (find_result == end(m)) {
            o << "__NOT_MAPPED__";
        } else {
            for(auto & node : m.at(k)) {
                o << "  " << *node << std::endl;
            }
        }
    }
}

void Mapping::outputMapping(const OpGraph& opgraph, std::ostream & o) const
{
    o << "Operation Mapping Result:" << std::endl;
    for(auto & op : opgraph.opNodes())
    {
        o << *op << ": ";
        if_found_print_mapping_else_print_message(o, mapping, op);
        o << '\n';
    }
    o << std::endl;
    o << "Connection Mapping Result:" << std::endl;
    for(auto & val : opgraph.valNodes())
    {
        o << *val << ":" << std::endl;
        if_found_print_mapping_else_print_message(o, mapping, val);
        o << std::endl;
    }
}

void Mapping::outputDetailedMapping(std::ostream & o) const
{
    o << "Operation Mapping Result:" << std::endl;
    for(auto & op : opgraph->opNodes())
    {
        o << *op << ": ";
        if_found_print_mapping_else_print_message(o, mapping, op);
        o << '\n';
    }
    o << std::endl;
    // o << "Connection Mapping Result:" << std::endl;
    // for(auto & val : opgraph->valNodes())
    // {
    //     for(int fanout_id = 0; fanout_id < (std::ptrdiff_t)val->fanout_result.size(); ++fanout_id)
    //     {
    //         o << *val << "->" << *(val->output.at(fanout_id)) << std::endl;
    //         for(auto & node : val->fanout_result.at(fanout_id))
    //             o << "  " << *node << std::endl;
    //     }
    //     o << std::endl;
    // }
}

void Mapping::outputValMapping(OpGraphVal* val, std::ostream & o) const
{

    o << "digraph G {\n";
    for(auto & r : mapping.at(val))
    {
        for(auto & fo : r->fanout)
        {
            if(std::find(mapping.at(val).begin(), mapping.at(val).end(), fo) != mapping.at(val).end())
            {
                o << "\"" << *r << "\""<< "->" << "\""<< *fo << "\""<< "\n";
            }
       }
    }

    o << "};\n";
}

void Mapping::check() const
{
}

Mapping withRemappedOps(
    const Mapping& src,
    const std::unordered_map<OpGraph::NodeDescriptor, OpGraph::NodeDescriptor>& forward_mappings
) {
    Mapping result(std::shared_ptr<CGRA>(), src.getII(), std::shared_ptr<OpGraph>());
    for (const auto& src_and_dest_og_nodes : forward_mappings) {
        if (not src.hasMapping(src_and_dest_og_nodes.first)) { continue; }
        for (const auto& mrrg_node : src.getMappingList(src_and_dest_og_nodes.first)) {
            result.mapMRRGNode(src_and_dest_og_nodes.second, mrrg_node);
        }
    }
    return result;
}

/************** MappingGraph ************/
auto MappingGraph::insert(MappingGraphNode node) ->std::pair<NodeDescriptor,bool> {
    nodes.emplace(nextId, std::move(node));
    fanout_lists.emplace(nextId, std::vector<NodeDescriptor>());
    fanin_lists.emplace(nextId, std::vector<NodeDescriptor>());

    auto thisId = nextId;
    nextId.id++;
    return {thisId, true};
}

std::ptrdiff_t MappingGraph::size() const {
    return nodes.size();
}

void MappingGraph::link(MappingGraph::NodeDescriptor driver, MappingGraph::NodeDescriptor fanout) {
    if (driver == fanout) return;   // Self-linking is unneccessary

    // TODO: Perform validity check on the graph prior/after to linking.
    // Update fanouts
    fanout_lists[driver].emplace_back(fanout);

    fanin_lists[fanout].emplace_back(driver);
}

void MappingGraph::unlink(MappingGraph::NodeDescriptor driver, MappingGraph::NodeDescriptor fanout) {

    // TODO: Perform validity check on the graph prior/after to linking.
    // Would this be a good place to do the try and catch? If the two aren't linked then throw an error?
    // Update fanouts
    auto findFanout = std::find(fanout_lists[driver].begin(), fanout_lists[driver].end(), fanout);
    fanout_lists[driver].erase(findFanout);

    // Update fanins
    auto findDriver = std::find(fanin_lists[fanout].begin(), fanin_lists[fanout].end(), driver);
    fanin_lists[fanout].erase(findDriver);
}

void MappingGraph::erase(NodeDescriptor n) {
    if (deletedIds.find(n.id) != deletedIds.end()) return;
    // Delete links at other nodes first
    // Nodes that n fans out from
    for (auto & fin: reversed(fanin_lists.at(n))) {
        unlink(fin, n);
    }
    fanin_lists.erase(fanin_lists.find(n));

    // Nodes that n fans into
    for (auto & fout: reversed(fanout_lists.at(n))) {
        unlink(n, fout);
    }
    fanout_lists.erase(fanout_lists.find(n));

    deletedIds.emplace(n.id);
    nodes.erase(nodes.find(n));
}

MappingGraphNode& MappingGraph::getNodeRef(NodeDescriptor ndesc) {
    return nodes.at(ndesc);
}

const MappingGraphNode& MappingGraph::getNodeRef(NodeDescriptor ndesc) const {
    return nodes.at(ndesc);
}

std::vector<MappingGraph::VerifyMessage> MappingGraph::verify(const MRRG& mrrg, const ToMRRGVertexMap& toMRRG) const {
    /** Conduct BFS from all the functional units, in both directions (fanouts
     * and fanins), any unvisited routing units after BFS should be removed
     * from the graph.
     */

    std::vector<VerifyMessage> verifyMessages;
    std::queue<MappingGraph::NodeDescriptor> toVisit;
    std::unordered_set<MappingGraph::NodeDescriptor, NodeDescriptorHash> visited;
    for (auto & node: nodes)  { // Find the non-routing nodes to start search from
        if (mrrg.getNodeRef(toMRRG.at(node.first)).type == MRRG_NODE_ROUTING ||
        mrrg.getNodeRef(toMRRG.at(node.first)).type == MRRG_NODE_ROUTING_FUNCTION) { continue; }
        toVisit.push(node.first);
    }

    while (!toVisit.empty()) {
        auto currentNode = toVisit.front();
        if (visited.find(currentNode) != visited.end()) {
            toVisit.pop();
            continue;
        }
        visited.emplace(currentNode);
        for (auto & fanout: fanout_lists.at(currentNode)) {
            if (visited.find(fanout) != visited.end()) { continue; }
            toVisit.push(fanout);
        }
        for (auto & fanin: fanin_lists.at(currentNode)) {
            if (visited.find(fanin) != visited.end()) { continue; }
            toVisit.push(fanin);
        }
        toVisit.pop();
    }

    // Find the number of isolated nodes
    int isolatedNodeNum = 0;
    for (auto node: nodes) {
        if (visited.find(node.first) == visited.end()) {
            verifyMessages.emplace_back(VerifyMessage::Type::Warning, mrrg.getNodeRef(toMRRG.at(node.first)).getFullName() + "|" + node.second.op_node_desc->getName() + " is isolated.cg\n"); // Adding the nodes that are isolated to the verify message
            isolatedNodeNum++;
        }
    }

    if (isolatedNodeNum > 0) {
        verifyMessages.emplace_back(VerifyMessage::Type::Warning, "Found " + std::to_string(isolatedNodeNum) + " isolated nodes\n");
    }
    else {
        verifyMessages.emplace_back(VerifyMessage::Type::Info, "No isolated found\n");
    }

    return verifyMessages;
}

void MappingGraph::printDot(std::ostream& os, const MRRG& mrrg, const OpGraph& opgraph, const ToMRRGVertexMap& toMRRG, const ConfigStore& archAttrs) const {
    os << "digraph {\ngraph[";
    // CGRA/Mapping attributes
    os << "II=" << mrrg.initiationInterval() << ", ";
    for (auto & attr : archAttrs) {
        os << attr.first << "=\"" << attr.second << "\", ";
    }
    // Visual attributes
    os << "splines=ortho, concentrate=true, landscape=false];\nnode[shape=record, fixedsize=true, height=0.8, width=2, fontsize=7, fontname=\"times bold\"];\n";

    // Nodes
    std::unordered_map<Module*, std::vector<NodeDescriptor>> moduleToMG;
    for (const auto& mg_ndesc_and_node : this->allNodes()) {
        auto& mg_ndesc = mg_ndesc_and_node.first;
        const auto& mrrg_node = mrrg.getNodeRef(toMRRG.at(mg_ndesc));
        const auto mrrg_node_name = mrrg_node.getFullName();
        const auto& op_name = opgraph.getNodeRef(this->getNodeRef(mg_ndesc).op_node_desc).getName();

        // "node_full_name"[name="node_name", cycle=`cycle`, maps_to="op_name", label="{node_full_name|op_name}"];
        printDotID(os, mrrg_node_name);
        os << "[";
        os << "name=";
        printDotID(os, mrrg_node.name);    // Regular name without cycle
        os << ", cycle=" << mrrg_node.cycle;
        if (dynamic_cast<OpGraph::OpDescriptor>(this->getNodeRef(mg_ndesc).op_node_desc) != NULL) {
            os << ", maps_to_op=";
        }
        else {
            os << ", maps_to_val=";
        }
        printDotID(os, op_name);
        os << ", label=\"{" << mrrg_node_name << "|" << op_name << "}\"";

        os << "];\n";
    }

    // Edges
    for (const auto& mg_ndesc_and_node : this->allNodes()) {
        const auto& mg_ndesc = mg_ndesc_and_node.first;
        const auto& mg_ndesc_fanout = this->fanout(mg_ndesc);

        for (auto fanout : this->fanout(mg_ndesc)) {
            // "driver"->"fanout"
            os << "\"" + mrrg.getNodeRef(toMRRG.at(mg_ndesc)).getFullName() + "\"->" << "\"" + mrrg.getNodeRef(toMRRG.at(fanout)).getFullName() + "\";\n";
        }
    }
    os << "}\n";
}

CreateMappingGraphResult createMappingGraph(const MRRG& mrrg, const Mapping& m) {
    const auto& mapping = m.getMapping();
    MappingGraph mg;
    std::unordered_map<MRRG::NodeDescriptor, MappingGraph::NodeDescriptor> fromMRRG;
    std::unordered_map<MappingGraph::NodeDescriptor, MRRG::NodeDescriptor, MappingGraph::NodeDescriptorHash> toMRRG;

    // Insert the mrrg nodes and their corresponding mapping graph nodes(opgraph nodes). Keep track of the mapping from both ways.
    for (auto & opToMRRGNodeList : mapping) {
        for (auto & mrrgNode : opToMRRGNodeList.second) {
            auto mgNode = mg.insert({opToMRRGNodeList.first}).first;
            toMRRG.emplace(mgNode, mrrgNode);
            fromMRRG.emplace(mrrgNode, mgNode);
        }
    }

    // Linking the mapped MRRG nodes together
    for (auto & opToMRRGNodeList1: mapping) {
        for (auto & mrrgNodeA : opToMRRGNodeList1.second) {
            for (auto & opToMRRGNodeList2: mapping) {
                for (auto & mrrgNodeB : opToMRRGNodeList2.second) {
                    if (mrrgNodeA == mrrgNodeB) continue;

                    // See if A drives B
                    const auto& nodeAfanout = mrrg.fanout(mrrgNodeA);
                    if (std::find(nodeAfanout.begin(), nodeAfanout.end(), mrrgNodeB) != nodeAfanout.end()) {
                        // If so, then link the nodes together
                        mg.link(fromMRRG.at(mrrgNodeA), fromMRRG.at(mrrgNodeB));
                    }
                }
            }
        }
    }

    return {mg, fromMRRG, toMRRG};
}

MappingGraph removeIsolatedRoutingNodes(const MappingGraph& mapping, const MRRG& mrrg, const MappingGraph::ToMRRGVertexMap& toMRRG) {
    MappingGraph fixedGraph(mapping);

    // Repeating the same BFS from the verify, because verify is const
    std::queue<MappingGraph::NodeDescriptor> toVisit;

    std::unordered_set<MappingGraph::NodeDescriptor, MappingGraph::NodeDescriptorHash> visited;


    for (auto & node: fixedGraph.allNodes())  { // Find the non-routing nodes to start search from
        if (mrrg.getNodeRef(toMRRG.at(node.first)).type == MRRG_NODE_ROUTING ||
        mrrg.getNodeRef(toMRRG.at(node.first)).type == MRRG_NODE_ROUTING_FUNCTION) { continue;}
        toVisit.push(node.first);
    }

    while (!toVisit.empty()) {
        auto currentNode = toVisit.front();
        if (visited.find(currentNode) != visited.end()) {
            toVisit.pop();
            continue;
        }
        visited.emplace(currentNode);
        for (auto & fanout: fixedGraph.fanout(currentNode)) {
            if (visited.find(fanout) != visited.end()) { continue; }
            toVisit.push(fanout);
        }
        for (auto & fanin: fixedGraph.fanin(currentNode)) {
            if (visited.find(fanin) != visited.end()) { continue; }
            toVisit.push(fanin);
        }
        toVisit.pop();
    }

    // Erase the unvisited nodes
    std::vector<MappingGraph::NodeDescriptor> nodesToDelete;
    for (auto node: fixedGraph.allNodes()) {
        if (visited.find(node.first) == visited.end()) {
            nodesToDelete.emplace_back(node.first);
        }
    }
    for (auto node: nodesToDelete) {
        fixedGraph.erase(node);
    }

    return std::move(fixedGraph);
}

namespace {

// Helper functions for latencyCheck
bool latencyCheckBFS(
    const MappingGraph& mapping, MappingGraphCycleAssignment& latenciesInfo,
    MappingGraph::NodeDescriptor start,
    std::set<MappingGraph::NodeDescriptor>& checkCycle,
    std::set<MappingGraph::NodeDescriptor>& check0Cycle,
    const MRRG& mrrg, const CreateMappingGraphResult::ToMRRG& toMRRG
);

/**
 * Helper to check for a 0 latency cycle. Returns true if and only if "start"
 * is part of a cycle with latency II.
 */
bool latencyCheckBFSCycle(
    const MappingGraph& mapping, MappingGraph::NodeDescriptor start,
    const MRRG& mrrg, const CreateMappingGraphResult::ToMRRG& toMRRG
);

/**
 * Helper to check for a 0 latency cycle. Returns false if "start" is part of a
 * 0 latency cycle, otherwise returns true.
  */
bool latencyCheck0Cycle(
    const MappingGraph& mapping, MappingGraph::NodeDescriptor start,
    const MRRG& mrrg, const CreateMappingGraphResult::ToMRRG& toMRRG, std::unordered_set<MappingGraph::NodeDescriptor, MappingGraph::NodeDescriptorHash> & nodesLeft
);

}

std::pair<bool, MappingGraphCycleAssignment> latencyCheck(
    const MappingGraph& mapping, const MRRG& mrrg,
    const CreateMappingGraphResult::ToMRRG& toMRRG
) {
    /** Algorithm:
     *  1. Conduct BFS from unvisited nodes, moving in both directions, fanouts
     *      and fanins, tracking the latency needed to reach nodes.
     *  2. During BFS, if a search visits a node with t(n) + II = t(f) +
     *      latency, then it should be added to the set of nodes to check for
     *      cycles
     *  3. For the nodes to check for cycles: Conduct a BFS and ensure that a)
     *      we visit these nodes again to confirm cycle and b) the latency is t
     *      (n) + II.
     */

    bool balanced = true;
    MappingGraphCycleAssignment latenciesInfo;  // Timing information for the nodes

    std::set<MappingGraph::NodeDescriptor> checkCycle; // Nodes to check for a cycle.
    std::set<MappingGraph::NodeDescriptor> check0Cycle; // Nodes to check for a 0 latency cycle.


    std::deque<MappingGraph::NodeDescriptor> toSearch;
    for (auto node: mapping.allNodes()) {
        if (mapping.fanin(node.first).empty()) {
            toSearch.push_front(node.first);
        }
        else {
            toSearch.push_back(node.first);
        }
    }

    // Do BFS starting from the nodes that haven't been visit. Each BFS should operate on 1 connected component of the entire graph
    while (!toSearch.empty()) {
        if (latenciesInfo.find(toSearch.front()) == latenciesInfo.end()) {
            if (!latencyCheckBFS(mapping, latenciesInfo, toSearch.front(), checkCycle, check0Cycle, mrrg, toMRRG)) {
                balanced = false;
            }
        }
        toSearch.pop_front();
    }

    for (auto node : checkCycle) {
        if (!latencyCheckBFSCycle(mapping, node, mrrg, toMRRG)) {
            balanced = false;
        }
    }

    std::unordered_set<MappingGraph::NodeDescriptor, MappingGraph::NodeDescriptorHash> check0CycleLeft;
    for (auto node: check0Cycle) {
        check0CycleLeft.insert(node);
    }
    for (auto node : check0Cycle) {
        if (check0CycleLeft.find(node) != check0CycleLeft.end()) {
            if (!latencyCheck0Cycle(mapping, node, mrrg, toMRRG, check0CycleLeft)) {
                balanced = false;
            }
        }
    }

    return {balanced, latenciesInfo};
}

namespace {

bool latencyCheckBFS(
    const MappingGraph& mapping, MappingGraphCycleAssignment& latenciesInfo,
    MappingGraph::NodeDescriptor start,
    std::set<MappingGraph::NodeDescriptor>& checkCycle,
    std::set<MappingGraph::NodeDescriptor>& check0Cycle,
    const MRRG& mrrg, const CreateMappingGraphResult::ToMRRG& toMRRG
) {
    const auto getMRRGNode = [&](const MappingGraph::NodeDescriptor& mg_ndesc) -> auto& {
        return mrrg.getNodeRef(toMRRG.at(mg_ndesc));
    };

    bool balanced = true;
    MappingGraphCycleAssignment latencies;

    using nodeLatencyPair = std::pair<MappingGraph::NodeDescriptor, int>;
    std::queue<nodeLatencyPair> toVisit;
    toVisit.emplace(start, 0);
    std::queue<bool> isFanout;  // Parallel queue to check if a node emplaced was from a fanout or fanin, used to avoid cycle checking on nodes that were reached by fanin
    isFanout.emplace(true);

    const auto II = mrrg.initiationInterval();
    int offset = 0; // Most negative offset that will have to be added onto each node for all t(n) >= 0

    while (!toVisit.empty()) {
        auto currentNodeLatencyP = toVisit.front();
        bool canCycleCheck = isFanout.front();
        toVisit.pop();
        isFanout.pop();
        // std::cout << "Visiting " << getMRRGNode(currentNodeLatencyP.first).getFullName() << " with latency of " << currentNodeLatencyP.second << "\n";

        if (currentNodeLatencyP.second < offset) {
            offset = currentNodeLatencyP.second;
        }

        // Update overall latencies information
        if (latenciesInfo.find(currentNodeLatencyP.first) != latenciesInfo.end()) {
            if (latenciesInfo[currentNodeLatencyP.first] < currentNodeLatencyP.second) {
                // In the case that we have multiple t(n) for a node, keep the larger value as that is likely more important, there will be a disconnect with this node's other fanins however.
                latenciesInfo[currentNodeLatencyP.first] = currentNodeLatencyP.second;
            }
        }
        else {
            latenciesInfo.emplace(currentNodeLatencyP);
        }

        // If the node has been visited during this search
        if (latencies.find(currentNodeLatencyP.first) != latencies.end()) {
            // Check that t(f) + latency(f) = t(n)
            if (latencies.at(currentNodeLatencyP.first) == currentNodeLatencyP.second) {
                check0Cycle.emplace(currentNodeLatencyP.first);
                continue;
            }
            // Check if t(f) + latency(f) = t(n) + II
            else if (latencies.at(currentNodeLatencyP.first) + II == currentNodeLatencyP.second) {
                if (canCycleCheck)
                    checkCycle.emplace(currentNodeLatencyP.first); // add to set of nodes to check for a cycle
                continue;
            }
            // Check if t(f) + latency(f) = t(n) - II
            else if (latencies.at(currentNodeLatencyP.first) - II == currentNodeLatencyP.second) {
                if (canCycleCheck)
                    checkCycle.emplace(currentNodeLatencyP.first); // add to set of nodes to check for a cycle
                continue;
            }
            else {
                // std::cout << getMRRGNode(currentNodeLatencyP.first).getFullName() << " has inbalanced latency\n";
                balanced = false;
            }
        }
        else {
            latencies.insert(currentNodeLatencyP);
            for (auto & fanout: mapping.fanout(currentNodeLatencyP.first)) {
                // If this fanout has already been visited and the recorded latency matches what we've seen on it, don't need to add to queue
                if (latencies.find(fanout) != latencies.end() && latencies.at(fanout) == currentNodeLatencyP.second + getMRRGNode(currentNodeLatencyP.first).latency) {
                    continue;
                }
                else {
                    // Emplace the fanin onto the queue, with latency = latencyToReachCurrentNode-latency of fanin
                    toVisit.emplace(fanout, currentNodeLatencyP.second+getMRRGNode(currentNodeLatencyP.first).latency);
                    isFanout.emplace(true);
                }
            }
            for (auto & fanin: mapping.fanin(currentNodeLatencyP.first)) {
                if (latencies.find(fanin) != latencies.end()) {
                    // Requirement to search fanin is that it must not have been visited before.
                    continue;
                }
                else {
                    // Emplace the fanin onto the queue, with latency = latencyToReachCurrentNode-latency of fanin
                    toVisit.emplace(fanin, currentNodeLatencyP.second - getMRRGNode(fanin).latency);
                    isFanout.emplace(false);
                }
            }
        }
    }

    // Make all the latencies >= 0
    if (offset < 0) {
        for (auto node: latencies) {
            latenciesInfo[node.first] -= offset;
        }
    }

    return balanced;
}

bool latencyCheckBFSCycle(
    const MappingGraph& mapping, MappingGraph::NodeDescriptor start,
    const MRRG& mrrg, const CreateMappingGraphResult::ToMRRG& toMRRG
) {
    using nodeLatencyPair = std::pair<MappingGraph::NodeDescriptor, int>;
    std::queue<nodeLatencyPair> toVisit;
    const auto II = mrrg.initiationInterval();
    const auto getMRRGNode = [&](const MappingGraph::NodeDescriptor& mg_ndesc) -> auto& {
        return mrrg.getNodeRef(toMRRG.at(mg_ndesc));
    };

    // Start a BFS from this node to try to find itself. If it cannot find itself, latency is unbalanced, or if it does find itself but the delay between != II then the latency is unbalanced.
    bool cycle = false;
    std::unordered_map<MappingGraph::NodeDescriptor, std::unordered_set<int>, MappingGraph::NodeDescriptorHash> visited;
    // std::cout << "Checking if " << getMRRGNode(start).getFullName() << " is part of a cycle\n";

    for (auto & fanout: mapping.fanout(start)) {
        toVisit.emplace(fanout, getMRRGNode(start).latency);
    }

    while (!toVisit.empty()) {
        auto currentNodeLatencyP = toVisit.front();
        toVisit.pop();
        if (visited.find(currentNodeLatencyP.first) != visited.end()) {
            if (visited[currentNodeLatencyP.first].find(currentNodeLatencyP.second) != visited[currentNodeLatencyP.first].end()) {
                continue;
            }
            visited[currentNodeLatencyP.first].emplace(currentNodeLatencyP.second);
        }
        else {
            visited.emplace(currentNodeLatencyP.first, std::unordered_set<int>() = {currentNodeLatencyP.second});
        }

        if (currentNodeLatencyP.first == start) {
            cycle = true;
            // If we've arrived back to the original node, then make sure the delay is correct.
            if (currentNodeLatencyP.second != II) {
                // std::cout << getMRRGNode(currentNodeLatencyP.first).getFullName() << " has inbalanced cyclical latency\n";
                return false;
            }
        }
        /** Only continue searching down this path if the latency hasn't
         * exceeded II, if it has exceeded it then, even if it is a cycle
         * then it has violated the latency constraint.
         */
        else if (currentNodeLatencyP.second <= II) {
            for (auto & fanout: mapping.fanout(currentNodeLatencyP.first)) {
                toVisit.emplace(fanout, currentNodeLatencyP.second+getMRRGNode(currentNodeLatencyP.first).latency);
            }
        }
    }

    // If we never found the orignal node from this search, then this is not a cycle and thus the latency was invalid
    return cycle;
}

bool latencyCheck0Cycle(
    const MappingGraph& mapping, MappingGraph::NodeDescriptor start,
    const MRRG& mrrg, const CreateMappingGraphResult::ToMRRG& toMRRG,
    std::unordered_set<MappingGraph::NodeDescriptor, MappingGraph::NodeDescriptorHash> & nodesToCheck

) {
    using nodeLatencyPair = std::pair<MappingGraph::NodeDescriptor, int>;
    std::queue<nodeLatencyPair> toVisit;
    const auto II = mrrg.initiationInterval();
    const auto getMRRGNode = [&](const MappingGraph::NodeDescriptor& mg_ndesc) -> auto& {
        return mrrg.getNodeRef(toMRRG.at(mg_ndesc));
    };

    for (auto & fanout: mapping.fanout(start)) {
        toVisit.emplace(fanout, getMRRGNode(start).latency);
    }

    while (!toVisit.empty()) {
        auto currentNodeLatencyP = toVisit.front();
        toVisit.pop();
        if (nodesToCheck.find(currentNodeLatencyP.first) != nodesToCheck.end()) {
            nodesToCheck.erase(nodesToCheck.find(currentNodeLatencyP.first));
        }

        if (currentNodeLatencyP.first == start) {
            if (currentNodeLatencyP.second == 0) {
                return false;
            }
        }
        /** Only continue searching down this path if the latency hasn't
         * exceeded II, if it has exceeded it then, even if it is a cycle
         * then it has violated the latency constraint.
         */
        else if (currentNodeLatencyP.second <= II) {
            for (auto & fanout: mapping.fanout(currentNodeLatencyP.first)) {
                toVisit.emplace(fanout, currentNodeLatencyP.second+getMRRGNode(currentNodeLatencyP.first).latency);
            }
        }
    }

    return true;
}

}
