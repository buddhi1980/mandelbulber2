/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * CalculateDistance function - calculates approximate distance to fractal surface
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */


#ifndef CALCULATE_DISTANCE_HPP_
#define CALCULATE_DISTANCE_HPP_

#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "four_fractals.hpp"

struct sDistanceIn
{
	CVector3 point;
	double detailSize;
	bool normalCalculationMode;
	sDistanceIn(CVector3 _point, double _detailSize, bool _normalCalculationMode) :
			point(_point), detailSize(_detailSize), normalCalculationMode(_normalCalculationMode) {};
};

struct sDistanceOut
{
	double distance;
	double colorIndex;
	int iters;
	int totalIters;
	fractal::enumObjectType object;
	int formulaIndex;
	double objectReflect;
	sRGB objectColor;
	bool maxiter;
};

double CalculateDistance(const cParamRender &params, const cFourFractals &four, const sDistanceIn &in, sDistanceOut *out);
double CalculateDistanceSimple(const cParamRender &params, const cFourFractals &four, const sDistanceIn &in, sDistanceOut *out, int forcedFormulaIndex);

#endif /* CALCULATE_DISTANCE_HPP_ */
