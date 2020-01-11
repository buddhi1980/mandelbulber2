/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Fractal formula created by Buddhi
 */

#include "all_fractal_definitions.h"

cFractalMandelbulb3::cFractalMandelbulb3() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb 3";
	internalName = "mandelbulb3";
	internalID = fractal::mandelbulb3;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulb3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	aux.DE = aux.DE * 2.0 * aux.r;

	double temp, tempR;

	double sign = 1.0;
	double sign2 = 1.0;

	if (z.x < 0.0) sign2 = -1.0;
	tempR = sqrt(z.x * z.x + z.y * z.y); //+ 1e-061
	z *= 1.0 / tempR;
	temp = z.x * z.x - z.y * z.y;
	z.y = 2.0 * z.x * z.y;
	z.x = temp;
	z *= tempR;

	if (z.x < 0.0) sign = -1.0;
	tempR = sqrt(z.x * z.x + z.z * z.z); //+ 1e-061
	z *= 1.0 / tempR;
	temp = z.x * z.x - z.z * z.z;
	z.z = 2.0 * z.x * z.z * sign2;
	z.x = temp * sign;
	z *= tempR;

	z *= aux.r;
}
