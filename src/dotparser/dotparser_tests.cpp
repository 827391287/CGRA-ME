/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/

#include <CGRA/dotparse.h>

#include <catch2/catch.hpp>

#include <sstream>

namespace {

struct DOTParsingTest {
	std::string input_description;
	bool expect_to_parse;
	std::string input;
	ConfigGraph expected;
};

template<typename Test>
void do_parseDot_test(const Test& test);

ConfigGraph singleNodeCG(
	std::string vertex_name, ConfigStore attrs = {},
	std::string graph_name = {}, ConfigStore graph_attrs = {}
) {
	ConfigGraph cg(std::move(graph_name), std::move(graph_attrs));
	cg.insert(std::move(vertex_name), std::move(attrs));
	return cg;
}

} // end anon namespace

TEST_CASE("DOT Parsing Tests -- Basic Functionality") {
	const auto test = GENERATE(values<DOTParsingTest>({
		{ "empty", true,
			"digraph { }",
			{ },
		},
		{ "EMPTY", true,
			"DIGRAPH { }",
			{ },
		},
		{ "EmPtY", true,
			"DiGrApH { }",
			{ },
		},
		{ "eMpTy", true,
			"dIgRaPh { }",
			{ },
		},
		{ "has graph name", true,
			"digraph G { v }",
			singleNodeCG("v", {}, "G"),
		},
		{ "has graph no name", true,
			"digraph { v }",
			singleNodeCG("v", {}, ""),
		},
		{ "has graph name and attributes", true,
			"digraph G { graph[a=b] v }",
			ConfigGraph("G", {{"a","b"}}, {{"v",{}}}),
		},
		{ "has graph name and attributes", true,
			"digraph G { graph[a=b] v }",
			ConfigGraph("G", {{"a","b"}}, {{"v",{}}}),
		},
		{ "has graph name and attributes, semicolon", true,
			"digraph G { graph[a=b]; v }",
			singleNodeCG("v", {}, "G", {{"a","b"}}),
		},
		{ "with 1 node, no attributes", true,
			"digraph { v }",
			singleNodeCG("v"),
		},
		{ "with 1 node, no attributes, with semicolon", true,
			"digraph { v; }",
			singleNodeCG("v"),
		},
		{ "with 1 node, no attributes, quoted", true,
			"digraph { \"v\" }",
			singleNodeCG("v"),
		},
		{ "with 1 node, 1 attribute", true,
			"digraph { v [a=b] }",
			singleNodeCG("v", {{"a","b"}}),
		},
		{ "with 1 node, 1 attribute, with semicolon", true,
			"digraph { v [a=b]; }",
			singleNodeCG("v", {{"a","b"}}),
		},
		{ "with 1 node, 2 attributes across 2 declarations, no override", true,
			"digraph { v [a=b] v [c=d] }",
			singleNodeCG("v", {{"a","b"},{"c","d"}}),
		},
		{ "with 1 node, 2 attributes across 2 declarations, with override", true,
			"digraph { v [a=b] v [a=c] }",
			singleNodeCG("v", {{"a","c"}}),
		},
		{ "with 1 keyword node, quoted", true,
			"digraph { \"node\" }",
			singleNodeCG("node"),
		},
		{ "with 1 edge, no nodes, no attributes", true,
			"digraph { a->b }",
			ConfigGraph("", {}, {}, {{"a","b",{}}}),
		},
		{ "with 1 edge, 1 node declared before, no attributes", true,
			"digraph { a a->b }",
			ConfigGraph("", {}, {}, {{"a","b",{}}}),
		},
		{ "with 1 edge, 1 node declared before, 1 after, no attributes", true,
			"digraph { a a->b b }",
			ConfigGraph("", {}, {}, {{"a","b",{}}}),
		},
		{ "with 1 edge, 1 node declared before, 1 after, no attributes, with semicolons", true,
			"digraph { a; a->b; b; }",
			ConfigGraph("", {}, {}, {{"a","b",{}}}),
		},
		{ "with 1 edge, no nodes, with attributes", true,
			"digraph { a->b [c=d]}",
			ConfigGraph("", {}, {}, {{"a","b",{{"c","d"}}}}),
		},
		{ "with 2 edges in continuation, no nodes, with attributes", true,
			"digraph { a->b->c [d=e]}",
			[&]{
				ConfigGraph cg;
				auto ab = cg.link(cg.insert("a").first, cg.insert("b").first, {{"d","e"}});
				cg.link(cg.target(ab), cg.insert("c").first, {{"d","e"}});
				return cg;
			}(),
		},
		{ "duplicate edge, no nodes, with attributes, not strict", true,
			"digraph { a->b [c=d]; a->b [c=e]}",
			[&]{
				ConfigGraph cg;
				cg.link(cg.insert("a").first, cg.insert("b").first, {{"c","d"}});
				cg.link(cg.insert("a").first, cg.insert("b").first, {{"c","e"}});
				CHECK(cg.numEdges() == 2);
				return cg;
			}(),
		},
		// requires 'strict' support
		// { "duplicate edge, no nodes, with attribute override, strict", true,
		// 	"strict digraph { a->b [c=d]; a->b [c=e]}",
		// 	[&]{
		// 		ConfigGraph cg;
		// 		cg.link(cg.insert("a").first, cg.insert("b").first, {{"c","e"}});
		// 		return cg;
		// 	}(),
		// },
	}));

	do_parseDot_test(test);
}
TEST_CASE("DOT Parsing Tests -- Default Attributes") {
	const auto test = GENERATE(values<DOTParsingTest>({
		{ "default node attrs -- one node", true,
			"digraph { node[a=b] v }",
			ConfigGraph("", {}, {{"v",{{"a","b"}}}}),
		},
		{ "default node attrs -- one node, with semicolon", true,
			"digraph { node[a=b]; v }",
			ConfigGraph("", {}, {{"v",{{"a","b"}}}}),
		},
		{ "default node attrs -- two nodes", true,
			"digraph { node[a=b] v w }",
			ConfigGraph("", {}, {{"v",{{"a","b"}}}, {"w",{{"a","b"}}}}),
		},
		{ "default node attrs -- two nodes, one before attrs", true,
			"digraph { v node[a=b] w }",
			ConfigGraph("", {}, {{"v",{}}, {"w",{{"a","b"}}}}),
		},
		{ "default node attrs -- two nodes, interleaved defaults, adding more", true,
			"digraph { node[a=a] v node[b=b] w }",
			ConfigGraph("", {}, {{"v",{{"a","a"}}}, {"w",{{"a","a"}, {"b","b"}}}}),
		},
		{ "default node attrs -- two nodes, interleaved defaults, overriding", true,
			"digraph { node[a=a] v node[a=b] w }",
			ConfigGraph("", {}, {{"v",{{"a","a"}}}, {"w",{{"a","b"}}}}),
		},
		{ "default node attrs -- nodes declared in edge", true,
			"digraph { node[a=b] v->w }",
			ConfigGraph("", {}, {{"v",{{"a","b"}}}, {"w",{{"a","b"}}}}, {{"v","w"}}),
		},
		{ "default node attrs -- nodes declared in chained edge", true,
			"digraph { node[a=b] v->w->x }",
			ConfigGraph("", {}, {{"v",{{"a","b"}}}, {"w",{{"a","b"}}}, {"x",{{"a","b"}}}}, {{"v","w"}, {"w","x"}}),
		},
		{ "default edge attrs -- one edge", true,
			"digraph { edge[a=b] v->w }",
			ConfigGraph("", {}, {{"v",{}}, {"w",{}}}, {{"v","w",{{"a","b"}}}}),
		},
		{ "default edge attrs -- one edge, with semicolon", true,
			"digraph { edge[a=b]; v->w }",
			ConfigGraph("", {}, {{"v",{}}, {"w",{}}}, {{"v","w",{{"a","b"}}}}),
		},
		{ "default edge attrs -- two edges", true,
			"digraph { edge[a=b] v->w x->y }",
			ConfigGraph("", {}, {{"v",{}}, {"w",{}}}, {{"v","w",{{"a","b"}}}, {"x","y",{{"a","b"}}}}),
		},
		{ "default edge attrs -- two edges, one before attrs", true,
			"digraph { v->w edge[a=b] x->y }",
			ConfigGraph("", {}, {{"v",{}}, {"w",{}}}, {{"v","w"}, {"x","y",{{"a","b"}}}}),
		},
		{ "default edge attrs -- two edges, interleaved defaults, adding more", true,
			"digraph { edge[a=a] v->w edge[b=b] x->y }",
			ConfigGraph("", {}, {{"v",{}}, {"w",{}}}, {{"v","w",{{"a","a"}}}, {"x","y",{{"a","a"}, {"b","b"}}}}),
		},
		{ "default edge attrs -- two edges, interleaved defaults, overriding", true,
			"digraph { edge[a=a] v->w edge[a=b] x->y }",
			ConfigGraph("", {}, {{"v",{}}, {"w",{}}}, {{"v","w",{{"a","a"}}}, {"x","y",{{"a","b"}}}}),
		},
		{ "default edge attrs -- chain edge", true,
			"digraph { edge[a=b] v->w->x }",
			ConfigGraph("", {}, {{"v",{}}, {"w",{}}}, {{"v","w",{{"a","b"}}}, {"w","x",{{"a","b"}}}}),
		},
	}));

	do_parseDot_test(test);
}

TEST_CASE("DOT Parsing Tests -- Simple Failures") {
	const auto test = GENERATE(values<DOTParsingTest>({
		{ "empty with extra }", false,
			"digraph { }}",
			{ },
		},
		{ "empty but has semicolon", false,
			"digraph { ; }",
			{ },
		},
		{ "with 1 node, no attributes, incorrectly quoted", false,
			"digraph { \"v\"\" }",
			{ },
		},
		{ "with 1 keyword node, not quoted", false,
			"digraph { node }",
			{ },
		},
	}));

	do_parseDot_test(test);
}

TEST_CASE("DOT Parsing Tests -- ID Tests") {
	const auto test = GENERATE(values<DOTParsingTest>({
		{ "empty string node", true,
			"digraph { \"\" }",
			singleNodeCG(""),
		},
		{ "node with escaped quote", true,
			"digraph { \"\\\"\" }",
			singleNodeCG("\""),
		},
		// honestly, this (and the following) is mildly ambiguous, according to my reading of the spec.
		// I think accepting makes slightly more sense because it allows IDs to end in backslashes,
		//   and therefore the only thing that needs to be done to quote an arbitrary string is
		//   to replace internal quotes.
		{ "node with escaped closing quote", true,
			"digraph { \"\\\" }",
			singleNodeCG("\\"),
		},
		{ "node with escaped closing quote, with other chars", true,
			"digraph { \"aaa\\\" }",
			singleNodeCG("aaa\\"),
		},
		{ "node with backslash and escaped quote", true,
			"digraph { \"\\\\\"\" }",
			singleNodeCG("\\\""),
		},
		{ "node with newline", true,
			"digraph { \"\n\" }",
			singleNodeCG("\n"),
		},
		{ "node that ends in a number", true,
			"digraph { abc123 }",
			singleNodeCG("abc123"),
		},
		{ "node that starts with a number", false,
			"digraph { 123abc }",
			{ },
		},
		{ "node that starts with a number -- quoted", true,
			"digraph { \"123abc\" }",
			singleNodeCG("123abc"),
		},
		{ "node is real number -- normal case", true,
			"digraph { 4.5 }",
			singleNodeCG("4.5"),
		},
		{ "node is real number -- negative", true,
			"digraph { -4.5 }",
			singleNodeCG("-4.5"),
		},
		{ "node is real number -- no whole part", true,
			"digraph { .5 }",
			singleNodeCG(".5"),
		},
		{ "node is real number -- no whole part, negative", true,
			"digraph { -.5 }",
			singleNodeCG("-.5"),
		},
		{ "node is real number -- no fractional part", true,
			"digraph { 4. }",
			singleNodeCG("4."),
		},
		{ "node is real number -- no fractional part, negative", true,
			"digraph { -4. }",
			singleNodeCG("-4."),
		},
		{ "node is . -- unquoted", false,
			"digraph { . }",
			{ },
		},
		{ "node is . -- quoted", true,
			"digraph { \".\" }",
			singleNodeCG("."),
		},
		// Some HTML ID tests. Note that the parser is quite permissive (will accept any <...> balanced string),
		//  	but it is only required to accept valid XML.
		{ "node is <>",                   true,  "digraph { <> }",                   singleNodeCG("<>") },
		{ "node is <a>",                  true,  "digraph { <a> }",                  singleNodeCG("<a>") },
		{ "node is <<>",                  false, "digraph { <>> }",                  {} },
		{ "node is <>>",                  false, "digraph { <>> }",                  {} },
		{ "node is <<>>",                 true,  "digraph { <<>> }",                 singleNodeCG("<<>>") },
		{ "node is <<a/>>",               true,  "digraph { <<a/>> }",               singleNodeCG("<<a/>>") },
		{ "node is <<a><a/>>",            true,  "digraph { <<a><a/>> }",            singleNodeCG("<<a><a/>>") },
		{ "node is <<a>b<a/>>",           true,  "digraph { <<a>b<a/>> }",           singleNodeCG("<<a>b<a/>>") },
		{ "node is <<a>b<a/><c>d<c/>>",   true,  "digraph { <<a>b<a/><c>d<c/>> }",   singleNodeCG("<<a>b<a/><c>d<c/>>") },
		{ "node is <<a>b<a/><c/>>",       true,  "digraph { <<a>b<a/><c/>> }",       singleNodeCG("<<a>b<a/><c/>>") },
		{ "node is <<a/><c/>>",           true,  "digraph { <<a/><c/>> }",           singleNodeCG("<<a/><c/>>") },
		{ "node is <<a/>\n<c/>>",         true,  "digraph { <<a/><c/>> }",           singleNodeCG("<<a/><c/>>") },
		{ "node is <<a/></\nc>>",         true,  "digraph { <<a/><c/>> }",           singleNodeCG("<<a/><c/>>") },
		{ "node is <<a>b\n<a/>>",         true,  "digraph { <<a>b<a/>> }",           singleNodeCG("<<a>b<a/>>") },
		{ "node is <<a\n>b<a/>>",         true,  "digraph { <<a>b<a/>> }",           singleNodeCG("<<a>b<a/>>") },
		{ "node is <<a>b<a/>>",           true,  "digraph { <<a>b<a/>> }",           singleNodeCG("<<a>b<a/>>") },
	}));

	do_parseDot_test(test);
}

TEST_CASE("DOT Parsing Tests -- Comments") {
	const auto test = GENERATE(values<DOTParsingTest>({
		{ "// comment -- at beginning of line", true,
			"digraph {\n// abcd \n}",
			{ },
		},
		{ "// comment -- not beginning of line", true,
			"digraph {\n a // abcd \n}",
			singleNodeCG("a"),
		},
		{ "// comment -- comments out closing brace", false,
			"digraph { // }",
			{ },
		},
		{ "# comment -- at beginning of line", true,
			"digraph\n# ababaa\n{ }",
			{ },
		},
		{ "# comment -- not a beginning of line", false,
			"digraph { # }",
			{ },
		},
		{ "block comment middle of graph", true,
			"digraph { /* */ }",
			{ },
		},
		{ "block comment -- comments out opening brace", false,
			"digraph /* { */ }",
			{ },
		},
		{ "block comment -- no closing sequence", false,
			"digraph /* { }",
			{ },
		},
		{ "block comment -- multi-line", true,
			"digraph { /*\n\nabcd*/ a }",
			singleNodeCG("a"),
		},
	}));

	do_parseDot_test(test);
}


namespace {

template<typename T, typename U>
ConfigGraph do_dotParse(const T& input, const U& input_description) {
	auto instr = std::istringstream(input);
	try {
		return parseDot(instr, input_description);
	} catch (const std::exception& e) {
		std::ostringstream s;
		s << "Exception when parsing: " << e.what()
		  << "\nInput was:\n" << input;
		std::throw_with_nested(std::logic_error(s.str()));
	}
}

template<typename Test>
void do_parseDot_test(const Test& test) {
	GIVEN("A '" << test.input_description << "' input graph") {
		WHEN("Parsing the input") {
			THEN("The result should " << (test.expect_to_parse ? "be as expected" : "fail to parse")) {
				if (test.expect_to_parse) {
					const auto actual = do_dotParse(test.input, test.input_description);
					REQUIRE(actual == test.expected);

					AND_WHEN("Parsing the DOT output of the result") {

						auto dot_dump = std::ostringstream{};
						actual.printDot(dot_dump);
						const auto actual2 = do_dotParse(dot_dump.str(), "re-parse: " + test.input_description);

						THEN("The graph should survive intact") {
							REQUIRE(actual2 == test.expected);
						}
					}
				} else {
					REQUIRE_THROWS([&]{
						const auto actual = do_dotParse(test.input, test.input_description);
						std::cout
							<< "Test should have thrown (ie. failed to parse). Dumping return value:\n"
							<< actual << std::endl;
					}());
				}
			}
		}
	}
}

} // end anon namespace
