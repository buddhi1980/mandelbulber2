/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * IQ-Bulb from Mandelbulb 3D and Inigo Quilez
 * @reference http://iquilezles.org/www/articles/mandelbulb/mandelbulb.htm
 */

#include "all_fractal_definitions.h"

cFractalIqBulb::cFractalIqBulb() : cAbstractFractal()
{
	nameInComboBox = "IQ Bulb";
	internalName = "iq_bulb";
	internalID = fractal::iqBulb;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalIqBulb::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// extract polar coordinates
	double wr = aux.r;
	double wo = acos(z.y / wr);
	double wi = atan2(z.x, z.z);

	// scale and rotate the point
	wr = pow(wr, fractal->transformCommon.pwr8 - 1.0);
	aux.DE = wr * aux.DE * fractal->transformCommon.pwr8 + 1.0;
	wr *= aux.r;
	wo *= fractal->transformCommon.pwr8;
	wi *= fractal->transformCommon.pwr8a;

	// convert back to cartesian coordinates
	z.x = sin(wo) * sin(wi);
	z.y = cos(wo);
	z.z = sin(wo) * cos(wi);

	z *= wr; // then add Cpixel constant
}
