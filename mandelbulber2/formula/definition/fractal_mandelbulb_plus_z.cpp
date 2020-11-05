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

cFractalMandelbulbPlusZ::cFractalMandelbulbPlusZ() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb Plus Z";
	internalName = "mandelbulb_plus_z";
	internalID = fractal::mandelbulbPlusZ;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbPlusZ::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zTmp = CVector4(0.0, 0.0, 0.0, 0.0);
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	if (aux.i == fractal->transformCommon.startIterations) aux.c = zTmp;

	if (aux.i >= fractal->transformCommon.startIterationsA) zTmp = z;

	double theta = (asin(z.z / aux.r) + fractal->bulb.betaAngleOffset) * fractal->transformCommon.int2;
	double phi = (atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset) * fractal->transformCommon.int2;
	double rp = pow(aux.r, fractal->transformCommon.int2 - 1.0);
	aux.DE = rp * aux.DE * fractal->transformCommon.int2 + 1.0;

	if (!fractal->transformCommon.functionEnabledBxFalse)
		rp = pow(aux.r, fractal->transformCommon.int2);
	else
		rp *= aux.r;

	if (!fractal->transformCommon.functionEnabledByFalse)
	{
		double sinth = sin(theta);
		z = rp * CVector4(sinth * cos(phi), sin(phi) * sinth, cos(theta), 0.0);
	}
	else
	{
		double costh = cos(theta);
		z = rp * CVector4(costh * sin(phi), cos(phi) * costh, sin(theta), 0.0);
	}
	z += aux.c * fractal->transformCommon.constantMultiplierC111;

	aux.c = zTmp;

	// offset or juliaC
	if (aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		z += fractal->transformCommon.offset000;
	}

	// z.z scale
	z.z *= fractal->transformCommon.scale1;

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
