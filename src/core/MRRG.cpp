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
#include <iostream>
#include <queue>
#include <sstream>
#include <vector>

#include <CGRA/Exception.h>
#include <CGRA/MRRG.h>
#include <CGRA/MRRGProcedures.h>
#include <CGRA/utility/Collections.h>

namespace {
    template<typename OSTREAM>
    bool analyze_mrrg_verify_results(OSTREAM&& os, const std::vector<MRRG::VerifyMessage>& messages,
        const bool silent_on_no_errors
    ) {
        using Type = MRRG::VerifyMessage::Type;

        const auto contains_error = std::any_of(begin(messages), end(messages),
            [](auto&& msg) { return msg.type == Type::Error; });
        const auto has_messages = !messages.empty();
        const auto print_all_messages = contains_error || not silent_on_no_errors;

        if (print_all_messages) {
            if (contains_error) {
                os << "MRRG verify FAILED";
            } else {
                os << "MRRG verify passed";
            }

            if (has_messages) {
                os << ". Begin report:\n";

                for (auto& msg : messages) {
                    os << msg.type << ": " << msg.message << '\n';
                }

                os << "End MRRG verify report\n";
            } else {
                os << ", and nothing to report.\n";
            }

        }

        return contains_error;
    }
}

void MRRGNode::applyReferenceRename(const std::unordered_map<const MRRGNode*, MRRGNode*>& rename_map) {
    for (auto& elem : fanout) {
        elem = value_for_key_or(rename_map, elem, elem);
    }
    for (auto& elem : fanin) {
        elem = value_for_key_or(rename_map, elem, elem);
    }
}

MRRG& MRRG::operator=(const MRRG& rhs) {
    std::unordered_map<const MRRGNode*, MRRGNode*> rename_map;

    this->nodes.clear();
    this->nodes.resize(rhs.initiationInterval());

    for (int cycle = 0; cycle < rhs.initiationInterval(); ++cycle) {
        for (auto& name_and_nodeptr : rhs.allNodesByCycle().at(cycle)) {
            auto new_node = std::make_unique<MRRGNode>(std::move(MRRGNode(*name_and_nodeptr.second)));
            rename_map[name_and_nodeptr.second.get()] = new_node.get();
            this->nodes.at(cycle).emplace(new_node->name, std::move(new_node));
        }
    }

    for (int cycle = 0; cycle < this->initiationInterval(); ++cycle) {
        for (auto& name_and_nodeptr : this->allNodesByCycle().at(cycle)) {
            name_and_nodeptr.second->applyReferenceRename(rename_map);
        }
    }

    return *this;
}

std::pair<MRRG::NodeDescriptor,bool> MRRG::insert(MRRGNode node) {
    auto& nodes_this_cycle = nodes.at(node.cycle);
    const auto& find_result = nodes_this_cycle.find(node.name);

    if (find_result == end(nodes_this_cycle)) {
        auto node_name_copy = node.name;
        auto new_node = std::make_unique<MRRGNode>(std::move(node));

        return {
            nodes_this_cycle.emplace(std::move(node_name_copy), std::move(new_node)).first->second.get(),
            true,
        };

    } else {
        return { find_result->second.get(), false };
    }
}

void MRRG::erase(MRRG::NodeDescriptor n) {
    auto& nodes_in_cycle = nodes.at(getNodeRef(n).cycle);
    auto name_and_node_it = nodes_in_cycle.find(getNodeRef(n).name);

    // check the name
    if (name_and_node_it == end(nodes_in_cycle)) {
        make_and_throw<cgrame_error>([&](auto&& s) {
            s << "tried to erase a node named " << this->getNodeRef(n).getFullName()
                << " but a node with that name doesn't exist!";
        });
    }

    // check that it's the same node
    if (name_and_node_it->second.get() != n) {
        make_and_throw<cgrame_error>([&](auto&& s) {
            s << "tried to erase a node named " << this->getNodeRef(n).getHierarchyQualifiedName()
                << " but the node is not the one expected!";
        });
    }

    // unlink everything. Reversed so that we don't walk over ourselves
    // as unlink removes elements from these lists
    for (auto& fi : reversed(getNodeRef(n).fanin)) {
        unlink(fi, n);
    }
    for (auto& fo : reversed(getNodeRef(n).fanout)) {
        unlink(n, fo);
    }

    // free the node & remove it from the graph
    nodes_in_cycle.erase(name_and_node_it);
}

MRRG::NodeDescriptor MRRG::getNode(int cycle, const std::string& name) const {
    if (cycle < 0 || cycle >= initiationInterval()) {
        return nullptr;
    } else {
        std::unique_ptr<MRRGNode> default_value = nullptr;
        return value_for_key_or(nodes.at(cycle), name, default_value).get();
    }
}

void MRRG::printDot(std::ostream& os, Module* topModule, const ConfigStore& archAttrs) const
{
    std::stringstream arch_args(archAttrs.getString("arch_opts"));
    std::string opt_pair;
    int rows = 0;
    int cols = 0;
    while(arch_args >> opt_pair)
    {
        auto n = opt_pair.find('=');
        if(n == std::string::npos) { make_and_throw<cgrame_error>([&](auto&& s) {
            s << "Ill-formatted C++ architecture option: " << opt_pair;
        });}
        auto key = opt_pair.substr(0, n);
        auto value = opt_pair.substr(n + 1, std::string::npos);
        if (key.find("row") != std::string::npos) {
            rows = std::stoi(value);
            std::cout<<value;
        }
        if (key.find("cols") != std::string::npos) {
            cols = std::stoi(value);
            std::cout<<value;
        }
    }
    std::cout<<rows<<"  "<<cols<<std::endl;
    if (rows == 0 || cols == 0){
        std::cout<<"ERROR"<<std::endl;
        return;
    }
    std::string top = std::to_string(0);
    std::string bottom = std::to_string(cols + 1) ;
    std::string right = std::to_string(rows + 1);
    std::string left = std::to_string(0);

    os << "digraph " <<topModule->getName()<<" {\ngraph[";
    os << "II=" << this->initiationInterval() << ", ";
    for (auto & attr : archAttrs) {
        os << attr.first << "=\"" << attr.second << "\", ";
    }
    // Visual attributes
    os << "splines=ortho, concentrate=true, landscape=false];\nnode[shape=record, fixedsize=true, height=0.8, width=2, fontsize=7, fontname=\"times bold\"];\n";

    for (auto& module : topModule->submodules){
        os << "subgraph cluster_" << module.first<<" {\n";
        bool hasFunctionNode = false;
        std::string moduleName = module.second->getName();
        auto n =moduleName.find("");
        if ((n = moduleName.find("mem_")) != std::string::npos){
            auto row = moduleName.substr(n+4, std::string::npos);
            os << "x_pos = " <<left<<" \n";
            os << "y_pos = " <<row<<" \n";
        } else if ((n = moduleName.find("top_")) != std::string::npos){
            auto col = moduleName.substr(n + 4, std::string::npos);
            os << "x_pos = " <<col<<" \n";
            os << "y_pos = " <<top<<" \n";
        } else if ((n = moduleName.find("bottom_")) != std::string::npos){
            auto col = moduleName.substr(n + 7, std::string::npos);
            os << "x_pos = " <<col<<" \n";
            os << "y_pos = " <<bottom<<" \n";
        } else if ((n = moduleName.find("right_")) != std::string::npos){
            auto row = moduleName.substr(n + 6, std::string::npos);
            os << "x_pos = " <<right<<" \n";
            os << "y_pos = " <<row<<" \n";
        } else if ((n = moduleName.find("pe_")) != std::string::npos){
            auto col = moduleName.substr(n + 4, moduleName.find("_r")); 
            auto row = moduleName.substr(moduleName.find("_r") + 2); 
            os << "x_pos = " <<std::stoi(col) + 1<<" \n";
            os << "y_pos = " <<std::stoi(row) + 1<<" \n";
        } 
        for(int i = 0; i < (std::ptrdiff_t)nodes.size(); i++)
        {
            for(auto n = nodes[i].begin(); n != nodes[i].end(); n++)
            {
                auto& node = n->second;
                if (node->parent != module.second) continue;
                os << "\"" << *(n->second) << "\"";
                if (node->type == MRRG_NODE_ROUTING_FUNCTION) {
                    hasFunctionNode = true;
                    os <<"[ type=MRRG_NODE_ROUTING_FUNCTION, latency=" <<node->latency<<", supportedOps=\"";
                    for (auto op : node->supported_ops){
                        os <<op<<"\t";
                    }
                    os << "\" supportedOperands=\"";
                    for (auto operand_tag : node->supported_operand_tags){
                        os <<operand_tag<<" ";
                    }
                } else if (n->second->type == MRRG_NODE_ROUTING){
                    os <<"[ type=MRRG_NODE_ROUTING, latency=" <<node->latency<<", supportedOperands=\"";
                    for (auto operand_tag : node->supported_operand_tags){
                        os <<operand_tag<<" ";
                    }
                } else if (n->second->type == MRRG_NODE_FUNCTION){
                    hasFunctionNode = true;
                    os <<"[ type=MRRG_NODE_FUNCTION, latency=" <<node->latency<<", supportedOps=\"";
                    for (auto op : node->supported_ops){
                        os <<op<<" ";
                    }
                } else {
                    //TODO::Add an error message
                }
                os << "\"];\n";
            }
        }

        for(int i = 0; i < (std::ptrdiff_t)nodes.size(); i++)
        {

            for(auto n = nodes[i].begin(); n != nodes[i].end(); n++)
            {
                auto& node = n->second;
                if (node->parent != module.second) continue;
                for(auto fanout = node->fanout.begin(); fanout != node->fanout.end(); fanout++)
                {
                    if ((*fanout)->parent == module.second)
                        os << "\"" << *(n->second) << "\"->\"" << **fanout << "\";\n";
                }
                
                
            }
        }
        if (!module.second->submodules.empty() & !hasFunctionNode){
            printSubmoduleDot(os, module.second);
        }
        os <<"}\n";
    }
    for(int i = 0; i < (std::ptrdiff_t)nodes.size(); i++)
    {
        for(auto n = nodes[i].begin(); n != nodes[i].end(); n++)
        {
            if (!topModule->isSubModule(n->second->parent) && n->second->parent != topModule) continue;
            for(auto fanout = n->second->fanout.begin(); fanout != n->second->fanout.end(); fanout++)
            {

                if (!topModule->isSubModule((*fanout)->parent) && (*fanout)->parent != topModule) continue;
                if (n->second->parent == (*fanout)->parent)  continue;
                    os << "\"" << *(n->second) << "\"->\"" << **fanout << "\";\n";
            }
        }
    }
    os << "}\n";
}

void MRRG::printSubmoduleDot(std::ostream& os, Module* submodule) const
{
    for (auto& module : submodule->submodules){
        os << "subgraph cluster_" << module.first<<" {\n";
        bool hasFunctionNode = false;
        for(int i = 0; i < (std::ptrdiff_t)nodes.size(); i++)
        {
            
            for(auto n = nodes[i].begin(); n != nodes[i].end(); n++)
            {
                auto& node = n->second;
                if (node->parent != module.second) continue;
                os << "\"" << *(n->second) << "\"";
                if (node->type == MRRG_NODE_ROUTING_FUNCTION) {
                    hasFunctionNode = true;
                    os <<"[ type=MRRG_NODE_ROUTING_FUNCTION, latency=" <<node->latency<<", supportedOps=\"";
                    for (auto op : node->supported_ops){
                        os <<op<<"\t";
                    }
                    os << "\" supportedOperands=\"";
                    for (auto operand_tag : node->supported_operand_tags){
                        os <<operand_tag<<" ";
                    }
                } else if (n->second->type == MRRG_NODE_ROUTING){
                    os <<"[ type=MRRG_NODE_ROUTING, latency=" <<node->latency<<", supportedOperands=\"";
                    for (auto operand_tag : node->supported_operand_tags){
                        os <<operand_tag<<" ";
                    }
                } else if (n->second->type == MRRG_NODE_FUNCTION){
                    hasFunctionNode = true;
                    os <<"[ type=MRRG_NODE_FUNCTION, latency=" <<node->latency<<", supportedOps=\"";
                    for (auto op : node->supported_ops){
                        os <<op<<" ";
                    }
                } else {
                    //TODO::Add an error message
                }
                os << "\"];\n";
            }
        }

        for(int i = 0; i < (std::ptrdiff_t)nodes.size(); i++)
        {
            for(auto n = nodes[i].begin(); n != nodes[i].end(); n++)
            {
                auto& node = n->second;
                if (node->parent != module.second) continue;
                for(auto fanout = node->fanout.begin(); fanout != node->fanout.end(); fanout++)
                {
                    if ((*fanout)->parent == module.second)
                        os << "\"" << *(n->second) << "\"->\"" << **fanout << "\";\n";
                }
                
                
            }
        }
        if (!module.second->submodules.empty() & !hasFunctionNode){
            printSubmoduleDot(os, module.second);
        }
        os <<"}\n";
    }
    for(int i = 0; i < (std::ptrdiff_t)nodes.size(); i++)
    {
        for(auto n = nodes[i].begin(); n != nodes[i].end(); n++)
        {
            if (!submodule->isSubModule(n->second->parent) && n->second->parent != submodule) continue;
            for(auto fanout = n->second->fanout.begin(); fanout != n->second->fanout.end(); fanout++)
            {
                if (!submodule->isSubModule((*fanout)->parent) && (*fanout)->parent != submodule) continue;
                if (n->second->parent == (*fanout)->parent)  continue;
                    os << "\"" << *(n->second) << "\"->\"" << **fanout << "\";\n";
            }
        }
    }
    return;
}

void MRRG::printSupportedOps(std::ostream& os) const
{
    std::set<std::pair<OpGraphOpCode, int>> supported_operands;
    const auto classes = computeNodeClassLists(*this);

    for (const auto& mrrg_node : classes.function_nodes) {
        for (auto supported_operand : mrrg_node->supported_ops) {
            supported_operands.emplace(std::make_pair(supported_operand, mrrg_node->latency));
        }
    }

    os << "digraph {\n";
    for (auto supp_operand : supported_operands){
        os << "\t" << supp_operand.first << "[OP_LATENCY = " << supp_operand.second << "]" << "\n";
    }
    for (auto supp_operand_i : supported_operands){
        for (auto supp_operand_j : supported_operands){
            os << "\t\"" << supp_operand_i.first << "\"->\"" << supp_operand_j.first << "\" ["; 
            os << "LOWER_BOUND_NETWORK_LATENCY = 1, UPPER_BOUND_NETWORK_LATENCY = "<<std::to_string(1000000)<<"];\n";
        }
    }
        
    os << "}\n";
}

void MRRG::printDot(std::ostream& os) const {
    os << "digraph {\n";
    for (const auto& nodes_in_cycle : nodes) {
        for (const auto& name_and_node : nodes_in_cycle) {
            const auto& node = *name_and_node.second;
            const auto has_latency = node.getLatency() != 0;
            const auto has_operand_tags = not node.supported_operand_tags.empty();
            const bool has_properties = has_latency || has_operand_tags;
            if (has_properties) {
                os << '"' << node << "\" [";
                auto comma = "";
                if (has_latency) {
                    os << comma << "CGRAME_latency=" << node.getLatency();
                    comma = ",";
                }
                if (has_operand_tags) {
                    os << comma << "CGRAME_operand_tags=\"";
                    for (const auto& t : node.supported_operand_tags) os << t << ';';
                    os << '"';
                    comma = ",";
                }
                os << "];\n";
            }
            for (const auto& fanout : node.fanout) {
                os << "\"" << node << "\"->\"" << *fanout << "\";\n";
            }
        }
    }
    os << "}\n";
}

static std::string find_cluster_name(std::string s1, std::string s2)
{
    int last_dot = 0;
    for(int i = 0; i < std::min((std::ptrdiff_t)s1.size(), (std::ptrdiff_t)s2.size()); i++)
    {
        if(s1[i] == s2[i])
        {
            if(s1[i] == '.' || s1[i] == ':')
                last_dot = i;
        }
        else
            break;
    }

    // std::cout << "last dot of (" << s1 << "," << s2 << ") is " << last_dot << "\n";
    return s1.substr(0, last_dot);
}

static bool isDirectSubCluster(std::string c, std::string sub)
{
    if(c == sub || c.size() >= sub.size())
        return false;

    bool found_dot = false;
    int i = 0;
    for(; i < (std::ptrdiff_t)c.size(); i++)
    {
        if(c[i] == sub[i])
        {
            if(sub[i] == '.')
                found_dot = true;
        }
        else
        {
            if(found_dot)
                break;
            else
                return false;
        }
    }
    for(; i < (std::ptrdiff_t)c.size(); i++)
    {
        if(sub[i] == '.')
            return false;
    }
    return true;
}

bool verifyAndPrintReport(const MRRG& mrrg, std::ostream& os,
    bool silent_on_no_errors, bool throw_if_errors, const ConfigStore& extra_opts
) {
    const auto verify_output = mrrg.verify(extra_opts);
    const auto found_errors = analyze_mrrg_verify_results(os, verify_output, silent_on_no_errors);
    if (throw_if_errors && found_errors) {
        make_and_throw<cgrame_error>([&](auto&& s) { s << "MRRG verify check failed! check stdout for results"; });
    } else {
        return not found_errors;
    }
}

static void print_subcluster(std::map<std::string, std::string> & clusters, std::string current_cluster, std::ostream& os)
{
    std::string s = current_cluster;
    if (current_cluster == "")
        return;

    std::string contents = clusters[current_cluster];

    os << "subgraph \"cluster_" << s << "\"{\n";
    for(auto c = clusters.begin(); c != clusters.end(); ++c)
    {
        if(isDirectSubCluster(s, c->first))
        {
            print_subcluster(clusters, c->first, os);
        }
    }
    clusters[current_cluster] = "";
    os << contents << "\nlabel = \"" << s << "\";\n}\n";
}

void MRRG::printDotClustered(std::ostream& os) const
{
    std::map<std::string, std::string> clusters;

    clusters[""] = "";

    for(int i = 0; i < initiationInterval(); i++)
    {
        clusters[std::to_string(i)] = "";
    }

    for(int i = 0; i < (std::ptrdiff_t)nodes.size(); i++) // nodes.begin(); it != nodes.end(); ++it)
    {
        for(auto n = nodes[i].begin(); n != nodes[i].end(); n++)
        {
            for(auto fanout = n->second->fanout.begin(); fanout != n->second->fanout.end(); fanout++)
            {
                std::string srcname = n->second->getFullName();
                std::string dstname = (*fanout)->getFullName();

                std::string subgraph = find_cluster_name(srcname, dstname);

                clusters[subgraph] += "\"" + srcname + "\"->\"" + dstname +  "\";\n";
            }
        }
    }

    os << "digraph {\n";
    for(int i = 0; i < initiationInterval(); i++)
    {
        print_subcluster(clusters, std::to_string(i), os);
    }

    os << "}\n";
}

void MRRG::printBasicStats(std::ostream& os) const {
    std::unordered_set<Module*> unique_modules;
    std::ptrdiff_t num_edges = 0;
    for (int cycle = 0; cycle < initiationInterval(); ++cycle) {
        for (auto& name_and_nodeptr : allNodesByCycle().at(cycle)) {
            num_edges += name_and_nodeptr.second->fanout.size();
            unique_modules.insert(name_and_nodeptr.second->parent);
        }
    }

    os
        << ' ' << size() << " vertices,"
        << ' ' << num_edges << " edges,"
        << ' ' << unique_modules.size() << " unique module instances,"
        << " and end statistics";
}

std::ptrdiff_t MRRG::size() const {
    std::ptrdiff_t num_nodes = 0;
    for (int cycle = 0; cycle < initiationInterval(); ++cycle) {
        for (auto& name_and_nodeptr : allNodesByCycle().at(cycle)) {
            (void)name_and_nodeptr;
            num_nodes += 1;
        }
    }
    return num_nodes;
}

std::string MRRGNode::getFullName() const
{
    return std::to_string(cycle) + ":" + name;
}

/*
float MRRGNode::getCost(float penalty_factor)
{
    return base_cost * occupancy + (occupancy <= capacity ? 0.0 : (occupancy - capacity) * penalty_factor);
}

*/
bool MRRGNode::canMapOp(OpGraphOp const* op) const {
    // If the operation bitwidth does not the mrrg node bitwidth
    // return false
    if (op->bitwidth != bitwidth) return false;

    for (const auto& supported_op : supported_ops) {
        if (op->opcode == supported_op)
            return true;
    }
    return false;
}

std::ostream& operator<< (std::ostream& out, const MRRGNode& node)
{
    return out << node.cycle << ":" << node.name;
}

template <typename T>
static bool check_unique(std::vector<T> v)
{
    if(v.size() < 2)
        return true;

    for(int i = 0; i < (std::ptrdiff_t)v.size() - 1; i++)
    {
        for(int j = i + 1; j < (std::ptrdiff_t)v.size(); j++)
        {
            if(v[i] == v[j])
                return false;
        }
    }

    return true;
}

namespace {

std::pair<bool, MRRG::VerifyMessage::Type> interpret_verify_msg_option(const ConfigStore& options, const std::string& opt_name) {
    const auto opt_value = options.getString(opt_name);
    if (false) { }
    else if (opt_value == "error")   { return {true,  MRRG::VerifyMessage::Type::Error}; }
    else if (opt_value == "warning") { return {true,  MRRG::VerifyMessage::Type::Warning}; }
    else if (opt_value == "info")    { return {true,  MRRG::VerifyMessage::Type::Info}; }
    else if (opt_value == "ignore")  { return {false, MRRG::VerifyMessage::Type::Info}; }
    else { throw std::logic_error(
        "don't understand MRRG verify check option " + opt_name + "=" + opt_value
    ); }
};

} // end anon namespace

std::vector<MRRG::VerifyMessage> MRRG::verify(const ConfigStore& overrides) const
{
    ConfigStore options {
        {"check_mux_exclusivity", "ignore"},
    };
    override_all(options, overrides); // throws if something is mis-spelt

    const auto mux_exclusivity_enabled_and_msg_type = interpret_verify_msg_option(options, "check_mux_exclusivity");

    std::vector<VerifyMessage> result;
    const auto& add_result = [&](VerifyMessage::Type type, auto&& f) {
        result.emplace_back(VerifyMessage{ type, string_from_stream(std::forward<decltype(f)>(f)) });
    };
    const auto& add_error = [&](auto&& f) { add_result(VerifyMessage::Type::Error, std::forward<decltype(f)>(f)); };
    const auto& add_warning = [&](auto&& f) { add_result(VerifyMessage::Type::Warning, std::forward<decltype(f)>(f)); };
    const auto& add_info = [&](auto&& f) { add_result(VerifyMessage::Type::Info, std::forward<decltype(f)>(f)); };
    (void)add_warning; (void)add_info; // Remove this line when these are used (it silences a warning)

    const auto& is_operand_node = [&](NodeDescriptor n) {return !n->supported_operand_tags.empty(); };

    for (int cycle = 0; cycle < initiationInterval(); ++cycle) {
        for (auto& name_and_node : nodes.at(cycle)) {
            if (name_and_node.first != name_and_node.second->name) { add_error([&](auto&& s) {
                s << "Node " << *name_and_node.second
                  << " is stored under the incorrect name '" << name_and_node.first << '\'';
            });}
            if (cycle != name_and_node.second->cycle) { add_error([&](auto&& s) {
                s << "Node " << *name_and_node.second
                  << " is stored under the incorrect cycle number of " << cycle;
            });}
        }
    }

    const auto node_classes = computeNodeClassLists(*this);

    // Check every routing node in the MRRG
    for(auto & r : node_classes.routing_nodes)
    {
        // check that fanins and fanouts are unique (i.e. no duplicate pointers)
        if(!check_unique(r->fanin))
        {
            add_error([&](auto&& s) { s << "Fanins not unique for node: " << *r; });
        }
        if(!check_unique(r->fanout))
        {
            add_error([&](auto&& s) { s << "Fanouts not unique for node: " << *r; });
        }
        // check that for every fanout, there is a fanin
        for(auto & fo : r->fanout)
        {

            bool found_r = false;
            for(auto & fi : fo->fanin)
            {
                if(fi == r)
                {
                    found_r = true;
                    break;
                }
            }

            if(!found_r)
            {
                add_error([&](auto&& s) { s << "Missing back link: " << *r << " <- " << *fo; });
            }
        }

        // Check that paths between operand nodes have a FU node through BFS
        if (is_operand_node(r)) {
            std::unordered_set<NodeDescriptor> visited = {r};
            std::queue<NodeDescriptor> toVisit;
            for (auto & fanout : r->fanout) {
                if (fanout->type == MRRG_NODE_ROUTING) {
                    toVisit.emplace(fanout);
                }
            }
            while (!toVisit.empty()) {
                auto n = toVisit.front();
                toVisit.pop();
                if (is_operand_node(n)) {
                    add_error([&](auto&& s) { s << "Path between operand nodes: " << *r << " and " << *n << " does not contain a FU node"; });
                }
                if (fanin(n).size() != 1) {
                    add_error([&](auto&& s) {
                        s << "Routing node between operand node " << *n << " and it's FU has more than one fanin."
                          << " There should be no other way to reach these nodes via fanouts."
                          << " This allows CGRA-ME to use simpler routing algorithms.";
                    });
                }
                visited.emplace(n);
                for (auto & fanout : n->fanout) {
                    if (fanout->type == MRRG_NODE_ROUTING && visited.find(fanout) == visited.end()) {
                        toVisit.emplace(fanout);
                    }
                }
            }
        }
    }

    for(auto & r : node_classes.function_nodes)
    {
        // check that fanins and fanouts are unique (i.e. no duplicate pointers)
        if(!check_unique(r->fanin))
        {
            add_error([&](auto&& s) { s << "Fanins not unique for node: " << *r; });
        }

        if(!check_unique(r->fanout))
        {
            add_error([&](auto&& s) { s << "Fanouts not unique for node: " << *r; });
        }

        // check that for every fanout, there is a fanin
        for(auto & fo : r->fanout)
        {

            bool found_r = false;
            for(auto & fi : fo->fanin)
            {
                if(fi == r)
                {
                    found_r = true;
                    break;
                }
            }

            if(!found_r) {
                add_error([&](auto&& s) { s << "Missing back link: " << *r << " <- " << *fo; });
            }
        }
    }

    for (const auto& nodes_for_ii : nodes) {
        for (const auto& name_and_nodeptr : nodes_for_ii) {
            const auto& node = *name_and_nodeptr.second;
            const auto& expected_fanout_cycle = (node.cycle + node.latency) % initiationInterval();

            // is latency annotated correctly
            for (const auto& fanoutptr : node.fanout) {
                const auto& fanout = *fanoutptr;

                if (fanout.cycle != (int)expected_fanout_cycle) {
                    add_error([&](auto&& s) {
                        s << "Node {" << node << "}, with latency " << node.latency
                            << " fans-out to node {" << fanout << "}, which does not have the expected cycle of "
                            << expected_fanout_cycle << " (II == " << this->initiationInterval() << ")";
                    });
                }
            }

            // 'mux invariant'
            if (mux_exclusivity_enabled_and_msg_type.first && node.fanin.size() > 1) {
                for (const auto& faninptr : node.fanin) {
                    const auto& fanin = *faninptr;
                    if (fanin.fanout.size() > 1) {
                        add_result(mux_exclusivity_enabled_and_msg_type.second, [&](auto&& s) {
                            s << "Node {" << node << "}, with fanin degree, greater than one, of " << node.fanin.size() << " has fanin node {" << fanin << "} which also has fanout degree, greater than one, of " << fanin.fanout.size();
                        });
                    }
                }
            }
        }
    }

    return result;
}

void MRRG::renameNode(NodeDescriptor ndesc, std::string new_name) {
    auto& the_node = getNodeRef(ndesc);
    auto& nodes_in_cycle = nodes.at(the_node.cycle);

    auto old_node_position = nodes_in_cycle.find(the_node.name);
    if (old_node_position == end(nodes_in_cycle)) {
        make_and_throw<cgrame_error>([&](auto&& s) {
            s << "This node " << the_node << " does not exist in this graph";
        });
    }

    auto seach_result_for_new_name = nodes_in_cycle.find(new_name);
    if (seach_result_for_new_name != end(nodes_in_cycle)) {
        make_and_throw<cgrame_error>([&](auto&& s) {
            s << "A node named " << new_name << " already exists! Cannot rename " << the_node;
        });
    }

    nodes_in_cycle[new_name] = std::move(old_node_position->second);
    nodes_in_cycle.erase(old_node_position);
    the_node.name = std::move(new_name);
}

static void link(MRRGNode* driver, MRRGNode* fanout)
{
    // std::cout << "linking {" << driver << "} -> {" << fanout << "}\n"; // for debugging. Prepare for spam.
    driver->fanout.push_back(fanout);
    fanout->fanin.push_back(driver);
}

static void unlink(MRRGNode* driver, MRRGNode* fanout)
{
    // std::cout << "unlinking {" << driver << "} -> {" << fanout << "}\n"; // for debugging. Prepare for spam.
    auto elem = std::find(driver->fanout.begin(), driver->fanout.end(), fanout);
    if(elem == driver->fanout.end()) {
        make_and_throw<cgrame_error>([&](auto&& s) {
            s << "trying to remove link {" << driver << "} -> {" << fanout << "} but link doesn't exist";
        });
    }
    driver->fanout.erase(elem);

    auto elem1 = std::find(fanout->fanin.begin(), fanout->fanin.end(), driver);
    if(elem1 == fanout->fanin.end()) {
        make_and_throw<cgrame_error>([&](auto&& s) {
            s << "trying to remove link {" << driver << "} -> {" << fanout << "} but link wasn't set up properly";
        });
    }
    fanout->fanin.erase(elem1);
}

void MRRG::link(NodeDescriptor driver, NodeDescriptor fanout) { ::link(&getNodeRef(driver), &getNodeRef(fanout)); }
void MRRG::unlink(NodeDescriptor driver, NodeDescriptor fanout) { ::unlink(&getNodeRef(driver), &getNodeRef(fanout)); }
