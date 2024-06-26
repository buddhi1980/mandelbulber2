﻿/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * https://nylander.wordpress.com/2009/07/03/3d-mandelbrot-set-2/
 * Found on http://bugman123.com/Hypercomplex/index.html
 * Quadratic 3D Mandelbulb set, based on D. White's new "triplex squaring formula".
 * Hyperbolic radius added by Darkbeam refer Quadrat3D in M3D
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbQuadrat::cFractalMandelbulbQuadrat() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Quadrat";
	internalName = "mandelbulb_quadrat";
	internalID = fractal::mandelbulbQuadrat;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbQuadrat::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	if (fractal->transformCommon.functionEnabled)
	{
		aux.DE = aux.DE * 2.0 * z.Length() + 1.0;
		double temp = 0.0;
		if (fractal->transformCommon.functionEnabledDFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD1)
			temp = fractal->transformCommon.offset0;

		temp = z.x * z.x + z.y * z.y + temp;
		if (temp == 0.0) z = aux.const_c;
		else if (temp < 0.0) z = CVector4(0.0, 0.0, 0.0, 0.0);
		else
		{
			CVector4 Mul = fractal->transformCommon.constantMultiplier122;
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

		double temp = 0.0;
		if (fractal->transformCommon.functionEnabledTFalse
				&& aux.i >= fractal->transformCommon.startIterationsT
				&& aux.i < fractal->transformCommon.stopIterationsT1)
			temp = fractal->transformCommon.offsetA0;

		temp = z.z * z.z + z.y * z.y + temp;
		if (temp == 0.0) z = aux.const_c;
		else if (temp < 0.0) z = CVector4(0.0, 0.0, 0.0, 0.0);
		else
		{
			CVector4 Mul = fractal->transformCommon.constantMultiplier221;
			Mul.w = 0.0;
			double ZR = fractal->transformCommon.offsetA1;
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
