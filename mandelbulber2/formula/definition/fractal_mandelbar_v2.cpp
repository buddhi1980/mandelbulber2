/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Mandelbar or Tricorn version based on pow2 code from Davis Makin
 * ref: http://www.fractalgallery.co.uk/ and https://www.facebook.com/david.makin.7
 */

#include "all_fractal_definitions.h"

cFractalMandelbarV2::cFractalMandelbarV2() : cAbstractFractal()
{
	nameInComboBox = "Mandelbar V2";
	internalName = "mandelbar_v2";
	internalID = fractal::mandelbarV2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbarV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	double m2 = z.x * z.x + z.y * z.y;
	aux.DE = aux.DE * 2.0 * sqrt(m2 + z.z * z.z) + 1.0;
	if (m2 == 0.0)
	{
		z.y = -z.z * z.z;
		z.z = 0.0;
	}
	else
	{
		double temp = m2 - z.z * z.z;
		z.z = 2.0 * sqrt(m2) * z.z;
		m2 = temp / m2;
		temp = 2.0 * z.x * z.y * m2;

		z.y = (z.y * z.y - z.x * z.x) * m2;
		z.x = -temp;
	}

	// offset (Julia)
	z += fractal->transformCommon.additionConstant000;

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
