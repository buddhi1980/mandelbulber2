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
	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (fractal->transformCommon.functionEnabledCxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledCyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledCzFalse) z.z = fabs(z.z);
	}

	if (fractal->transformCommon.functionEnabledAy
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		double m2 = z.x * z.x + z.z * z.z;
		aux.DE = aux.DE * 2.0 * sqrt(m2 + z.y * z.y) + 1.0;
		if (m2 == 0.0f)
		{
			z.x = -z.y * z.y;
			z.y = 0.0f;
		}
		else
		{
			double temp = -(m2 - z.y * z.y);

			z.y = 2.0 * sqrt(m2) * z.y;
			//if (fractal->transformCommon.functionEnabledAFalse && z.y < 0.0) z.x = -z.x;
			//if (fractal->transformCommon.functionEnabledBFalse && z.x < 0.0) z.y = -z.y;
			m2 = temp / m2;
			temp = 2.0 * z.x * z.z * m2;
			z.z = (z.z * z.z - z.x * z.x) * m2;
			if (!fractal->transformCommon.functionEnabledBFalse)
				z.x = temp;
			else
				z.x = -temp;
			if (fractal->transformCommon.functionEnabledDFalse)
			{
				if (fractal->transformCommon.functionEnabledCFalse) aux.pos_neg *= -1.0;
				z.y *= aux.pos_neg;

				aux.pos_neg *= -1.0f;
			}
		}
	}

	if (fractal->transformCommon.functionEnabledAzFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		double m2 = z.y * z.y + z.x * z.x;
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
		//	if (fractal->transformCommon.functionEnabledAFalse && z.x < 0.0) z.z = -z.z;
		//	if (fractal->transformCommon.functionEnabledBFalse && z.z < 0.0) z.x = -z.x;

			m2 = temp / m2;
			temp = 2.0 * z.y * z.x * m2;
			z.y = (z.y * z.y - z.x * z.x) * m2;

			if (!fractal->transformCommon.functionEnabledAFalse)
				z.x = temp;
			else
				z.x = -temp;

			if (fractal->transformCommon.functionEnabledFFalse)
			{
				if (fractal->transformCommon.functionEnabledEFalse) aux.pos_neg *= -1.0;
				z.z *= aux.pos_neg;
				aux.pos_neg *= -1.0;
			}
		}
	}

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
