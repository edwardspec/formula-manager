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
#include <ctype.h>
#include <time.h>

#include "symtable.h"

int main()
{
	int rand_number;
	int i; symtable t;
	char rand_char[2];
	rand_char[1] = '\0';

	srand(time(NULL));

	rand_number = 5 + rand() % 5;

	if(strcmp(symtable_method, "bitmask"))
	{
		printf("This is a test for 'bitmask' symtable, but '%s' symtable is compiled in.\nNo tests were run.\n", symtable_method);
		return 0;
	}

	printf("Running symtable test: %s (%scase-sensitive)\n", symtable_method, symtable_case_sensitive ? "" : "NOT ");

	t = symtable_new();
	if(!t)
	{
		perror(symerror);
		return 1;
	}

	printf("Allocated ok. Going to make %i symtable_add() calls...\n", rand_number);
	for(i = 0; i < rand_number; i ++)
	{
		rand_char[0] = 'A' + rand() % 25;
		if(!symtable_case_sensitive)
			if(rand() % 2) rand_char[0] = tolower(rand_char[0]);

		printf("Adding '%c'... ", rand_char[0]);
		if(!symtable_add(t, rand_char))
		{
			printf("\n");
			perror(symerror);
			return 1;
		}
		printf("Ok, '%c' is now registered. Count = %i\n", rand_char[0], symtable_count(t));
	}
	printf("All symbols have been registered in symtable. Count = %i\n", symtable_count(t));

	printf("Now please enter characters (latin letters) for manual testing.\nNot a latin letter = EXIT.\n");
	while(1)
	{
		rand_char[0] = getc(stdin);
		if(rand_char[0] == '\n') continue;

		if((symtable_case_sensitive || rand_char[0] < 'a' || rand_char[0] > 'z') && (rand_char[0] < 'A' || rand_char[0] > 'Z'))
		{
			printf("Exiting (not [A-Z%s] entered).\n", symtable_case_sensitive ? "" : "a-z");
			return 0;
		}

		printf("symtable_isset(): '%c' is%s set.\n", rand_char[0], (symtable_isset(t, rand_char) ? "" : " NOT"));
	}

	return 0;
}
