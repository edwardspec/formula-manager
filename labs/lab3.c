/*
	Mathematical optimization - LAB3.
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

/*
	The task.
*/
const double precision = 0.001;
#if 1
const char *EXPR = "X^4 + exp(X)";
const double a = 0;
const double b = 1;
#else
const char *EXPR = "X^2 - sin X";
const double a = 0;
const double b = 1.57; // PI/2
#endif

int main( void )
{
	formula F = parse(EXPR);
	if(!F)
	{
		printf("parse() failed\n");
		return 1;
	}

	formula dF = F;
	upgrade_derivative(&dF, "X");

	/*
		Tangent method.
	*/
	double A, B, X, dX, fA, fB, dA, dB;
	A = a; B = b;

	/* Do we need to calculate something? */
	dA = eval(dF, A);
	if(dA > 0) { X = A; printf("f'(A) > 0: minimum point is A\n"); goto minimum_found; }

	dB = eval(dF, B);
	if(dB < 0) { X = B; printf("f'(B) < 0: minimum point is B\n"); goto minimum_found; }

	/* Main loop.
		On each step we look for minimal value in [A; B] range
	*/
	int i = 0; /* not needed, just for debugging messages */
	printf("%5s %10s %10s %10s %10s\n", "№", "A", "B", "X", "F'(X)");
	printf("%5i %10.3lf %10.3lf %10.3lf %10.3lf\n",
		i ++, A, B, NAN, NAN);

	while(1)
	{
		fA = eval(F, A);
		fB = eval(F, B);

		X = ((A * dA - fA) - (B * dB - fB)) / (dA - dB);
		dX = eval(dF, X);

		printf("%5i %10.3lf %10.3lf %10.3lf %10.3lf\n",
			i ++, A, B, X, dX);

		if(dX < -1 * precision)
			A = X;
		else if(dX > precision)
			B = X;
		else break;

		getchar();
		dA = eval(dF, A);
		dB = eval(dF, B);
	}

minimum_found:
	printf("Minimum point: S =~ %lf\n", X);
	printf("F(S) =~ %lf\n", eval(F, X));

	return 0;
}


