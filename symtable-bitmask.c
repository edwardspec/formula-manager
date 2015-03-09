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

#include "symtable.h"

/*
	This is a bitmask symtable implementation.
	It supports single-symbol variables from A to Z (case sensitive).

	The accent of this implementation is minimal memory consumption.

	NOTE: #define CASE_SENSITIVE makes it faster, but disallows lowercase variables.
*/

#define BITS 32 /* Don't change */
#include <malloc.h>
#include <string.h>

const int symtable_case_sensitive =
#ifdef CASE_SENSITIVE
	1;
#define TO_UPPERCASE(c) c /* Case-sensitive implementation */
#define CHAR_OUT_OF_RANGE(c) (c < 'A' || c > 'Z')
#else
	0;
#include <ctype.h>
#define TO_UPPERCASE(c) toupper(c)
#define CHAR_OUT_OF_RANGE(c) ((c < 'A' || c > 'Z') && (c < 'a' || c > 'z'))
#endif

typedef unsigned long INTTYPE;

struct _symtable
{
	INTTYPE mask : BITS;
};

char *symerror = NULL;
const char *symtable_method = "bitmask";

#define ERROR(msg) { symerror = msg; return 0; }
const INTTYPE FULLMASK = (INTTYPE) -1;

/*
	NOTE:
	For best perfomance, we avoid 'bad-params' checking in
	all functions that will be called very often.

	Only symtable_add() has this kind of check. It's being
	involked only when compiling a formula via parse().
*/
static inline struct _symtable construct_mask(char c)
{
	/*
		We return _symtable, not long, just for portability
		(size of long can change, _symtable has 4 bytes).
	*/
	struct _symtable mask_holder;

	/* No error-checking ("bad 'c' case"), see "NOTE" above */
	mask_holder.mask = 1 << (c - 'A');

	return mask_holder;
}

symtable symtable_new()
{
	symtable t = (symtable) malloc(sizeof(struct _symtable));
	if(!t)
		symerror = "malloc() failed in symtable_new()";
	else
		t->mask = 0;

	return t;
}
symtable symtable_new_mpool(mpool pool)
{
	symtable t = (symtable) mpool_malloc(pool, sizeof(struct _symtable));
	if(!t)
		symerror = "malloc() failed in symtable_new()";
	else
		t->mask = 0;

	return t;
}

__attribute__((fastcall)) void symtable_clear(symtable t)
{
	t->mask = 0;
}

__attribute__((fastcall)) int symtable_add(symtable t, const char *ID)
{
	if(!t) ERROR("symtable_add() called on undefined symtable");
	if(!ID) ERROR("symtable_add() called with undefined ID");

	if(CHAR_OUT_OF_RANGE(ID[0]) || ID[1] != '\0')
		ERROR("(bitmask%s)symtable.c: identifier is not a single latin letter (A-Z): symtable_add() failed");

	struct _symtable mask_holder = construct_mask(TO_UPPERCASE(ID[0]));
	t->mask |= mask_holder.mask;

	return 1;
}

__attribute__((fastcall)) void symtable_import(symtable t, symtable child)
{
	t->mask |= child->mask; /* really easy, isn't it */
}

symtable symtable_clone(const symtable t)
{
	symtable copy = symtable_new();
	symtable_import(copy, t);
	return copy;
}

__attribute__((fastcall))  void symtable_del(symtable t, const char *ID)
{
	struct _symtable mask_holder = construct_mask(TO_UPPERCASE(ID[0]));
	t->mask &= (FULLMASK - mask_holder.mask);
}

__attribute__((fastcall))  int symtable_isset(symtable t, const char *ID)
{
	/* No error-checking, see "NOTE" above */
	struct _symtable mask_holder = construct_mask(TO_UPPERCASE(ID[0]));

	return (t->mask & mask_holder.mask) ? 1 : 0;
}

__attribute__((fastcall))  int symtable_count(symtable t)
{
	int pos = 0, count = 0;
	INTTYPE key = 1;
	for(pos = 0; pos < BITS && key <= t->mask; pos ++, key <<= 1)
	{
		if(t->mask & key) count ++;
	}

	return count;
}

/*
	The order for 'bitmask' symtable is alphabetical: A, B, C, ..., Z.

	NOTE: it checks F->args, not F->vars!
*/
__attribute__((fastcall)) int symtable_order(formula F)
{
	char c = (int) F->arg1;
	struct _symtable mask_holder = construct_mask(c);
	int pos = 0, order = 0;
	INTTYPE key = 1;

	for(pos = 0; pos < BITS && key <= F->args->mask; pos ++, key <<= 1)
	{
		if(key == mask_holder.mask)
			return order;

		if(F->args->mask & key)
			order ++;
	}
	return order;
}
__attribute__((fastcall)) int symtable_order_raw(symtable t, const char *ID)
{
	struct _symtable mask_holder = construct_mask(TO_UPPERCASE(ID[0]));
	int pos = 0, order = 0;
	INTTYPE key = 1;

	for(pos = 0; pos < BITS && key <= t->mask; pos ++, key <<= 1)
	{
		if(key == mask_holder.mask)
			return order;

		if(t->mask & key)
			order ++;
	}
	return order;
}

__attribute__((fastcall)) char *symtable_varname(formula F)
{
	char *buf = (char *) malloc(2);
	if(!buf) return NULL;

	buf[0] = (int) F->arg1;
	buf[1] = '\0';

	return buf;
}

__attribute__((fastcall)) void symtable_free(symtable t)
{
	free(t);
}

 __attribute__((fastcall)) char *symtable_print(symtable t)
{
	int pos = 0, i = 0;
	INTTYPE key = 1;
	char symbols[BITS * 2];
	for(pos = 0; pos < BITS && key <= t->mask; pos ++, key <<= 1)
	{
		if(t->mask & key)
		{
			symbols[i] = 'A' + pos;
			symbols[i + 1] = ',';
			i += 2;
		}
	}

	symbols[i ? i - 1 : 0] = '\0';
	return strdup(symbols);
}
