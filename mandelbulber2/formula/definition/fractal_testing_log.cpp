/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * https://nylander.wordpress.com/2009/07/03/3d-mandelbrot-set-2/
 */

#include "all_fractal_definitions.h"

cFractalTestingLog::cFractalTestingLog() : cAbstractFractal()
{
	nameInComboBox = "Testing Log";
	internalName = "testing_log";
	internalID = fractal::testingLog;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingLog::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	CVector4 Mul;

	if (fractal->transformCommon.functionEnabled)
	{
		aux.DE = aux.DE * 2.0 * z.Length() + 1.0;

		double temp = z.x * z.x + z.y * z.y + fractal->transformCommon.offset0;
		if (temp == 0.0) z = aux.const_c;
		else if (temp < 0.0) z = CVector4(0.0, 0.0, 0.0, 0.0);
		else
		{
			Mul = fractal->transformCommon.constantMultiplier122;
			Mul.w = 0.0;
			double ZR = fractal->transformCommon.offset1;
			Mul.z = -Mul.z * z.z * sqrt(temp);
			temp = ZR - z.z * z.z / temp;
			Mul.x = Mul.x * (z.x * z.x - z.y * z.y) * temp;
			Mul.y = Mul.y * z.x * z.y * temp;
			z = Mul;

			// offset (Julia)
			z += fractal->transformCommon.additionConstant000;
		}
	}
	if (fractal->transformCommon.functionEnabledFalse)
	{
		aux.DE = aux.DE * 2.0 * z.Length() + 1.0;

		double temp = z.z * z.z + z.y * z.y + fractal->transformCommon.offsetA0;
		if (temp == 0.0) z = aux.const_c;
		else if (temp < 0.0) z = CVector4(0.0, 0.0, 0.0, 0.0);
		else
		{
			Mul = fractal->transformCommon.constantMultiplier221;
			Mul.w = 0.0;
			double ZR = fractal->transformCommon.offset1;
			Mul.x = -Mul.x * z.x * sqrt(temp);
			temp = ZR - z.x * z.x / temp;
			Mul.z = Mul.z * (z.z * z.z - z.y * z.y) * temp;
			Mul.y = Mul.y * z.z * z.y * temp;
			z = Mul;

			// offset (Julia)
			z += fractal->transformCommon.additionConstantA000;
		}
	}



	z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	 // DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
