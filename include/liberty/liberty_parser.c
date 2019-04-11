/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

// Guilherme Flach 2016-04-04
// Added to ignore some warning messages in this file.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#pragma GCC diagnostic ignored "-Wimplicit-int"

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the variable and function names.  */
#define yyparse liberty_parser_parse
#define yylex liberty_parser_lex
#define yyerror liberty_parser_error
#define yydebug liberty_parser_debug
#define yynerrs liberty_parser_nerrs

#define yylval liberty_parser_lval
#define yychar liberty_parser_char

/* Copy the first part of user declarations.  */
#line 3 "liberty_parser.y" /* yacc.c:339  */

/******************************************************************************
  Copyright (c) 1996-2005 Synopsys, Inc.    ALL RIGHTS RESERVED

  The contents of this file are subject to the restrictions and limitations
  set forth in the SYNOPSYS Open Source License Version 1.0  (the "License");
  you may not use this file except in compliance with such restrictions
  and limitations. You may obtain instructions on how to receive a copy of
  the License at

http://www.synopsys.com/partners/tapin/tapinprogram.html.

Software distributed by Original Contributor under the License is
distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
expressed or implied. See the License for the specific language governing
rights and limitations under the License.

 ******************************************************************************/
#include <stdio.h>
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include "liberty_structs.h"
#include "si2dr_liberty.h"
#include "group_enum.h"
#include "attr_enum.h"
#include "libhash.h"
#include "libstrtab.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include "mymalloc.h" /* meant to override the my_*alloc calls if dmalloc defined*/

static si2drGroupIdT gs[1000];
static int gsindex = 0;

static si2drErrorT err;
static si2drAttrTypeT atype;
static si2drAttrIdT curr_attr;
static si2drDefineIdT curr_def;
void push_group(liberty_head *h);
void pop_group(liberty_head *h);
si2drValueTypeT convert_vt(char *type);
int lineno;
int syntax_errors;
static char PB[8000]; /* so as not to have a bunch of local buffers */
extern int tight_colon_ok;
extern char token_comment_buf[SI2DR_MAX_STRING_LEN];
extern char token_comment_buf2[SI2DR_MAX_STRING_LEN];
static char token_comment_buft[SI2DR_MAX_STRING_LEN];
extern int tok_encountered;
extern char *curr_file;
extern liberty_strtable *master_string_table;

struct xnumber {
        int type; /* 0=int, 1=float */
        int intnum;
        double floatnum;
};
typedef struct xnumber xnumber;

void make_complex(liberty_head *h);
void make_simple(char *name, liberty_attribute_value *v);

#line 143 "liberty_parser.c" /* yacc.c:339  */

#ifndef YY_NULLPTR
#if defined __cplusplus && 201103L <= __cplusplus
#define YY_NULLPTR nullptr
#else
#define YY_NULLPTR 0
#endif
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
#undef YYERROR_VERBOSE
#define YYERROR_VERBOSE 1
#else
#define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_LIBERTY_PARSER_Y_TAB_H_INCLUDED
#define YY_LIBERTY_PARSER_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#if YYDEBUG
extern int liberty_parser_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
#define YYTOKENTYPE
enum yytokentype {
        COMMA = 258,
        SEMI = 259,
        LPAR = 260,
        RPAR = 261,
        LCURLY = 262,
        RCURLY = 263,
        COLON = 264,
        KW_DEFINE = 265,
        KW_DEFINE_GROUP = 266,
        KW_TRUE = 267,
        KW_FALSE = 268,
        PLUS = 269,
        MINUS = 270,
        MULT = 271,
        DIV = 272,
        EQ = 273,
        UNARY = 274,
        NUM = 275,
        STRING = 276,
        IDENT = 277
};
#endif
/* Tokens.  */
#define COMMA 258
#define SEMI 259
#define LPAR 260
#define RPAR 261
#define LCURLY 262
#define RCURLY 263
#define COLON 264
#define KW_DEFINE 265
#define KW_DEFINE_GROUP 266
#define KW_TRUE 267
#define KW_FALSE 268
#define PLUS 269
#define MINUS 270
#define MULT 271
#define DIV 272
#define EQ 273
#define UNARY 274
#define NUM 275
#define STRING 276
#define IDENT 277

/* Value type.  */
#if !defined YYSTYPE && !defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE {
#line 71 "liberty_parser.y" /* yacc.c:355  */

        char *str;
        xnumber num;
        liberty_group *group;
        liberty_attribute *attr;
        liberty_attribute_value *val;
        liberty_define *def;
        liberty_head *head;
        si2drExprT *expr;

#line 239 "liberty_parser.c" /* yacc.c:355  */
};
#define YYSTYPE_IS_TRIVIAL 1
#define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE liberty_parser_lval;

int liberty_parser_parse(void);

#endif /* !YY_LIBERTY_PARSER_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 254 "liberty_parser.c" /* yacc.c:358  */

#ifdef short
#undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
#ifdef __SIZE_TYPE__
#define YYSIZE_T __SIZE_TYPE__
#elif defined size_t
#define YYSIZE_T size_t
#elif !defined YYSIZE_T
#include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#define YYSIZE_T size_t
#else
#define YYSIZE_T unsigned int
#endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T)-1)

#ifndef YY_
#if defined YYENABLE_NLS && YYENABLE_NLS
#if ENABLE_NLS
#include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#define YY_(Msgid) dgettext("bison-runtime", Msgid)
#endif
#endif
#ifndef YY_
#define YY_(Msgid) Msgid
#endif
#endif

#ifndef YY_ATTRIBUTE
#if (defined __GNUC__ &&                                           \
     (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__))) || \
    defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#define YY_ATTRIBUTE(Spec) __attribute__(Spec)
#else
#define YY_ATTRIBUTE(Spec) /* empty */
#endif
#endif

#ifndef YY_ATTRIBUTE_PURE
#define YY_ATTRIBUTE_PURE YY_ATTRIBUTE((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
#define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE((__unused__))
#endif

#if !defined _Noreturn && \
    (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
#if defined _MSC_VER && 1200 <= _MSC_VER
#define _Noreturn __declspec(noreturn)
#else
#define _Noreturn YY_ATTRIBUTE((__noreturn__))
#endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if !defined lint || defined __GNUC__
#define YYUSE(E) ((void)(E))
#else
#define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                       \
        _Pragma("GCC diagnostic push")                            \
            _Pragma("GCC diagnostic ignored \"-Wuninitialized\"") \
                _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
#define YY_IGNORE_MAYBE_UNINITIALIZED_END _Pragma("GCC diagnostic pop")
#else
#define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
#define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
#define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
#define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if !defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

#ifdef YYSTACK_USE_ALLOCA
#if YYSTACK_USE_ALLOCA
#ifdef __GNUC__
#define YYSTACK_ALLOC __builtin_alloca
#elif defined __BUILTIN_VA_ARG_INCR
#include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#elif defined _AIX
#define YYSTACK_ALLOC __alloca
#elif defined _MSC_VER
#include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#define alloca _alloca
#else
#define YYSTACK_ALLOC alloca
#if !defined _ALLOCA_H && !defined EXIT_SUCCESS
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
/* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#endif
#endif
#endif
#endif

#ifdef YYSTACK_ALLOC
/* Pacify GCC's 'empty if-body' warning.  */
#define YYSTACK_FREE(Ptr) \
        do { /* empty */  \
                ;         \
        } while (0)
#ifndef YYSTACK_ALLOC_MAXIMUM
/* The OS might guarantee only one guard page at the bottom of the stack,
   and a page size can be as small as 4096 bytes.  So we cannot safely
   invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
   to allow for a few compiler-allocated temporary stack slots.  */
#define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#endif
#else
#define YYSTACK_ALLOC YYMALLOC
#define YYSTACK_FREE YYFREE
#ifndef YYSTACK_ALLOC_MAXIMUM
#define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#endif
#if (defined __cplusplus && !defined EXIT_SUCCESS && \
     !((defined YYMALLOC || defined malloc) &&       \
       (defined YYFREE || defined free)))
#include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#endif
#ifndef YYMALLOC
#define YYMALLOC malloc
#if !defined malloc && !defined EXIT_SUCCESS
void *malloc(YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#endif
#endif
#ifndef YYFREE
#define YYFREE free
#if !defined free && !defined EXIT_SUCCESS
void free(void *);      /* INFRINGES ON USER NAME SPACE */
#endif
#endif
#endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */

#if (!defined yyoverflow &&   \
     (!defined __cplusplus || \
      (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc {
        yytype_int16 yyss_alloc;
        YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
#define YYSTACK_GAP_MAXIMUM (sizeof(union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
#define YYSTACK_BYTES(N) \
        ((N) * (sizeof(yytype_int16) + sizeof(YYSTYPE)) + YYSTACK_GAP_MAXIMUM)

#define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
#define YYSTACK_RELOCATE(Stack_alloc, Stack)                            \
        do {                                                            \
                YYSIZE_T yynewbytes;                                    \
                YYCOPY(&yyptr->Stack_alloc, Stack, yysize);             \
                Stack = &yyptr->Stack_alloc;                            \
                yynewbytes =                                            \
                    yystacksize * sizeof(*Stack) + YYSTACK_GAP_MAXIMUM; \
                yyptr += yynewbytes / sizeof(*yyptr);                   \
        } while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
#ifndef YYCOPY
#if defined __GNUC__ && 1 < __GNUC__
#define YYCOPY(Dst, Src, Count) \
        __builtin_memcpy(Dst, Src, (Count) * sizeof(*(Src)))
#else
#define YYCOPY(Dst, Src, Count)                                              \
        do {                                                                 \
                YYSIZE_T yyi;                                                \
                for (yyi = 0; yyi < (Count); yyi++) (Dst)[yyi] = (Src)[yyi]; \
        } while (0)
#endif
#endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL 3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST 88

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS 23
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS 21
/* YYNRULES -- Number of rules.  */
#define YYNRULES 49
/* YYNSTATES -- Number of states.  */
#define YYNSTATES 85

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK 2
#define YYMAXUTOK 277

#define YYTRANSLATE(YYX) \
        ((unsigned int)(YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] = {
    0, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2, 2,
    2, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  1,  2, 3, 4,
    5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] = {
    0,   102, 102, 102, 105, 105, 106, 106, 110, 111, 115, 116, 117,
    118, 119, 122, 122, 123, 124, 124, 127, 128, 131, 131, 132, 136,
    137, 150, 165, 174, 182, 183, 186, 200, 206, 215, 221, 234, 240,
    246, 252, 328, 336, 344, 352, 360, 368, 376, 384, 402};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] = {"$end",
                                      "error",
                                      "$undefined",
                                      "COMMA",
                                      "SEMI",
                                      "LPAR",
                                      "RPAR",
                                      "LCURLY",
                                      "RCURLY",
                                      "COLON",
                                      "KW_DEFINE",
                                      "KW_DEFINE_GROUP",
                                      "KW_TRUE",
                                      "KW_FALSE",
                                      "PLUS",
                                      "MINUS",
                                      "MULT",
                                      "DIV",
                                      "EQ",
                                      "UNARY",
                                      "NUM",
                                      "STRING",
                                      "IDENT",
                                      "$accept",
                                      "file",
                                      "$@1",
                                      "group",
                                      "$@2",
                                      "$@3",
                                      "statements",
                                      "statement",
                                      "simple_attr",
                                      "$@4",
                                      "$@5",
                                      "complex_attr",
                                      "head",
                                      "$@6",
                                      "param_list",
                                      "define",
                                      "define_group",
                                      "s_or_i",
                                      "attr_val",
                                      "attr_val_expr",
                                      "expr",
                                      YY_NULLPTR};
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] = {0,   256, 257, 258, 259, 260, 261, 262,
                                         263, 264, 265, 266, 267, 268, 269, 270,
                                         271, 272, 273, 274, 275, 276, 277};
#endif

#define YYPACT_NINF -41

#define yypact_value_is_default(Yystate) (!!((Yystate) == (-41)))

#define YYTABLE_NINF -16

#define yytable_value_is_error(Yytable_value) 0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] = {
    -41, 9,   -2,  -41, 17,  -41, 30, 24,  33,  -41, 54,  -5,  47,  -41, -41,
    -41, -41, -41, 26,  51,  -41, 56, 63,  35,  -41, 23,  -41, -41, -41, 3,
    -41, -41, -41, 54,  -41, -41, -6, -6,  -6,  37,  37,  -41, -41, -41, -41,
    -41, 53,  70,  -1,  -41, -41, -1, -1,  -41, -41, -41, 73,  55,  -41, -6,
    -6,  48,  -41, -41, 74,  -1,  -1, -1,  -1,  75,  77,  76,  -41, -41, 19,
    19,  -41, -41, -41, -6,  79,  78, -41, 81,  -41};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] = {
    2,  0,  0,  1,  0,  3,  0,  22, 4,  24, 0,  0,  0,  35, 36, 32, 30,
    31, 0,  33, 25, 0,  0,  0,  14, 0,  8,  10, 11, 21, 12, 13, 7,  0,
    23, 27, 0,  0,  0,  0,  0,  5,  9,  20, 26, 34, 0,  0,  0,  38, 39,
    0,  0,  48, 37, 49, 17, 40, 18, 0,  0,  0,  47, 46, 0,  0,  0,  0,
    0,  0,  0,  0,  45, 16, 41, 42, 43, 44, 19, 0,  0,  0,  29, 0,  28};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] = {-41, -41, -41, 84,  -41, -41, -41,
                                      62,  -41, -41, -41, -41, 86,  -41,
                                      -41, -41, -41, -36, -15, 41,  -40};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] = {-1, 1,  2,  24, 11, 12, 25,
                                        26, 27, 64, 69, 28, 29, 10,
                                        18, 30, 31, 19, 20, 56, 57};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] = {
    45, 46, 47, 35, 48, 21,  22, 43, 61, 3,  8,  62, 63, 51, 52, 16, 17, 23,
    44, 53, 4,  55, 7,  70,  71, 74, 75, 76, 77, 33, 9,  41, 34, 21, 22, 67,
    68, 8,  13, 14, 7,  -6,  48, 81, 39, 23, 15, 16, 17, 49, 50, 51, 52, 40,
    72, 32, 59, 53, 54, 55,  36, 37, 65, 66, 67, 68, 13, 14, 38, 65, 66, 67,
    68, 60, 15, 16, 17, -15, 73, 78, 79, 58, 80, 82, 83, 84, 5,  42, 6};

static const yytype_uint8 yycheck[] = {
    36, 37, 38, 18, 5,  10, 11, 4,  48, 0,  7,  51, 52, 14, 15, 21, 22, 22,
    33, 20, 22, 22, 5,  59, 60, 65, 66, 67, 68, 3,  6,  8,  6,  10, 11, 16,
    17, 7,  12, 13, 5,  8,  5,  79, 9,  22, 20, 21, 22, 12, 13, 14, 15, 18,
    6,  8,  3,  20, 21, 22, 9,  5,  14, 15, 16, 17, 12, 13, 5,  14, 15, 16,
    17, 3,  20, 21, 22, 4,  4,  4,  3,  40, 6,  4,  6,  4,  2,  25, 2};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] = {
    0,  24, 25, 0,  22, 26, 35, 5,  7,  6,  36, 27, 28, 12, 13, 20, 21,
    22, 37, 40, 41, 10, 11, 22, 26, 29, 30, 31, 34, 35, 38, 39, 8,  3,
    6,  41, 9,  5,  5,  9,  18, 8,  30, 4,  41, 40, 40, 40, 5,  12, 13,
    14, 15, 20, 21, 22, 42, 43, 42, 3,  3,  43, 43, 43, 32, 14, 15, 16,
    17, 33, 40, 40, 6,  4,  43, 43, 43, 43, 4,  3,  6,  40, 4,  6,  4};

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] = {
    0,  23, 25, 24, 27, 26, 28, 26, 29, 29, 30, 30, 30, 30, 30, 32, 31,
    31, 33, 31, 34, 34, 36, 35, 35, 37, 37, 37, 38, 39, 40, 40, 41, 41,
    41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 43, 43, 43, 43, 43};

/* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] = {
    0, 2, 0, 2, 0, 5, 0, 4, 1, 2, 1, 1, 1, 1, 1, 0, 5, 3, 0, 5, 2, 1, 0, 5, 3,
    1, 3, 2, 9, 7, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 2, 2, 1, 1};

#define yyerrok (yyerrstatus = 0)
#define yyclearin (yychar = YYEMPTY)
#define YYEMPTY (-2)
#define YYEOF 0

#define YYACCEPT goto yyacceptlab
#define YYABORT goto yyabortlab
#define YYERROR goto yyerrorlab

#define YYRECOVERING() (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                        \
        do                                                            \
                if (yychar == YYEMPTY) {                              \
                        yychar = (Token);                             \
                        yylval = (Value);                             \
                        YYPOPSTACK(yylen);                            \
                        yystate = *yyssp;                             \
                        goto yybackup;                                \
                } else {                                              \
                        yyerror(YY_("syntax error: cannot back up")); \
                        YYERROR;                                      \
                }                                                     \
        while (0)

/* Error token number */
#define YYTERROR 1
#define YYERRCODE 256

/* Enable debugging if requested.  */
#if YYDEBUG

#ifndef YYFPRINTF
#include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#define YYFPRINTF fprintf
#endif

#define YYDPRINTF(Args)                      \
        do {                                 \
                if (yydebug) YYFPRINTF Args; \
        } while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
#define YY_LOCATION_PRINT(File, Loc) ((void)0)
#endif

#define YY_SYMBOL_PRINT(Title, Type, Value, Location)         \
        do {                                                  \
                if (yydebug) {                                \
                        YYFPRINTF(stderr, "%s ", Title);      \
                        yy_symbol_print(stderr, Type, Value); \
                        YYFPRINTF(stderr, "\n");              \
                }                                             \
        } while (0)

/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void yy_symbol_value_print(FILE *yyoutput, int yytype,
                                  YYSTYPE const *const yyvaluep) {
        FILE *yyo = yyoutput;
        YYUSE(yyo);
        if (!yyvaluep) return;
#ifdef YYPRINT
        if (yytype < YYNTOKENS) YYPRINT(yyoutput, yytoknum[yytype], *yyvaluep);
#endif
        YYUSE(yytype);
}

/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void yy_symbol_print(FILE *yyoutput, int yytype,
                            YYSTYPE const *const yyvaluep) {
        YYFPRINTF(yyoutput, "%s %s (", yytype < YYNTOKENS ? "token" : "nterm",
                  yytname[yytype]);

        yy_symbol_value_print(yyoutput, yytype, yyvaluep);
        YYFPRINTF(yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void yy_stack_print(yytype_int16 *yybottom, yytype_int16 *yytop) {
        YYFPRINTF(stderr, "Stack now");
        for (; yybottom <= yytop; yybottom++) {
                int yybot = *yybottom;
                YYFPRINTF(stderr, " %d", yybot);
        }
        YYFPRINTF(stderr, "\n");
}

#define YY_STACK_PRINT(Bottom, Top)                           \
        do {                                                  \
                if (yydebug) yy_stack_print((Bottom), (Top)); \
        } while (0)

/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void yy_reduce_print(yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule) {
        unsigned long int yylno = yyrline[yyrule];
        int yynrhs = yyr2[yyrule];
        int yyi;
        YYFPRINTF(stderr, "Reducing stack by rule %d (line %lu):\n", yyrule - 1,
                  yylno);
        /* The symbols being reduced.  */
        for (yyi = 0; yyi < yynrhs; yyi++) {
                YYFPRINTF(stderr, "   $%d = ", yyi + 1);
                yy_symbol_print(stderr, yystos[yyssp[yyi + 1 - yynrhs]],
                                &(yyvsp[(yyi + 1) - (yynrhs)]));
                YYFPRINTF(stderr, "\n");
        }
}

#define YY_REDUCE_PRINT(Rule)                                     \
        do {                                                      \
                if (yydebug) yy_reduce_print(yyssp, yyvsp, Rule); \
        } while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
#define YYDPRINTF(Args)
#define YY_SYMBOL_PRINT(Title, Type, Value, Location)
#define YY_STACK_PRINT(Bottom, Top)
#define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

#if YYERROR_VERBOSE

#ifndef yystrlen
#if defined __GLIBC__ && defined _STRING_H
#define yystrlen strlen
#else
/* Return the length of YYSTR.  */
static YYSIZE_T yystrlen(const char *yystr) {
        YYSIZE_T yylen;
        for (yylen = 0; yystr[yylen]; yylen++) continue;
        return yylen;
}
#endif
#endif

#ifndef yystpcpy
#if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#define yystpcpy stpcpy
#else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *yystpcpy(char *yydest, const char *yysrc) {
        char *yyd = yydest;
        const char *yys = yysrc;

        while ((*yyd++ = *yys++) != '\0') continue;

        return yyd - 1;
}
#endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T yytnamerr(char *yyres, const char *yystr) {
        if (*yystr == '"') {
                YYSIZE_T yyn = 0;
                char const *yyp = yystr;

                for (;;) switch (*++yyp) {
                                case '\'':
                                case ',':
                                        goto do_not_strip_quotes;

                                case '\\':
                                        if (*++yyp != '\\')
                                                goto do_not_strip_quotes;
                                /* Fall through.  */
                                default:
                                        if (yyres) yyres[yyn] = *yyp;
                                        yyn++;
                                        break;

                                case '"':
                                        if (yyres) yyres[yyn] = '\0';
                                        return yyn;
                        }
        do_not_strip_quotes:;
        }

        if (!yyres) return yystrlen(yystr);

        return yystpcpy(yyres, yystr) - yyres;
}
#endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int yysyntax_error(YYSIZE_T *yymsg_alloc, char **yymsg,
                          yytype_int16 *yyssp, int yytoken) {
        YYSIZE_T yysize0 = yytnamerr(YY_NULLPTR, yytname[yytoken]);
        YYSIZE_T yysize = yysize0;
        enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
        /* Internationalized format string. */
        const char *yyformat = YY_NULLPTR;
        /* Arguments of yyformat. */
        char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
        /* Number of reported tokens (one for the "unexpected", one per
           "expected"). */
        int yycount = 0;

        /* There are many possibilities here to consider:
           - If this state is a consistent state with a default action, then
             the only way this function was invoked is if the default action
             is an error action.  In that case, don't check for expected
             tokens because there are none.
           - The only way there can be no lookahead present (in yychar) is if
             this state is a consistent state with a default action.  Thus,
             detecting the absence of a lookahead is sufficient to determine
             that there is no unexpected or expected token to report.  In that
             case, just report a simple "syntax error".
           - Don't assume there isn't a lookahead just because this state is a
             consistent state with a default action.  There might have been a
             previous inconsistent state, consistent state with a non-default
             action, or user semantic action that manipulated yychar.
           - Of course, the expected token list depends on states to have
             correct lookahead information, and it depends on the parser not
             to perform extra reductions after fetching a lookahead from the
             scanner and before detecting a syntax error.  Thus, state merging
             (from LALR or IELR) and default reductions corrupt the expected
             token list.  However, the list is correct for canonical LR with
             one exception: it will still contain any token that will not be
             accepted due to an error action in a later state.
        */
        if (yytoken != YYEMPTY) {
                int yyn = yypact[*yyssp];
                yyarg[yycount++] = yytname[yytoken];
                if (!yypact_value_is_default(yyn)) {
                        /* Start YYX at -YYN if negative to avoid negative
                           indexes in
                           YYCHECK.  In other words, skip the first -YYN actions
                           for
                           this state because they are default actions.  */
                        int yyxbegin = yyn < 0 ? -yyn : 0;
                        /* Stay within bounds of both yycheck and yytname.  */
                        int yychecklim = YYLAST - yyn + 1;
                        int yyxend =
                            yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
                        int yyx;

                        for (yyx = yyxbegin; yyx < yyxend; ++yyx)
                                if (yycheck[yyx + yyn] == yyx &&
                                    yyx != YYTERROR &&
                                    !yytable_value_is_error(
                                        yytable[yyx + yyn])) {
                                        if (yycount ==
                                            YYERROR_VERBOSE_ARGS_MAXIMUM) {
                                                yycount = 1;
                                                yysize = yysize0;
                                                break;
                                        }
                                        yyarg[yycount++] = yytname[yyx];
                                        {
                                                YYSIZE_T yysize1 =
                                                    yysize +
                                                    yytnamerr(YY_NULLPTR,
                                                              yytname[yyx]);
                                                if (!(yysize <= yysize1 &&
                                                      yysize1 <=
                                                          YYSTACK_ALLOC_MAXIMUM))
                                                        return 2;
                                                yysize = yysize1;
                                        }
                                }
                }
        }

        switch (yycount) {
#define YYCASE_(N, S)         \
        case N:               \
                yyformat = S; \
                break
                YYCASE_(0, YY_("syntax error"));
                YYCASE_(1, YY_("syntax error, unexpected %s"));
                YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
                YYCASE_(3,
                        YY_("syntax error, unexpected %s, expecting %s or %s"));
                YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or "
                               "%s or %s"));
                YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or "
                               "%s or %s or %s"));
#undef YYCASE_
        }

        {
                YYSIZE_T yysize1 = yysize + yystrlen(yyformat);
                if (!(yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                        return 2;
                yysize = yysize1;
        }

        if (*yymsg_alloc < yysize) {
                *yymsg_alloc = 2 * yysize;
                if (!(yysize <= *yymsg_alloc &&
                      *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
                        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
                return 1;
        }

        /* Avoid sprintf, as that infringes on the user's name space.
           Don't have undefined behavior even if the translation
           produced a string with the wrong number of "%s"s.  */
        {
                char *yyp = *yymsg;
                int yyi = 0;
                while ((*yyp = *yyformat) != '\0')
                        if (*yyp == '%' && yyformat[1] == 's' &&
                            yyi < yycount) {
                                yyp += yytnamerr(yyp, yyarg[yyi++]);
                                yyformat += 2;
                        } else {
                                yyp++;
                                yyformat++;
                        }
        }
        return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void yydestruct(const char *yymsg, int yytype, YYSTYPE *yyvaluep) {
        YYUSE(yyvaluep);
        if (!yymsg) yymsg = "Deleting";
        YY_SYMBOL_PRINT(yymsg, yytype, yyvaluep, yylocationp);

        YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
        YYUSE(yytype);
        YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;

/*----------.
| yyparse.  |
`----------*/

int yyparse(void) {
        int yystate;
        /* Number of tokens to shift before error messages enabled.  */
        int yyerrstatus;

        /* The stacks and their tools:
           'yyss': related to states.
           'yyvs': related to semantic values.

           Refer to the stacks through separate pointers, to allow yyoverflow
           to reallocate them elsewhere.  */

        /* The state stack.  */
        yytype_int16 yyssa[YYINITDEPTH];
        yytype_int16 *yyss;
        yytype_int16 *yyssp;

        /* The semantic value stack.  */
        YYSTYPE yyvsa[YYINITDEPTH];
        YYSTYPE *yyvs;
        YYSTYPE *yyvsp;

        YYSIZE_T yystacksize;

        int yyn;
        int yyresult;
        /* Lookahead token as an internal (translated) token number.  */
        int yytoken = 0;
        /* The variables used to return semantic value and location from the
           action routines.  */
        YYSTYPE yyval;

#if YYERROR_VERBOSE
        /* Buffer for error messages, and its allocated size.  */
        char yymsgbuf[128];
        char *yymsg = yymsgbuf;
        YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N) (yyvsp -= (N), yyssp -= (N))

        /* The number of symbols on the RHS of the reduced rule.
           Keep to zero when no symbol should be popped.  */
        int yylen = 0;

        yyssp = yyss = yyssa;
        yyvsp = yyvs = yyvsa;
        yystacksize = YYINITDEPTH;

        YYDPRINTF((stderr, "Starting parse\n"));

        yystate = 0;
        yyerrstatus = 0;
        yynerrs = 0;
        yychar = YYEMPTY; /* Cause a token to be read.  */
        goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
        /* In all cases, when you get here, the value and location stacks
           have just been pushed.  So pushing a state here evens the stacks.  */
        yyssp++;

yysetstate:
        *yyssp = yystate;

        if (yyss + yystacksize - 1 <= yyssp) {
                /* Get the current used size of the three stacks, in elements.
                 */
                YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
                {
                        /* Give user a chance to reallocate the stack.  Use
                           copies of
                           these so that the &'s don't force the real ones into
                           memory.  */
                        YYSTYPE *yyvs1 = yyvs;
                        yytype_int16 *yyss1 = yyss;

                        /* Each stack pointer address is followed by the size of
                           the
                           data in use in that stack, in bytes.  This used to be
                           a
                           conditional around just the two extra args, but that
                           might
                           be undefined if yyoverflow is a macro.  */
                        yyoverflow(YY_("memory exhausted"), &yyss1,
                                   yysize * sizeof(*yyssp), &yyvs1,
                                   yysize * sizeof(*yyvsp), &yystacksize);

                        yyss = yyss1;
                        yyvs = yyvs1;
                }
#else /* no yyoverflow */
#ifndef YYSTACK_RELOCATE
                goto yyexhaustedlab;
#else
                /* Extend the stack our own way.  */
                if (YYMAXDEPTH <= yystacksize) goto yyexhaustedlab;
                yystacksize *= 2;
                if (YYMAXDEPTH < yystacksize) yystacksize = YYMAXDEPTH;

                {
                        yytype_int16 *yyss1 = yyss;
                        union yyalloc *yyptr = (union yyalloc *)YYSTACK_ALLOC(
                            YYSTACK_BYTES(yystacksize));
                        if (!yyptr) goto yyexhaustedlab;
                        YYSTACK_RELOCATE(yyss_alloc, yyss);
                        YYSTACK_RELOCATE(yyvs_alloc, yyvs);
#undef YYSTACK_RELOCATE
                        if (yyss1 != yyssa) YYSTACK_FREE(yyss1);
                }
#endif
#endif /* no yyoverflow */

                yyssp = yyss + yysize - 1;
                yyvsp = yyvs + yysize - 1;

                YYDPRINTF((stderr, "Stack size increased to %lu\n",
                           (unsigned long int)yystacksize));

                if (yyss + yystacksize - 1 <= yyssp) YYABORT;
        }

        YYDPRINTF((stderr, "Entering state %d\n", yystate));

        if (yystate == YYFINAL) YYACCEPT;

        goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

        /* Do appropriate processing given the current state.  Read a
           lookahead token if we need one and don't already have one.  */

        /* First try to decide what to do without reference to lookahead token.
         */
        yyn = yypact[yystate];
        if (yypact_value_is_default(yyn)) goto yydefault;

        /* Not known => get a lookahead token if don't already have one.  */

        /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
        if (yychar == YYEMPTY) {
                YYDPRINTF((stderr, "Reading a token: "));
                yychar = yylex();
        }

        if (yychar <= YYEOF) {
                yychar = yytoken = YYEOF;
                YYDPRINTF((stderr, "Now at end of input.\n"));
        } else {
                yytoken = YYTRANSLATE(yychar);
                YY_SYMBOL_PRINT("Next token is", yytoken, &yylval, &yylloc);
        }

        /* If the proper action on seeing token YYTOKEN is to reduce or to
           detect an error, take that action.  */
        yyn += yytoken;
        if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken) goto yydefault;
        yyn = yytable[yyn];
        if (yyn <= 0) {
                if (yytable_value_is_error(yyn)) goto yyerrlab;
                yyn = -yyn;
                goto yyreduce;
        }

        /* Count tokens shifted since error; after three, turn off error
           status.  */
        if (yyerrstatus) yyerrstatus--;

        /* Shift the lookahead token.  */
        YY_SYMBOL_PRINT("Shifting", yytoken, &yylval, &yylloc);

        /* Discard the shifted token.  */
        yychar = YYEMPTY;

        yystate = yyn;
        YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
        *++yyvsp = yylval;
        YY_IGNORE_MAYBE_UNINITIALIZED_END

        goto yynewstate;

/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
        yyn = yydefact[yystate];
        if (yyn == 0) goto yyerrlab;
        goto yyreduce;

/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
        /* yyn is the number of a rule to reduce with.  */
        yylen = yyr2[yyn];

        /* If YYLEN is nonzero, implement the default value of the action:
           '$$ = $1'.

           Otherwise, the following line sets YYVAL to garbage.
           This behavior is undocumented and Bison
           users should not rely upon it.  Assigning to YYVAL
           unconditionally makes the parser a bit smaller, and it avoids a
           GCC warning that YYVAL may be used uninitialized.  */
        yyval = yyvsp[1 - yylen];

        YY_REDUCE_PRINT(yyn);
        switch (yyn) {
                case 2:
#line 102 "liberty_parser.y" /* yacc.c:1646  */
                {
                        lineno = 1;
                        syntax_errors = 0;
                }
#line 1384 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 3:
#line 102 "liberty_parser.y" /* yacc.c:1646  */
                {
                }
#line 1390 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 4:
#line 105 "liberty_parser.y" /* yacc.c:1646  */
                {
                        push_group((yyvsp[-1].head));
                }
#line 1396 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 5:
#line 105 "liberty_parser.y" /* yacc.c:1646  */
                {
                        pop_group((yyvsp[-4].head));
                }
#line 1402 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 6:
#line 106 "liberty_parser.y" /* yacc.c:1646  */
                {
                        push_group((yyvsp[-1].head));
                }
#line 1408 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 7:
#line 106 "liberty_parser.y" /* yacc.c:1646  */
                {
                        pop_group((yyvsp[-3].head));
                }
#line 1414 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 8:
#line 110 "liberty_parser.y" /* yacc.c:1646  */
                {
                }
#line 1420 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 9:
#line 111 "liberty_parser.y" /* yacc.c:1646  */
                {
                }
#line 1426 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 10:
#line 115 "liberty_parser.y" /* yacc.c:1646  */
                {
                }
#line 1432 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 11:
#line 116 "liberty_parser.y" /* yacc.c:1646  */
                {
                }
#line 1438 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 12:
#line 117 "liberty_parser.y" /* yacc.c:1646  */
                {
                }
#line 1444 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 13:
#line 118 "liberty_parser.y" /* yacc.c:1646  */
                {
                }
#line 1450 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 14:
#line 119 "liberty_parser.y" /* yacc.c:1646  */
                {
                }
#line 1456 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 15:
#line 122 "liberty_parser.y" /* yacc.c:1646  */
                {
                        make_simple((yyvsp[-2].str), (yyvsp[0].val));
                }
#line 1462 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 17:
#line 123 "liberty_parser.y" /* yacc.c:1646  */
                {
                        make_simple((yyvsp[-2].str), (yyvsp[0].val));
                }
#line 1468 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 18:
#line 124 "liberty_parser.y" /* yacc.c:1646  */
                {
                        make_simple((yyvsp[-2].str), (yyvsp[0].val));
                        si2drSimpleAttrSetIsVar(curr_attr, &err);
                }
#line 1474 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 20:
#line 127 "liberty_parser.y" /* yacc.c:1646  */
                {
                        make_complex((yyvsp[-1].head));
                }
#line 1480 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 21:
#line 128 "liberty_parser.y" /* yacc.c:1646  */
                {
                        make_complex((yyvsp[0].head));
                }
#line 1486 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 22:
#line 131 "liberty_parser.y" /* yacc.c:1646  */
                {
                        tight_colon_ok = 1;
                }
#line 1492 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 23:
#line 131 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.head) =
                            (liberty_head *)my_calloc(sizeof(liberty_head), 1);
                        (yyval.head)->name = (yyvsp[-4].str);
                        (yyval.head)->list = (yyvsp[-1].val);
                        (yyval.head)->lineno = lineno;
                        (yyval.head)->filename = curr_file;
                        tight_colon_ok = 0;
                }
#line 1498 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 24:
#line 132 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.head) =
                            (liberty_head *)my_calloc(sizeof(liberty_head), 1);
                        (yyval.head)->name = (yyvsp[-2].str);
                        (yyval.head)->list = 0;
                        (yyval.head)->lineno = lineno;
                        (yyval.head)->filename = curr_file;
                }
#line 1504 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 25:
#line 136 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.val) = (yyvsp[0].val);
                }
#line 1510 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 26:
#line 138 "liberty_parser.y" /* yacc.c:1646  */
                {
                        liberty_attribute_value *v;
                        for (v = (yyvsp[-2].val); v; v = v->next) {
                                if (!v->next) {
                                        v->next = (yyvsp[0].val);
                                        break;
                                }
                        }
                        (yyval.val) = (yyvsp[-2].val);
                }
#line 1527 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 27:
#line 151 "liberty_parser.y" /* yacc.c:1646  */
                {
                        liberty_attribute_value *v;
                        for (v = (yyvsp[-1].val); v; v = v->next) {
                                if (!v->next) {
                                        v->next = (yyvsp[0].val);
                                        break;
                                }
                        }
                        (yyval.val) = (yyvsp[-1].val);
                }
#line 1544 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 28:
#line 166 "liberty_parser.y" /* yacc.c:1646  */
                {
                        curr_def = si2drGroupCreateDefine(
                            gs[gsindex - 1], (yyvsp[-6].str), (yyvsp[-4].str),
                            convert_vt((yyvsp[-2].str)), &err);
                        si2drObjectSetLineNo(curr_def, lineno, &err);
                        si2drObjectSetFileName(curr_def, curr_file, &err);
                        if (token_comment_buf[0]) {
                                si2drDefineSetComment(curr_def,
                                                      token_comment_buf, &err);
                                token_comment_buf[0] = 0;
                        }
                        if (token_comment_buf2[0]) {
                                strcpy(token_comment_buf, token_comment_buf2);
                                token_comment_buf2[0] = 0;
                        }
                        tok_encountered = 0;
                }
#line 1554 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 29:
#line 175 "liberty_parser.y" /* yacc.c:1646  */
                {
                        curr_def = si2drGroupCreateDefine(
                            gs[gsindex - 1], (yyvsp[-4].str), (yyvsp[-2].str),
                            SI2DR_UNDEFINED_VALUETYPE, &err);
                        si2drObjectSetLineNo(curr_def, lineno, &err);
                        si2drObjectSetFileName(curr_def, curr_file, &err);
                        if (token_comment_buf[0]) {
                                si2drDefineSetComment(curr_def,
                                                      token_comment_buf, &err);
                                token_comment_buf[0] = 0;
                        }
                        if (token_comment_buf2[0]) {
                                strcpy(token_comment_buf, token_comment_buf2);
                                token_comment_buf2[0] = 0;
                        }
                        tok_encountered = 0;
                }
#line 1564 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 30:
#line 182 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.str) = (yyvsp[0].str);
                }
#line 1570 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 31:
#line 183 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.str) = (yyvsp[0].str);
                }
#line 1576 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 32:
#line 186 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.val) = (liberty_attribute_value *)my_calloc(
                            sizeof(liberty_attribute_value), 1);
                        /* I get back a floating point number... not a string,
                           and I have to
                           tell if it's an integer, without using any math lib
                           funcs? */
                        if ((yyvsp[0].num).type == 0) {
                                (yyval.val)->type = LIBERTY__VAL_INT;
                                (yyval.val)->u.int_val =
                                    (int)(yyvsp[0].num).intnum;
                        } else {
                                (yyval.val)->type = LIBERTY__VAL_DOUBLE;
                                (yyval.val)->u.double_val =
                                    (yyvsp[0].num).floatnum;
                        }
                }
#line 1595 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 33:
#line 201 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.val) = (liberty_attribute_value *)my_calloc(
                            sizeof(liberty_attribute_value), 1);
                        (yyval.val)->type = LIBERTY__VAL_STRING;
                        (yyval.val)->u.string_val = (yyvsp[0].str);
                }
#line 1605 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 34:
#line 207 "liberty_parser.y" /* yacc.c:1646  */
                {
                        char *x;
                        (yyval.val) = (liberty_attribute_value *)my_calloc(
                            sizeof(liberty_attribute_value), 1);
                        (yyval.val)->type = LIBERTY__VAL_STRING;
                        x = (char *)alloca(strlen((yyvsp[-2].str)) +
                                           strlen((yyvsp[0].str)) +
                                           2); /* get a scratchpad */
                        sprintf(x, "%s:%s", (yyvsp[-2].str), (yyvsp[0].str));
                        (yyval.val)->u.string_val =
                            liberty_strtable_enter_string(
                                master_string_table,
                                x); /* scratchpad goes away after this */
                }
#line 1618 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 35:
#line 216 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.val) = (liberty_attribute_value *)my_calloc(
                            sizeof(liberty_attribute_value), 1);
                        (yyval.val)->type = LIBERTY__VAL_BOOLEAN;
                        (yyval.val)->u.bool_val = 1;
                }
#line 1628 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 36:
#line 222 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.val) = (liberty_attribute_value *)my_calloc(
                            sizeof(liberty_attribute_value), 1);
                        (yyval.val)->type = LIBERTY__VAL_BOOLEAN;
                        (yyval.val)->u.bool_val = 0;
                }
#line 1638 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 37:
#line 235 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.val) = (liberty_attribute_value *)my_calloc(
                            sizeof(liberty_attribute_value), 1);
                        (yyval.val)->type = LIBERTY__VAL_STRING;
                        (yyval.val)->u.string_val = (yyvsp[0].str);
                }
#line 1648 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 38:
#line 241 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.val) = (liberty_attribute_value *)my_calloc(
                            sizeof(liberty_attribute_value), 1);
                        (yyval.val)->type = LIBERTY__VAL_BOOLEAN;
                        (yyval.val)->u.bool_val = 1;
                }
#line 1658 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 39:
#line 247 "liberty_parser.y" /* yacc.c:1646  */
                {
                        (yyval.val) = (liberty_attribute_value *)my_calloc(
                            sizeof(liberty_attribute_value), 1);
                        (yyval.val)->type = LIBERTY__VAL_BOOLEAN;
                        (yyval.val)->u.bool_val = 0;
                }
#line 1668 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 40:
#line 253 "liberty_parser.y" /* yacc.c:1646  */
                {
                        /* all the if/else if's are to reduce the total number
                         * of exprs to a minimum */
                        if ((yyvsp[0].expr)->type == SI2DR_EXPR_VAL &&
                            (yyvsp[0].expr)->valuetype == SI2DR_FLOAT64 &&
                            !(yyvsp[0].expr)->left && !(yyvsp[0].expr)->right) {
                                (yyval.val) =
                                    (liberty_attribute_value *)my_calloc(
                                        sizeof(liberty_attribute_value), 1);
                                (yyval.val)->type = LIBERTY__VAL_DOUBLE;
                                (yyval.val)->u.double_val =
                                    (yyvsp[0].expr)->u.d;
                                /* printf("EXPR->double %g \n", $1->u.d); */
                                si2drExprDestroy((yyvsp[0].expr), &err);
                        } else if ((yyvsp[0].expr)->type == SI2DR_EXPR_VAL &&
                                   (yyvsp[0].expr)->valuetype == SI2DR_INT32 &&
                                   !(yyvsp[0].expr)->left &&
                                   !(yyvsp[0].expr)->right) {
                                (yyval.val) =
                                    (liberty_attribute_value *)my_calloc(
                                        sizeof(liberty_attribute_value), 1);
                                (yyval.val)->type = LIBERTY__VAL_INT;
                                (yyval.val)->u.int_val = (yyvsp[0].expr)->u.i;
                                /* printf("EXPR->int - %d \n", $1->u.i); */
                                si2drExprDestroy((yyvsp[0].expr), &err);
                        } else if ((yyvsp[0].expr)->type == SI2DR_EXPR_OP_SUB &&
                                   (yyvsp[0].expr)->left &&
                                   !(yyvsp[0].expr)->right &&
                                   (yyvsp[0].expr)->left->valuetype ==
                                       SI2DR_FLOAT64 &&
                                   !(yyvsp[0].expr)->left->left &&
                                   !(yyvsp[0].expr)->left->right) {
                                (yyval.val) =
                                    (liberty_attribute_value *)my_calloc(
                                        sizeof(liberty_attribute_value), 1);
                                (yyval.val)->type = LIBERTY__VAL_DOUBLE;
                                (yyval.val)->u.double_val =
                                    -(yyvsp[0].expr)->left->u.d;
                                /* printf("EXPR->double - %g \n", $1->u.d); */
                                si2drExprDestroy((yyvsp[0].expr), &err);
                        } else if ((yyvsp[0].expr)->type == SI2DR_EXPR_OP_SUB &&
                                   (yyvsp[0].expr)->left &&
                                   !(yyvsp[0].expr)->right &&
                                   (yyvsp[0].expr)->left->valuetype ==
                                       SI2DR_INT32 &&
                                   !(yyvsp[0].expr)->left->left &&
                                   !(yyvsp[0].expr)->left->right) {
                                (yyval.val) =
                                    (liberty_attribute_value *)my_calloc(
                                        sizeof(liberty_attribute_value), 1);
                                (yyval.val)->type = LIBERTY__VAL_INT;
                                (yyval.val)->u.int_val =
                                    -(yyvsp[0].expr)->left->u.i;
                                /* printf("EXPR->double - %g \n", $1->u.d); */
                                si2drExprDestroy((yyvsp[0].expr), &err);
                        } else if ((yyvsp[0].expr)->type == SI2DR_EXPR_OP_ADD &&
                                   (yyvsp[0].expr)->left &&
                                   !(yyvsp[0].expr)->right &&
                                   (yyvsp[0].expr)->left->valuetype ==
                                       SI2DR_FLOAT64 &&
                                   !(yyvsp[0].expr)->left->left &&
                                   !(yyvsp[0].expr)->left->right) {
                                (yyval.val) =
                                    (liberty_attribute_value *)my_calloc(
                                        sizeof(liberty_attribute_value), 1);
                                (yyval.val)->type = LIBERTY__VAL_DOUBLE;
                                (yyval.val)->u.double_val =
                                    (yyvsp[0].expr)->left->u.d;
                                /* printf("EXPR->double + %g \n", $1->u.d); */
                                si2drExprDestroy((yyvsp[0].expr), &err);
                        } else if ((yyvsp[0].expr)->type == SI2DR_EXPR_OP_ADD &&
                                   (yyvsp[0].expr)->left &&
                                   !(yyvsp[0].expr)->right &&
                                   (yyvsp[0].expr)->left->valuetype ==
                                       SI2DR_INT32 &&
                                   !(yyvsp[0].expr)->left->left &&
                                   !(yyvsp[0].expr)->left->right) {
                                (yyval.val) =
                                    (liberty_attribute_value *)my_calloc(
                                        sizeof(liberty_attribute_value), 1);
                                (yyval.val)->type = LIBERTY__VAL_INT;
                                (yyval.val)->u.int_val =
                                    (yyvsp[0].expr)->left->u.i;
                                /* printf("EXPR->double + %g \n", $1->u.d); */
                                si2drExprDestroy((yyvsp[0].expr), &err);
                        } else if (
                            (yyvsp[0].expr)->type == SI2DR_EXPR_VAL &&
                            (yyvsp[0].expr)->valuetype == SI2DR_STRING &&
                            !(yyvsp[0].expr)->left && !(yyvsp[0].expr)->right
                            /* && ( strcmp($1->u.s,"VDD") && strcmp($1->u.s,"VSS")  )  I'm getting complaints about excluding VSS and VDD, so.... they'll not be exprs any more it they are all alone */) { /* uh, do we need to exclude all but VSS and VDD ? no! */
                                /* The only way a string would turned into an
                                   expr, is if it were parsed
                                   as an IDENT -- so no quotes will ever be
                                   seen... */
                                (yyval.val) =
                                    (liberty_attribute_value *)my_calloc(
                                        sizeof(liberty_attribute_value), 1);
                                (yyval.val)->type = LIBERTY__VAL_STRING;
                                (yyval.val)->u.string_val =
                                    (yyvsp[0].expr)->u.s;
                                /* printf("EXPR->string = %s \n", $1->u.s); */
                                si2drExprDestroy((yyvsp[0].expr), &err);
                        } else {
                                (yyval.val) =
                                    (liberty_attribute_value *)my_calloc(
                                        sizeof(liberty_attribute_value), 1);
                                (yyval.val)->type = LIBERTY__VAL_EXPR;
                                (yyval.val)->u.expr_val = (yyvsp[0].expr);
                                /* printf("left EXPR alone\n"); */
                        }
                }
#line 1746 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 41:
#line 329 "liberty_parser.y" /* yacc.c:1646  */
                {
                        si2drErrorT err;

                        (yyval.expr) = si2drCreateExpr(SI2DR_EXPR_OP_ADD, &err);
                        (yyval.expr)->left = (yyvsp[-2].expr);
                        (yyval.expr)->right = (yyvsp[0].expr);
                }
#line 1758 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 42:
#line 337 "liberty_parser.y" /* yacc.c:1646  */
                {
                        si2drErrorT err;

                        (yyval.expr) = si2drCreateExpr(SI2DR_EXPR_OP_SUB, &err);
                        (yyval.expr)->left = (yyvsp[-2].expr);
                        (yyval.expr)->right = (yyvsp[0].expr);
                }
#line 1770 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 43:
#line 345 "liberty_parser.y" /* yacc.c:1646  */
                {
                        si2drErrorT err;

                        (yyval.expr) = si2drCreateExpr(SI2DR_EXPR_OP_MUL, &err);
                        (yyval.expr)->left = (yyvsp[-2].expr);
                        (yyval.expr)->right = (yyvsp[0].expr);
                }
#line 1782 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 44:
#line 353 "liberty_parser.y" /* yacc.c:1646  */
                {
                        si2drErrorT err;

                        (yyval.expr) = si2drCreateExpr(SI2DR_EXPR_OP_DIV, &err);
                        (yyval.expr)->left = (yyvsp[-2].expr);
                        (yyval.expr)->right = (yyvsp[0].expr);
                }
#line 1794 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 45:
#line 361 "liberty_parser.y" /* yacc.c:1646  */
                {
                        si2drErrorT err;

                        (yyval.expr) =
                            si2drCreateExpr(SI2DR_EXPR_OP_PAREN, &err);
                        (yyval.expr)->left = (yyvsp[-1].expr);
                        (yyval.expr)->right = 0;
                }
#line 1806 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 46:
#line 369 "liberty_parser.y" /* yacc.c:1646  */
                {
                        si2drErrorT err;

                        (yyval.expr) = si2drCreateExpr(SI2DR_EXPR_OP_SUB, &err);
                        (yyval.expr)->left = (yyvsp[0].expr);
                        (yyval.expr)->right = 0;
                }
#line 1818 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 47:
#line 377 "liberty_parser.y" /* yacc.c:1646  */
                {
                        si2drErrorT err;

                        (yyval.expr) = si2drCreateExpr(SI2DR_EXPR_OP_ADD, &err);
                        (yyval.expr)->left = (yyvsp[0].expr);
                        (yyval.expr)->right = 0;
                }
#line 1830 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 48:
#line 385 "liberty_parser.y" /* yacc.c:1646  */
                {
                        si2drErrorT err;
                        (yyval.expr) = si2drCreateExpr(SI2DR_EXPR_VAL, &err);
                        (yyval.expr)->left = 0;
                        (yyval.expr)->right = 0;
                        if ((yyvsp[0].num).type == 0) {
                                (yyval.expr)->valuetype = SI2DR_INT32;
                                (yyval.expr)->u.i = (yyvsp[0].num).intnum;
                        } else {
                                (yyval.expr)->valuetype = SI2DR_FLOAT64;
                                (yyval.expr)->u.d = (yyvsp[0].num).floatnum;
                        }

                }
#line 1852 "liberty_parser.c" /* yacc.c:1646  */
                break;

                case 49:
#line 403 "liberty_parser.y" /* yacc.c:1646  */
                {
                        si2drErrorT err;

                        (yyval.expr) = si2drCreateExpr(SI2DR_EXPR_VAL, &err);
                        (yyval.expr)->valuetype = SI2DR_STRING;
                        (yyval.expr)->u.s = (yyvsp[0].str);
                        (yyval.expr)->left = 0;
                        (yyval.expr)->right = 0;
                }
#line 1866 "liberty_parser.c" /* yacc.c:1646  */
                break;

#line 1870 "liberty_parser.c" /* yacc.c:1646  */
                default:
                        break;
        }
        /* User semantic actions sometimes alter yychar, and that requires
           that yytoken be updated with the new translation.  We take the
           approach of translating immediately before every use of yytoken.
           One alternative is translating here after every semantic action,
           but that translation would be missed if the semantic action invokes
           YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
           if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
           incorrect destructor might then be invoked immediately.  In the
           case of YYERROR or YYBACKUP, subsequent parser actions might lead
           to an incorrect destructor call or verbose syntax error message
           before the lookahead is translated.  */
        YY_SYMBOL_PRINT("-> $$ =", yyr1[yyn], &yyval, &yyloc);

        YYPOPSTACK(yylen);
        yylen = 0;
        YY_STACK_PRINT(yyss, yyssp);

        *++yyvsp = yyval;

        /* Now 'shift' the result of the reduction.  Determine what state
           that goes to, based on the state we popped back to and the rule
           number reduced by.  */

        yyn = yyr1[yyn];

        yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
        if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
                yystate = yytable[yystate];
        else
                yystate = yydefgoto[yyn - YYNTOKENS];

        goto yynewstate;

/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
        /* Make sure we have latest lookahead translation.  See comments at
           user semantic actions for why this is necessary.  */
        yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE(yychar);

        /* If not already recovering from an error, report this error.  */
        if (!yyerrstatus) {
                ++yynerrs;
#if !YYERROR_VERBOSE
                yyerror(YY_("syntax error"));
#else
#define YYSYNTAX_ERROR yysyntax_error(&yymsg_alloc, &yymsg, yyssp, yytoken)
                {
                        char const *yymsgp = YY_("syntax error");
                        int yysyntax_error_status;
                        yysyntax_error_status = YYSYNTAX_ERROR;
                        if (yysyntax_error_status == 0)
                                yymsgp = yymsg;
                        else if (yysyntax_error_status == 1) {
                                if (yymsg != yymsgbuf) YYSTACK_FREE(yymsg);
                                yymsg = (char *)YYSTACK_ALLOC(yymsg_alloc);
                                if (!yymsg) {
                                        yymsg = yymsgbuf;
                                        yymsg_alloc = sizeof yymsgbuf;
                                        yysyntax_error_status = 2;
                                } else {
                                        yysyntax_error_status = YYSYNTAX_ERROR;
                                        yymsgp = yymsg;
                                }
                        }
                        yyerror(yymsgp);
                        if (yysyntax_error_status == 2) goto yyexhaustedlab;
                }
#undef YYSYNTAX_ERROR
#endif
        }

        if (yyerrstatus == 3) {
                /* If just tried and failed to reuse lookahead token after an
                   error, discard it.  */

                if (yychar <= YYEOF) {
                        /* Return failure if at end of input.  */
                        if (yychar == YYEOF) YYABORT;
                } else {
                        yydestruct("Error: discarding", yytoken, &yylval);
                        yychar = YYEMPTY;
                }
        }

        /* Else will try to reuse lookahead token after shifting the error
           token.  */
        goto yyerrlab1;

/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

        /* Pacify compilers like GCC when the user code never invokes
           YYERROR and the label yyerrorlab therefore never appears in user
           code.  */
        if (/*CONSTCOND*/ 0) goto yyerrorlab;

        /* Do not reclaim the symbols of the rule whose action triggered
           this YYERROR.  */
        YYPOPSTACK(yylen);
        yylen = 0;
        YY_STACK_PRINT(yyss, yyssp);
        yystate = *yyssp;
        goto yyerrlab1;

/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
        yyerrstatus = 3; /* Each real token shifted decrements this.  */

        for (;;) {
                yyn = yypact[yystate];
                if (!yypact_value_is_default(yyn)) {
                        yyn += YYTERROR;
                        if (0 <= yyn && yyn <= YYLAST &&
                            yycheck[yyn] == YYTERROR) {
                                yyn = yytable[yyn];
                                if (0 < yyn) break;
                        }
                }

                /* Pop the current state because it cannot handle the error
                 * token.  */
                if (yyssp == yyss) YYABORT;

                yydestruct("Error: popping", yystos[yystate], yyvsp);
                YYPOPSTACK(1);
                yystate = *yyssp;
                YY_STACK_PRINT(yyss, yyssp);
        }

        YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
        *++yyvsp = yylval;
        YY_IGNORE_MAYBE_UNINITIALIZED_END

        /* Shift the error token.  */
        YY_SYMBOL_PRINT("Shifting", yystos[yyn], yyvsp, yylsp);

        yystate = yyn;
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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
        yyerror(YY_("memory exhausted"));
        yyresult = 2;
/* Fall through.  */
#endif

yyreturn:
        if (yychar != YYEMPTY) {
                /* Make sure we have latest lookahead translation.  See comments
                   at
                   user semantic actions for why this is necessary.  */
                yytoken = YYTRANSLATE(yychar);
                yydestruct("Cleanup: discarding lookahead", yytoken, &yylval);
        }
        /* Do not reclaim the symbols of the rule whose action triggered
           this YYABORT or YYACCEPT.  */
        YYPOPSTACK(yylen);
        YY_STACK_PRINT(yyss, yyssp);
        while (yyssp != yyss) {
                yydestruct("Cleanup: popping", yystos[*yyssp], yyvsp);
                YYPOPSTACK(1);
        }
#ifndef yyoverflow
        if (yyss != yyssa) YYSTACK_FREE(yyss);
#endif
#if YYERROR_VERBOSE
        if (yymsg != yymsgbuf) YYSTACK_FREE(yymsg);
#endif
        return yyresult;
}
#line 414 "liberty_parser.y" /* yacc.c:1906  */

void push_group(liberty_head *h) {
        liberty_attribute_value *v, *vn;
        extern group_enum si2drGroupGetID(si2drGroupIdT group,
                                          si2drErrorT * err);
        si2drErrorT err;

        si2drMessageHandlerT MsgPrinter;

        group_enum ge;
        MsgPrinter = si2drPIGetMessageHandler(
            &err); /* the printer is in another file! */

        if (gsindex == 0) {
                gs[gsindex] = si2drPICreateGroup(0, h->name, &err);
        }

        else {
                gs[gsindex] =
                    si2drGroupCreateGroup(gs[gsindex - 1], 0, h->name, &err);
        }

        if (token_comment_buf[0]) {
                si2drGroupSetComment(gs[gsindex], token_comment_buf,
                                     &err); /* heaven help us if there's more
                                               than 100K of comment! */
                token_comment_buf[0] = 0;
                tok_encountered = 0;
                if (token_comment_buf2[0])
                        strcpy(token_comment_buf, token_comment_buf2);
                token_comment_buf2[0] = 0;
        }

        ge = si2drGroupGetID(gs[gsindex], &err);

        if (err == SI2DR_OBJECT_ALREADY_EXISTS &&
            (ge != LIBERTY_GROUPENUM_internal_power)) {
                sprintf(PB,
                        "%s:%d: The group name %s is already being used in "
                        "this context.",
                        curr_file, lineno, h->name);
                (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, PB, &err);
        }
        if (err == SI2DR_INVALID_NAME) {
                sprintf(PB, "%s:%d: The group name \"%s\" is invalid.",
                        curr_file, lineno, h->name);
                (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, PB, &err);
        }
        gsindex++;

        si2drObjectSetLineNo(gs[gsindex - 1], h->lineno, &err);
        si2drObjectSetFileName(gs[gsindex - 1], h->filename, &err);
        for (v = h->list; v; v = vn) {
                if (v->type != LIBERTY__VAL_STRING) {
                        char buf[1000], *buf2;

                        if (v->type == LIBERTY__VAL_INT) {
                                sprintf(buf, "%d", v->u.int_val);
                        } else {
                                sprintf(buf, "%.12g", v->u.double_val);
                        }
                        si2drGroupAddName(gs[gsindex - 1], buf, &err);

                        if (err == SI2DR_OBJECT_ALREADY_EXISTS &&
                            (ge != LIBERTY_GROUPENUM_internal_power) &&
                            (ge != LIBERTY_GROUPENUM_vector) &&
                            (ge != LIBERTY_GROUPENUM_ccs_timing_base_curve)) {
                                sprintf(PB,
                                        "%s:%d: The group name %s is already "
                                        "being used in this context.",
                                        curr_file, lineno, buf);
                                (*MsgPrinter)(SI2DR_SEVERITY_ERR,
                                              SI2DR_SYNTAX_ERROR, PB, &err);
                        } else if (err == SI2DR_INVALID_NAME) {
                                sprintf(
                                    PB,
                                    "%s:%d: The group name \"%s\" is invalid.",
                                    curr_file, lineno, buf);
                                (*MsgPrinter)(SI2DR_SEVERITY_ERR,
                                              SI2DR_SYNTAX_ERROR, PB, &err);
                        } else if (err != SI2DR_NO_ERROR) {
                                sprintf(
                                    PB,
                                    "%s:%d: Adding group name \"%s\" failed.",
                                    curr_file, lineno, buf);
                                (*MsgPrinter)(SI2DR_SEVERITY_ERR,
                                              SI2DR_SYNTAX_ERROR, PB, &err);
                        }
                } else {
                        si2drGroupAddName(gs[gsindex - 1], v->u.string_val,
                                          &err);

                        if (err == SI2DR_OBJECT_ALREADY_EXISTS &&
                            (ge != LIBERTY_GROUPENUM_internal_power) &&
                            (ge != LIBERTY_GROUPENUM_vector) &&
                            (ge != LIBERTY_GROUPENUM_library) &&
                            (ge != LIBERTY_GROUPENUM_intrinsic_resistance) &&
                            (ge != LIBERTY_GROUPENUM_va_compact_ccs_rise) &&
                            (ge != LIBERTY_GROUPENUM_va_compact_ccs_fall) &&
                            (ge !=
                             LIBERTY_GROUPENUM_va_receiver_capacitance1_fall) &&
                            (ge !=
                             LIBERTY_GROUPENUM_va_receiver_capacitance2_fall) &&
                            (ge !=
                             LIBERTY_GROUPENUM_va_receiver_capacitance1_rise) &&
                            (ge !=
                             LIBERTY_GROUPENUM_va_receiver_capacitance2_rise) &&
                            (ge !=
                             LIBERTY_GROUPENUM_normalized_driver_waveform) &&
                            (ge != LIBERTY_GROUPENUM_dc_current) &&
                            (ge != LIBERTY_GROUPENUM_va_rise_constraint) &&
                            (ge != LIBERTY_GROUPENUM_va_fall_constraint) &&
                            (ge != 0)) {
                                sprintf(PB,
                                        "%s:%d: The group name %s is already "
                                        "being used in this context.",
                                        curr_file, lineno, v->u.string_val);
                                (*MsgPrinter)(SI2DR_SEVERITY_ERR,
                                              SI2DR_SYNTAX_ERROR, PB, &err);
                        }
                        if (err == SI2DR_INVALID_NAME) {
                                sprintf(PB,
                                        "%s:%d: The group name \"%s\" is "
                                        "invalid. It will not be added to the "
                                        "database\n",
                                        curr_file, lineno, v->u.string_val);
                                (*MsgPrinter)(SI2DR_SEVERITY_ERR,
                                              SI2DR_SYNTAX_ERROR, PB, &err);
                        }
                }

                vn = v->next;
                my_free(v);
        }
}

void pop_group(liberty_head *h) {
        gsindex--;
        my_free(h);
}

void make_complex(liberty_head *h) {
        liberty_attribute_value *v, *vn;

        curr_attr =
            si2drGroupCreateAttr(gs[gsindex - 1], h->name, SI2DR_COMPLEX, &err);
        if (token_comment_buf[0]) {
                si2drAttrSetComment(curr_attr, token_comment_buf, &err);
                token_comment_buf[0] = 0;
                tok_encountered = 0;
        }

        si2drObjectSetLineNo(curr_attr, h->lineno, &err);
        si2drObjectSetFileName(curr_attr, h->filename, &err);
        for (v = h->list; v; v = vn) {
                if (v->type == LIBERTY__VAL_BOOLEAN)
                        si2drComplexAttrAddBooleanValue(curr_attr,
                                                        v->u.bool_val, &err);
                else if (v->type == LIBERTY__VAL_STRING)
                        si2drComplexAttrAddStringValue(curr_attr,
                                                       v->u.string_val, &err);
                else if (v->type == LIBERTY__VAL_DOUBLE)
                        si2drComplexAttrAddFloat64Value(curr_attr,
                                                        v->u.double_val, &err);
                else if (v->type == LIBERTY__VAL_INT)
                        si2drComplexAttrAddInt32Value(curr_attr, v->u.int_val,
                                                      &err);
                else
                        si2drComplexAttrAddStringValue(curr_attr,
                                                       v->u.string_val, &err);
                vn = v->next;
                my_free(v);
        }
        my_free(h);
}

void make_simple(char *name, liberty_attribute_value *v) {
        curr_attr =
            si2drGroupCreateAttr(gs[gsindex - 1], name, SI2DR_SIMPLE, &err);
        if (token_comment_buf[0]) {
                si2drAttrSetComment(curr_attr, token_comment_buf, &err);
                token_comment_buf[0] = 0;
                tok_encountered = 0;
        }

        si2drObjectSetLineNo(curr_attr, lineno, &err);
        si2drObjectSetFileName(curr_attr, curr_file, &err);
        if (v->type == LIBERTY__VAL_BOOLEAN)
                si2drSimpleAttrSetBooleanValue(curr_attr, v->u.bool_val, &err);
        else if (v->type == LIBERTY__VAL_EXPR)
                si2drSimpleAttrSetExprValue(curr_attr, v->u.expr_val, &err);
        else if (v->type == LIBERTY__VAL_STRING)
                si2drSimpleAttrSetStringValue(curr_attr, v->u.string_val, &err);
        else if (v->type == LIBERTY__VAL_DOUBLE)
                si2drSimpleAttrSetFloat64Value(curr_attr, v->u.double_val,
                                               &err);
        else if (v->type == LIBERTY__VAL_INT)
                si2drSimpleAttrSetInt32Value(curr_attr, v->u.int_val, &err);
        else
                si2drSimpleAttrSetStringValue(curr_attr, v->u.string_val, &err);

        my_free(v);
}

si2drValueTypeT convert_vt(char *type) {
        if (!strcmp(type, "string")) return SI2DR_STRING;
        if (!strcmp(type, "integer")) return SI2DR_INT32;
        if (!strcmp(type, "float")) return SI2DR_FLOAT64;
        if (!strcmp(type, "boolean")) return SI2DR_BOOLEAN;
        return SI2DR_UNDEFINED_VALUETYPE;
}

yyerror(char *s) {
        si2drErrorT err;

        si2drMessageHandlerT MsgPrinter;

        MsgPrinter = si2drPIGetMessageHandler(
            &err); /* the printer is in another file! */

        sprintf(PB, "===\nERROR === %s file: %s, line number %d\nERROR ===", s,
                curr_file, lineno);
        (*MsgPrinter)(SI2DR_SEVERITY_ERR, SI2DR_SYNTAX_ERROR, PB, &err);

        syntax_errors++;
}

// Guilherme Flach 2016-04-04
// Added to ignore some warning messages in this file.
#pragma GCC diagnostic pop