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

#ifndef _MIN1VAR_H
#define _MIN1VAR_H

#include "formula.h"

/**
	@brief Flag to enable debugging output from minify_golden_section().
		Set to 1 to enable (default 0).
*/
extern int min1var_debug;

/**
	@brief Find the point of minimum for F(X) in [a; b] range
	@param F Formula object.
	@param a Starting point of [a; b] range.
	@param b Ending point of [a; b] range. Can be INFINITY (see below).
	@param precision Needed precision (e.g. 0.05).
	@param ... The step to determine maximum b. Only needed if b=INFINITY.
		Must be of type double (e.g. "9.", not "9").
	@returns The value of X (within [a; b]) where F(X) is the smallest.

	@example
		minify_golden_section(F, 5, 88, 0.2);
		minify_golden_section(F, 5, INFINITY, 0.2, 100.);
*/
double minify_golden_section(const formula F, double a, double b, double precision, ...)
	__attribute__((nonnull(1,4) warn_unused_result));

#endif

