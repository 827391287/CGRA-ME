/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#ifndef __DOT_PARSE_H___
#define __DOT_PARSE_H___

#include <CGRA/utility/ConfigGraph.h>

#include <iosfwd>
#include <string>

/**
 * Reads a DOT (https://www.graphviz.org/doc/info/lang.html) formatted istream,
 *   and produces a graph with string IDs that can be then converted into
 *   some other data structure.
 * The return value is essentially uninterpreted nodes and edges
 *   with string-key=string-value attributes.
 * The return type provides many checked conversion functions.
 * Throws a class derived from std::exception if any parsing errors,
 *   whose `.what()` will contain information about the error and it's location.
 *
 * Some fine points about the DOT format:
 *   - Vertex attributes are merged if a vertex is specified more that once,
 *     with later declarations' values taking precedence.
 *   - On the other hand, for non-strict graphs, duplicate edges will result in a multi-graph.
 *   - You can specify default node & edge values.
 *   - Attribute values (the "=value" part) are optional,
 *     and is the string "true" if unspecified (note: this is different from the empty string).
 *   - Semicolons and commas are mostly optional!
 *   - The DOT format defines many standard attributes, though mostly related to layout.
 *     I recommend prefixing any custom attributes.
 *   - ID rules are the same for node names, edge names, attribute names, *and* attribute values.
 *   - The usual valid ID is basically a "C" identifier, so quote it if it's not
 *     alphanum+underscore & not starting with a number.
 *   - Quoting rules are different from C, though: backslash only escapes double quote, nothing else.
 *   - (X)HTML IDs are allowed, ie. internally balanced quoting with '<' and '>'
 *   - Unquoted numeric IDs of the form [0-9]*\.[0-9]* are allowed (but not '.' by itself).
 *   - All nodes names are in the same namespace, including those in subgraphs.
 *
 * Limitations: (will throw if encountered unless specified otherwise)
 *   - Subgraphs are not supported
 *      - likely implementation: an attribute on the vertex/edge.
 *   - The 'strict' keyword is not implemented.
 *   - Undirected graph behaviour is not implemented, and will throw.
 *   - Ports are parsed but discarded.
 *      - likely implementation: the standard "headport" & "tailport" attributes
 */
ConfigGraph parseDot(std::istream& input, std::string file_name);

#endif
