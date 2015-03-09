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

#include "formula.h"

const char *app = "test-eval";

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		printf("Usage: %s FORMULA [ARG1] [ARG2]...\n", app);
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
	if(argc - 2 != expected_args)
	{
		printf("This formula expects %i arguments, not these %i of yours.\n", expected_args, argc-2);

		formula_free(F);
		return 1;
	}

	double *args = malloc(sizeof(double) * (argc - 2));
	int i;
	for(i = 2; i < argc; i ++)
	{
		args[i - 2] = strtold(argv[i], NULL);
		printf("arg %i: %lf\n", i-2, args[i-2]);
	}


	double ret = eval_array(F, args);
	printf("Result: %lf\n", ret);

	free(args);
	formula_free(F);

	return 0;
}
