% {
#include <string>

/* Implementation of yyFlexScanner */
#include "ScriptScanner.h"
#undef YY_DECL
#define YY_DECL                                                     \
        int ScriptParsing::ScriptScanner::yylex(                    \
            ScriptParsing::ScriptParser::semantic_type* const lval, \
            ScriptParsing::ScriptParser::location_type* loc)

        /* typedef to make the returns for the tokens shorter */
        using token = ScriptParsing::ScriptParser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return (token::END)

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION \
        loc->step();   \
        loc->columns(yyleng);

        %
}

% option nodefault % option posix - compat % option yyclass =
    "ScriptParsing::ScriptScanner" % option noyywrap % option c++ %
    option prefix = "yyScript"

                        % x MULTI_LINE_COMMENT % x SINGLE_LINE_COMMENT

                                                     WS[ \t\b\f\r] +

                    % % % {
        /* Executed at the beginning of yylex */
        yylval = lval;
        %
}

/* Whitespaces and New Lines */

{WS} { ; }
\n { loc->lines(); }

/* Single Line Comment */

"#" { BEGIN(SINGLE_LINE_COMMENT); }
<SINGLE_LINE_COMMENT>.{ yymore(); }
<SINGLE_LINE_COMMENT>\n {
        loc->lines();
        BEGIN(0);
}

/* Keywords */
true {
        yylval->build<bool>(true);
        return token::BOOLEAN;
}

false {
        yylval->build<bool>(false);
        return token::BOOLEAN;
}

null { return token::NIL; }

/* Identifiers */
[a - zA - Z_][a - zA - Z0 - 9_]* {
        yylval->build<std::string>(yytext);
        return token::IDENTIFIER;
}

- [a - zA - Z_][a - zA - Z0 - 9_]* {
        yylval->build<std::string>(yytext + 1);
        return token::PARAM_NAME;
}

/* Integer */

[0 - 9][0 - 9]* {
        yylval->build<int>(std::stoi(yytext));
        return token::INTEGER;
}

/* Floating Point: +1e10, 1e5, -1e-3 */
[+ - ] ? [0 - 9] + ([Ee][+ - ] ? [0 - 9] +) {
        yylval->build<float>(std::stof(yytext));
        return token::FLOAT;
}

/* Floating Point: +1.02e10, 1.003, -1e-3, .0 */
[+ - ] ? [0 - 9] *\.[0 - 9] + ([Ee][+ - ] ? [0 - 9] +) ? {
        yylval->build<float>(std::stof(yytext));
        return token::FLOAT;
}

/* String */

\"[^"\n]*["\n] {
		// Note: '\r' is handled as a whitespace, so we don't need to deal 
		// with it here.
		
		const bool malformed = yytext[yyleng-1] != '"';
		if (!malformed)
			 yytext[yyleng-1] = '\0';
		yylval->build<std::string>(yytext + 1); // +1 skips initial quote		
		return malformed? token::MALFORMED_STRING : token::STRING;
}

/* Unmatched */

.{ return yytext[0]; }

% %
