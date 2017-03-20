
%{
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
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "libhash.h"
#include "syntax.h"
#include "syntax_parser.h"
#ifdef DMALLOC
#include <dmalloc.h>
#endif
#include "mymalloc.h"

int syntax_parser_lex(void);
int synt_lineno = 1;
char *search_string_for_linefeeds(char *str);
%}
%option   noyywrap
%option prefix="syntax_parser_"
%option outfile="lex.yy.c"

%x comment
%%

\,	{return COMMA;}
\;	{return SEMI;}
\(	{return LPAR;}
\)	{return RPAR;}
\[	{return LBRACK;}
\]	{return RBRACK;}
\{	{return LCURLY;}
\}[ 	]*\;?	{return RCURLY;}
\:	{return COLON;}
float 					{return KW_FLOAT;}
string 					{return KW_STRING;}
enum 					{return KW_ENUM;}
"<unknown_arguments>" 	{return KW_UNK_ARGS;}
integer {return KW_INTEGER;}
int {return KW_INTEGER;}
"virtual_attribute" {return KW_VIRTATTR;}
short {return KW_SHORT;}
or {return KW_OR;}
and {return KW_AND;}
boolean {return KW_BOOLEAN;}
list {return KW_LIST;}
"..." {return DOTS;}
":=" {return COLONEQ;}
">" {return GREATERTHAN;}
"<" {return LESSTHAN;}

\n	{synt_lineno++;}
\r  {}
\t  {}
" "	{}

\"[^"]*\"		{syntax_parser_lval.str = my_malloc(strlen(yytext)); yytext[strlen(yytext)-1]=0; search_string_for_linefeeds(yytext); strcpy(syntax_parser_lval.str,yytext+1); return STRING;}
\"		|
\"[^"]*		{fprintf(stderr,"No closing quote for quoted string begun in line %d\n",synt_lineno);}

"/*"	BEGIN(comment);

<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             ++synt_lineno;
<comment>"*"+"/"	BEGIN(INITIAL);


[-+]?[0-9]*\.?[0-9]*([Ee][-+]?[0-9]+)?	{syntax_parser_lval.num = strtod(yytext,(char**)NULL); return NUM;}

[-a-zA-Z0-9!@#$%^&_+=|~?<>.]+ {syntax_parser_lval.str = my_malloc(strlen(yytext)+1); strcpy(syntax_parser_lval.str,yytext);  return IDENT;}

%%
/*  \/\*.*\*\/    {printf("Line %d: Comment found: %s\n", synt_lineno, yytext);} */

char *search_string_for_linefeeds(char *str)
{
   char *s;
   s = str;
   while (*s){ if( *s++ == '\n' ) {synt_lineno++; if( *(s-2) != '\\' ){printf("Warning: line %d: String constant spanning input lines does not use continuation character.\n",synt_lineno);} } }
   return str;
}
