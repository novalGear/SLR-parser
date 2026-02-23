%{
#include "lexer_defs.hpp"
#include <string>
#include <iostream>
#include <cstdlib>
%}

%option noyywrap
%option yylineno
%option nounput
%option c++

%%

[ \t\r]+          { /* ignore */ }
\n                { /* ignore */ }

"+"               {
                    yylval = Token::make(Symbol::PLUS, yytext, yylineno);
                    return TOK_PLUS;
                  }
"-"               {
                    yylval = Token::make(Symbol::MINUS, yytext, yylineno);
                    return TOK_MINUS;
                  }
"*"               {
                    yylval = Token::make(Symbol::STAR, yytext, yylineno);
                    return TOK_STAR;
                  }
"/"               {
                    yylval = Token::make(Symbol::SLASH, yytext, yylineno);
                    return TOK_SLASH;
                  }
"("               {
                    yylval = Token::make(Symbol::LPAREN, yytext, yylineno);
                    return TOK_LPAREN;
                  }
")"               {
                    yylval = Token::make(Symbol::RPAREN, yytext, yylineno);
                    return TOK_RPAREN;
                  }

[0-9]+            {
                    yylval = Token::make(Symbol::NUMBER, yytext, yylineno);
                    return TOK_NUMBER;
                  }

[a-zA-Z_][a-zA-Z0-9_]*  {
                    yylval = Token::make(Symbol::VAR, yytext, yylineno);
                    return TOK_VAR;
                  }

.                 {
                    std::cerr << "Error: Unknown char '" << yytext << "' at line " << yylineno << "\n";
                    return TOK_ERROR;
                  }

<<EOF>>           {
                    yylval = Token::make(Symbol::END_MARKER, "", yylineno);
                    return TOK_END_FILE;
                  }

%%
int yywrap() { return 1; }
