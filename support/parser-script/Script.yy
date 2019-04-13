%skeleton "lalr1.cc"
%require  "2.3"
%defines
%define api.namespace {ScriptParsing}
%define parser_class_name {ScriptParser}

%code requires{
   namespace ScriptParsing {
      class ScriptScanner;
      class ScriptReader;
   }

   #include "ScriptCommand.h"
   using ScriptParsing::ParsedParamValue;
   using ScriptParsing::Json;

// The following definitions is missing when "%locations" isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { ScriptScanner &scanner }
%parse-param { ScriptReader &reader }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   #include "ScriptReader.h"
   #include "ScriptScanner.h"

   #undef yylex
   #define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%token               END    0     "end of file"
%token <std::string> STRING
%token <std::string> MALFORMED_STRING
%token <std::string> IDENTIFIER
%token <std::string> PARAM_NAME
%token <bool>        BOOLEAN
%token <int>         INTEGER
%token <float>       FLOAT
%token               NIL

%type <ParsedParamValue> param_value

%type <std::string> json
%type <std::string> json_element_list
%type <std::string> json_element
%type <std::string> json_value
%type <std::string> json_list
%type <std::string> json_list_elements

%locations

%%

/* TODO: I noticed that some unicode characters (I belive they start with a
   zero) are mistaken for an end-of-file. I don't know yet how to solve this. */
/*
start
	: command_list END
	;
*/
command_list
	: command
	| command_list ';' command
	;

command
	: /* empty */
	| IDENTIFIER { reader.readCommandName($1); } pos_param_list named_param_list
	;

pos_param_list
	: /* empty */
	| pos_param_list param_value { reader.readPositionalParam(ParsedParamValue($2)); }
	;

named_param_list
	: /* empty */
	| named_param named_param_list
	;

named_param
	: PARAM_NAME { reader.readNamedParam($1, ParsedParamValue()); }
	| PARAM_NAME param_value { reader.readNamedParam($1, $2); }
	;

param_value
	: INTEGER { $$ = ParsedParamValue($1); }
	| FLOAT   { $$ = ParsedParamValue($1); }
	| STRING  { $$ = ParsedParamValue($1); }
	| BOOLEAN { $$ = ParsedParamValue($1); }
	| NIL     { $$ = ParsedParamValue(); }
	| json    { $$ = ParsedParamValue(Json::parse($1)); }
	;

json
	: '{' json_element_list '}'           { $$ = '{' + $2 + '}'; }
	;

json_element_list
	: /* empty */                         { $$ = ""; }
	| json_element                        { $$ = $1; }
	| json_element_list ',' json_element  { $$ = $1 + ',' + $3; }
	;


json_element
	: STRING ':' json_value               { $$ = '"' + $1 + '"' + ':' + $3; }
	;

json_value
	: INTEGER                             { $$ = std::to_string($1); }
	| FLOAT                               { $$ = std::to_string($1); }
	| STRING                              { $$ = '"' + $1 + '"'; }
	| BOOLEAN                             { $$ = $1? "true" : "false"; }
	| NIL								  { $$ = "null"; }
	| json_list                           { $$ = $1; }
	| json                                { $$ = $1; }
	;

json_list
	: '[' ']'                             { $$ = "[]"; }
	| '[' json_list_elements ']'          { $$ = '[' + $2 + ']'; }
	;

json_list_elements
	: json_value                          { $$ = $1; }
	| json_list_elements ',' json_value   { $$ = $1 + ',' + $3; }
	;

%%

void
ScriptParsing::ScriptParser::error(const ScriptParsing::ScriptParser::location_type &l, const std::string &err_message) {
   std::cout << "ScriptParsing Error: " << err_message << " at " << l << "\n";
}
