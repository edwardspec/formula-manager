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

#include <stdio.h>
#include <stdlib.h>

#include "rungekutta.h"

const char *app = "test-rungekutta";

int main(int argc, char **argv)
{
	if(argc < 4)
	{
		printf("Usage: %s FORMULA X0 Y0...\n", app);
		return 1;
	}

	const char *code = argv[1];
	formula F = parse(code);
	if(!F)
	{
		printf("parse() failed\n");
		return 1;
	}
	dump(F);

	int expected_args = formula_args(F);
	if(expected_args != 2)
	{
		printf("The formula must expect two parameters, not these %i of yours\n", expected_args);
		formula_free(F);
		return 1;
	}

	formula solved;

	printf("Rungekutta method:\n");
	solved = rungekutta_solve(F, strtold(argv[2], NULL), strtold(argv[3], NULL));
	if(solved) dump(solved);

	printf("---\nY(%lf) = %lf\n", 1., eval(solved, 1.));

/*	printf("Euler method:\n");
	solved = euler_solve(F, strtold(argv[2], NULL), strtold(argv[3], NULL));
	if(solved) dump(solved);
*/

//	formula_free(solved);
	formula_free(F);
	return 0;
}
