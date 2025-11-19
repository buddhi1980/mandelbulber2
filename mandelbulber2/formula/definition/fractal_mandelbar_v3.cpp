/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Mandelbar or Tricorn version based on pow2 code from Davis Makin
 * ref: https://www.facebook.com/david.makin.7
 */

#include "all_fractal_definitions.h"

cFractalMandelbarV3::cFractalMandelbarV3() : cAbstractFractal()
{
	nameInComboBox = "Mandelbar V3";
	internalName = "mandelbar_v3";
	internalID = fractal::mandelbarV3;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbarV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
	//	z.z *= fractal->transformCommon.scale2;

		m2 = temp / m2;
		temp = 2.0 * z.x * z.y * m2;

		z.y = (z.y * z.y - z.x * z.x) * m2;

		if (fractal->transformCommon.functionEnabledM
				&& aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA)
			z.x = -temp;

		if (fractal->transformCommon.functionEnabledBFalse
				&& aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			z.x = -temp;
			z.z = -z.z;
		}

		if (fractal->transformCommon.functionEnabledCFalse
				&& aux.i >= fractal->transformCommon.startIterationsC
				&& aux.i < fractal->transformCommon.stopIterationsC)
			z.x = temp;

		if (fractal->transformCommon.functionEnabledDFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD)
		{
			z.x = temp;
			z.z = -z.z;
		}
	}

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP)
	{
		z.z *= fractal->transformCommon.scale1;
	}

	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		z *= fractal->transformCommon.scaleA1;
		aux.DE *= fabs(fractal->transformCommon.scaleA1);
	}

	// offset (Julia)
	if (fractal->transformCommon.functionEnabledJFalse
			&& aux.i >= fractal->transformCommon.startIterationsJ
			&& aux.i < fractal->transformCommon.stopIterationsJ)
		z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
		z += aux.const_c * fractal->transformCommon.constantMultiplier111;

	// aux->color test
	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colAdd = fractal->foldColor.difs0000.w
				+ aux.i * fractal->foldColor.difs0;

		// last two z lengths
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			double lastVec = 0.0;
			CVector4 oldPt = aux.old_z;
			double lastZ = oldPt.Length(); // aux.old_r;
			double newZ = z.Length();
			if (fractal->transformCommon.functionEnabledBwFalse) lastVec = newZ / lastZ;
			if (fractal->transformCommon.functionEnabledByFalse) lastVec = lastZ / newZ;
			if (fractal->transformCommon.functionEnabledBzFalse) lastVec = fabs(lastZ - newZ);
			lastVec *= fractal->foldColor.difs1;
			colAdd += lastVec;

			aux.old_z = z; // update for next iter
		}

		colAdd += fractal->foldColor.difs0000.z * fabs(z.x * z.y);

		if (!fractal->foldColor.auxColorEnabledBFalse) aux.color = colAdd;
		else aux.color += colAdd;

	}




	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
