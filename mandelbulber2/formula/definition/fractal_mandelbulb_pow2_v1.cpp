/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Non-trig mandelbulber pow2 version based on pow2 code from Davis Makin
 * ref: https://www.facebook.com/david.makin.7
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbPow2V1::cFractalMandelbulbPow2V1() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Pow2 V1";
	internalName = "mandelbulb_pow2_v1";
	internalID = fractal::mandelbulbPow2V1;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbPow2V1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	double m2 = z.y * z.y + z.x * z.x;
	aux.DE = aux.DE * 2.0 * sqrt(m2 + z.z * z.z) + 1.0;
	double temp = m2 - z.z * z.z;

	z.z = 2.0 * sqrt(m2) * z.z;
	if (fractal->transformCommon.functionEnabledAFalse && z.x < 0.0) z.z = -z.z;
	if (fractal->transformCommon.functionEnabledBFalse && z.z < 0.0) z.x = -z.x;

	m2 = temp / m2;
	temp = 2.0 * z.y * z.x * m2;
	z.y = (z.y * z.y - z.x * z.x) * m2;

	if (!fractal->transformCommon.functionEnabledCFalse)
		z.x = temp;
	else
		z.x = -temp;

	if (fractal->transformCommon.functionEnabledDFalse)
	{
		z.z *= aux.pos_neg;
		aux.pos_neg *= -1.0;
	}

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
