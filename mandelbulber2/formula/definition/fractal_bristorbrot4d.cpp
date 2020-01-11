/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Bristorbrot formula 4D
 * @reference https://code.google.com/archive/p/fractaldimension/
 * by Doug Bristor
 */

#include "all_fractal_definitions.h"

cFractalBristorbrot4d::cFractalBristorbrot4d() : cAbstractFractal()
{
	nameInComboBox = "Bristorbrot 4D";
	internalName = "bristorbrot4d";
	internalID = fractal::bristorbrot4d;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalBristorbrot4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r;
	double newx = z.x * z.x - z.y * z.y - z.z * z.z - z.w * z.w;
	double newy = z.y * (2.0 * z.x - z.z - z.w);
	double newz = z.z * (2.0 * z.x + z.y - z.w);
	double neww = z.w * (2.0 * z.x + z.y + z.z);
	z.x = newx;
	z.y = newy;
	z.z = newz;
	z.w = neww;

	z += fractal->transformCommon.additionConstant0000;
}
