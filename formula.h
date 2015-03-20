/*
	Formula manager - the mathematical library.
	Copyright (C) 2010-2015 Edward Chernenko.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/

/**
	@file
	@brief Parsing, construction and evaluation of complex formulas.
*/

#ifndef _FORMULA_H
#define _FORMULA_H

#include <math.h>
#ifndef NAN
# define NAN __builtin_nanf("")
#endif
#include <malloc.h>

#include "mpool.h"

/**
	@brief Formula object. Created by \b parse() method.
*/
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

/**
	@brief Create formula object from text.
	@param code Textual representation of the formula, e.g. "cos(A) + 3*B".
	@returns Formula object.

	@warning This function is not reentrant.
	@note Returned memory must be formula_free()d by application.
*/
formula parse(const char *code) __attribute__((malloc nonnull warn_unused_result)); /*  */


/**
	@brief Free all memory used by the formula object.
	@param F Formula to be destroyed.
*/
void formula_free(formula F) __attribute__((nonnull));

/**
	@brief Create an exact copy of existing formula.
	@param F Formula to be cloned.
	@returns New formula object.
	@note Returned memory must be formula_free()d by application.
*/
formula formula_clone(const formula F) __attribute__((malloc nonnull warn_unused_result));

/**
	@brief Calculate the formula with all arguments specified.
	@param F Formula to be evaluated.
	@param ... Comma-separated values of arguments, type 'double'.
	@returns Value of F() in the specified point.

	@warning This function MUST take as many arguments as formula requires.
	@warning This function takes double params ONLY.
		Writing eval(F, 2, 5) is WRONG; use eval(F, 2., 5.) instead.
*/
double eval(const formula F, ...) __attribute__((const nonnull(1)));

/**
	@brief Same as eval(), but takes array of arguments as a parameter.
	@param F Formula to be evaluated.
	@param args Array of arguments.
	@returns Value of F() in the specified point.

	@warning This function MUST take as many arguments as formula requires.
*/
double eval_array(const formula F, const double *args) __attribute__((nonnull(1)));

/* TODO: Get rid of the ugly comment below by adressing the issue.

	NOTE: even while 'F' parameter is declared 'const' here,
	it MUST be in writable memory (not protected against modifications or,
	for example, not in a file mmaped as 'read only').
	This function modifies F->args temporarily when computing integrals,
	but restores it's value after calculations are done (before eval() returns).
*/

/**
	@brief Return the number of arguments that this formula requires.
	@param F Formula object.
	@returns Number of arguments.
*/
int formula_args(const formula F);

/**
	@brief Print the formula in a human-readable form (for debugging).
	@param F Formula to be printed.
*/
void dump(const formula F) __attribute__((nonnull));

/**
	@brief Construct a new formula from one or more existing formulas.
	@param action Top-level action of a newly-created formula, e.g. F_SIN.
	@param ... Pointers to formula objects (&F1, &F2, etc.) to be consumed.

	@note Resulting formula will be placed into the first argument (&F1).
	@note Second and following arguments will be consumed (set to NULL).
	@warning After this function returns, old values of F1, F2 can't
		be used, because these formulas were absorbed into a new one.
	@note If you need to preserve any of the arguments,
		you can formula_clone() them before calling upgrade().

	@example
		Suppose we have
			F1(X)=sin(X),
			F2(X,Y)=Y^2-X,
		and we call
			upgrade(F_ADD, &F1, &F2);
		After this call:
			F2 will be NULL
			F1 will be F1(X,Y)=sin(X)+Y^2-X
*/
void upgrade(int action, ...); /* upgrade(F_ADD, &F1, &F2); // - the result is in F1, F2 becomes NULL */

/**
	@brief Construct a formula that calculates derivative of existing one.
	@param Fp Formula to be consumed.
	@param by Name of the argument to calculate derivative by (e.g. "Z").

	@note Resulting formula will be placed into the first argument (*Fp).
*/
void upgrade_derivative(formula *Fp, const char *by) __attribute__((nonnull));

/**
	@brief Replace one of the arguments in F with the constant.
	@param F Formula to be reduced.
	@param var Name of the argument to be removed (e.g. "X").
	@param val Value to be put in place of this argument (e.g. 3.14).
*/
/* Replace a variable with the const value */
void reduce(formula F, const char *var, double val) __attribute__((nonnull(1,2)));

/* NOT YET IMPLEMENTED */
void optimize(formula F) __attribute__((nonnull));

#endif
