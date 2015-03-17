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

#warning "Not yet implemented"


#ifndef _MINNVARS_H
#define _MINNVARS_H

#include "formula.h"

typedef double *point; /* array of double values */

/*
	Gradient descent method.
	Find the minimum point of F(x1, x2, x3, ...)
*/
point minify_fastest_down(const formula F, double precision);

#endif
