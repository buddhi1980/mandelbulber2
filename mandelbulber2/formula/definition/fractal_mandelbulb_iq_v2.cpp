/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * trig pow based on code by Inigo Quilez
 * @reference https://www.iquilezles.org/www/articles/mandelbulb/mandelbulb.htm
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbIqV2::cFractalMandelbulbIqV2() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb IQ V2";
	internalName = "mandelbulb_iq_v2";
	internalID = fractal::mandelbulbIqV2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbIqV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double Pow = fractal->bulb.power;
	// extract polar coordinates
	double wr = aux.r;
	double wi = atan2(z.x, z.y);
	double wo = z.z / aux.r;

	if (!fractal->transformCommon.functionEnabledAFalse)
	{
		if (!fractal->transformCommon.functionEnabledFalse)
			wo = acos(wo);
		else
			wo = asin(wo);
	}
	else
	{
		wo = acos(wo) * (1.0 - fractal->transformCommon.scale0)
				+ asin(wo) * fractal->transformCommon.scale0;
	}

	// scale and rotate the point
	wr = pow(aux.r, Pow);
	wo = wo * Pow * fractal->transformCommon.scaleB1;
	wi = wi * Pow * fractal->transformCommon.scaleA1;

	// convert back to cartesian coordinates
	double swo = sin(wo);
	z.x = swo * sin(wi);
	z.z = cos(wo);
	z.y = swo * cos(wi);
	z *= wr;

	aux.DE = pow(aux.r, Pow - 1.0) * aux.DE * Pow + 1.0;
	z.z *= fractal->transformCommon.scale1;
	z += fractal->transformCommon.offset000;
}
