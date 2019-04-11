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

#ifndef YY_LIBERTY_PARSER_LIBERTY_PARSER_H_INCLUDED
#define YY_LIBERTY_PARSER_LIBERTY_PARSER_H_INCLUDED
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
#line 71 "liberty_parser.y" /* yacc.c:1909  */

        char *str;
        xnumber num;
        liberty_group *group;
        liberty_attribute *attr;
        liberty_attribute_value *val;
        liberty_define *def;
        liberty_head *head;
        si2drExprT *expr;

#line 110 "liberty_parser.h" /* yacc.c:1909  */
};
#define YYSTYPE_IS_TRIVIAL 1
#define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE liberty_parser_lval;

int liberty_parser_parse(void);

#endif /* !YY_LIBERTY_PARSER_LIBERTY_PARSER_H_INCLUDED  */
