/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Cross KIFS
 * from code by Knighty
 * http://www.fractalforums.com/fragmentarium/
 * cross-menger!-can-anyone-do-this/msg93972/#new
 */

#include "all_fractal_definitions.h"

cFractalMengerCrossKIFS::cFractalMengerCrossKIFS() : cAbstractFractal()
{
	nameInComboBox = "Menger - Cross KIFS";
	internalName = "menger_cross_kifs";
	internalID = fractal::mengerCrossKIFS;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerCrossKIFS::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledFFalse) z.x = fabs(z.x);

		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= (t * -SQRT_3) - 0.5;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z.y -= 0.75;
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);

		if (z.z > z.x) swap(z.z, z.x);

		z *= fractal->transformCommon.constantMultiplier111; // post scale

		if (fractal->transformCommon.functionEnabledyFalse)
		{
			z.y = max(0.0, z.y) * fractal->transformCommon.scaleA1;
			z.z = max(0.0, z.z) * fractal->transformCommon.scaleB1;
		}

		z += fractal->transformCommon.additionConstant000; // post offset
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

		aux.DE *= fractal->analyticDE.scale1; // tweak
	}

	if (fractal->transformCommon.functionEnabled && aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{ // CrossMengerKIFS
		double lengthL;
		// z = ( z );//+ preAdd );
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledzFalse) z.x = fabs(z.x);

		lengthL = z.x * SQRT_3_4 - z.y * 0.5;
		if (lengthL < 0.0)
		{
			z.x -= SQRT_3_4 * (2.0 * lengthL);
			z.y -= -lengthL;
		}
		lengthL = z.y;
		if (lengthL < 0.0)
		{
			z.y -= 2.0 * lengthL;
		}
		lengthL = (-z.y + z.z) * SQRT_1_2;
		if (lengthL < 0.0)
		{
			z.y -= -SQRT_1_2 * (2.0 * lengthL);
			z.z -= SQRT_1_2 * (2.0 * lengthL);
		}
		lengthL = z.y;
		if (lengthL < 0.5)
		{
			z.y -= 2.0 * (lengthL - 0.5);
		}
		CVector4 edge = fractal->transformCommon.offset222;
		if (fractal->transformCommon.functionEnabledxFalse)
		{
			z.x = edge.x - z.x;
			z.y = edge.y - z.y;
			z.z = edge.z - z.z;
		}

		z *= fractal->transformCommon.scale3;
		z -= CVector4(SQRT_3_4, 1.5, 1.5, 0.0) * (fractal->transformCommon.scale3 - 1.0);
		aux.DE *= fractal->transformCommon.scale3;

		z += fractal->transformCommon.offset000; // post offset
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			z.x = fabs(z.x + fractal->transformCommon.offset) + fractal->transformCommon.offset0;
		}
		if (fractal->transformCommon.functionEnabledRFalse
				&& aux.i >= fractal->transformCommon.startIterationsR
				&& aux.i < fractal->transformCommon.stopIterationsR)
		{
			z = fractal->transformCommon.rotationMatrix2.RotateVector(z);
		}
	}
}
