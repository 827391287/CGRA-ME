// A Bison parser, made by GNU Bison 3.5.1.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2020 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.

// "%code top" blocks.
#line 32 "dot.y"

    #include "../DotParserDriver.h"
    #include <sstream>

#line 43 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"




#include "dot.tab.hh"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 141 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"


  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  parser::parser (DotParserDriver& d_yyarg, DOTScannerWrapper& scanner_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      d (d_yyarg),
      scanner (scanner_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_number_type
  parser::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[+state];
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 47: // kvp
        value.YY_MOVE_OR_COPY< AttributeKVP > (YY_MOVE (that.value));
        break;

      case 43: // attr_list
      case 44: // maybe_attr_list
      case 45: // a_list
        value.YY_MOVE_OR_COPY< AttributeStorage > (YY_MOVE (that.value));
        break;

      case 52: // edge_rhs
        value.YY_MOVE_OR_COPY< std::list<std::string> > (YY_MOVE (that.value));
        break;

      case 3: // ID
      case 4: // QUOTED_ID
      case 5: // BAD_ID
      case 6: // "<"
      case 7: // ">"
      case 8: // DIR_EDGEOP
      case 9: // UNDIR_EDGEOP
      case 10: // HTML_CONTENT
      case 25: // LEX_ERROR
      case 48: // id
      case 49: // html_id
      case 50: // html
      case 53: // edge_target
      case 56: // node_id
      case 58: // subgraph
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 47: // kvp
        value.move< AttributeKVP > (YY_MOVE (that.value));
        break;

      case 43: // attr_list
      case 44: // maybe_attr_list
      case 45: // a_list
        value.move< AttributeStorage > (YY_MOVE (that.value));
        break;

      case 52: // edge_rhs
        value.move< std::list<std::string> > (YY_MOVE (that.value));
        break;

      case 3: // ID
      case 4: // QUOTED_ID
      case 5: // BAD_ID
      case 6: // "<"
      case 7: // ">"
      case 8: // DIR_EDGEOP
      case 9: // UNDIR_EDGEOP
      case 10: // HTML_CONTENT
      case 25: // LEX_ERROR
      case 48: // id
      case 49: // html_id
      case 50: // html
      case 53: // edge_target
      case 56: // node_id
      case 58: // subgraph
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 47: // kvp
        value.copy< AttributeKVP > (that.value);
        break;

      case 43: // attr_list
      case 44: // maybe_attr_list
      case 45: // a_list
        value.copy< AttributeStorage > (that.value);
        break;

      case 52: // edge_rhs
        value.copy< std::list<std::string> > (that.value);
        break;

      case 3: // ID
      case 4: // QUOTED_ID
      case 5: // BAD_ID
      case 6: // "<"
      case 7: // ">"
      case 8: // DIR_EDGEOP
      case 9: // UNDIR_EDGEOP
      case 10: // HTML_CONTENT
      case 25: // LEX_ERROR
      case 48: // id
      case 49: // html_id
      case 50: // html
      case 53: // edge_target
      case 56: // node_id
      case 58: // subgraph
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 47: // kvp
        value.move< AttributeKVP > (that.value);
        break;

      case 43: // attr_list
      case 44: // maybe_attr_list
      case 45: // a_list
        value.move< AttributeStorage > (that.value);
        break;

      case 52: // edge_rhs
        value.move< std::list<std::string> > (that.value);
        break;

      case 3: // ID
      case 4: // QUOTED_ID
      case 5: // BAD_ID
      case 6: // "<"
      case 7: // ">"
      case 8: // DIR_EDGEOP
      case 9: // UNDIR_EDGEOP
      case 10: // HTML_CONTENT
      case 25: // LEX_ERROR
      case 48: // id
      case 49: // html_id
      case 50: // html
      case 53: // edge_target
      case 56: // node_id
      case 58: // subgraph
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    // User initialization code.
#line 37 "dot.y"
{
    yyla.location.initialize(&d.getFileName()); // set the initial location's filename, so it shows up in error messages
}

#line 579 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (d, scanner, yyla));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case 47: // kvp
        yylhs.value.emplace< AttributeKVP > ();
        break;

      case 43: // attr_list
      case 44: // maybe_attr_list
      case 45: // a_list
        yylhs.value.emplace< AttributeStorage > ();
        break;

      case 52: // edge_rhs
        yylhs.value.emplace< std::list<std::string> > ();
        break;

      case 3: // ID
      case 4: // QUOTED_ID
      case 5: // BAD_ID
      case 6: // "<"
      case 7: // ">"
      case 8: // DIR_EDGEOP
      case 9: // UNDIR_EDGEOP
      case 10: // HTML_CONTENT
      case 25: // LEX_ERROR
      case 48: // id
      case 49: // html_id
      case 50: // html
      case 53: // edge_target
      case 56: // node_id
      case 58: // subgraph
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2:
#line 58 "dot.y"
                                                           { d.setGraphName(std::move(yystack_[3].value.as < std::string > ())); }
#line 738 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 4:
#line 62 "dot.y"
                       { error(yylhs.location, "strict keyword not implemented"); YYERROR; }
#line 744 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 6:
#line 65 "dot.y"
                      { error(yylhs.location, "undirected graphs not implemented"); YYERROR; }
#line 750 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 16:
#line 77 "dot.y"
                                { d.appendGraphAttributes(std::move(yystack_[0].value.as < AttributeStorage > ())); }
#line 756 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 17:
#line 78 "dot.y"
                                { d.appendDefaultNodeAttributes(std::move(yystack_[0].value.as < AttributeStorage > ())); }
#line 762 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 18:
#line 79 "dot.y"
                                { d.appendDefaultEdgeAttributes(std::move(yystack_[0].value.as < AttributeStorage > ())); }
#line 768 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 19:
#line 81 "dot.y"
                                       { set_all(yystack_[0].value.as < AttributeStorage > (),yystack_[2].value.as < AttributeStorage > ()); yylhs.value.as < AttributeStorage > () = yystack_[0].value.as < AttributeStorage > (); }
#line 774 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 20:
#line 82 "dot.y"
                                       { yylhs.value.as < AttributeStorage > () = yystack_[0].value.as < AttributeStorage > (); }
#line 780 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 21:
#line 83 "dot.y"
                                       { yylhs.value.as < AttributeStorage > () = AttributeStorage(); }
#line 786 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 22:
#line 84 "dot.y"
                                       { yylhs.value.as < AttributeStorage > () = yystack_[1].value.as < AttributeStorage > (); }
#line 792 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 23:
#line 86 "dot.y"
                  { yylhs.value.as < AttributeStorage > () = yystack_[0].value.as < AttributeStorage > (); }
#line 798 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 24:
#line 86 "dot.y"
                              { yylhs.value.as < AttributeStorage > () = AttributeStorage(); }
#line 804 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 25:
#line 88 "dot.y"
                                          { yystack_[0].value.as < AttributeStorage > ().setString(yystack_[2].value.as < AttributeKVP > ().first, yystack_[2].value.as < AttributeKVP > ().second); yylhs.value.as < AttributeStorage > () = yystack_[0].value.as < AttributeStorage > (); }
#line 810 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 26:
#line 89 "dot.y"
                                          { yylhs.value.as < AttributeStorage > () = AttributeStorage{yystack_[0].value.as < AttributeKVP > ()}; }
#line 816 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 30:
#line 93 "dot.y"
                         { yylhs.value.as < AttributeKVP > () = AttributeKVP(yystack_[0].value.as < std::string > (), "true"); }
#line 822 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 31:
#line 94 "dot.y"
                         { yylhs.value.as < AttributeKVP > () = AttributeKVP(yystack_[2].value.as < std::string > (),     yystack_[0].value.as < std::string > ()); }
#line 828 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 32:
#line 96 "dot.y"
                { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 834 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 33:
#line 97 "dot.y"
                          {
                    yystack_[0].value.as < std::string > ().erase(0, 1); // remove first quote
                    yystack_[0].value.as < std::string > ().erase(yystack_[0].value.as < std::string > ().size()-1, 1); // remove last quote
                    // for each instance of \"
                    const auto next_bs_qu = [&](auto i) { return (std::ptrdiff_t)yystack_[0].value.as < std::string > ().find("\\\"", i); };
                    for (auto i = next_bs_qu(0); i != yystack_[0].value.as < std::string > ().npos; i = next_bs_qu(i+1)) {
                        yystack_[0].value.as < std::string > ().erase(i, 1); // remove the backslash
                    }
                    yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > ();
                }
#line 849 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 34:
#line 107 "dot.y"
                { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 855 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 35:
#line 110 "dot.y"
                                         { yylhs.value.as < std::string > () = yystack_[2].value.as < std::string > () + yystack_[1].value.as < std::string > () + yystack_[0].value.as < std::string > (); }
#line 861 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 36:
#line 113 "dot.y"
                             { yylhs.value.as < std::string > () = yystack_[1].value.as < std::string > () + yystack_[0].value.as < std::string > (); }
#line 867 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 37:
#line 114 "dot.y"
                                  { yylhs.value.as < std::string > () = yystack_[1].value.as < std::string > () + yystack_[0].value.as < std::string > (); }
#line 873 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 38:
#line 115 "dot.y"
                {}
#line 879 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 39:
#line 117 "dot.y"
                                                     {
                    auto lhs = yystack_[2].value.as < std::string > ();
                    for (const auto& rhs : yystack_[1].value.as < std::list<std::string> > ()) {
                        d.addEdge(lhs, rhs, yystack_[0].value.as < AttributeStorage > ());
                        lhs = rhs;
                    }
                }
#line 891 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 40:
#line 126 "dot.y"
                                            { yystack_[0].value.as < std::list<std::string> > ().push_front(yystack_[1].value.as < std::string > ()); yylhs.value.as < std::list<std::string> > () = yystack_[0].value.as < std::list<std::string> > (); }
#line 897 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 41:
#line 127 "dot.y"
                                   { yylhs.value.as < std::list<std::string> > () = { yystack_[0].value.as < std::string > () }; }
#line 903 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 42:
#line 129 "dot.y"
                { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 909 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 43:
#line 130 "dot.y"
                { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 915 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 46:
#line 134 "dot.y"
                                        { d.addOrCreateNode(std::move(yystack_[1].value.as < std::string > ()), std::move(yystack_[0].value.as < AttributeStorage > ())); }
#line 921 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 47:
#line 136 "dot.y"
                { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 927 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 48:
#line 137 "dot.y"
                        { yylhs.value.as < std::string > () = yystack_[1].value.as < std::string > (); }
#line 933 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 51:
#line 143 "dot.y"
                                            { error(yylhs.location, "subgraphs not implemented"); YYERROR; yylhs.value.as < std::string > () = yystack_[3].value.as < std::string > (); }
#line 939 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 52:
#line 144 "dot.y"
                                            { error(yylhs.location, "subgraphs not implemented"); YYERROR; yylhs.value.as < std::string > () = ""; }
#line 945 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;

  case 53:
#line 145 "dot.y"
                                            { error(yylhs.location, "subgraphs not implemented"); YYERROR; yylhs.value.as < std::string > () = ""; }
#line 951 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"
    break;


#line 955 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[+yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yy_error_token_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yy_error_token_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    std::ptrdiff_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */
    if (!yyla.empty ())
      {
        symbol_number_type yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];

        int yyn = yypact_[+yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yy_error_token_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char parser::yypact_ninf_ = -16;

  const signed char parser::yytable_ninf_ = -44;

  const signed char
  parser::yypact_[] =
  {
     -10,   -16,     3,    29,   -16,   -16,   -16,    54,   -16,   -16,
       0,    47,    -9,   -16,     0,     0,     5,    -4,    70,    -4,
      -4,    47,     2,    10,   -16,    28,   -16,    46,   -16,    -4,
      57,    47,   -16,   -16,   -16,    15,   -16,    47,    17,   -16,
     -16,    61,   -16,    47,   -16,    27,    27,   -16,   -16,   -16,
      -4,    66,   -16,   -16,    68,    -4,    25,    55,    63,    80,
      47,   -16,   -16,   -16,    75,   -16,   -16,    46,   -16,   -16,
     -16,    -4,   -16,   -16,    27,    27,   -16,    82,    62,   -16,
     -16,   -16,   -16,   -16,   -16,   -16,   -16,   -16,   -16,   -16,
     -16,   -16,   -16,   -16,   -16
  };

  const signed char
  parser::yydefact_[] =
  {
       5,     4,     0,     0,     1,     6,     7,     0,    32,    33,
      38,    10,     0,    34,    38,    38,     0,     0,     0,     0,
       0,    10,     0,    10,    13,    47,    12,     0,    11,    24,
      15,    10,    37,    36,    35,     0,    16,    10,     0,    17,
      18,     0,     3,    10,     9,     0,     0,    48,    45,    44,
      24,     0,    23,    46,     0,    21,     0,    29,    30,     0,
      10,    53,     8,    14,    49,    39,    42,    41,    43,     2,
      20,    22,    28,    27,     0,     0,    52,     0,     0,    40,
      19,    25,    31,    51,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    50
  };

  const signed char
  parser::yypgoto_[] =
  {
     -16,   -16,   -16,   -16,   -14,   -16,   -16,   -15,    35,    30,
     -16,   -16,    -7,    33,    67,   -16,    34,    51,   -16,   -16,
     -16,   -16,    52,   -16
  };

  const signed char
  parser::yydefgoto_[] =
  {
      -1,     2,     3,     7,    22,    23,    24,    52,    53,    56,
      74,    57,    25,    13,    16,    26,    50,    27,    51,    28,
      29,    47,    30,    94
  };

  const signed char
  parser::yytable_[] =
  {
      12,     1,    36,     4,    39,    40,    10,    41,    31,    44,
      14,    38,    34,     8,     9,    35,    10,    54,     8,     9,
      42,    10,    17,    59,    18,    19,    20,    21,    58,    62,
       8,     9,    43,    10,    60,    55,   -42,   -42,    63,    64,
      70,     5,     6,    15,    66,    71,    77,    15,    15,    45,
       8,     9,    46,    10,    48,    49,    80,     8,     9,    17,
      10,    18,    19,    20,    21,   -43,   -43,    58,    82,     8,
       9,    11,    10,     8,     9,   -26,    10,    72,    73,    61,
      18,    32,    33,    21,    75,    65,    69,    37,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    76,    78,
      83,    79,    67,    68,    81
  };

  const signed char
  parser::yycheck_[] =
  {
       7,    11,    17,     0,    19,    20,     6,    21,    17,    23,
      10,    18,     7,     3,     4,    19,     6,    31,     3,     4,
      18,     6,    12,    37,    14,    15,    16,    17,    35,    43,
       3,     4,    22,     6,    17,    20,     8,     9,    45,    46,
      55,    12,    13,    10,    51,    20,    60,    14,    15,    21,
       3,     4,    24,     6,     8,     9,    71,     3,     4,    12,
       6,    14,    15,    16,    17,     8,     9,    74,    75,     3,
       4,    17,     6,     3,     4,    20,     6,    22,    23,    18,
      14,    14,    15,    17,    21,    50,    18,    17,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    18,    24,
      18,    67,    51,    51,    74
  };

  const signed char
  parser::yystos_[] =
  {
       0,    11,    37,    38,     0,    12,    13,    39,     3,     4,
       6,    17,    48,    49,    10,    49,    50,    12,    14,    15,
      16,    17,    40,    41,    42,    48,    51,    53,    55,    56,
      58,    17,    50,    50,     7,    19,    43,    17,    48,    43,
      43,    40,    18,    22,    40,    21,    24,    57,     8,     9,
      52,    54,    43,    44,    40,    20,    45,    47,    48,    40,
      17,    18,    40,    48,    48,    44,    48,    53,    58,    18,
      43,    20,    22,    23,    46,    21,    18,    40,    24,    52,
      43,    45,    48,    18,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    59
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    36,    37,    37,    38,    38,    39,    39,    40,    40,
      40,    41,    41,    41,    41,    41,    42,    42,    42,    43,
      43,    43,    43,    44,    44,    45,    45,    46,    46,    46,
      47,    47,    48,    48,    48,    49,    50,    50,    50,    51,
      52,    52,    53,    53,    54,    54,    55,    56,    56,    57,
      57,    58,    58,    58,    59,    59,    59,    59,    59,    59,
      59,    59,    59,    59
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     6,     5,     1,     0,     1,     1,     3,     2,
       0,     1,     1,     1,     3,     1,     2,     2,     2,     4,
       3,     2,     3,     1,     0,     3,     1,     1,     1,     0,
       1,     3,     1,     1,     1,     3,     2,     2,     0,     3,
       3,     2,     1,     1,     1,     1,     2,     1,     2,     2,
       4,     5,     4,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "ID", "QUOTED_ID", "BAD_ID",
  "\"<\"", "\">\"", "DIR_EDGEOP", "UNDIR_EDGEOP", "HTML_CONTENT", "STRICT",
  "GRAPH", "DIGRAPH", "SUBGRAPH", "NODE", "EDGE", "\"{\"", "\"}\"",
  "\"[\"", "\"]\"", "\"=\"", "\";\"", "\",\"", "\":\"", "LEX_ERROR",
  "\"n\"", "\"ne\"", "\"e\"", "\"se\"", "\"s\"", "\"sw\"", "\"w\"",
  "\"nw\"", "\"c\"", "\"_\"", "$accept", "graph", "maybe_strict",
  "graph_type", "stmt_list", "stmt", "attr_stmt", "attr_list",
  "maybe_attr_list", "a_list", "maybe_alist_sep", "kvp", "id", "html_id",
  "html", "edge_stmt", "edge_rhs", "edge_target", "edgeop", "node_stmt",
  "node_id", "port", "subgraph", "compas_pt", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned char
  parser::yyrline_[] =
  {
       0,    58,    58,    59,    62,    62,    65,    65,    67,    68,
      69,    71,    72,    73,    74,    75,    77,    78,    79,    81,
      82,    83,    84,    86,    86,    88,    89,    91,    91,    91,
      93,    94,    96,    97,   107,   110,   113,   114,   115,   117,
     126,   127,   129,   130,   132,   132,   134,   136,   137,   139,
     140,   143,   144,   145,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147
  };

  // Print the state stack on the debug stream.
  void
  parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  parser::yy_reduce_print_ (int yyrule)
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 1410 "/media/maple_tb_disk/raghebom/cgrame-release/src/dotparser/generated/dot.tab.cc"

#line 149 "dot.y"


void yy::parser::error (const location_type& l, const std::string& m)
{
    throw d.makeError(l, m);
}
