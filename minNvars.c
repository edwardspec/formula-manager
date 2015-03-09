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

#include <stdarg.h>
#include <stdio.h>

#include "minNvars.h"
#include "min1var.h"
#include "formula.h"

__attribute__((fastcall)) point newpoint(int N)
{
	return calloc(sizeof(double), N);
}

point minify_fastest_down(const formula F, double precision)
{
	if(!F) return NULL;
	if(precision <= 0) return NULL;

	int N = formula_args(F);
	point X = newpoint(N);

	return X;
}
