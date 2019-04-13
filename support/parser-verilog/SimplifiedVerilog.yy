%skeleton "lalr1.cc"
%require  "2.3"
%defines 
%define api.namespace {Parsing}
%define parser_class_name {SimplifiedVerilogParser}

%code requires{
   namespace Parsing {
      class SimplifiedVerilogScanner;
      class SimplifiedVerilogReader;
   }

// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { SimplifiedVerilogScanner &scanner }
%parse-param { SimplifiedVerilogReader &reader }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   /* include for all driver functions */
   #include "SimplifiedVerilogReader.h"

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%token               END    0     "end of file"
%token <int>	     INTEGER
%token               MODULE
%token               END_MODULE
%token               INPUT
%token               OUTPUT
%token               WIRE
%token <std::string> IDENTIFIER
%token               CHAR

%locations

%%

//RULES

start
   : module_declaration END
   ;

module_declaration
	: MODULE IDENTIFIER {reader.readModuleName($2);} io ';' implementation END_MODULE
	;

io
    : /* empty */
    | '(' identifier_list ')'

identifier_list
    : IDENTIFIER { reader.readIdentifier($1); }
    | identifier_list ',' IDENTIFIER { reader.readIdentifier($3); }
    ;
    
implementation
	: /*empty*/
	| declaration_list
	;

declaration_list
   : declaration
   | declaration_list declaration
   ;
   
declaration
   : port_declaration
   | net_declaration
   | instance_declaration
   ;

port_declaration
   : INPUT { reader.setCurrentIdentifierListType(IDENTIFIER_LIST_INPUT_PORT); } identifier_list ';'
   | INPUT '[' INTEGER ':' INTEGER ']' { reader.setCurrentIdentifierListType(IDENTIFIER_LIST_INPUT_PORT); reader.setBusRange($3, $5); } identifier_list ';'      
   | OUTPUT { reader.setCurrentIdentifierListType(IDENTIFIER_LIST_OUTPUT_PORTS); } identifier_list ';'
   | OUTPUT '[' INTEGER ':' INTEGER ']' { reader.setCurrentIdentifierListType(IDENTIFIER_LIST_OUTPUT_PORTS); reader.setBusRange($3, $5); } identifier_list ';'
   ;

net_declaration
   : WIRE { reader.setCurrentIdentifierListType(IDENTIFIER_LIST_NETS); } identifier_list ';'
   | WIRE '[' INTEGER ':' INTEGER ']'  { reader.setCurrentIdentifierListType(IDENTIFIER_LIST_NETS); reader.setBusRange($3, $5);} identifier_list ';'
   ;

   
instance_declaration
   : IDENTIFIER IDENTIFIER { reader.readInstance($1, $2); } '(' port_mapping ')' ';'
   ;

port_mapping
   : /*empty*/
   | ordered_port_mapping { reader.error(ERROR_UNNAMED_PORT_MAPPING_NOT_SUPPORTED); }
   | named_port_mapping
   ;

ordered_port_mapping
   : identifier_list 
   ;
 
named_port_mapping
   : connection
   | named_port_mapping ',' connection
   ;
   
connection
   : '.' IDENTIFIER '(' ')' { reader.readConnection($2, ""); }
   | '.' IDENTIFIER '(' IDENTIFIER ')' { reader.readConnection($2, $4); }
   ;

%%

void 
Parsing::SimplifiedVerilogParser::error(const Parsing::SimplifiedVerilogParser::location_type &l, const std::string &err_message) {
   std::cout << "Parsing Error: " << err_message << " at " << l << "\n";
}
