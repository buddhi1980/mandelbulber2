/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2019 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Mandelbulb fractal.
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbSinCos::cFractalMandelbulbSinCos() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb Sin Cos";
	internalName = "mandelbulb_sin_cos";
	internalID = fractal::mandelbulbSinCos;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbSinCos::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp, th, ph, rp, cth;
	if (fractal->transformCommon.functionEnabled
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		temp = fractal->transformCommon.pwr8 + 1.0;
		th = (asin(z.z / aux.r) + fractal->bulb.betaAngleOffset) * temp;
		ph = (atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset) * temp;
		rp = pow(aux.r, fractal->transformCommon.pwr8);
		aux.DE = rp * aux.DE * temp + 1.0;
		rp *= aux.r;
		cth = cos(th);
		z.x = cth * cos(ph) * rp;
		z.y = cth * sin(ph) * rp;
		z.z = sin(th) * rp;
		z += fractal->transformCommon.offsetA000;
		z += aux.const_c * fractal->transformCommon.constantMultiplierA111;
	}
	if (fractal->transformCommon.functionEnabledBFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		aux.r = z.Length();
		temp = fractal->transformCommon.scale8 + 1.0;
		th = (acos(z.z / aux.r) + fractal->transformCommon.offsetB0) * temp;
		ph = (atan(z.y / z.x) + fractal->transformCommon.offsetA0) * temp;
		rp = pow(aux.r, fractal->transformCommon.scale8);
		aux.DE = rp * aux.DE * temp + 1.0;
		rp *= aux.r;
		cth = cos(th);
		z.x = cth * cos(ph) * rp;
		z.y = cth * sin(ph) * rp;
		z.z = sin(th) * rp;
		z += fractal->transformCommon.offset000;
		z += aux.const_c * fractal->transformCommon.constantMultiplierB111;
	}
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}

	if (fractal->analyticDE.enabled)
	{
		aux.DE0 = z.Length();
		aux.DE0 = 0.5 * log(aux.DE0) * aux.DE0 / (aux.DE);
		if (aux.i >= fractal->transformCommon.startIterationsO
					&& aux.i < fractal->transformCommon.stopIterationsO)
			aux.dist = min(aux.dist, aux.DE0);
		else
			aux.dist = aux.DE0;
	}
}
