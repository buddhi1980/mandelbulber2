/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2019 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Classic Mandelbulb fractal.
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbSinCos::cFractalMandelbulbSinCos() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb Sin Cos";
	internalName = "mandelbulb_sin_cos";
	internalID = fractal::mandelbulbSinCos;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbSinCos::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp;
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		temp = fractal->transformCommon.pwr8 + 1.0;
		const double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
		const double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
		double rp = pow(aux.r, fractal->transformCommon.pwr8);
		const double th = th0 * temp;
		const double ph = ph0 * temp;
		const double cth = cos(th);
		aux.DE = rp * aux.DE * temp + 1.0;
		rp *= aux.r;
		z.x = cth * cos(ph) * rp;
		z.y = cth * sin(ph) * rp;
		z.z = sin(th) * rp;

		z += aux.const_c;
	}
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		aux.r = z.Length();
		temp = fractal->transformCommon.scale8 + 1.0;
		const double th0 = acos(z.z / aux.r) + fractal->transformCommon.offsetB0;
		const double ph0 = atan(z.y / z.x) + fractal->transformCommon.offsetA0 ;
		double rp = pow(aux.r, fractal->transformCommon.scale8);
		const double th = th0 * temp;
		const double ph = ph0 * temp;
		const double sth = sin(th);
		aux.DE = rp * aux.DE * temp + 1.0;
		rp *= aux.r;
		z.x = sth * sin(ph) * rp;
		z.y = sth * cos(ph) * rp;
		z.z = cos(th) * rp;

		z += aux.const_c;
	}
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}

	if (fractal->transformCommon.functionEnabledOFalse)
	{
		aux.DE0 = z.Length();

		aux.DE0 = 0.5 * log(aux.DE0) * aux.DE0 / (aux.DE);
		if (!fractal->transformCommon.functionEnabledYFalse)
			aux.dist = aux.DE0;
		else
			aux.dist = min(aux.dist, aux.DE0);
	}

}
