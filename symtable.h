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

#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include "formula.h"

/*
	NOTE:
	these functions have universal syntax, but their implementation
	(in symtable.c) can be very different (from simple 'bitmask' method
	for one-symbol variables A-Z to normal symtables with dynamic memory allocation).

	NOTE: Only symtable_add() does all required input error-checking. Other
	functions may do it in several implementations of symtable, but you
	should NEVER rely on this (e.g. 'bitmask' symtable ignores such checks).

	This is because normally (as in formula.c) 'symtable_isset', 'symtable_del' etc.
	are always called with correct params (params used for 'symtable_add' before, that
	means the program has already checked them), and appending any error-checking
	to these functions will seriously affect perfomance of large formula calculations.
*/

typedef struct _symtable *symtable;
extern char *symerror; /* Last error occured (or NULL if there's no error) */
extern const char *symtable_method; /* 'bitmask' for "best memory-preserving" method */
extern const int symtable_case_sensitive;

/* Allocate a new symtable */
symtable symtable_new() __attribute__((malloc warn_unused_result)); /* must symtable_free() it */
symtable symtable_new_mpool(mpool pool) __attribute__((warn_unused_result)); /* don't free, just delete it's mpool */
symtable symtable_clone(const symtable t) __attribute__((malloc nonnull warn_unused_result));

/* Clear a symtable */
void symtable_clear(symtable t) __attribute__((fastcall nonnull));

/* Append ID to symtable, returns 1 if ok, 0 on error (symerror is being updated) */
int symtable_add(symtable t, const char *ID) __attribute__((fastcall nonnull));

/* Update first symtable, adding everything enabled in the second one */
void symtable_import(symtable t, symtable child) __attribute__((fastcall nonnull));

/* Check whether there's ID in symtable (returns 1) or not (returns 0) */
int symtable_isset(symtable t, const char *ID) __attribute__((fastcall nonnull const warn_unused_result));

/* Return the order of F->arg1 in F->args symtable (the same order in which variables should be in eval() parameters) */
int symtable_order(formula F) __attribute__((fastcall nonnull const warn_unused_result));

int symtable_order_raw(symtable t, const char *ID) __attribute__((fastcall nonnull const warn_unused_result));

/* Return the variable name F->arg1 from F->vars symtable (caller must free() this memory) */
char *symtable_varname(formula F) __attribute__((fastcall nonnull const warn_unused_result));

/* Remove ID from symtable */
void symtable_del(symtable t, const char *ID) __attribute__((fastcall nonnull));

/* Return the number of variables in symtable */
int symtable_count(symtable t) __attribute__((fastcall nonnull const warn_unused_result));

/* Free all resources allocated for symtable */
void symtable_free(symtable t) __attribute__((fastcall nonnull));

/* Return a string with comma-separated list of symbols (caller must free() this memory) */
char *symtable_print(symtable t) __attribute__((fastcall nonnull const warn_unused_result));

#endif
