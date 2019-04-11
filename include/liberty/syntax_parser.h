/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

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

#ifndef YY_SYNTAX_PARSER_SYNTAX_PARSER_H_INCLUDED
#define YY_SYNTAX_PARSER_SYNTAX_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#if YYDEBUG
extern int syntax_parser_debug;
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
        LBRACK = 265,
        RBRACK = 266,
        KW_FLOAT = 267,
        KW_STRING = 268,
        KW_ENUM = 269,
        KW_UNK_ARGS = 270,
        KW_INTEGER = 271,
        KW_VIRTATTR = 272,
        KW_SHORT = 273,
        KW_OR = 274,
        KW_AND = 275,
        KW_BOOLEAN = 276,
        COLONEQ = 277,
        KW_LIST = 278,
        GREATERTHAN = 279,
        LESSTHAN = 280,
        DOTS = 281,
        NUM = 282,
        STRING = 283,
        IDENT = 284
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
#define LBRACK 265
#define RBRACK 266
#define KW_FLOAT 267
#define KW_STRING 268
#define KW_ENUM 269
#define KW_UNK_ARGS 270
#define KW_INTEGER 271
#define KW_VIRTATTR 272
#define KW_SHORT 273
#define KW_OR 274
#define KW_AND 275
#define KW_BOOLEAN 276
#define COLONEQ 277
#define KW_LIST 278
#define GREATERTHAN 279
#define LESSTHAN 280
#define DOTS 281
#define NUM 282
#define STRING 283
#define IDENT 284

/* Value type.  */
#if !defined YYSTYPE && !defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE {
#line 53 "syntax_parser.y" /* yacc.c:1909  */

        char *str;
        double num;
        void *voidptr;
        libsynt_attr_type attrtype;
        libsynt_float_constraint *float_constr;
        libsynt_int_constraint *int_constr;
        libsynt_string_enum *str_enum;
        libsynt_argument *arg;
        libsynt_attribute_info *attr;
        libsynt_name_constraints nameconstr;
        libsynt_group_info *group;
        libsynt_head *head;
        libsynt_technology *tech;

#line 128 "syntax_parser.h" /* yacc.c:1909  */
};
#define YYSTYPE_IS_TRIVIAL 1
#define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE syntax_parser_lval;

int syntax_parser_parse(void);

#endif /* !YY_SYNTAX_PARSER_SYNTAX_PARSER_H_INCLUDED  */
