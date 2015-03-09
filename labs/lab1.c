/*
	Mathematical optimization - LAB1.
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

#include <stdio.h>
#include <stdlib.h>
#include <formula.h>

#include "min1var.h"

/*
	The task.
*/
const char *EXPR = "$[(arcsin T - S*T*T)^2]dT|0_1";
const double a = 0;
double b = INFINITY;
const double precision = 0.01;
const double H = 100;

int main(void)
{
	formula F = parse(EXPR);
	if(!F)
	{
		printf("parse() failed\n");
		return 1;
	}
	dump(F);

	min1var_debug = 1;
	printf("%lf\n", minify_golden_section(F, a, b, precision, H));

	formula_free(F);
	return 0;
}
