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

%token NUMBER
%token VARIABLE
%token SIN
%token COS
%token TAN
%token CTG
%token IOPEN
%token DIFF
%token ASIN
%token ACOS
%token ATAN
%token EXP
%token LN
%token LG
%token LOG2

%left '+' '-'
%left '*' '/'
%left SIN COS TAN CTG ASIN ACOS ATAN EXP LN LG LOG2
%left UMINUS
%left '^'
%left INTEGRAL DERIVATIVE
//%left '_'

%right 'O'
%%

result  : expr { _formula_top = $1; }

expr	: expr '+' expr { $$ = _alloc2(F_ADD, $1, $3); }
	| expr '-' expr { $$ = _alloc2(F_SUB, $1, $3); }
	| expr '*' expr { $$ = _alloc2(F_MUL, $1, $3); }
	| expr '/' expr { $$ = _alloc2(F_DIV, $1, $3); }
	| expr '^' expr { $$ = _alloc2(F_POW, $1, $3); }
	| '(' expr ')' { $$ = $2; }
	| '-' expr %prec UMINUS  { $$ = _alloc1(F_NOT, $2); }
	| SIN expr { $$ = _alloc1(F_SIN, $2); }
	| COS expr { $$ = _alloc1(F_COS, $2); }
	| TAN  expr { $$ = _alloc1(F_TAN, $2); }
	| CTG expr { $$ = _alloc1(F_CTG, $2); }
	| ASIN expr { $$ = _alloc1(F_ASIN, $2); }
	| ACOS expr { $$ = _alloc1(F_ACOS, $2); }
	| ATAN expr { $$ = _alloc1(F_ATAN, $2); }
	| EXP expr { $$ = _alloc1(F_EXP, $2); }
	| LN expr { $$ = _alloc1(F_LN, $2); }
	| LG expr { $$ = _alloc1(F_LG, $2); }
	| LOG2 expr { $$ = _alloc1(F_LOG2, $2); }
	| '|' expr '|'  { $$ = _alloc1(F_ABS, $2); }

	| DIFF expr '/' DIFF expr %prec DERIVATIVE { $$ = _alloc2(F_DERIVATIVE, $2, $5); }

	| expr 'O' { $$ = _alloc1(F_D2R, $1); }
	| IOPEN expr ']' DIFF expr '|' expr '_' expr %prec INTEGRAL { $$ = _alloc4(F_INTEGRAL, $2, $7, $9, $5); }
//	| IOPEN expr ']' '|' expr '_' expr { $$ = _alloc3(F_INTEGRAL, $2, $4, $6); }
	| NUMBER
	| VARIABLE

%%
