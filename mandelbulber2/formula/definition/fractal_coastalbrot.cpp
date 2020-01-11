/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Fractal formula created by Robert Pancoast
 */

#include "all_fractal_definitions.h"

cFractalCoastalbrot::cFractalCoastalbrot() : cAbstractFractal()
{
	nameInComboBox = "Coastalbrot";
	internalName = "coastalbrot";
	internalID = fractal::coastalbrot;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalCoastalbrot::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	double temp = aux.r;
	temp = pow(temp, 7.7);
	aux.DE = temp * aux.DE * 7.7;
	temp *= aux.r;

	z.x = sin(sin(sin(M_PI / 3 + z.x * M_PI)));
	z.y = sin(sin(sin(M_PI / 3 + z.y * M_PI)));
	z.z = sin(sin(sin(M_PI / 3 + z.z * M_PI)));

	z *= temp;
}
