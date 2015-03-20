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
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define _FORMULA_C
#include "formula_internal.h"

#include "integral.h"
#include "symtable.h"

const char *action_descriptions[] = {
	"const",
	"var",
	"not",
	"add",
	"sub",
	"multi",
	"divide",
	"sin",
	"cos",
	"tan",
	"ctan",
	"degrees-to-radians",
	"integral",
	"arcsin",
	"arccos",
	"arctg",
	"power",
	"exp",
	"ln",
	"log10",
	"log2",
	"derivative",
	"abs"
};
const int action_descriptions_last = sizeof(action_descriptions) / sizeof(char *) - 1;

#define ACTION_DESCRIPTION(code) (code < 0 || code > action_descriptions_last) ? "unknown" : action_descriptions[code]

static double _simpson_eval(formula F, const double *args, int steps) __attribute__((fastcall nonnull(1,2) const));
static double _derivative_eval(formula F, const double *args, double offset) __attribute__((fastcall nonnull(1,2) const));

char *_formula_string_p = ""; /* used in lex_rules.l */
formula _formula_top = NULL; /* used in lex_rules.l */
symtable _symtable_top = NULL; /* used in _alloc4 to fill in F->args */

void yyparse();
static void _formula_free(formula F);

static int _init_symtable_top()
{
	_symtable_top = symtable_new();
	if(!_symtable_top)
	{
		perror("Not enough memory.\n");
		return 0;
	}
	return 1;
}
static void _free_symtable_top()
{
	free(_symtable_top);
	_symtable_top = NULL;
}

/* Compile formula and return pointer for future use by eval() */
formula parse(const char *code)
{ /* This memory MUST be free()d by application */
	if(!code) return NULL;
	_formula_string_p = (char *) code;

	_formula_top = NULL;
	if(!_init_symtable_top())
		return NULL;

	yyparse();
	if(!_formula_top)
	{
		_free_symtable_top(_symtable_top);
		return NULL;
	}

	symtable_import(_symtable_top, _formula_top->vars);
	_symtable_top = NULL;
	return _formula_top;
}

/* Apply an operation to two constants */
static double _calc(F_TYPE action, double p1, double p2)
{
	double t;
	switch(action)
	{
		case F_NOT: return -p1;
		case F_ADD: return p1 + p2;
		case F_SUB: return p1 - p2;
		case F_MUL: return p1 * p2;
		case F_DIV: return p2 ? (p1 / p2) : NAN;
		case F_POW: return pow(p1, p2);
		case F_EXP: return exp(p1);
		case F_SIN: return sin(p1);
		case F_COS: return cos(p1);
		case F_TAN: return tan(p1);
		case F_CTG: t = tan(p1); return t ? (1 / t) : NAN;
		case F_D2R: return p1 * 3.14 / 180;
		case F_ASIN: return asin(p1);
		case F_ACOS: return acos(p1);
		case F_ATAN: return atan(p1);
		case F_LN : return log(p1);
		case F_LG : return log10(p1);
		case F_LOG2 : return log2(p1);
		case F_ABS: return fabs(p1);
	}
	return NAN;
}

/* F parameter MUST be F_CONST, or this call will fail */
inline static double _get_const(formula F)
{
	return *(double *)F->arg1;
}

__attribute__((fastcall)) YYSTYPE _palloc4(mpool pool, F_TYPE type, YYSTYPE arg1, YYSTYPE arg2,  YYSTYPE arg3, YYSTYPE arg4)
{
	formula F;
#ifdef DEBUG
	printf("DEBUG: alloc %i: (%p, %p, %p, %p)\n", type, arg1, arg2, arg3, arg4);
#endif

	if(arg1 == NULL)
	{
#ifdef DEBUG
		printf("DEBUG: return NULL\n");
#endif
		return NULL;
	}

	/* Optimization:
		if we have arg1=CONST1, arg2=CONST2, then we
		1) move (CONST1 <type> CONST2) to arg1,
		2) free arg2
		3) return arg1

		NOTE: this won't convert "x + 7 + 8" into "x + 15", because "x + 7" operation
		is being processed first; however, "x + (7 + 8)" and "7 + 8 + x" will be optimized.
	*/
	if(pool) goto optimization_disabled;
	if(type != F_CONST && type != F_VAR && type != F_INTEGRAL)
	{
		if(arg1->action == F_CONST && (!arg2 || arg2->action == F_CONST))
		{
			double v = _calc(type, _get_const(arg1), arg2 ? _get_const(arg2) : 0);
			if(arg2) _formula_free(arg2);
			free(arg1->arg1);

			if(!isnanl(v))
			{
				arg1->action = F_CONST;

				double *mem = malloc(sizeof(double));
				*mem = v;
				arg1->arg1 = (formula) mem;
				symtable_clear(arg1->vars);

#ifdef DEBUG
				printf("DEBUG: return %p\n", arg1);
#endif
				return arg1;
			}
			free(arg1);

#ifdef DEBUG
			printf("DEBUG: return NULL\n");
#endif
			return NULL; /* This formula can never be calculated */
		}
	}

optimization_disabled:
	/* Can't optimize, allocate a new structure */
	F = malloc(sizeof(struct _formula));
	if(F)
	{
		F->action = type;
		F->arg1 = arg1;
		F->arg2 = arg2;

		if(arg3)
		{
			F->other_args = (struct _other_args *)
				malloc(sizeof( struct _other_args));

			F->other_args->count = arg4 ? 2 : 1;
			F->other_args->arg = malloc(sizeof(void *) * F->other_args->count);

			F->other_args->arg[0] = arg3;
			F->other_args->arg[1] = arg4;
		}
		else
			F->other_args = NULL;

		F->args = _symtable_top;
		F->vars = symtable_new();

		if(F->action != F_CONST)
		{
			if(F->action == F_VAR)
			{
				char newvar[2];
				newvar[0] = (int) F->arg1;
				newvar[1] = '\0';
				symtable_add(F->vars, newvar);
			}
			else
			{
				symtable_import(F->vars, arg1->vars);
				if(arg2) symtable_import(F->vars, arg2->vars);
				if(arg3) symtable_import(F->vars, arg3->vars);
				if(arg4) symtable_import(F->vars, arg4->vars);

				if(F->action == F_INTEGRAL)
				{
					/*
						Delete unneeded variable from F->vars.

						E.g in $[2+A+B]dA|1_2 variable A is not needed.
					*/
					char oldvar[2];
					oldvar[0] = (int) arg4->arg1;
					oldvar[1] = '\0';

#ifdef DEBUG
					printf("DEBUG: do not need '%s'\n", oldvar);
#endif
					symtable_del(F->vars, oldvar);
				}
			}
		}
	}

#ifdef DEBUG
	printf("DEBUG: return %p\n", F);
#endif
	return F;
}

formula _formula_clone(const formula F, const symtable args)
{
	formula N = malloc(sizeof(struct _formula));
	memcpy(N, F, sizeof(struct _formula));

	if(F->action == F_CONST)
	{
		double *nr = malloc(sizeof(double));
		*nr = *(double *) F->arg1;
		N->arg1 = (formula) nr;
	}
	else if(F->action != F_VAR)
	{
		N->arg1 = _formula_clone(F->arg1, args);
		if(F->arg2) N->arg2 = _formula_clone(F->arg2, args);
		if(F->other_args)
		{
			N->other_args->arg[0] = _formula_clone(F->other_args->arg[0], args);
			if(F->other_args->count > 1)
				N->other_args->arg[1] = _formula_clone(F->other_args->arg[1], args);
		}
	}

	N->vars = symtable_clone(F->vars);
	N->args = args;

	return N;
}
formula formula_clone(const formula F)
{
	if(!F) return NULL;
	return _formula_clone(F, symtable_clone(F->args));
}

static void _dump(const formula F, int howdeep)
{
	int i;

	for(i = 0; i < howdeep; i ++) printf("\t");
	printf("action %i [%s]: ", F->action, ACTION_DESCRIPTION(F->action));
	if(F->action == F_VAR)
	{
		char *vn = symtable_varname(F);
		printf(" <%s>\n", vn ? vn : "?");
		if(vn) free(vn);
	}
	else if(F->action == F_CONST)
		printf("%.2f\n", _get_const(F));
	else
	{
		char *vars_dump = symtable_print(F->vars);
		if(vars_dump)
		{
			printf("%s", vars_dump);
			free(vars_dump);
		}

		printf("\n");
		_dump(F->arg1, howdeep + 1);
		if(F->arg2) _dump(F->arg2, howdeep + 1);
		if(F->other_args)
		{
			_dump(F->other_args->arg[0], howdeep + 1);
			if(F->other_args->count > 1)
			{
				_dump(F->other_args->arg[1], howdeep + 1);
			}
		}
	}
}

void dump(const formula F)
{
	if(F) {
		int vars_count;

		_dump(F, 1);

		vars_count = symtable_count(F->vars);
		printf("Dump ok! %i var%s found.\n", vars_count,
			vars_count == 1 ? "" : "s");
	}
}

static void _formula_free(formula F)
{
	symtable_free(F->vars);
	if(F->action == F_CONST)
	{
		free(F->arg1);
	}
	else if(F->action != F_VAR)
	{
		_formula_free(F->arg1);
		if(F->arg2)
		{
			_formula_free(F->arg2);
			if(F->other_args)
			{
				_formula_free(F->other_args->arg[0]);
				if(F->other_args->count > 1)
				{
					_formula_free(F->other_args->arg[1]);
				}
			}
		}
	}

	free(F);
}
void formula_free(formula F)
{
	if(F)
	{
		symtable args_top = F->args;
		_formula_free(F);
		symtable_free(args_top);
	}
}

int formula_args(const formula F)
{
	return symtable_count(F->args);
}

__attribute__((fastcall)) static double _eval(const formula F, const double *args)
{
	double p1 = 0, p2 = 0; // Never needed, that's just to avoid compiler warnings

	if(F->action == F_CONST)
		return _get_const(F);
	else if(F->action == F_VAR)
	{
		int var_idx = symtable_order(F);
		return args[var_idx];
	}
	else if(F->action == F_INTEGRAL)
	{
		return _simpson_eval(F, args, 100);
	}
	else if(F->action == F_DERIVATIVE)
	{
		return _derivative_eval(F, args, 0.01);
	}

	/* Operations */
	p1 = _eval(F->arg1, args);
	if(isnanl(p1)) return NAN;

	if(F->arg2) /* Binary operations */
	{
		p2 = _eval(F->arg2, args);
		if(isnanl(p2)) return NAN;
	}

//	printf("_calc returned %lf [action = %i, arg1 = %lf, arg2 = %lf]\n", _calc(F->action, p1, p2), F->action, p1, p2);
	return _calc(F->action, p1, p2);
}

double eval(const formula F, ...)
{
	double *args = NULL;
 	int count;
	if(!F) return NAN;

	count = symtable_count(F->vars);
	if(count)
	{
		int i;
		args = malloc(sizeof(double) * count);
		if(!args) return NAN;

		va_list params;
		va_start(params, F);
		for(i = 0; i < count; i ++)
		{
			args[i] = va_arg(params, double);
		}
		va_end(params);
	}

//	return _eval(F, args);

	double d = _eval(F, args);
//	printf("eval() done. symtable_count(F->vars)=%i, symtable_count(F->args)=%i\n", symtable_count(F->vars), symtable_count(F->args));
	return d;
}
double eval_array(formula F, const double *args)
{
	if(!F) return NAN;
	return _eval(F, args);
}

/*
	NOTE: F is NOT a formula under integral.
	It is the integral itself!
*/
__attribute__((fastcall)) static double _simpson_eval(formula F, const double *args, int steps)
{
	if(!F->other_args || F->other_args->count != 2) return NAN;

	int args_count = symtable_count(F->args);
	size_t args_size = (args_count + 1) * sizeof(double);
	double *args_copy = malloc(args_size);
	if(!args_copy) return NAN;
	memcpy(args_copy, args, args_size - sizeof(double));

	formula expr = F->arg1;
	double a = _eval(F->arg2, args);
	double b = _eval(F->other_args->arg[0], args);
	char variable[2];
	variable[0] = (int) F->other_args->arg[1]->arg1;
	variable[1] = '\0';
	int var_order_in_args = symtable_order_raw(F->args, variable);

	/* Deal with infinite values */
	int a_isinf = isinf(a), b_isinf = isinf(b);
	if(a_isinf != 0 || b_isinf != 0)
	{
		if(a_isinf != 0) a = 200 * a_isinf;
		if(b_isinf != 0) b = 200 * b_isinf;
		steps *= 100;
	}

	/* Clear a space in 'args_copy' table for main integral variable */
	int i;
	for(i = args_count; i > var_order_in_args; i --)
	{ /* args_copy[var_order_in_args] will be filled with main integral variable */
		args_copy[i] = args_copy[i - 1];
	}

//	printf("Integral by variable %s! №%i in args\n", variable, var_order_in_args);

	int swap = 1;
	if(a > b)
	{
		int t = a;
		a = b;
		b = t;
		swap = -1;
	}

	double step = (b - a) / steps;
	double x, I;
	int two_or_four = 4;

	/* Temporarily: symtable_del() is being called when everything is done */
	symtable_add(F->args, variable);

	args_copy[var_order_in_args] = a;
	I = _eval(expr, args_copy);
//	printf("F(%.2lf) = %.4lf\n", a, _eval(expr, args_copy));
	args_copy[var_order_in_args] = b;
	I += _eval(expr, args_copy);
//	printf("F(%.2lf) = %.4lf\n", b, _eval(expr, args_copy));

	for(x = a + step; x < b; x += step)
	{
		args_copy[var_order_in_args] = x;
//		printf("args_copy[] = [ %lf, %lf ]\n", args_copy[0], args_copy[1]);
//		printf("F(%.2lf) = %.4lf\n", x, _eval(expr, args_copy));

		I += _eval(expr, args_copy) * two_or_four;
		two_or_four = 6 - two_or_four;
	}

	/* Cleanup */
	symtable_del(F->args, variable); /* F must not be modified by eval() call, so let's restore it's state */
	free(args_copy);

	return swap * step * I / 3;
}

/*
	NOTE: F is NOT a formula under derivative.
	It is the derivative itself!
*/
__attribute__((fastcall)) static double _derivative_eval(formula F, const double *args, double offset)
{
	formula expr = F->arg1;
	formula by = F->arg2;

	if(by->action != F_VAR)
	{
		fprintf(stderr, "Can't calculate df(x)/dg(x) yet, g(x) must be a variable (e.g. df(x)/dx)\n");
		return NAN;
	}

	double *ar_p = (double *) args;

	int var_idx = symtable_order(by);

	double a, b;
	ar_p[var_idx] -= offset;
	a = _eval(expr, ar_p);
	ar_p[var_idx] += 2 * offset;
	b = _eval(expr, ar_p);
	ar_p[var_idx] -= offset;

	return (b-a) / (2*offset);
}

void upgrade_derivative(formula *Fp, const char *by)
{
	formula by_f = _alloc1(F_VAR, (formula) (int) by[0]);
	by_f->args = (*Fp)->args;
	upgrade(F_DERIVATIVE, Fp, &by_f);
}
void upgrade(int action, ...)
{
	formula *P[4];
	int count;
	switch(action) {
		case F_NOT:
		case F_EXP:
		case F_SIN:
		case F_COS:
		case F_TAN:
		case F_CTG:
		case F_D2R:
		case F_ASIN:
		case F_ACOS:
		case F_ATAN:
		case F_LN:
		case F_LG:
		case F_LOG2:
			count = 1;
			break;
		case F_ADD:
		case F_SUB:
		case F_MUL:
		case F_DIV:
		case F_POW:
		case F_DERIVATIVE:
			count = 2;
			break;

		case F_CONST:
		case F_VAR:
			fprintf(stderr, "CONST and VAR actions can't be used in upgrade()");
			exit(1);

		case F_INTEGRAL:
			count = 4;
			break;

		default:
			fprintf(stderr, "Unknown action %i as upgrade() parameter", action);
			exit(1);
	}

	va_list params; int i;
	va_start(params, action);
	for(i = 0; i < count; i ++)
	{
		P[i] = va_arg(params, formula *);
	}
	va_end(params);

	formula ret = _alloc4(action, *P[0],
		count > 1 ? *P[1] : NULL,
		count > 2 ? *P[2] : NULL,
		count > 3 ? *P[3] : NULL
	);

	symtable_import((*P[0])->args, ret->vars);
	ret->args = (*P[0])->args;

	*P[0] = ret;
	if(count > 1)
	{
		*P[1] = NULL;
		if(count > 2)
		{
			*P[2] = NULL;
			if(count > 3)
				*P[3] = NULL;
		}
	}
}

static void _reduce(formula F, const char *var, double val)
{
	if(F->action == F_VAR)
	{
		if(symtable_isset(F->vars, var))
		{
			symtable_clear(F->vars);

			double *nr = malloc(sizeof(double));
			*nr = val;

			F->action = F_CONST;
			F->arg1 = (formula) nr;
		}
	}
	else if(F->action != F_CONST)
	{
		_reduce(F->arg1, var, val);
		if(F->arg2)
		{
			_reduce(F->arg2, var, val);
			if(F->other_args)
			{
				_reduce(F->other_args->arg[0], var, val);
				if(F->other_args->count > 1)
				{
					_reduce(F->other_args->arg[1], var, val);
				}
			}
		}
		symtable_del(F->vars, var);
	}
}
void reduce(formula F, const char *var, double val)
{
	if(F) {
		_reduce(F, var, val);
		symtable_del(F->args, var);
	}
}
