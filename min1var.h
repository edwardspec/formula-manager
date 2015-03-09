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

#ifndef _MIN1VAR_H
#define _MIN1VAR_H

#include "formula.h"

extern int min1var_debug; /* Set this to 1 to get debugging output. Default 0 */

/* Find the point of minimum for F(x) */

/*
	NOTE: b can be INFINITY, then this function takes one more
	argument of type double, the step to determine maximum b.

	E.g.,
		minify_golden_section(F, 5, 88, 0.2);
		minify_golden_section(F, 5, INFINITY, 0.2, 100);
*/
double minify_golden_section(const formula F, double a, double b, double precision, ...)
	__attribute__((nonnull(1,4) warn_unused_result));

#endif

