%{
    #include <CGRA/dotparse.h>

    #include "../DotParserDriver.h"
    #include "dot.tab.hh"

    #include <iostream>
    #include <iterator>
    #include <sstream>
    #include <string>

    using Token = yy::parser::token;

    using YYSTYPE = yy::parser::semantic_type;
    using YYLTYPE = yy::location;

    /* Init line and column info... isn't done for some reason when the input is a buffer
       (run only once per instance, the first time yylex is called) */
    #define YY_USER_INIT \
        yylineno = 1; \
        yycolumn = 0; \

    /* Maintain **Bison** line and column info (runs before every rule) */
    #define YY_USER_ACTION \
        yylloc->begin.line = yylloc->end.line; \
        yylloc->begin.column = yylloc->end.column; \
        yycolumn += yyleng; \
        yylloc->end.line = yylineno; \
        yylloc->end.column = yycolumn; \

    struct ExtraLexData {
        int html_quote_depth = 0; // depth of < and > quoting. We need to know when to exit the HTML start state.
    };
    #define YY_EXTRA_TYPE ExtraLexData
%}

/* DOT is one file, so generate EOF when end-of-file is hit */
%option noyywrap

/* error if no matching rule for current character */
%option nodefault

/* no global variables; every function takes a scanner argument */
%option reentrant

/* so we don't have to manually update the line number */
%option yylineno

/* Add YYLTYPE* to yylex (implies bison-bridge, which adds YYSTYPE*),
 *   and define some helper variables to access it (namely yylloc)
 * Allows us to update the Bison location from this lexer.
 */
%option bison-locations

/* %option debug */

/* A state for when in a block comment */
%x BLOCK_COMMENT
/* A state for HTML IDs */
%x HTML

/* Characters that DOT considers to be alphabetic */
ALPH [a-zA-Z_\200-\377]

%%

[ \t\n]+ { /* ignore whitespace */ }

"["   { return Token::OS; }
"]"   { return Token::CS; }
"{"   { return Token::OB; }
"}"   { return Token::CB; }
"="   { return Token::EQ; }
";"   { return Token::SEMI; }
","   { return Token::COMM; }
":"   { return Token::COLN; }

"->"  { return Token::DIR_EDGEOP; }
"--"  { return Token::UNDIR_EDGEOP; }

(?i:strict)    { return Token::STRICT; }
(?i:graph)     { return Token::GRAPH; }
(?i:digraph)   { return Token::DIGRAPH; }
(?i:subgraph)  { return Token::SUBGRAPH; }
(?i:node)      { return Token::NODE; }
(?i:edge)      { return Token::EDGE; }

"." { /* reject: not a number */ return Token::BAD_ID; }

-?[0-9]*\.[0-9]* {
    // Numbers: an optional "-", then an optional whole part
    //   then a '.', then an optional fractional part.
    // Just '.' is an error, and handled by a previous rule
    return Token::ID;
}

[0-9]+{ALPH}+ { /* reject: starts with a digit */ return Token::BAD_ID; }

([0-9]|{ALPH})+ {
    // Anything alphanum+underscore. Anything starting with a digit was rejected earlier
    return Token::ID;
}

\"([^\\\"]*(\\\"?)?)*\" {
    // a wide variety of quoted strings. Backslash escapes ", but nothing else
    return Token::QUOTED_ID;
}

<INITIAL,HTML>"<" {
    if (yyextra.html_quote_depth == 0) { BEGIN(HTML); }
    yyextra.html_quote_depth += 1;
    return Token::HTML_BEGIN;
}
<HTML>[^<>]+ { return Token::HTML_CONTENT; /* gobble up anything except the quotes */ }
<HTML>">"   {
    yyextra.html_quote_depth -= 1;
    if (yyextra.html_quote_depth == 0) { BEGIN(INITIAL); }
    return Token::HTML_END;
}

^#[^\n]*     { /* a comment -- ignore until end-of-line */ }
"//"[^\n]*   { /* a comment -- ignore until end-of-line */ }

"/*"                  { BEGIN(BLOCK_COMMENT); }
<BLOCK_COMMENT>"*/"   { BEGIN(INITIAL); }
<BLOCK_COMMENT>.|\n   { }

<*>.       { return Token::LEX_ERROR; }
<<EOF>>    { return Token::END; }

%%

ConfigGraph parseDot(std::istream& input, std::string file_name) {
    return DotParserDriver(input, std::move(file_name)).getGraph();
}

struct DOTScannerWrapper {
    yyscan_t scanner = {};

    DOTScannerWrapper(const std::string& bytes) {
        if (yylex_init(&scanner)) {
            throw std::system_error(errno, std::generic_category(), "yylex_init failure");
        }
        // makes an internal copy
        yy_scan_bytes(bytes.data(), bytes.size(), scanner);
    }

    ~DOTScannerWrapper() { yylex_destroy(scanner); }
};

yy::parser::symbol_type yylex(DotParserDriver& d, DOTScannerWrapper& scanner_, yy::parser::symbol_type& context) {
    const auto& scanner = scanner_.scanner; // for convenience

    // call the real yylex
    const auto token = yylex(&context.value, &context.location, scanner);

    // convert the token to a real Bison token
    switch (token) {
        // simple tokens
        case Token::OS:
        case Token::CS:
        case Token::OB:
        case Token::CB:
        case Token::EQ:
        case Token::SEMI:
        case Token::COMM:
        case Token::COLN:
        case Token::END:
        case Token::STRICT:
        case Token::GRAPH:
        case Token::DIGRAPH:
        case Token::SUBGRAPH:
        case Token::NODE:
        case Token::EDGE: return yy::parser::symbol_type(token, context.location);

        // strings
        case Token::DIR_EDGEOP:
        case Token::UNDIR_EDGEOP:
        case Token::QUOTED_ID:
        case Token::HTML_END:
        case Token::HTML_CONTENT:
        case Token::HTML_BEGIN:
        case Token::BAD_ID:
        case Token::ID: return yy::parser::symbol_type(token, yyget_text(scanner), context.location);

        // errors
        case Token::LEX_ERROR:
            throw d.makeError(context.location, std::string("scanning error at ") + yyget_text(scanner));
    }
    throw d.makeError(context.location, "lexer token conversion: unhandled token id " + std::to_string(token));
}

DotParserDriver::DotParserDriver(std::istream& input, std::string file_name_)
    : file_name(std::move(file_name_))
{
    DOTScannerWrapper scanner([&]() { // (immediately invoked)
        std::stringstream temp;
        temp << input.rdbuf();
        return temp.str();
    }());

    yy::parser parser(*this, scanner);
    // parser.set_debug_level(true); // for debugging
    if (parser.parse()) {
        throw std::logic_error("DOT parser returned failure");
    }
}

std::logic_error DotParserDriver::makeError(const yy::location& l, const std::string& m) const {
    std::ostringstream ss;
    ss << l << ": " << m;
    return std::logic_error(ss.str());
}
