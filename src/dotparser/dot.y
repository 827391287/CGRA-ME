%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.3.2"

%code requires /* appears in generated header (.tab.hh) */ {
    #include <CGRA/ConfigStore.h>

    #include <list>
    #include <map>
    #include <string>

    class DotParserDriver;
    class DOTScannerWrapper;

    using AttributeStorage = ConfigStore;
    using AttributeKVP = std::pair<std::string, std::string>;
    #ifndef NDEBUG // enable debugging unless disabled (same as %define parse.trace)
    # define YYDEBUG 1
    #endif
}

/* Matches yylex provided in DotParserDriver.h */
%param { DotParserDriver& d }
%param { DOTScannerWrapper& scanner }
%lex-param { yy::location& yyla }
%locations /* enable location tracking -- mostly for better error messages */
%defines /* enable generating the header file. Scanner code needs it. */
%define api.token.constructor /* Tell bison that our value type needs to be constructed */
%define api.value.type variant /* Use a variant-like type (instead of a union) for values */
%define parse.assert /* Check that types in value type are cast correctly, and also destructed properly */
%define parse.error verbose /* Add some context to syntax error messages */

%code top /* appears in generated source (.tab.cc) */ {
    #include "../DotParserDriver.h"
    #include <sstream>
}

%initial-action {
    @$.initialize(&d.getFileName()); // set the initial location's filename, so it shows up in error messages
}

%start	graph
%token END 0 "end of file"
%token <std::string> ID QUOTED_ID BAD_ID
%token <std::string> HTML_BEGIN "<" HTML_END ">"
%token <std::string> DIR_EDGEOP UNDIR_EDGEOP HTML_CONTENT
%token STRICT GRAPH DIGRAPH SUBGRAPH NODE EDGE
%token OB "{" CB "}"
%token OS "[" CS "]"
%token EQ "=" SEMI ";" COMM "," COLN ":"
%token <std::string> LEX_ERROR

%type <AttributeStorage> a_list attr_list maybe_attr_list;
%type <AttributeKVP> kvp
%type <std::string> edge_target subgraph node_id id html html_id
%type <std::list<std::string>> edge_rhs

%%
graph       :   maybe_strict graph_type id OB stmt_list CB { d.setGraphName(std::move($3)); }
            |   maybe_strict graph_type    OB stmt_list CB

                // todo: handle strict mode -- not a multi-graph
maybe_strict:   STRICT { error(@$, "strict keyword not implemented"); YYERROR; } |

                // todo: handle undirected
graph_type  :   GRAPH { error(@$, "undirected graphs not implemented"); YYERROR; } | DIGRAPH

stmt_list   :   stmt SEMI stmt_list
            |   stmt      stmt_list
            |

stmt        :   node_stmt
            |   edge_stmt
            |   attr_stmt
            |   id EQ id
            |   subgraph

attr_stmt   :   GRAPH attr_list { d.appendGraphAttributes(std::move($2)); }
            |   NODE  attr_list { d.appendDefaultNodeAttributes(std::move($2)); }
            |   EDGE  attr_list { d.appendDefaultEdgeAttributes(std::move($2)); }

attr_list   :   OS a_list CS attr_list { set_all($4,$2); $$ = $4; }
            |   OS        CS attr_list { $$ = $3; }
            |   OS        CS           { $$ = AttributeStorage(); }
            |   OS a_list CS           { $$ = $2; }

maybe_attr_list : attr_list | { $$ = AttributeStorage(); }

a_list      :  kvp maybe_alist_sep a_list { $3.setString($1.first, $1.second); $$ = $3; }
            |  kvp                        { $$ = AttributeStorage{$1}; }

maybe_alist_sep : COMM | SEMI | // comma, semicolon, or nothing!

kvp         :   id       { $$ = AttributeKVP($1, "true"); }
            |   id EQ id { $$ = AttributeKVP($1,     $3); }

id          :   ID
            |   QUOTED_ID {
                    $1.erase(0, 1); // remove first quote
                    $1.erase($1.size()-1, 1); // remove last quote
                    // for each instance of \"
                    const auto next_bs_qu = [&](auto i) { return (std::ptrdiff_t)$1.find("\\\"", i); };
                    for (auto i = next_bs_qu(0); i != $1.npos; i = next_bs_qu(i+1)) {
                        $1.erase(i, 1); // remove the backslash
                    }
                    $$ = $1;
                }
            |   html_id

                // note: html IDs are defined to be quoted with < and >
html_id     :   HTML_BEGIN html HTML_END { $$ = $1 + $2 + $3; }

                // check for < and > balancing, but nothing else... leave checking other stuff to an HTML engine.
html        :   html_id html { $$ = $1 + $2; }
            |   HTML_CONTENT html { $$ = $1 + $2; }
            |   {} // empty string is well defined!

edge_stmt   :   edge_target edge_rhs maybe_attr_list {
                    auto lhs = $1;
                    for (const auto& rhs : $2) {
                        d.addEdge(lhs, rhs, $3);
                        lhs = rhs;
                    }
                }

                // todo check edgeop type
edge_rhs    :   edgeop edge_target edge_rhs { $3.push_front($2); $$ = $3; }
            |   edgeop edge_target { $$ = { $2 }; }

edge_target :   id
            |   subgraph

edgeop      :   UNDIR_EDGEOP | DIR_EDGEOP

node_stmt   :   node_id maybe_attr_list { d.addOrCreateNode(std::move($1), std::move($2)); }

node_id     :   id
            |   id port { $$ = $1; } // todo: do something with port

port        :   COLN id
            |   COLN id COLN compas_pt

            // todo handle subgraphs at all eg:  digraph { a -> {b c} }
subgraph    :   SUBGRAPH id OB stmt_list CB { error(@$, "subgraphs not implemented"); YYERROR; $$ = $2; }
            |   SUBGRAPH    OB stmt_list CB { error(@$, "subgraphs not implemented"); YYERROR; $$ = ""; } // todo: alloc anon ID?
            |               OB stmt_list CB { error(@$, "subgraphs not implemented"); YYERROR; $$ = ""; } // todo: alloc anon ID?

compas_pt   :   "n" | "ne" | "e" | "se" | "s" | "sw" | "w" | "nw" | "c" | "_"

%%

void yy::parser::error (const location_type& l, const std::string& m)
{
    throw d.makeError(l, m);
}
