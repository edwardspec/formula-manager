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

#ifndef _FORMULA_H
#define _FORMULA_H

#include <math.h>
#ifndef NAN
# define NAN __builtin_nanf("")
#endif
#include <malloc.h>

#include "mpool.h"

typedef struct _formula
{
	int action;
	struct _formula *arg1;
	struct _formula *arg2;

	struct _other_args
	{
		int count;
		struct _formula **arg;
	} *other_args; /* third arg etc. */

	struct _symtable *vars; /* variables involved in current formula */
	struct _symtable *args; /* 'vars' of most top-level formula: expected parameters to eval() */
} *formula;

/*
	Warning.
	parse() function is not reentrant.
	Never use multiple calls of it in different threads at the same time.

	Returned memory MUST be formula_free()d by application
*/
formula parse(const char *code) __attribute__((malloc nonnull warn_unused_result)); /*  */
void formula_free(formula F) __attribute__((nonnull));

formula formula_clone(const formula F) __attribute__((malloc nonnull warn_unused_result));

/*
	eval() and eval_array() calculate the formula with all parameters specified.

	They MUST take as many variables as formula requires.

	NOTE: eval() takes double params ONLY!
	Writing eval(F, 2, 5) is WRONG! Use eval(F, 2., 5.) instead!

	NOTE: even while 'F' parameter is declared 'const' here,
	it MUST be in writable memory (not protected against modifications or,
	for example, not in a file mmaped as 'read only').
	This function modifies F->args temporarily when computing integrals,
	but restores it's value after calculations are done (before eval() returns).
*/
double eval(const formula F, ...) __attribute__((const nonnull(1))); // double eval(formula F, double arg);
double eval_array(const formula F, const double *args) __attribute__((nonnull(1)));

/* Returns the number of arguments */
int formula_args(const formula F);

/* Print the result of parsing in a human-readable form */
void dump(const formula F) __attribute__((nonnull));

/* Construct a formula using other. WARNING: OBSOLETE PART OF DOCS
	NOTE: *pool is used for memory allocation, if it's not NULL, otherwise new pool
		is being created (and pointer to it is written to *pool).
	All formulas returned by these functions are in *pool
		and they MUST NOT be formula_free()d one by one: you should
		use mpool_free() instead to free them all at once.
	Parameters of these functions that are formulas themselves become arguments of
	returned formulas, without any copying (so they're not constant). This is why
	you can't call formula_free() on the result - it would free parameters as well.

	Usage example:
	mpool pool = NULL;
	formula der = derivative(&pool, myformula, "varname");
	WARNING: OBSOLETE PART OF DOCS
*/

void upgrade(int action, ...); /* upgrade(F_ADD, &F1, &F2); // - the result is in F1, F2 becomes NULL */
void upgrade_derivative(formula *Fp, const char *by) __attribute__((nonnull));

/* Replace a variable with the const value */
void reduce(formula F, const char *var, double val) __attribute__((nonnull(1,2)));

/* NOT YET IMPLEMENTED */
void optimize(formula F) __attribute__((nonnull));

#endif
