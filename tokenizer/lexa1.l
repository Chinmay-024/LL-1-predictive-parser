%{
#include <stdio.h>
%}

%%
[a-zA-Z]   { printf("id\n"); }
\+          { printf("+\n"); }
\*          { printf("*\n"); }
.          { printf("Unknown - %s\n", yytext); }
%%

int yywrap(void){}
int main() {
    yylex();
    return 0;
}
