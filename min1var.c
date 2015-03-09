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

#include "min1var.h"

int min1var_debug = 0;
double minify_golden_section(const formula F, double a, double b, double precision, ...)
{
	/* Find minimum b to use */
	if(b == INFINITY)
	{
		va_list extra_param;
		va_start(extra_param, precision);
		double H = va_arg(extra_param, double);
		va_end(extra_param);

		double FA = eval(F, a);
		double FH;
		b = H; // 1 << 10;

		do
		{
			b += H;
			FH = eval(F, b);
		} while(FA >= FH);

		if(min1var_debug)
			printf("b = INF, limiting to %lf (because F(%lf) < F(%lf))\n", b, a, b);
	}

	/*
		The 'Golden Section' algorithm.
	*/
	const double factor = (1 + sqrt(5)) / 2; // factor
	const double delta = (b - a) / (factor*factor);
	double A, B, X, Y, FX, FY;

	/* initialize first step
		|.......................|
		A X                   Y B
	*/
	A = a; B = b; /* start with the whole range */
	X = A + delta;
	Y = B - delta;
	FX = eval(F, X);
	FY = eval(F, Y);

	/* Main loop.
		On each step we look for minimal value in [A; B] range
	*/
	int i = 0; /* not needed, just for debugging messages */
	if(min1var_debug)
	{
		printf("%5s %10s %10s %10s %10s %10s %10s\n", "№", "A", "B", "X", "Y", "FX", "FY");
		printf("%5i %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf\n",
			i ++, A, B, X, Y, FX, FY);
	}
	while(fabs(B - A) > 2 * precision)
	{
//		printf("A = %lf, B = %lf\n", A, B);
		if(FX > FY)
		{
			/*
				To make things clear...
				here we have a diagram for SECOND step:

				|.......................|
				A X                   Y B

				becomes

				|.......................|
				  A Y                 X B

				What happened?
				We had F(X)>F(Y), that means the minimal value of a function
				is somewhere at [X; B]. That's why we move A, leaving B intact.
				Then we adjust Y and X (to make Golden Section once more).
			*/

			A = X;
			X = Y;
			FX = FY;

//			printf("Setting Y from %lf to %lf (%lf + %lf - %lf)\n", Y, A+B-X, A, B, X);
			Y = A + B - X;
			FY = eval(F, Y);
		}
		else
		{
			/*
				The same thing as in first case,
				but we move B here (and A is left intact).
			*/

			B = Y;
			Y = X;
			FY = FX;

//			printf("Setting X from %lf to %lf (%lf + %lf - %lf)\n", X, A+B-Y, A, B, Y);
			X = A + B - Y;
			FX = eval(F, X);
		}

		if(min1var_debug)
		{
			printf("%5i %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf\n",
				i ++, A, B, X, Y, FX, FY);
		}
	}

	double result = (A + B) / 2;
	if(min1var_debug)
	{
		printf("Minimum point: %lf < S < %lf, ", result - precision, result + precision);
		printf("F(S) =~ %lf\n", eval(F, result));
	}

	return result;
}
