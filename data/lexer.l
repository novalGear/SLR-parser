%{
#include <stdio.h>
#include "tokens.h"
%}

%option noyywrap
%option yylineno

%%

[ \t\r\n]+        ;  /* пропускаем пробелы */

"+"               { return TOK_PLUS; }
"-"               { return TOK_MINUS; }
"*"               { return TOK_STAR; }
"/"               { return TOK_SLASH; }
"("               { return TOK_LPAREN; }
")"               { return TOK_RPAREN; }

[0-9]+            { return TOK_NUMBER; }

[a-zA-Z_][a-zA-Z0-9_]*  { return TOK_VAR; }

.                 { fprintf(stderr, "Unknown character: %s (line %d)\n", yytext, yylineno); return -1; }

<<EOF>>           { return TOK_END_OF_FILE; }

%%
