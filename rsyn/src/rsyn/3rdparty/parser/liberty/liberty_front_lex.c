#include <stdio.h>

/* I need to be able to inject tokens into the stream going into the
   liberty_parser parser. I first thought I could sed a macro into the
   token.c file, but this would require some possibly messy changes
   to the Makefile stuff; which I am reluctant to play with.

   Then it hit me: liberty_parser.y calls liberty_parser_lex; why not
   move the lexer to liberty_parser2_lex and create my own liberty_parser_lex
   routine that will call it or do my token injections?

   And so, this file is born...

*/
 
 /* defined in the token.l file */
extern int liberty_parser2_lex(void);
int token_q_empty(void);
int injected_token(void);


int liberty_parser_lex(void)
{
	if( !token_q_empty() )
	{
		return injected_token();
	}
	else
		return liberty_parser2_lex();
	
}
