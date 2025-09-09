/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef CONFIG_GRAPH_H___
#define CONFIG_GRAPH_H___

#include <CGRA/ConfigStore.h>
#include <CGRA/utility/Collections.h>

#include <iomanip>
#include <iostream>
#include <map>
#include <string>

/**
 * Helper function for printing DOT IDs (nodes, attribute key & values)
 * Escapes quotes correctly, and mostly doesn't quote things unless necessary
 */
inline auto printDotID(std::ostream& os, const std::string& id) -> std::ostream& {
    // handle some edge cases, and keywords
    if (   id.empty()      || id == "."
        || id == "node"    || id == "edge" || id == "graph"
        || id == "digraph" || id == "subgraph" || id == "strict") {
        return os << '"' << id << '"';
    }

    // just alphanum+underscore, not starting with digit
    if (std::all_of(id.begin(), id.end(), [&](const auto& c){
        return std::isalnum(c) || c == '_';
    }) && not std::isdigit(*id.begin())) {
        return os << id;
    }

    // The general case. Escapes quotes.
    os << '"';
    for (std::ptrdiff_t i = 0; i != (std::ptrdiff_t)id.size(); ++i) {
        if (id[i] == '"') os << '\\';
        os << id[i];
    }
    return os << '"';
}

/* A helper class for printing string-convertable objects as DOT IDs.
 * see dotIDPrinter(T&) below */
template<typename T>
struct DotIDPrinter {
    T* value_ptr;
    friend auto operator<<(std::ostream& os, const DotIDPrinter& rhs) -> std::ostream& {
        return printDotID(os, *rhs.value_ptr);
    }
};

/**
 * A make_* function for DotIDPrinter. Constructs one from the argument given.
 * Note: the return value uses the address of the argument.
 * Ex.
 *     std::cout << dotIDPrinter("abc");
 */
template<typename T>
auto dotIDPrinter(T& t) -> DotIDPrinter<T> { return {&t}; }

/**
 * Intended to be used as an uninterpreted representation of the DOT format.
 */
class ConfigGraph {
public:
    struct VertexID;
    struct EdgeID;

    // used in constructor (hack for compilers/stdlibs with explicit tuple constructors)
    struct NameNameConfigStore { std::string name1, name2; ConfigStore cs; };

    /**
     * General constructor.
     * Can take a name, graph properties, 0 or more vertices, and 0 or more edges.
     * Like the DOT format, edges will create an vertices that have not been mentioned
     *  with empty attribute list(s).
     *
     * Eg.
     * ConfigGraph("", {}, {}, {{"a","b",{{"c","d"}}}}); // digraph { a->b [c=d] }
     * ConfigGraph("G", {{"a","b"}}, {{"v",{}}}); // digraph G { graph[a=b] v }
     */
    template<
        typename VertexList = std::vector<std::pair<std::string,ConfigStore>>,
        typename EdgeList = std::vector<NameNameConfigStore>
    >
    ConfigGraph(
        std::string graph_name_ = {}, ConfigStore graph_attributes_ = {},
        VertexList vertices_ = {}, EdgeList edges_ = {}
    )
        : graph_name(std::move(graph_name_))
        , graph_attributes(std::move(graph_attributes_))
    {
        for (const auto& v : vertices_) { insert(std::move(v.first), std::move(v.second)); }
        for (const auto& e : edges_) { link(insert(std::move(e.name1)).first,
            insert(std::move(e.name2)).first, std::move(e.cs)); }
    }

    /**
     * Get/set graph's attributes
     */
    const ConfigStore& graphAttributes() const& { return graph_attributes; }
    void setGraphAttributes(ConfigStore&& attrs) { graph_attributes = std::move(attrs); }
    void setGraphAttributes(const ConfigStore& attrs) { graph_attributes = attrs; }

    /**
     * Get/set graph's name
     */
    const std::string& graphName() const& { return graph_name; }
    void setGraphName(const std::string& s) { graph_name = s; }
    void setGraphName(std::string&& s) { graph_name = std::move(s); }

    /**
     * Get the name/unique identifier for a given vertex ID
     */
    const std::string& name(const VertexID& v) const { return v.id; }

    /**
     * Get all attributes associated with the given vertex or edge
     */
    const ConfigStore& attributes(const VertexID& v) const { return vertices.at(v); }
    const ConfigStore& attributes(const EdgeID& e)   const { return    edges.at(e); }

    /**
     * Get nodes at either end of an edge
     */
    const VertexID& target(const EdgeID& e) const { return e.target; }
    const VertexID& source(const EdgeID& e) const { return e.source; }
    VertexID target(EdgeID&& e) const { return std::move(e.target); }
    VertexID source(EdgeID&& e) const { return std::move(e.source); }

    /**
     * For iterating the nodes in this graph
     */
    auto allNodes() const { return make_first_of_pair_range(vertices); }

    /**
     * Access to all the edges that originate at `v`.
     * Note: graph may be a multi-graph
     */
    auto outEdges(const VertexID& v) const {
        return make_second_of_pair_range(make_iterator_range(out_edges.equal_range(v)));
    }

    /**
     * For when you just want the vertices.
     * Note: if graph is a multi-graph, there may be duplicate entries
     */
    auto fanout(const VertexID& v) const {
        std::vector<VertexID> result;
        for (const auto& e : outEdges(v)) { result.push_back(e.target); }
        return result;
    }

    /**
     * Add a node to this graph.
     * If `name` is not unique, then any `vertex_attrs` passed-in
     *   will be merged into the existing vertex's attributes,
     *   overriding any that share the same key.
     * If name is not unique, the bool in the return value will be false,
     *   and the existing vertex ID will be returned.
     * Otherwise, the Vertex ID will be for the newly added node,
     *   and the bool will be true.
     *
     */
    std::pair<VertexID,bool> insert(std::string name, ConfigStore vertex_attrs = {}) {
        auto vid = VertexID{ name };
        auto it_and_is_new = vertices.insert({vid, {}});
        if (it_and_is_new.second) {
            it_and_is_new.first->second = std::move(vertex_attrs);
        } else {
            set_all(it_and_is_new.first->second, std::move(vertex_attrs));
        }
        return {vid,true};
    }

    /**
     * Add an edge to this graph.
     * Always inserts a new edge, and the returned value will uniquely
     *   represent this edge.
     * Call with the same `source` and `target` to create a multi-graph.
     */
    EdgeID link(VertexID source, VertexID target, ConfigStore edge_attrs = {}) {
        decltype(EdgeID::id) edge_count = 0;
        for (const auto& fo : fanout(source)) {
            if (fo == target) { edge_count += 1; }
        }
        auto edge_id = EdgeID{ source, std::move(target), edge_count };
        out_edges.emplace(std::move(source), edge_id);
        edges.emplace(edge_id, std::move(edge_attrs));
        return edge_id;
    }

    /**
     * Print out the contents in DOT format.
     * Should not loose any information; it will represent this graph exactly.
     */
    std::ostream& printDot(std::ostream& os) const {
        const auto print_attribute_list = [](auto& os, const auto& attrs) -> auto& {
            if (attrs.empty()) { return os; }
            os << " [";
            bool first_iter = true;
            for (const auto& kv : attrs) {
                if (not first_iter) { os << ", "; }
                printDotID(os,kv.first) << "="; printDotID(os,kv.second);
                first_iter = false;
            }
            return os << "]";
        };

        os << "digraph " << dotIDPrinter(graph_name) << " {\n";
        if (not graph_attributes.empty()) {
            os << "    graph"; print_attribute_list(os,graph_attributes) << ";\n";
        }

        for (const auto& v : allNodes()) {
            os << "    "; printDotID(os,name(v));
            print_attribute_list(os,attributes(v)) << ";\n";
        }

        for (const auto& v : allNodes()) {
            for (const auto& e : outEdges(v)) {
                os << "    "; printDotID(os,name(source(e))) << " -> "; printDotID(os,name(target(e)));
                print_attribute_list(os,attributes(e)) << ";\n";
            }
        }
        return os << '}';
    }

    friend std::ostream& operator<<(std::ostream& os, const ConfigGraph& cs) { return cs.printDot(os); }
    friend bool operator==(const ConfigGraph& lhs, const ConfigGraph& rhs);

    bool empty() const { return vertices.empty(); }
    std::ptrdiff_t numVertices() const { return vertices.size(); }
    std::ptrdiff_t numEdges()    const { return edges.size(); }

    ///
    /// end member functions; start struct definitions
    ///

    struct VertexID {
        bool operator<(const VertexID& rhs) const {
            return std::tie(this->id) < std::tie(rhs.id);
        }
        bool operator==(const VertexID& rhs) const {
            return std::tie(this->id) == std::tie(rhs.id);
        }
        friend std::ostream& operator<<(std::ostream& os, const VertexID& v) {
            return os << v.id;
        }
    private:
        VertexID(std::string id_) : id(std::move(id_)) { }
        friend ConfigGraph;
        std::string id;
    };

    struct EdgeID {
        bool operator<(const EdgeID& rhs) const {
            return std::tie(this->source, this->target, this->id)
                 < std::tie(  rhs.source,   rhs.target,   rhs.id);
        }
        bool operator==(const EdgeID& rhs) const {
            return std::tie(this->source, this->target, this->id)
                == std::tie(  rhs.source,   rhs.target,   rhs.id);
        }
        friend std::ostream& operator<<(std::ostream& os, const EdgeID& e) {
            return os << e.source << "-" << e.id << "->" << e.target;
        }
    private:
        EdgeID(VertexID source, VertexID target, int id)
            : source(std::move(source)), target(std::move(target)), id(id)
        { }
        friend ConfigGraph;
        VertexID source; VertexID target; int id;
    };

private:
    std::string graph_name;
    ConfigStore graph_attributes;

    std::map<VertexID,ConfigStore> vertices = {};
    std::multimap<VertexID,EdgeID> out_edges = {};
    std::map<EdgeID,ConfigStore> edges = {};

    friend bool operator==(const ConfigGraph& lhs, const ConfigGraph& rhs) {
        return std::tie(lhs.graph_name, lhs.graph_attributes, lhs.vertices, lhs.out_edges, lhs.edges)
            == std::tie(rhs.graph_name, rhs.graph_attributes, rhs.vertices, rhs.out_edges, rhs.edges);
    }
};

#endif /* CONFIG_GRAPH_H___ */
