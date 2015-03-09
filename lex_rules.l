/*
	Formula manager - the mathematical library.
	Copyright (C) 2010 Edward Chernenko.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/

%option noyywrap
%{
#include <math.h>
#include <stdlib.h>

#ifdef CASE_SENSITIVE
#define TO_UPPERCASE(c) c /* Case-sensitive implementation */
#else
#include <ctype.h>
#define TO_UPPERCASE(c) toupper(c)
#endif

#define YY_INPUT(buf,result,max_size) { \
	if(!*_formula_string_p) result = YY_NULL; \
	else { buf[0] = *_formula_string_p; _formula_string_p ++; result = 1; } }

int yylex();
void yyerror(char *msg)
{
	printf("%s\n", msg);
}

#include "formula_internal.h" // Also defines YYSTYPE as double
#include "symtable.h"
#include "y.tab.c"
%}

%%

[0-9]+("."[0-9]*)? {
	double *nr = malloc(sizeof(double));
	*nr = strtold(yytext, NULL);
	yylval = _alloc1(F_CONST, (formula) nr);
	return NUMBER;
}
INF {
	double *nr = malloc(sizeof(double));
	*nr = INFINITY;
	yylval = _alloc1(F_CONST, (formula) nr);
	return NUMBER;
}

d { return DIFF; }

[a-zA-Z] {
	int c = TO_UPPERCASE(yytext[0]);
#ifdef CASE_SENSITIVE
	if(c < 'A' || c > 'Z')
	{
		fprintf(stderr, "[error] 'case-sensitive' parser: lowercase variables are not allowed.\n");
		exit(1);
	}
#endif

	yylval = _alloc1(F_VAR, (formula) c);

	symtable_add(yylval->vars, yytext);
	return VARIABLE;
}

sin { return SIN; }
cos { return COS; }
tg { return TAN; }
ctg { return CTG; }

arcsin { return ASIN; }
arccos { return ACOS; }
arctg { return ATAN; }

exp { return EXP; }
ln { return LN; }
lg { return LG; }
log2 { return LOG2; }

$\[ { return IOPEN; }

[ \t\n]+

.          { return yytext[0]; }

%%
