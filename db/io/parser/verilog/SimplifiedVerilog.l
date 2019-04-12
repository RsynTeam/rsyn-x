% {
#include <string>

/* Implementation of yyFlexScanner */
#include "SimplifiedVerilogScanner.h"
#undef YY_DECL
#define YY_DECL                                                          \
        int Parsing::SimplifiedVerilogScanner::yylex(                    \
            Parsing::SimplifiedVerilogParser::semantic_type* const lval, \
            Parsing::SimplifiedVerilogParser::location_type* loc)

        /* typedef to make the returns for the tokens shorter */
        using token = Parsing::SimplifiedVerilogParser::token;

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
    "Parsing::SimplifiedVerilogScanner" % option noyywrap % option c++

        % x CCOMMENT % x LCOMMENT

                           WS[ \t\b\f\r] +

    % % % { /** Code executed at the beginning of yylex **/
        yylval = lval;
        %
}

[ \t\b\f\r] { ; }
\n { loc->lines(); }

"//" { BEGIN(LCOMMENT); }
<LCOMMENT>.{ yymore(); }
<LCOMMENT>\n {
        loc->lines();
        BEGIN(0);
}

"/*" { BEGIN(CCOMMENT); }
<CCOMMENT>.{ yymore(); }
<CCOMMENT>\n {
        loc->lines();
        yymore();
}
<CCOMMENT> "*/" { BEGIN(0); }

/* ************************************************************************ */
/* Keywords */

input { return token::INPUT; }
output { return token::OUTPUT; }
module { return token::MODULE; }
endmodule { return token::END_MODULE; }
wire { return token::WIRE; }

/* ************************************************************************ */
/* Identifiers */

[a - zA - Z_][a - zA - Z0 - 9 $_]* {
        yylval->build<std::string>(yytext);
        return token::IDENTIFIER;
}

\\[^ \t\b\f\r] + {
        yylval->build<std::string>(yytext + 1);
        return token::IDENTIFIER;
}

/* ************************************************************************ */
/* Misc */

[
}
{
        ;:\[\],()#=.@&!?<>%|^~+*/-] { return yytext[0]; }

        [0 - 9] + {
                yylval->build<int>(std::atoi(yytext));
                return token::INTEGER;
        }

        .{ return token::CHAR; }

% %
