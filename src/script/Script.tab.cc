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

#line 37 "Script.tab.cc"  // lalr1.cc:404

#ifndef YY_NULLPTR
#if defined __cplusplus && 201103L <= __cplusplus
#define YY_NULLPTR nullptr
#else
#define YY_NULLPTR 0
#endif
#endif

#include "Script.tab.hh"

// User implementation prologue.

#line 51 "Script.tab.cc"  // lalr1.cc:412
// Unqualified %code blocks.
#line 31 "Script.yy"  // lalr1.cc:413

#include <iostream>
#include <cstdlib>
#include <fstream>

#include "ScriptReader.h"
#include "ScriptScanner.h"

#undef yylex
#define yylex scanner.yylex

#line 65 "Script.tab.cc"  // lalr1.cc:413

#ifndef YY_
#if defined YYENABLE_NLS && YYENABLE_NLS
#if ENABLE_NLS
#include <libintl.h>  // FIXME: INFRINGES ON USER NAME SPACE.
#define YY_(msgid) dgettext("bison-runtime", msgid)
#endif
#endif
#ifndef YY_
#define YY_(msgid) msgid
#endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(Current, Rhs, N)                           \
        do                                                        \
                if (N) {                                          \
                        (Current).begin = YYRHSLOC(Rhs, 1).begin; \
                        (Current).end = YYRHSLOC(Rhs, N).end;     \
                } else {                                          \
                        (Current).begin = (Current).end =         \
                            YYRHSLOC(Rhs, 0).end;                 \
                }                                                 \
        while (/*CONSTCOND*/ false)
#endif

// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void)(E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
#define YYCDEBUG \
        if (yydebug_) (*yycdebug_)

#define YY_SYMBOL_PRINT(Title, Symbol)                 \
        do {                                           \
                if (yydebug_) {                        \
                        *yycdebug_ << Title << ' ';    \
                        yy_print_(*yycdebug_, Symbol); \
                        *yycdebug_ << std::endl;       \
                }                                      \
        } while (false)

#define YY_REDUCE_PRINT(Rule)                         \
        do {                                          \
                if (yydebug_) yy_reduce_print_(Rule); \
        } while (false)

#define YY_STACK_PRINT()                        \
        do {                                    \
                if (yydebug_) yystack_print_(); \
        } while (false)

#else  // !YYDEBUG

#define YYCDEBUG \
        if (false) std::cerr
#define YY_SYMBOL_PRINT(Title, Symbol) YYUSE(Symbol)
#define YY_REDUCE_PRINT(Rule) static_cast<void>(0)
#define YY_STACK_PRINT() static_cast<void>(0)

#endif  // !YYDEBUG

#define yyerrok (yyerrstatus_ = 0)
#define yyclearin (yyla.clear())

#define YYACCEPT goto yyacceptlab
#define YYABORT goto yyabortlab
#define YYERROR goto yyerrorlab
#define YYRECOVERING() (!!yyerrstatus_)

#line 4 "Script.yy"  // lalr1.cc:479
namespace ScriptParsing {
#line 151 "Script.tab.cc"  // lalr1.cc:479

/// Build a parser object.
ScriptParser::ScriptParser(ScriptScanner& scanner_yyarg,
                           ScriptReader& reader_yyarg)
    :
#if YYDEBUG
      yydebug_(false),
      yycdebug_(&std::cerr),
#endif
      scanner(scanner_yyarg),
      reader(reader_yyarg) {
}

ScriptParser::~ScriptParser() {}

/*---------------.
| Symbol types.  |
`---------------*/

inline ScriptParser::syntax_error::syntax_error(const location_type& l,
                                                const std::string& m)
    : std::runtime_error(m), location(l) {}

// basic_symbol.
template <typename Base>
inline ScriptParser::basic_symbol<Base>::basic_symbol() : value() {}

template <typename Base>
inline ScriptParser::basic_symbol<Base>::basic_symbol(const basic_symbol& other)
    : Base(other), value(), location(other.location) {
        switch (other.type_get()) {
                case 25:  // param_value
                        value.copy<ParsedParamValue>(other.value);
                        break;

                case 7:  // BOOLEAN
                        value.copy<bool>(other.value);
                        break;

                case 9:  // FLOAT
                        value.copy<float>(other.value);
                        break;

                case 8:  // INTEGER
                        value.copy<int>(other.value);
                        break;

                case 3:   // STRING
                case 4:   // MALFORMED_STRING
                case 5:   // IDENTIFIER
                case 6:   // PARAM_NAME
                case 26:  // json
                case 27:  // json_element_list
                case 28:  // json_element
                case 29:  // json_value
                case 30:  // json_list
                case 31:  // json_list_elements
                        value.copy<std::string>(other.value);
                        break;

                default:
                        break;
        }
}

template <typename Base>
inline ScriptParser::basic_symbol<Base>::basic_symbol(
    typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base(t), value(), location(l) {
        (void)v;
        switch (this->type_get()) {
                case 25:  // param_value
                        value.copy<ParsedParamValue>(v);
                        break;

                case 7:  // BOOLEAN
                        value.copy<bool>(v);
                        break;

                case 9:  // FLOAT
                        value.copy<float>(v);
                        break;

                case 8:  // INTEGER
                        value.copy<int>(v);
                        break;

                case 3:   // STRING
                case 4:   // MALFORMED_STRING
                case 5:   // IDENTIFIER
                case 6:   // PARAM_NAME
                case 26:  // json
                case 27:  // json_element_list
                case 28:  // json_element
                case 29:  // json_value
                case 30:  // json_list
                case 31:  // json_list_elements
                        value.copy<std::string>(v);
                        break;

                default:
                        break;
        }
}

// Implementation of basic_symbol constructor for each type.

template <typename Base>
ScriptParser::basic_symbol<Base>::basic_symbol(typename Base::kind_type t,
                                               const location_type& l)
    : Base(t), value(), location(l) {}

template <typename Base>
ScriptParser::basic_symbol<Base>::basic_symbol(typename Base::kind_type t,
                                               const ParsedParamValue v,
                                               const location_type& l)
    : Base(t), value(v), location(l) {}

template <typename Base>
ScriptParser::basic_symbol<Base>::basic_symbol(typename Base::kind_type t,
                                               const bool v,
                                               const location_type& l)
    : Base(t), value(v), location(l) {}

template <typename Base>
ScriptParser::basic_symbol<Base>::basic_symbol(typename Base::kind_type t,
                                               const float v,
                                               const location_type& l)
    : Base(t), value(v), location(l) {}

template <typename Base>
ScriptParser::basic_symbol<Base>::basic_symbol(typename Base::kind_type t,
                                               const int v,
                                               const location_type& l)
    : Base(t), value(v), location(l) {}

template <typename Base>
ScriptParser::basic_symbol<Base>::basic_symbol(typename Base::kind_type t,
                                               const std::string v,
                                               const location_type& l)
    : Base(t), value(v), location(l) {}

template <typename Base>
inline ScriptParser::basic_symbol<Base>::~basic_symbol() {
        clear();
}

template <typename Base>
inline void ScriptParser::basic_symbol<Base>::clear() {
        // User destructor.
        symbol_number_type yytype = this->type_get();
        basic_symbol<Base>& yysym = *this;
        (void)yysym;
        switch (yytype) {
                default:
                        break;
        }

        // Type destructor.
        switch (yytype) {
                case 25:  // param_value
                        value.template destroy<ParsedParamValue>();
                        break;

                case 7:  // BOOLEAN
                        value.template destroy<bool>();
                        break;

                case 9:  // FLOAT
                        value.template destroy<float>();
                        break;

                case 8:  // INTEGER
                        value.template destroy<int>();
                        break;

                case 3:   // STRING
                case 4:   // MALFORMED_STRING
                case 5:   // IDENTIFIER
                case 6:   // PARAM_NAME
                case 26:  // json
                case 27:  // json_element_list
                case 28:  // json_element
                case 29:  // json_value
                case 30:  // json_list
                case 31:  // json_list_elements
                        value.template destroy<std::string>();
                        break;

                default:
                        break;
        }

        Base::clear();
}

template <typename Base>
inline bool ScriptParser::basic_symbol<Base>::empty() const {
        return Base::type_get() == empty_symbol;
}

template <typename Base>
inline void ScriptParser::basic_symbol<Base>::move(basic_symbol& s) {
        super_type::move(s);
        switch (this->type_get()) {
                case 25:  // param_value
                        value.move<ParsedParamValue>(s.value);
                        break;

                case 7:  // BOOLEAN
                        value.move<bool>(s.value);
                        break;

                case 9:  // FLOAT
                        value.move<float>(s.value);
                        break;

                case 8:  // INTEGER
                        value.move<int>(s.value);
                        break;

                case 3:   // STRING
                case 4:   // MALFORMED_STRING
                case 5:   // IDENTIFIER
                case 6:   // PARAM_NAME
                case 26:  // json
                case 27:  // json_element_list
                case 28:  // json_element
                case 29:  // json_value
                case 30:  // json_list
                case 31:  // json_list_elements
                        value.move<std::string>(s.value);
                        break;

                default:
                        break;
        }

        location = s.location;
}

// by_type.
inline ScriptParser::by_type::by_type() : type(empty_symbol) {}

inline ScriptParser::by_type::by_type(const by_type& other)
    : type(other.type) {}

inline ScriptParser::by_type::by_type(token_type t) : type(yytranslate_(t)) {}

inline void ScriptParser::by_type::clear() { type = empty_symbol; }

inline void ScriptParser::by_type::move(by_type& that) {
        type = that.type;
        that.clear();
}

inline int ScriptParser::by_type::type_get() const { return type; }
// Implementation of make_symbol for each symbol type.
ScriptParser::symbol_type ScriptParser::make_END(const location_type& l) {
        return symbol_type(token::END, l);
}

ScriptParser::symbol_type ScriptParser::make_STRING(const std::string& v,
                                                    const location_type& l) {
        return symbol_type(token::STRING, v, l);
}

ScriptParser::symbol_type ScriptParser::make_MALFORMED_STRING(
    const std::string& v, const location_type& l) {
        return symbol_type(token::MALFORMED_STRING, v, l);
}

ScriptParser::symbol_type ScriptParser::make_IDENTIFIER(
    const std::string& v, const location_type& l) {
        return symbol_type(token::IDENTIFIER, v, l);
}

ScriptParser::symbol_type ScriptParser::make_PARAM_NAME(
    const std::string& v, const location_type& l) {
        return symbol_type(token::PARAM_NAME, v, l);
}

ScriptParser::symbol_type ScriptParser::make_BOOLEAN(const bool& v,
                                                     const location_type& l) {
        return symbol_type(token::BOOLEAN, v, l);
}

ScriptParser::symbol_type ScriptParser::make_INTEGER(const int& v,
                                                     const location_type& l) {
        return symbol_type(token::INTEGER, v, l);
}

ScriptParser::symbol_type ScriptParser::make_FLOAT(const float& v,
                                                   const location_type& l) {
        return symbol_type(token::FLOAT, v, l);
}

ScriptParser::symbol_type ScriptParser::make_NIL(const location_type& l) {
        return symbol_type(token::NIL, l);
}

// by_state.
inline ScriptParser::by_state::by_state() : state(empty_state) {}

inline ScriptParser::by_state::by_state(const by_state& other)
    : state(other.state) {}

inline void ScriptParser::by_state::clear() { state = empty_state; }

inline void ScriptParser::by_state::move(by_state& that) {
        state = that.state;
        that.clear();
}

inline ScriptParser::by_state::by_state(state_type s) : state(s) {}

inline ScriptParser::symbol_number_type ScriptParser::by_state::type_get()
    const {
        if (state == empty_state)
                return empty_symbol;
        else
                return yystos_[state];
}

inline ScriptParser::stack_symbol_type::stack_symbol_type() {}

inline ScriptParser::stack_symbol_type::stack_symbol_type(state_type s,
                                                          symbol_type& that)
    : super_type(s, that.location) {
        switch (that.type_get()) {
                case 25:  // param_value
                        value.move<ParsedParamValue>(that.value);
                        break;

                case 7:  // BOOLEAN
                        value.move<bool>(that.value);
                        break;

                case 9:  // FLOAT
                        value.move<float>(that.value);
                        break;

                case 8:  // INTEGER
                        value.move<int>(that.value);
                        break;

                case 3:   // STRING
                case 4:   // MALFORMED_STRING
                case 5:   // IDENTIFIER
                case 6:   // PARAM_NAME
                case 26:  // json
                case 27:  // json_element_list
                case 28:  // json_element
                case 29:  // json_value
                case 30:  // json_list
                case 31:  // json_list_elements
                        value.move<std::string>(that.value);
                        break;

                default:
                        break;
        }

        // that is emptied.
        that.type = empty_symbol;
}

inline ScriptParser::stack_symbol_type& ScriptParser::stack_symbol_type::
operator=(const stack_symbol_type& that) {
        state = that.state;
        switch (that.type_get()) {
                case 25:  // param_value
                        value.copy<ParsedParamValue>(that.value);
                        break;

                case 7:  // BOOLEAN
                        value.copy<bool>(that.value);
                        break;

                case 9:  // FLOAT
                        value.copy<float>(that.value);
                        break;

                case 8:  // INTEGER
                        value.copy<int>(that.value);
                        break;

                case 3:   // STRING
                case 4:   // MALFORMED_STRING
                case 5:   // IDENTIFIER
                case 6:   // PARAM_NAME
                case 26:  // json
                case 27:  // json_element_list
                case 28:  // json_element
                case 29:  // json_value
                case 30:  // json_list
                case 31:  // json_list_elements
                        value.copy<std::string>(that.value);
                        break;

                default:
                        break;
        }

        location = that.location;
        return *this;
}

template <typename Base>
inline void ScriptParser::yy_destroy_(const char* yymsg,
                                      basic_symbol<Base>& yysym) const {
        if (yymsg) YY_SYMBOL_PRINT(yymsg, yysym);
}

#if YYDEBUG
template <typename Base>
void ScriptParser::yy_print_(std::ostream& yyo,
                             const basic_symbol<Base>& yysym) const {
        std::ostream& yyoutput = yyo;
        YYUSE(yyoutput);
        symbol_number_type yytype = yysym.type_get();
        // Avoid a (spurious) G++ 4.8 warning about "array subscript is
        // below array bounds".
        if (yysym.empty()) std::abort();
        yyo << (yytype < yyntokens_ ? "token" : "nterm") << ' '
            << yytname_[yytype] << " (" << yysym.location << ": ";
        YYUSE(yytype);
        yyo << ')';
}
#endif

inline void ScriptParser::yypush_(const char* m, state_type s,
                                  symbol_type& sym) {
        stack_symbol_type t(s, sym);
        yypush_(m, t);
}

inline void ScriptParser::yypush_(const char* m, stack_symbol_type& s) {
        if (m) YY_SYMBOL_PRINT(m, s);
        yystack_.push(s);
}

inline void ScriptParser::yypop_(unsigned int n) { yystack_.pop(n); }

#if YYDEBUG
std::ostream& ScriptParser::debug_stream() const { return *yycdebug_; }

void ScriptParser::set_debug_stream(std::ostream& o) { yycdebug_ = &o; }

ScriptParser::debug_level_type ScriptParser::debug_level() const {
        return yydebug_;
}

void ScriptParser::set_debug_level(debug_level_type l) { yydebug_ = l; }
#endif  // YYDEBUG

inline ScriptParser::state_type ScriptParser::yy_lr_goto_state_(
    state_type yystate, int yysym) {
        int yyr = yypgoto_[yysym - yyntokens_] + yystate;
        if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
                return yytable_[yyr];
        else
                return yydefgoto_[yysym - yyntokens_];
}

inline bool ScriptParser::yy_pact_value_is_default_(int yyvalue) {
        return yyvalue == yypact_ninf_;
}

inline bool ScriptParser::yy_table_value_is_error_(int yyvalue) {
        return yyvalue == yytable_ninf_;
}

int ScriptParser::parse() {
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
        try {
                YYCDEBUG << "Starting parse" << std::endl;

                /* Initialize the stack.  The initial state will be set in
                   yynewstate, since the latter expects the semantical and the
                   location values to have been already stored, initialize these
                   stacks with a primary value.  */
                yystack_.clear();
                yypush_(YY_NULLPTR, 0, yyla);

        // A new symbol was pushed on the stack.
        yynewstate:
                YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

                // Accept?
                if (yystack_[0].state == yyfinal_) goto yyacceptlab;

                goto yybackup;

        // Backup.
        yybackup:

                // Try to take a decision without lookahead.
                yyn = yypact_[yystack_[0].state];
                if (yy_pact_value_is_default_(yyn)) goto yydefault;

                // Read a lookahead token.
                if (yyla.empty()) {
                        YYCDEBUG << "Reading a token: ";
                        try {
                                yyla.type = yytranslate_(
                                    yylex(&yyla.value, &yyla.location));
                        } catch (const syntax_error& yyexc) {
                                error(yyexc);
                                goto yyerrlab1;
                        }
                }
                YY_SYMBOL_PRINT("Next token is", yyla);

                /* If the proper action on seeing token YYLA.TYPE is to reduce
                   or
                   to detect an error, take that action.  */
                yyn += yyla.type_get();
                if (yyn < 0 || yylast_ < yyn ||
                    yycheck_[yyn] != yyla.type_get())
                        goto yydefault;

                // Reduce or error.
                yyn = yytable_[yyn];
                if (yyn <= 0) {
                        if (yy_table_value_is_error_(yyn)) goto yyerrlab;
                        yyn = -yyn;
                        goto yyreduce;
                }

                // Count tokens shifted since error; after three, turn off error
                // status.
                if (yyerrstatus_) --yyerrstatus_;

                // Shift the lookahead token.
                yypush_("Shifting", yyn, yyla);
                goto yynewstate;

        /*-----------------------------------------------------------.
        | yydefault -- do the default action for the current state.  |
        `-----------------------------------------------------------*/
        yydefault:
                yyn = yydefact_[yystack_[0].state];
                if (yyn == 0) goto yyerrlab;
                goto yyreduce;

        /*-----------------------------.
        | yyreduce -- Do a reduction.  |
        `-----------------------------*/
        yyreduce:
                yylen = yyr2_[yyn];
                {
                        stack_symbol_type yylhs;
                        yylhs.state = yy_lr_goto_state_(yystack_[yylen].state,
                                                        yyr1_[yyn]);
                        /* Variants are always initialized to an empty instance
                           of the
                           correct type. The default '$$ = $1' action is NOT
                           applied
                           when using variants.  */
                        switch (yyr1_[yyn]) {
                                case 25:  // param_value
                                        yylhs.value.build<ParsedParamValue>();
                                        break;

                                case 7:  // BOOLEAN
                                        yylhs.value.build<bool>();
                                        break;

                                case 9:  // FLOAT
                                        yylhs.value.build<float>();
                                        break;

                                case 8:  // INTEGER
                                        yylhs.value.build<int>();
                                        break;

                                case 3:   // STRING
                                case 4:   // MALFORMED_STRING
                                case 5:   // IDENTIFIER
                                case 6:   // PARAM_NAME
                                case 26:  // json
                                case 27:  // json_element_list
                                case 28:  // json_element
                                case 29:  // json_value
                                case 30:  // json_list
                                case 31:  // json_list_elements
                                        yylhs.value.build<std::string>();
                                        break;

                                default:
                                        break;
                        }

                        // Compute the default @$.
                        {
                                slice<stack_symbol_type, stack_type> slice(
                                    yystack_, yylen);
                                YYLLOC_DEFAULT(yylhs.location, slice, yylen);
                        }

                        // Perform the reduction.
                        YY_REDUCE_PRINT(yyn);
                        try {
                                switch (yyn) {
                                        case 5:
#line 83 "Script.yy"  // lalr1.cc:859
                                        {
                                                reader.readCommandName(
                                                    yystack_[0]
                                                        .value
                                                        .as<std::string>());
                                        }
#line 928 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 8:
#line 88 "Script.yy"  // lalr1.cc:859
                                        {
                                                reader.readPositionalParam(
                                                    ParsedParamValue(
                                                        yystack_[0]
                                                            .value
                                                            .as<ParsedParamValue>()));
                                        }
#line 934 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 11:
#line 97 "Script.yy"  // lalr1.cc:859
                                        {
                                                reader.readNamedParam(
                                                    yystack_[0]
                                                        .value
                                                        .as<std::string>(),
                                                    ParsedParamValue());
                                        }
#line 940 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 12:
#line 98 "Script.yy"  // lalr1.cc:859
                                        {
                                                reader.readNamedParam(
                                                    yystack_[1]
                                                        .value
                                                        .as<std::string>(),
                                                    yystack_[0]
                                                        .value
                                                        .as<ParsedParamValue>());
                                        }
#line 946 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 13:
#line 102 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value
                                                    .as<ParsedParamValue>() =
                                                    ParsedParamValue(
                                                        yystack_[0]
                                                            .value.as<int>());
                                        }
#line 952 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 14:
#line 103 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value
                                                    .as<ParsedParamValue>() =
                                                    ParsedParamValue(
                                                        yystack_[0]
                                                            .value.as<float>());
                                        }
#line 958 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 15:
#line 104 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value
                                                    .as<ParsedParamValue>() =
                                                    ParsedParamValue(
                                                        yystack_[0]
                                                            .value
                                                            .as<std::string>());
                                        }
#line 964 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 16:
#line 105 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value
                                                    .as<ParsedParamValue>() =
                                                    ParsedParamValue(
                                                        yystack_[0]
                                                            .value.as<bool>());
                                        }
#line 970 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 17:
#line 106 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value
                                                    .as<ParsedParamValue>() =
                                                    ParsedParamValue();
                                        }
#line 976 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 18:
#line 107 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value
                                                    .as<ParsedParamValue>() =
                                                    ParsedParamValue(Json::parse(
                                                        yystack_[0]
                                                            .value
                                                            .as<std::
                                                                    string>()));
                                        }
#line 982 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 19:
#line 111 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    '{' +
                                                    yystack_[1]
                                                        .value
                                                        .as<std::string>() +
                                                    '}';
                                        }
#line 988 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 20:
#line 115 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    "";
                                        }
#line 994 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 21:
#line 116 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    yystack_[0]
                                                        .value
                                                        .as<std::string>();
                                        }
#line 1000 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 22:
#line 117 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    yystack_[2]
                                                        .value
                                                        .as<std::string>() +
                                                    ',' +
                                                    yystack_[0]
                                                        .value
                                                        .as<std::string>();
                                        }
#line 1006 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 23:
#line 122 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    '"' +
                                                    yystack_[2]
                                                        .value
                                                        .as<std::string>() +
                                                    '"' + ':' +
                                                    yystack_[0]
                                                        .value
                                                        .as<std::string>();
                                        }
#line 1012 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 24:
#line 126 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    std::to_string(
                                                        yystack_[0]
                                                            .value.as<int>());
                                        }
#line 1018 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 25:
#line 127 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    std::to_string(
                                                        yystack_[0]
                                                            .value.as<float>());
                                        }
#line 1024 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 26:
#line 128 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    '"' +
                                                    yystack_[0]
                                                        .value
                                                        .as<std::string>() +
                                                    '"';
                                        }
#line 1030 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 27:
#line 129 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    yystack_[0].value.as<bool>()
                                                        ? "true"
                                                        : "false";
                                        }
#line 1036 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 28:
#line 130 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    "null";
                                        }
#line 1042 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 29:
#line 131 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    yystack_[0]
                                                        .value
                                                        .as<std::string>();
                                        }
#line 1048 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 30:
#line 132 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    yystack_[0]
                                                        .value
                                                        .as<std::string>();
                                        }
#line 1054 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 31:
#line 136 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    "[]";
                                        }
#line 1060 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 32:
#line 137 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    '[' +
                                                    yystack_[1]
                                                        .value
                                                        .as<std::string>() +
                                                    ']';
                                        }
#line 1066 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 33:
#line 141 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    yystack_[0]
                                                        .value
                                                        .as<std::string>();
                                        }
#line 1072 "Script.tab.cc"  // lalr1.cc:859
                                        break;

                                        case 34:
#line 142 "Script.yy"  // lalr1.cc:859
                                        {
                                                yylhs.value.as<std::string>() =
                                                    yystack_[2]
                                                        .value
                                                        .as<std::string>() +
                                                    ',' +
                                                    yystack_[0]
                                                        .value
                                                        .as<std::string>();
                                        }
#line 1078 "Script.tab.cc"  // lalr1.cc:859
                                        break;

#line 1082 "Script.tab.cc"  // lalr1.cc:859
                                        default:
                                                break;
                                }
                        } catch (const syntax_error& yyexc) {
                                error(yyexc);
                                YYERROR;
                        }
                        YY_SYMBOL_PRINT("-> $$ =", yylhs);
                        yypop_(yylen);
                        yylen = 0;
                        YY_STACK_PRINT();

                        // Shift the result of the reduction.
                        yypush_(YY_NULLPTR, yylhs);
                }
                goto yynewstate;

        /*--------------------------------------.
        | yyerrlab -- here on detecting error.  |
        `--------------------------------------*/
        yyerrlab:
                // If not already recovering from an error, report this error.
                if (!yyerrstatus_) {
                        ++yynerrs_;
                        error(yyla.location,
                              yysyntax_error_(yystack_[0].state, yyla));
                }

                yyerror_range[1].location = yyla.location;
                if (yyerrstatus_ == 3) {
                        /* If just tried and failed to reuse lookahead token
                           after an
                           error, discard it.  */

                        // Return failure if at end of input.
                        if (yyla.type_get() == yyeof_)
                                YYABORT;
                        else if (!yyla.empty()) {
                                yy_destroy_("Error: discarding", yyla);
                                yyla.clear();
                        }
                }

                // Else will try to reuse lookahead token after shifting the
                // error token.
                goto yyerrlab1;

        /*---------------------------------------------------.
        | yyerrorlab -- error raised explicitly by YYERROR.  |
        `---------------------------------------------------*/
        yyerrorlab:

                /* Pacify compilers like GCC when the user code never invokes
                   YYERROR and the label yyerrorlab therefore never appears in
                   user
                   code.  */
                if (false) goto yyerrorlab;
                yyerror_range[1].location = yystack_[yylen - 1].location;
                /* Do not reclaim the symbols of the rule whose action triggered
                   this YYERROR.  */
                yypop_(yylen);
                yylen = 0;
                goto yyerrlab1;

        /*-------------------------------------------------------------.
        | yyerrlab1 -- common code for both syntax error and YYERROR.  |
        `-------------------------------------------------------------*/
        yyerrlab1:
                yyerrstatus_ = 3;  // Each real token shifted decrements this.
                {
                        stack_symbol_type error_token;
                        for (;;) {
                                yyn = yypact_[yystack_[0].state];
                                if (!yy_pact_value_is_default_(yyn)) {
                                        yyn += yyterror_;
                                        if (0 <= yyn && yyn <= yylast_ &&
                                            yycheck_[yyn] == yyterror_) {
                                                yyn = yytable_[yyn];
                                                if (0 < yyn) break;
                                        }
                                }

                                // Pop the current state because it cannot
                                // handle the error token.
                                if (yystack_.size() == 1) YYABORT;

                                yyerror_range[1].location =
                                    yystack_[0].location;
                                yy_destroy_("Error: popping", yystack_[0]);
                                yypop_();
                                YY_STACK_PRINT();
                        }

                        yyerror_range[2].location = yyla.location;
                        YYLLOC_DEFAULT(error_token.location, yyerror_range, 2);

                        // Shift the error token.
                        error_token.state = yyn;
                        yypush_("Shifting", error_token);
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
                if (!yyla.empty())
                        yy_destroy_("Cleanup: discarding lookahead", yyla);

                /* Do not reclaim the symbols of the rule whose action triggered
                   this YYABORT or YYACCEPT.  */
                yypop_(yylen);
                while (1 < yystack_.size()) {
                        yy_destroy_("Cleanup: popping", yystack_[0]);
                        yypop_();
                }

                return yyresult;
        } catch (...) {
                YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                         << std::endl;
                // Do not try to display the values of the reclaimed symbols,
                // as their printer might throw an exception.
                if (!yyla.empty()) yy_destroy_(YY_NULLPTR, yyla);

                while (1 < yystack_.size()) {
                        yy_destroy_(YY_NULLPTR, yystack_[0]);
                        yypop_();
                }
                throw;
        }
}

void ScriptParser::error(const syntax_error& yyexc) {
        error(yyexc.location, yyexc.what());
}

// Generate an error message.
std::string ScriptParser::yysyntax_error_(state_type,
                                          const symbol_type&) const {
        return YY_("syntax error");
}

const signed char ScriptParser::yypact_ninf_ = -32;

const signed char ScriptParser::yytable_ninf_ = -1;

const signed char ScriptParser::yypact_[] = {
    -2,  -32, 1,   -32, -32, -32, -2,  19,  -32, -32, 27, -32, -32, -32, -32,
    5,   -32, 13,  -32, -32, -32, 6,   31,  -32, -32, 8,  -32, 5,   -32, -32,
    -32, -32, -32, -3,  -32, -32, -32, -32, -32, -32, 24, 8,   -32, -32};

const unsigned char ScriptParser::yydefact_[] = {
    4,  5,  0,  2, 7,  1,  4,  9,  3,  15, 11, 16, 13, 14, 17,
    20, 6,  9,  8, 18, 12, 0,  0,  21, 10, 0,  19, 0,  26, 27,
    24, 25, 28, 0, 30, 23, 29, 22, 31, 33, 0,  0,  32, 34};

const signed char ScriptParser::yypgoto_[] = {
    -32, -32, 17, -32, -32, 15, -32, 23, 33, -32, 20, -31, -32, -32};

const signed char ScriptParser::yydefgoto_[] = {-1, 2,  3,  4,  7,  16, 17,
                                                18, 34, 22, 23, 35, 36, 40};

const unsigned char ScriptParser::yytable_[] = {
    28, 5,  39, 1,  29, 30, 31, 32, 21, 15, 43, 28, 6,  33, 38, 29,
    30, 31, 32, 10, 15, 25, 9,  8,  33, 10, 11, 12, 13, 14, 9,  15,
    24, 20, 11, 12, 13, 14, 41, 15, 19, 42, 0,  19, 26, 27, 0,  37};

const signed char ScriptParser::yycheck_[] = {
    3,  0,  33, 5, 7,  8,  9,  10, 3,  12, 41, 3,  11, 16, 17, 7,
    8,  9,  10, 6, 12, 15, 3,  6,  16, 6,  7,  8,  9,  10, 3,  12,
    17, 10, 7,  8, 9,  10, 14, 12, 7,  17, -1, 10, 13, 14, -1, 27};

const unsigned char ScriptParser::yystos_[] = {
    0,  5,  19, 20, 21, 0,  11, 22, 20, 3,  6,  7,  8,  9, 10,
    12, 23, 24, 25, 26, 25, 3,  27, 28, 23, 15, 13, 14, 3, 7,
    8,  9,  10, 16, 26, 29, 30, 28, 17, 29, 31, 14, 17, 29};

const unsigned char ScriptParser::yyr1_[] = {
    0,  18, 19, 19, 20, 21, 20, 22, 22, 23, 23, 24, 24, 25, 25, 25, 25, 25,
    25, 26, 27, 27, 27, 28, 29, 29, 29, 29, 29, 29, 29, 30, 30, 31, 31};

const unsigned char ScriptParser::yyr2_[] = {0, 2, 1, 3, 0, 0, 4, 0, 2, 0, 2, 1,
                                             2, 1, 1, 1, 1, 1, 1, 3, 0, 1, 3, 3,
                                             1, 1, 1, 1, 1, 1, 1, 2, 3, 1, 3};

#if YYDEBUG
// YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
// First, the terminals, then, starting at \a yyntokens_, nonterminals.
const char* const ScriptParser::yytname_[] = {"\"end of file\"",
                                              "error",
                                              "$undefined",
                                              "STRING",
                                              "MALFORMED_STRING",
                                              "IDENTIFIER",
                                              "PARAM_NAME",
                                              "BOOLEAN",
                                              "INTEGER",
                                              "FLOAT",
                                              "NIL",
                                              "';'",
                                              "'{'",
                                              "'}'",
                                              "','",
                                              "':'",
                                              "'['",
                                              "']'",
                                              "$accept",
                                              "command_list",
                                              "command",
                                              "$@1",
                                              "pos_param_list",
                                              "named_param_list",
                                              "named_param",
                                              "param_value",
                                              "json",
                                              "json_element_list",
                                              "json_element",
                                              "json_value",
                                              "json_list",
                                              "json_list_elements",
                                              YY_NULLPTR};

const unsigned char ScriptParser::yyrline_[] = {
    0,   77,  77,  78,  81,  83,  83,  86,  88,  91,  93,  97,
    98,  102, 103, 104, 105, 106, 107, 111, 115, 116, 117, 122,
    126, 127, 128, 129, 130, 131, 132, 136, 137, 141, 142};

// Print the state stack on the debug stream.
void ScriptParser::yystack_print_() {
        *yycdebug_ << "Stack now";
        for (stack_type::const_iterator i = yystack_.begin(),
                                        i_end = yystack_.end();
             i != i_end; ++i)
                *yycdebug_ << ' ' << i->state;
        *yycdebug_ << std::endl;
}

// Report on the debug stream that the rule \a yyrule is going to be reduced.
void ScriptParser::yy_reduce_print_(int yyrule) {
        unsigned int yylno = yyrline_[yyrule];
        int yynrhs = yyr2_[yyrule];
        // Print the symbols being reduced, and their result.
        *yycdebug_ << "Reducing stack by rule " << yyrule - 1 << " (line "
                   << yylno << "):" << std::endl;
        // The symbols being reduced.
        for (int yyi = 0; yyi < yynrhs; yyi++)
                YY_SYMBOL_PRINT("   $" << yyi + 1 << " =",
                                yystack_[(yynrhs) - (yyi + 1)]);
}
#endif  // YYDEBUG

// Symbol number corresponding to token number t.
inline ScriptParser::token_number_type ScriptParser::yytranslate_(int t) {
        static const token_number_type translate_table[] = {
            0, 2,  2,  2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 14, 2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 15, 11, 2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 16, 2, 17, 2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 12, 2, 13, 2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 2,  2, 2,  2, 2, 2, 2,  2, 2,  2,
            2, 2,  2,  2, 2, 2, 2,  2, 2, 1,  2, 3,  4, 5, 6, 7,  8, 9,  10};
        const unsigned int user_token_number_max_ = 265;
        const token_number_type undef_token_ = 2;

        if (static_cast<int>(t) <= yyeof_)
                return yyeof_;
        else if (static_cast<unsigned int>(t) <= user_token_number_max_)
                return translate_table[t];
        else
                return undef_token_;
}

#line 4 "Script.yy"  // lalr1.cc:1167
}  // ScriptParsing
#line 1438 "Script.tab.cc"  // lalr1.cc:1167
#line 145 "Script.yy"       // lalr1.cc:1168

void ScriptParsing::ScriptParser::error(
    const ScriptParsing::ScriptParser::location_type& l,
    const std::string& err_message) {
        std::cout << "ScriptParsing Error: " << err_message << " at " << l
                  << "\n";
}
