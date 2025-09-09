/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef DOTPARSER_DRIVER_HH
#define DOTPARSER_DRIVER_HH

// contains forward declarations for various things, see dot.y's "code requires" section
#include "generated/dot.tab.hh"

#include <CGRA/utility/ConfigGraph.h>

#include <string>

class DotParserDriver
{
    public:
        DotParserDriver(std::istream& input, std::string file_name_);

        /**
         * Access the graph.
         * && overload allows for moving the graph out of a rvalue *this
         */
              ConfigGraph  getGraph() &&     { return std::move(graph); }
        const ConfigGraph& getGraph() const& { return graph; }

        /**
         * Filename we are parsing. Mutable overloaded needed because bison
         * wants a pointer to mutable string...
         */
        const std::string& getFileName() const& { return file_name; }
              std::string& getFileName() &      { return file_name; }

        /**
         * Make something human readable from the location `l` and message `m`.
         */
        std::logic_error makeError(const yy::location& l, const std::string& m) const;

        /**
         * Set/append graph properties
         */
        void setGraphName(std::string name) { graph.setGraphName(std::move(name)); }
        void appendGraphAttributes(ConfigStore attrs) { graph.setGraphAttributes(std::move(attrs)); }

        /**
         * Update the default attributes. Overwrites any duplicate keys
         */
        void appendDefaultNodeAttributes(ConfigStore new_attrs) { set_all(default_node_attributes, std::move(new_attrs)); }
        void appendDefaultEdgeAttributes(ConfigStore new_attrs) { set_all(default_edge_attributes, std::move(new_attrs)); }

        /**
         * Create a node, respecting any default node attributes
         */
        ConfigGraph::VertexID addOrCreateNode(std::string name, ConfigStore custom_attrs = {}) {
            add_all(custom_attrs, default_node_attributes);
            return graph.insert(std::move(name), custom_attrs).first;
        }

        /**
         * Create a node, respecting any default edge and default node attributes
         */
        ConfigGraph::EdgeID addEdge(std::string src_vname, std::string target_vname, ConfigStore custom_attrs = {}) {
            set_all(custom_attrs, default_edge_attributes);
            return graph.link(addOrCreateNode(src_vname), addOrCreateNode(target_vname), custom_attrs);
        }

        // The yylex that we call from Bison
        friend yy::parser::symbol_type yylex(
            DotParserDriver& d, DOTScannerWrapper& scanner, yy::parser::symbol_type& context);
    private:
        // The name of the file being parsed.
        // Used later to pass the file name to the location tracker.
        std::string file_name;

        ConfigGraph graph = {};

        ConfigStore default_node_attributes = {};
        ConfigStore default_edge_attributes = {};
};


#endif /* DOTPARSER_DRIVER_HH */
