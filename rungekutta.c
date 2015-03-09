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

#include "rungekutta.h"
#include "formula_internal.h"

#include <stdio.h>

const float STEP = 0.006;

formula rungekutta_solve(formula F, double X0, double Y0)
{
	const float step = STEP;
	const float step_div_2 = step / 2;

	double x = X0, y = Y0, y_temp, y_next;
	formula top = parse("0");

	int i = 1; int factorial = 1;
//	printf("%8s %15s %15s\n", "n", "Xi", "Y^(n)(Xi)");
	do
	{
		y_temp = y + step_div_2 * eval(F, x, y);
		y_next = y + step * eval(F, x + step_div_2, y_temp);

//		printf("%8i %15.3lf %15.3lf\n", i - 1, x, y);
		y = y_temp;
		y_temp = y_next;
		x += step;

		factorial *= i;

		formula TaylorElement = parse("D/N*(X-T)^P");
		reduce(TaylorElement, "D", y);
		reduce(TaylorElement, "T", x);
		reduce(TaylorElement, "N", factorial);
		reduce(TaylorElement, "P", i - 1);

		upgrade(F_ADD, &top, &TaylorElement);
	} while(++ i < 5);

	return top;
}

