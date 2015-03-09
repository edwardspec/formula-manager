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

#ifndef _MINNVARS_H
#define _MINNVARS_H

#include "formula.h"

extern int minNvars_debug; /* Set this to 1 to get debugging output. Default 0 */

/* Find the point of minimum for F(x1, x2, x3...) */

typedef double *point; /* array of double values */
point newpoint(int N) /* just a malloc */
	__attribute__((fastcall malloc warn_unused_result nonnull));

/*
	Find the minimum point of F(x1, x2, x3, ...)
*/
point minify_fastest_down(const formula F, double precision);

#endif
