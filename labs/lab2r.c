/*
	Mathematical optimization - LAB2r.
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
#include <rungekutta.h>

/*
	The task.
*/
const double precision = 0.0001;
const char *DIFF_TASK = "X * Y + cos X";
const double X0 = 0;
const double T0 = 1;

formula TASK_COMPILED;

/* Three random points */
const double a = 3, b = 5, c = 2;

#define sgn(x) ((x) > 0 ? 1 : (((x) == 0) ? 0 : -1))

double FI_OF_S(double s)
{
	formula F = rungekutta_solve(TASK_COMPILED, X0, s);
	return pow(fabs(eval(F, T0)), 1.5);
}

void re_sort_hash(int count, double *keys, double *vals)
{ /* Sort two arrays by keys, preserving the same index for each key=val */
	int i, j, done; double t;

	do
	{
		done = 1;
		for(i = 0; i < count - 1; i ++)
		{
			for(j = i + 1; j < count; j ++)
			{

				if(keys[j] < keys[i])
				{
					t = keys[j];
					keys[j] = keys[i];
					keys[i] = t;

					done = 0;

					t = vals[j];
					vals[j] = vals[i];
					vals[i] = t;
				}
			}
		}
	} while(!done);
}


int main( void )
{
	TASK_COMPILED = parse(DIFF_TASK);
	if(!TASK_COMPILED)
	{
		printf("parse() failed\n");
		return 1;
	}

	/*
		Parabola method.
	*/
	double x[4], f[4], xmin; int sign1;

	x[0] = a; x[1] = b; x[2] = c; /* Select three points */
	f[0] = FI_OF_S(x[0]); f[1] = FI_OF_S(x[1]); f[2] = FI_OF_S(x[2]);

	int i = 0; /* not needed, just for debugging messages */
	printf("%5s %10s %10s %10s %10s %10s %10s %10s %10s\n", "№", "X1", "X2", "X3", "X4", "F1", "F2", "F3", "F4");
	printf("%5i %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf\n",
		i ++, x[0], x[1], x[2], NAN, f[0], f[1], f[2], NAN);
	while(1)
	{
		x[3] = (x[0] + x[1]) / 2 +
			(f[0] - f[1]) * (x[1] - x[2]) * (x[2] - x[0]) / (2*(f[0]*(x[1]-x[2]) + f[1]*(x[2]-x[0]) + f[2]*(x[0]-x[1])));
		f[3] = FI_OF_S(x[3]);

		/* Sort the hash array x[i]<=>f[i] by the values of f[] */
		re_sort_hash(4, f, x);

		printf("%5i %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf %10.3lf\n",
			i ++, x[0], x[1], x[2], x[3], f[0], f[1], f[2], f[3]);

		if(f[1] - f[0] < precision)
		{
			xmin = x[0];
			break;
		}

		sign1 = sgn(x[1] - x[0]);
		if(sign1 == sgn(x[2] - x[0]) && sign1 == -1 * sgn(x[3] - x[0]))
		{
			x[2] = x[3];
			f[2] = f[3];
		}
	}

	printf("Minimum point: %lf < S < %lf\n", xmin - precision, xmin + precision);
	printf("F(S) =~ %lf\n", FI_OF_S(xmin));

	return 0;
}
