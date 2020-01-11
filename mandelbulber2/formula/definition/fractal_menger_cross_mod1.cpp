/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Cross Mod1
 * from code by Knighty
 * http://www.fractalforums.com/fragmentarium/
 * cross-menger!-can-anyone-do-this/msg93972/#new
 */

#include "all_fractal_definitions.h"

cFractalMengerCrossMod1::cFractalMengerCrossMod1() : cAbstractFractal()
{
	nameInComboBox = "Menger - Cross Mod1";
	internalName = "menger_cross_mod1";
	internalID = fractal::mengerCrossMod1;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerCrossMod1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 gap = fractal->transformCommon.constantMultiplier000;

	if (fractal->transformCommon.functionEnabledx && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledFFalse) z.x = fabs(z.x);
		double dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		double t = max(0.0, dot1);
		z.x -= t * -SQRT_3 - (0.5 * SQRT_3_4);

		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z.y -= 1.5;
		z -= gap * CVector4(SQRT_3_4, -1.5, 1.5, 0.0);

		if (z.z > z.x) swap(z.z, z.x);
		if (fractal->transformCommon.functionEnabledyFalse)
		{
			if (z.x >= 0.0)
			{
				z.y = max(0.0, z.y) * fractal->transformCommon.scaleA1;
				z.z = max(0.0, z.z) * fractal->transformCommon.scaleB1;
			}
		}
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		aux.DE *= fractal->analyticDE.scale1; // tweak
	}

	if (fractal->transformCommon.functionEnabledy
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{ // CrossMengerTrick
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledzFalse) z.x = fabs(z.x);

		double dot1 = (z.x * -SQRT_3_4 + z.y * 0.5);
		double t = 1.0 * max(0.0, dot1);
		z.x -= t * -SQRT_3;
		if (fractal->transformCommon.functionEnabledXFalse)
			z.y = fabs(z.y) - t;
		else
		{
			z.y = fabs(z.y - t);
		}
		z.x -= SQRT_3_4;

		// Choose nearest corner/edge to get translation symmetry (all y & z code)
		double dy = 0.0;
		double dz = 0.0;
		if (z.y > 0.5 && z.z > 0.5) // if both y & z > 0.5  then =1.5
		{
			dy = 1.5;
			dz = 1.5;
		}
		else if (z.z < z.y)
		{
			dy = 1.5; // and dz is unchanged
		}
		else
			dz = 1.5; // and dy is unchanged

		z.y -= dy;
		z.z -= dz;
		z *= fractal->transformCommon.scale3;
		aux.DE *= fractal->transformCommon.scale3;
		z.y += dy;
		z.z += dz;

		z.x += SQRT_3_4;

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
