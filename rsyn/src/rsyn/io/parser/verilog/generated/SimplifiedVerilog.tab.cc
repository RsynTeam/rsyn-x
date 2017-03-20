// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

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


// First part of user declarations.

#line 37 "SimplifiedVerilog.tab.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "SimplifiedVerilog.tab.hh"

// User implementation prologue.

#line 51 "SimplifiedVerilog.tab.cc" // lalr1.cc:412
// Unqualified %code blocks.
#line 27 "SimplifiedVerilog.yy" // lalr1.cc:413

   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   /* include for all driver functions */
   #include "../SimplifiedVerilogReader.h"

#undef yylex
#define yylex scanner.yylex

#line 65 "SimplifiedVerilog.tab.cc" // lalr1.cc:413


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
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

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
      *yycdebug_ << std::endl;                  \
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
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 4 "SimplifiedVerilog.yy" // lalr1.cc:479
namespace Parsing {
#line 151 "SimplifiedVerilog.tab.cc" // lalr1.cc:479

  /// Build a parser object.
  SimplifiedVerilogParser::SimplifiedVerilogParser (SimplifiedVerilogScanner &scanner_yyarg, SimplifiedVerilogReader &reader_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg),
      reader (reader_yyarg)
  {}

  SimplifiedVerilogParser::~SimplifiedVerilogParser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  SimplifiedVerilogParser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  SimplifiedVerilogParser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  SimplifiedVerilogParser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
      switch (other.type_get ())
    {
      case 8: // IDENTIFIER
        value.copy< std::string > (other.value);
        break;

      default:
        break;
    }

  }


  template <typename Base>
  inline
  SimplifiedVerilogParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {
    (void) v;
      switch (this->type_get ())
    {
      case 8: // IDENTIFIER
        value.copy< std::string > (v);
        break;

      default:
        break;
    }
}


  // Implementation of basic_symbol constructor for each type.

  template <typename Base>
  SimplifiedVerilogParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  SimplifiedVerilogParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::string v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  template <typename Base>
  inline
  SimplifiedVerilogParser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  SimplifiedVerilogParser::basic_symbol<Base>::clear ()
  {
    // User destructor.
    symbol_number_type yytype = this->type_get ();
    basic_symbol<Base>& yysym = *this;
    (void) yysym;
    switch (yytype)
    {
   default:
      break;
    }

    // Type destructor.
    switch (yytype)
    {
      case 8: // IDENTIFIER
        value.template destroy< std::string > ();
        break;

      default:
        break;
    }

    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  SimplifiedVerilogParser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  SimplifiedVerilogParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
      switch (this->type_get ())
    {
      case 8: // IDENTIFIER
        value.move< std::string > (s.value);
        break;

      default:
        break;
    }

    location = s.location;
  }

  // by_type.
  inline
  SimplifiedVerilogParser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  SimplifiedVerilogParser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  SimplifiedVerilogParser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  SimplifiedVerilogParser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  SimplifiedVerilogParser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  SimplifiedVerilogParser::by_type::type_get () const
  {
    return type;
  }
  // Implementation of make_symbol for each symbol type.
  SimplifiedVerilogParser::symbol_type
  SimplifiedVerilogParser::make_END (const location_type& l)
  {
    return symbol_type (token::END, l);
  }

  SimplifiedVerilogParser::symbol_type
  SimplifiedVerilogParser::make_MODULE (const location_type& l)
  {
    return symbol_type (token::MODULE, l);
  }

  SimplifiedVerilogParser::symbol_type
  SimplifiedVerilogParser::make_END_MODULE (const location_type& l)
  {
    return symbol_type (token::END_MODULE, l);
  }

  SimplifiedVerilogParser::symbol_type
  SimplifiedVerilogParser::make_INPUT (const location_type& l)
  {
    return symbol_type (token::INPUT, l);
  }

  SimplifiedVerilogParser::symbol_type
  SimplifiedVerilogParser::make_OUTPUT (const location_type& l)
  {
    return symbol_type (token::OUTPUT, l);
  }

  SimplifiedVerilogParser::symbol_type
  SimplifiedVerilogParser::make_WIRE (const location_type& l)
  {
    return symbol_type (token::WIRE, l);
  }

  SimplifiedVerilogParser::symbol_type
  SimplifiedVerilogParser::make_IDENTIFIER (const std::string& v, const location_type& l)
  {
    return symbol_type (token::IDENTIFIER, v, l);
  }

  SimplifiedVerilogParser::symbol_type
  SimplifiedVerilogParser::make_CHAR (const location_type& l)
  {
    return symbol_type (token::CHAR, l);
  }



  // by_state.
  inline
  SimplifiedVerilogParser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  SimplifiedVerilogParser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  SimplifiedVerilogParser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  SimplifiedVerilogParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  SimplifiedVerilogParser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  SimplifiedVerilogParser::symbol_number_type
  SimplifiedVerilogParser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  SimplifiedVerilogParser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  SimplifiedVerilogParser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
      switch (that.type_get ())
    {
      case 8: // IDENTIFIER
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  SimplifiedVerilogParser::stack_symbol_type&
  SimplifiedVerilogParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
      switch (that.type_get ())
    {
      case 8: // IDENTIFIER
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  SimplifiedVerilogParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  SimplifiedVerilogParser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  SimplifiedVerilogParser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  SimplifiedVerilogParser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  SimplifiedVerilogParser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  SimplifiedVerilogParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  SimplifiedVerilogParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  SimplifiedVerilogParser::debug_level_type
  SimplifiedVerilogParser::debug_level () const
  {
    return yydebug_;
  }

  void
  SimplifiedVerilogParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline SimplifiedVerilogParser::state_type
  SimplifiedVerilogParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  SimplifiedVerilogParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  SimplifiedVerilogParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  SimplifiedVerilogParser::parse ()
  {
    // State.
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

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

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
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
        switch (yyr1_[yyn])
    {
      case 8: // IDENTIFIER
        yylhs.value.build< std::string > ();
        break;

      default:
        break;
    }


      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 3:
#line 62 "SimplifiedVerilog.yy" // lalr1.cc:859
    {reader.readModuleName(yystack_[0].value.as< std::string > ());}
#line 719 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
    break;

  case 7:
#line 70 "SimplifiedVerilog.yy" // lalr1.cc:859
    { reader.readIdentifier(yystack_[0].value.as< std::string > ()); }
#line 725 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
    break;

  case 8:
#line 71 "SimplifiedVerilog.yy" // lalr1.cc:859
    { reader.readIdentifier(yystack_[0].value.as< std::string > ()); }
#line 731 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
    break;

  case 16:
#line 91 "SimplifiedVerilog.yy" // lalr1.cc:859
    { reader.setCurrentIdentifierListType(IDENTIFIER_LIST_INPUT_PORT); }
#line 737 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
    break;

  case 18:
#line 92 "SimplifiedVerilog.yy" // lalr1.cc:859
    { reader.setCurrentIdentifierListType(IDENTIFIER_LIST_OUTPUT_PORTS); }
#line 743 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
    break;

  case 20:
#line 96 "SimplifiedVerilog.yy" // lalr1.cc:859
    { reader.setCurrentIdentifierListType(IDENTIFIER_LIST_NETS); }
#line 749 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
    break;

  case 22:
#line 99 "SimplifiedVerilog.yy" // lalr1.cc:859
    { reader.readInstance(yystack_[1].value.as< std::string > (), yystack_[0].value.as< std::string > ()); }
#line 755 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
    break;

  case 25:
#line 104 "SimplifiedVerilog.yy" // lalr1.cc:859
    { reader.error(ERROR_UNNAMED_PORT_MAPPING_NOT_SUPPORTED); }
#line 761 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
    break;

  case 30:
#line 118 "SimplifiedVerilog.yy" // lalr1.cc:859
    { reader.readConnection(yystack_[2].value.as< std::string > (), ""); }
#line 767 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
    break;

  case 31:
#line 119 "SimplifiedVerilog.yy" // lalr1.cc:859
    { reader.readConnection(yystack_[3].value.as< std::string > (), yystack_[1].value.as< std::string > ()); }
#line 773 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
    break;


#line 777 "SimplifiedVerilog.tab.cc" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
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

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
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
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
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
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

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
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  SimplifiedVerilogParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  SimplifiedVerilogParser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char SimplifiedVerilogParser::yypact_ninf_ = -27;

  const signed char SimplifiedVerilogParser::yytable_ninf_ = -1;

  const signed char
  SimplifiedVerilogParser::yypact_[] =
  {
      16,     3,    12,    22,   -27,   -27,   -27,    13,    17,    18,
     -27,    -7,     9,   -27,    19,   -27,   -27,   -27,    21,    26,
       9,   -27,   -27,   -27,   -27,   -27,    17,    17,    17,   -27,
     -27,   -27,    -3,     8,    10,    15,   -27,   -27,   -27,    -5,
      23,    20,    24,   -27,    25,   -27,    28,    27,    29,    -4,
     -27,   -27,    30,   -27,   -27
  };

  const unsigned char
  SimplifiedVerilogParser::yydefact_[] =
  {
       0,     0,     0,     0,     3,     1,     2,     5,     0,     0,
       7,     0,     9,     6,     0,    16,    18,    20,     0,     0,
      10,    11,    13,    14,    15,     8,     0,     0,     0,    22,
       4,    12,     0,     0,     0,     0,    17,    19,    21,    24,
       0,    27,     0,    25,    26,    28,     0,     0,     0,     0,
      23,    29,     0,    30,    31
  };

  const signed char
  SimplifiedVerilogParser::yypgoto_[] =
  {
     -27,   -27,   -27,   -27,   -27,   -26,   -27,   -27,    14,   -27,
     -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,   -16
  };

  const signed char
  SimplifiedVerilogParser::yydefgoto_[] =
  {
      -1,     2,     3,     7,     9,    11,    19,    20,    21,    22,
      26,    27,    23,    28,    24,    35,    42,    43,    44,    45
  };

  const unsigned char
  SimplifiedVerilogParser::yytable_[] =
  {
      32,    33,    34,    10,    52,    13,    14,    36,    53,    40,
      14,     4,     5,    41,    15,    16,    17,    18,    37,     1,
      38,    14,     6,    14,     8,    10,    39,    25,    12,    29,
      30,    46,    51,    14,    31,     0,    47,    50,    48,    49,
       0,     0,    54,    40
  };

  const signed char
  SimplifiedVerilogParser::yycheck_[] =
  {
      26,    27,    28,     8,     8,    12,    13,    10,    12,    14,
      13,     8,     0,    39,     5,     6,     7,     8,    10,     3,
      10,    13,     0,    13,    11,     8,    11,     8,    10,     8,
       4,     8,    48,    13,    20,    -1,    12,    10,    13,    11,
      -1,    -1,    12,    14
  };

  const unsigned char
  SimplifiedVerilogParser::yystos_[] =
  {
       0,     3,    16,    17,     8,     0,     0,    18,    11,    19,
       8,    20,    10,    12,    13,     5,     6,     7,     8,    21,
      22,    23,    24,    27,    29,     8,    25,    26,    28,     8,
       4,    23,    20,    20,    20,    30,    10,    10,    10,    11,
      14,    20,    31,    32,    33,    34,     8,    12,    13,    11,
      10,    34,     8,    12,    12
  };

  const unsigned char
  SimplifiedVerilogParser::yyr1_[] =
  {
       0,    15,    16,    18,    17,    19,    19,    20,    20,    21,
      21,    22,    22,    23,    23,    23,    25,    24,    26,    24,
      28,    27,    30,    29,    31,    31,    31,    32,    33,    33,
      34,    34
  };

  const unsigned char
  SimplifiedVerilogParser::yyr2_[] =
  {
       0,     2,     2,     0,     7,     0,     3,     1,     3,     0,
       1,     1,     2,     1,     1,     1,     0,     4,     0,     4,
       0,     4,     0,     7,     0,     1,     1,     1,     1,     3,
       4,     5
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const SimplifiedVerilogParser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "MODULE", "END_MODULE",
  "INPUT", "OUTPUT", "WIRE", "IDENTIFIER", "CHAR", "';'", "'('", "')'",
  "','", "'.'", "$accept", "start", "module_declaration", "$@1", "io",
  "identifier_list", "implementation", "declaration_list", "declaration",
  "port_declaration", "$@2", "$@3", "net_declaration", "$@4",
  "instance_declaration", "$@5", "port_mapping", "ordered_port_mapping",
  "named_port_mapping", "connection", YY_NULLPTR
  };


  const unsigned char
  SimplifiedVerilogParser::yyrline_[] =
  {
       0,    58,    58,    62,    62,    65,    67,    70,    71,    74,
      76,    80,    81,    85,    86,    87,    91,    91,    92,    92,
      96,    96,    99,    99,   102,   104,   105,   109,   113,   114,
     118,   119
  };

  // Print the state stack on the debug stream.
  void
  SimplifiedVerilogParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  SimplifiedVerilogParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  SimplifiedVerilogParser::token_number_type
  SimplifiedVerilogParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      11,    12,     2,     2,    13,     2,    14,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    10,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9
    };
    const unsigned int user_token_number_max_ = 264;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

#line 4 "SimplifiedVerilog.yy" // lalr1.cc:1167
} // Parsing
#line 1137 "SimplifiedVerilog.tab.cc" // lalr1.cc:1167
#line 122 "SimplifiedVerilog.yy" // lalr1.cc:1168


void 
Parsing::SimplifiedVerilogParser::error(const Parsing::SimplifiedVerilogParser::location_type &l, const std::string &err_message) {
   std::cout << "Parsing Error: " << err_message << " at " << l << "\n";
}
