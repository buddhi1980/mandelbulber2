/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Non trig pow 8 based on code by Inigo Quilez
 * @reference https://www.iquilezles.org/www/articles/mandelbulb/mandelbulb.htm
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbIqPow8::cFractalMandelbulbIqPow8() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb IQ Pow8";
	internalName = "mandelbulb_iq_pow8";
	internalID = fractal::mandelbulbIqPow8;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbIqPow8::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
		double tp = z.Dot(z);
		tp = tp * tp * tp * aux.r;
		aux.DE = tp * aux.DE * 8.0 + 1.0;

		CVector4 z2 = z * z;
		CVector4 z4 = z2 * z2;
		double k3 = z2.x + z2.y;
		tp = k3 * k3;
		tp = tp * tp * tp * k3;
		double k2 = fractal->transformCommon.scaleA1 / sqrt(tp);
		aux.DE *= fractal->transformCommon.scaleA1;
		tp = z2.x * z2.y;
		double k1 = z4.x + z4.y + z4.z - 6.0f * z2.z * k3 + 2.0f * tp;
		double k4 = k3 - z2.z;
		double k12 = k1 * k2;

		z.x = fractal->transformCommon.scale1 * 64.0 * z.x * z.y * z.z * k4 * (z2.x - z2.y) * (z4.x - 6.0 * tp + z4.y) * k12;
		z.y = -8.0 * z.z * k4 * (z4.x * z4.x - 28.0 * tp * (z4.x + z4.y) + 70.0 * z4.x * z4.y + z4.y * z4.y) * k12;
		z.z = -16.0 * z2.z * k3 * k4 * k4 + k1 * k1;
		z.z *= fractal->transformCommon.scaleB1;
		z += fractal->transformCommon.offset000;
}
