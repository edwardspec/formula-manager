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

#include "integral.h"

#include <stdlib.h>
#include <time.h>

inline void randomize()
{
	srandom(time(NULL));
}
inline double random_ab(double a, double b)
{
	return a + (b-a)*(random() / RAND_MAX);
}

double simpson(formula F, int steps, double a, double b)
{
	if(!F || !steps) return NAN;

	int swap = 1; int t;
	if(a > b)
	{
		t = a;
		a = b;
		b = t;
		swap = -1;
	}

	double step = (b - a) / steps;
	double x, I;
	int two_or_four = 4;

#if 0
	printf("step = %.2lf\n", step);
	printf("F(%.2lf) = %.2lf\n", a, eval(F, a));
	printf("F(%.2lf) = %.2lf\n", b, eval(F, b));
#endif
	I = eval(F, a) + eval(F, b);
	for(x = a + step; x < b; x += step)
	{
//		printf("F(%.2lf) = %.2lf\n", x, eval(F, x));

		I += eval(F, x) * two_or_four;
		two_or_four = 6 - two_or_four;
	}

	return swap * step * I / 3;
}
/*formula simpson_formula(formula F, int steps, double a, double b)
{
}
*/





double trap(formula F, int steps, double a, double b)
{
	if(!F || !steps) return NAN;

	int swap = 1; int t;
	if(a > b)
	{
		t = a;
		a = b;
		b = t;
		swap = -1;
	}

	double step = (b - a) / steps;
	double x, I;

	I = eval(F, a) + eval(F, b) + 3 * eval(F, b - step);
	for(x = a + step; x < b - step; x += step)
	{
		I += eval(F, x) * 2;
	}

	return swap * step * I / 2;
}

