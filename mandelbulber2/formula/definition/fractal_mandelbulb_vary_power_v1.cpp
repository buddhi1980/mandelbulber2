/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * mandelbulb vary scaleV1
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbVaryPowerV1::cFractalMandelbulbVaryPowerV1() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Vary Power V1";
	internalName = "mandelbulb_vary_power_v1";
	internalID = fractal::mandelbulbVaryPowerV1;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbVaryPowerV1::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double tempVC = fractal->bulb.power; // constant to be varied

	if (aux.i >= fractal->transformCommon.startIterations250
			&& aux.i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					!= 0))
	{
		int iterationRange =
			fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250;
		int currentIteration = (aux.i - fractal->transformCommon.startIterations250);
		tempVC += fractal->transformCommon.offset0 * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset0);
	}

	// if (aux.r < 1e-21)
	//	aux.r = 1e-21;
	double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, tempVC - 1.0);
	double th = th0 * tempVC;
	double ph = ph0 * tempVC;
	double cth = cos(th);
	aux.DE = rp * aux.DE * tempVC + 1.0;
	rp *= aux.r;
	z = CVector4(cth * cos(ph), cth * sin(ph), sin(th), 0.0) * rp;
}
