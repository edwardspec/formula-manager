/*
	Mathematical optimization - LAB4.
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

#include <stdio.h>
#include <stdlib.h>

#include <formula.h>

/*
	The task.
*/
const double precision = 0.01; // 0.001;
const double R = 15;

const char *EXPR = "(X-4)^2 + ln X";
const double a = 3;
const double b = 5;

const int STEPS_MAX = 200000;




int main( void )
{
	formula F = parse(EXPR);
	if(!F)
	{
		printf("parse() failed\n");
		return 1;
	}
	dump(F);

	/*
		Piyavskii's algorithm.
		(in Russian: метод ломаных).
	*/
	double x, m, delta, fA, fB, fX, diff, Mk;
	double X[STEPS_MAX], M[STEPS_MAX];
	int n = 0, j, k;

	fA = eval(F, a);
	fB = eval(F, b);

	m = (a - b)*R/2 + (fA + fB) / 2;
	x = (a + b)/2 + (fA - fB) / (2*R);
	fX = eval(F, x);

	printf("%5s %12s %12s %12s %12s\n", "№", "X", "F(X)", "M", "DELTA");
	while(1)
	{
		diff = fX - m;
		if(diff < precision) break;
		delta = diff / (2*R);

		printf("%5i %12.5lf %12.5lf %12.5lf %12.5lf\n",
			n, x, fX, m, delta);

		if(!(n % 20)) printf("%5s %12s %12s %12s %12s\n", "№", "X", "F(X)", "M", "DELTA");

		X[2 * n] = x - delta;
		X[2 * n + 1] = x + delta;
		M[2 * n + 1] = M[2 * n] = (fX + m) / 2;

		/* Find min { M[j], 0 < j < 2n+1 } */
		k = 0; Mk = M[0];
		for(j = 1; j < 2*n + 1; j ++)
		{
			if(M[j] < Mk)
			{
				Mk = M[j];
				k = j;
			}
		}

		x = X[k]; fX = eval(F, x);
		m = M[k];
		M[k] = 1 << 30;
		n ++;
	}

	printf("Minimum point: %lf < S < %lf, calculated for R=%lf\n", x - precision, x + precision, R);
	printf("F(S) =~ %lf\n", eval(F, x));

	return 0;
}
