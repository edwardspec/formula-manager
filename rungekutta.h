/*
	Formula manager - the mathematical library.
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

#ifndef _RUNGEKUTTA_H
#define _RUNGEKUTTA_H

#include "formula.h"

/**
	@brief Solve a differential equation
		Y' = F(X, Y) with Y(X0) = Y0, where X0 and Y0 are given.

	@param F Formula F(X, Y) in the right part of the equation.
	@param X0 Some value of X.
	@param Y0 Value of Y(X) in point \b X0.
	@returns Formula object that contains Y(X) function.

	@note The formula returned is independent from its first parameter
		(so the F can be later freed, and Y will continue to be
		operational).
	@note The formula returned must be formula_free()d.
*/
formula rungekutta_solve(formula F, double X0, double Y0);

#endif
