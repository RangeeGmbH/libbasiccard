%option noyywrap
%{
#include <stdio.h>
#include <stdlib.h>
#include "keyparse.tab.h"
%}

%%
[0-9]+                  yylval = atoi(yytext); return NUMBER;
\&H[0-9A-Fa-f]+         yylval = strtol(yytext+2, NULL, 16); return NUMBER;
declare                 return DECLARECMD;
key                     return KEY;
\_[ \t]*\n              /* ignore */
[ \t]+                  /* ignore */
\n                      return NEWLINE;
.                       return (int) yytext[0];
%%

