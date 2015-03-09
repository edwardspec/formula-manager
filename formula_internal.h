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

#ifndef _FORMULA_INTERNAL_H
#define _FORMULA_INTERNAL_H

#include "formula.h"
#include "symtable.h"

#define YYSTYPE formula

#ifndef _FORMULA_C
extern char *_formula_string_p; /* really defined in formula.c */
extern formula _formula_top; /* really defined in formula.c */
#endif

typedef int F_TYPE;
#define F_CONST 0
#define F_VAR 1
#define F_NOT 2
#define F_ADD 3
#define F_SUB 4
#define F_MUL 5
#define F_DIV 6
#define F_SIN 7
#define F_COS 8
#define F_TAN 9
#define F_CTG 10
#define F_D2R 11 // degrees to radians conversion
#define F_INTEGRAL 12
#define F_ASIN 13
#define F_ACOS 14
#define F_ATAN 15
#define F_POW 16
#define F_EXP 17
#define F_LN 18
#define F_LG 19
#define F_LOG2 20
#define F_DERIVATIVE 21
#define F_ABS 22

YYSTYPE _palloc4(mpool optional_pool, F_TYPE type, YYSTYPE arg1, YYSTYPE arg2, YYSTYPE arg3, YYSTYPE arg4) __attribute__((fastcall malloc nonnull(3) warn_unused_result));
#define _palloc3(pool, type, arg1, arg2, arg3) _palloc4(pool, type, arg1, arg2, arg3, 0)
#define _palloc2(pool, type, arg1, arg2) _palloc4(pool, type, arg1, arg2, 0, 0)
#define _palloc1(pool, type, arg) _palloc4(pool, type, arg, 0, 0, 0)
#define _palloc0(pool, type) _palloc4(pool, type, 0, 0, 0, 0)

#define _alloc4(type, arg1, arg2, arg3, arg4) _palloc4(NULL, type, arg1, arg2, arg3, arg4)
#define _alloc3(type, arg1, arg2, arg3) _palloc4(NULL, type, arg1, arg2, arg3, 0)
#define _alloc2(type, arg1, arg2) _palloc4(NULL, type, arg1, arg2, 0, 0)
#define _alloc1(type, arg) _palloc4(NULL, type, arg, 0, 0, 0)
#define _alloc0(type) _palloc4(NULL, type, 0, 0, 0, 0)

#endif
