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
#include <string.h>
#include <errno.h>

#include <getopt.h>
#include <unistd.h>

#include "integral.h"

const char *app = "integral";
const char *version = "1.0.0";

static const struct option options[] = {
	{ "simpson", 0, 0, 's' },
	{ "trap", 0, 0, 't' },

	{ "from", 1, 0, 'a' },
	{ "to", 1, 0, 'b' },

	{ "steps", 1, 0, 'N' },

	{ "version", 0, 0, 'V' },
	{ "help", 0, 0, 'h' },

	{ 0, 0, 0, 0 }
};
const char *option_description[] = {
	"use Simpson's method",
	"use method of trapezoids",
	"first value of X",
	"last value of X",
	"number of steps",
	"",
	"output version information and exit",
	"display this help and exit",
	0
};

void print_help()
{
	int i, j;
	printf("Usage: %s [OPTION] [FORMULA]\nCalculate integral value.\n\n", app);

	for(i = 0, j = 0; option_description[i] != 0; i ++)
	{
		if(!strlen(option_description[i]))
		{
			printf("\n");
			continue;
		}

		printf("  -%c", options[j].val);
		if(options[j].name)
			printf(", --%-5s%5s", options[j].name, options[j].has_arg == 1 ? " bred" : "");
		else
			printf("%14s", "");
		printf("\t%s\n", option_description[i]);
		j ++;
	}
	exit(0);
}
void print_version()
{
	printf(
"%s %s\n\n\
Copyright (C) 2009 Edward Chernenko.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",
		app, version);
	exit(0);
}


double (*evaluator)(formula, int, double, double);

const int FORMULA_MAX = 1000;
int main(int argc, char **argv)
{
	int i, j, c;
	char code[FORMULA_MAX + 1];
	char *p;

	int N = 50;
	double a = NAN, b = NAN;
	int mode = 0; /* 0 means Simpson's method, 1 means method of trapezoids */

	while((c = getopt_long(argc, argv, "sta:b:N:Vh", options, NULL)) != -1)
	{
		switch(c)
		{
			case 'V':
				print_version();
				break;

			case 'h':
				print_help();
				break;

			case 's':
				mode = 0;
				break;

			case 't':
				mode = 1;
				break;

			case 'a':
				a = strtold(optarg, NULL);
				break;

			case 'b':
				b = strtold(optarg, NULL);
				break;

			case 'N':
				N = strtol(optarg, NULL, 10);
		}
	}

	if(isnanl(a) || isnanl(b))
	{
		fprintf(stderr, "Required parameters -a and -b not specified\n");
		return 1;
	}

	/* Copy argv[] into 'code' string */
	for(i = optind, j = 0; i < argc; i ++)
	{
		for(p = argv[i]; *p && (j <= FORMULA_MAX); j ++, p ++)
			code[j] = *p;

		if(j > FORMULA_MAX && *p)
		{
 			fprintf(stderr, "Formula can't be longer than %i bytes\n", FORMULA_MAX);
			return 1;
		}

		// Not needed if spaces are ignored by grammar (check lex_rules.l, yacc.y).
		// Just for compability.
		code[j ++] = ' ';
	}
	if(j > 1)
		code[j - 1] = 0;
	else
	{
		fgets(code, FORMULA_MAX + 1, stdin);

		int l = strlen(code) - 1;
		if(l && code[l] == '\n')
			code[l] = '\0';
	}

	formula F = parse(code);
	if(!F)
	{
		fprintf(stderr, "Failed to compile formula. Either there is syntax error or value is always undefined\n");
		return 1;
	}

	dump(F);

	evaluator = mode ? trap : simpson;
	double v = evaluator(F, N, a, b);

	if(isnanl(v))
	{
		fprintf(stderr, "Can't evaluate this integral: function is undefined somewhere at [a; b]\n");
		return 1;
	}

	printf("evaluated: $[ %s ]|%.3lf_%.3lf ~= %.3lf\n", code, a, b, v);
	return 0;
}
