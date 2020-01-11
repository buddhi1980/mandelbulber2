/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * mandelbulb Kali modification
 * @reference http://www.fractalforums.com/theory/mandelbulb-variant/
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbKali::cFractalMandelbulbKali() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Kali";
	internalName = "mandelbulb_kali";
	internalID = fractal::mandelbulbKali;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbKali::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	double th0 =
		acos(z.z / aux.r) + fractal->bulb.betaAngleOffset + 1e-061; // MUST keep exception catch
	double ph0 = atan(z.y / z.x) + fractal->bulb.alphaAngleOffset;
	th0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleA1;
	double sinth = sin(th0);
	z = aux.r * CVector4(sinth * cos(ph0), sin(ph0) * sinth, cos(th0), 0.0);

	th0 = acos(z.z / aux.r) + fractal->transformCommon.betaAngleOffset
				+ 1e-061; // MUST keep exception catch ??;
	ph0 = atan(z.y / z.x);
	ph0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleB1;
	double zp = pow(aux.r, fractal->transformCommon.pwr8);
	sinth = sin(th0);
	z = zp * CVector4(sinth * cos(ph0), sin(ph0) * sinth, cos(th0), 0.0);

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		aux.DE = pow(aux.r, fractal->transformCommon.pwr8 - fractal->analyticDE.offset1) * aux.DE
							 * fractal->transformCommon.pwr8 * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset2;
	}
	else // default, i.e. scale1 & offset1 & offset2 = 1.0
	{
		aux.DE =
			pow(aux.r, fractal->transformCommon.pwr8 - 1.0) * fractal->transformCommon.pwr8 * aux.DE
			+ 1.0;
	}
}
