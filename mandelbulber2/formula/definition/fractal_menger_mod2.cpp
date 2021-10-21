/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Smooth Mod1, based on :
 * http://www.fractalforums.com/fragmentarium/help-t22583/
 */

#include "all_fractal_definitions.h"

cFractalMengerMod2::cFractalMengerMod2() : cAbstractFractal()
{
	nameInComboBox = "Menger - Mod 2";
	internalName = "menger_mod2";
	internalID = fractal::mengerMod2;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalMengerMod2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledCFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		double ang = sqrt(z.x * z.x + z.y * z.y) * fractal->transformCommon.scaleA0
				+ z.z * fractal->transformCommon.scaleB0
				+ fractal->transformCommon.angleDegA;
		double cosA = cos(ang);
		double sinB = sin(ang);
		double temp = z.x;
		z.x = z.x * cosA - z.y * sinB;
		z.y = temp * sinB + z.y * cosA;
		aux.DE = aux.DE + fractal->analyticDE.offset1;
	}

	// octo
	if (fractal->transformCommon.functionEnabledEFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		if (z.x + z.y < 0.0) z = CVector4(-z.y, -z.x, z.z, z.w);

		if (z.x + z.z < 0.0) // z.xz = -z.zx;
			z = CVector4(-z.z, z.y, -z.x, z.w);

		if (z.x - z.y < 0.0) // z.xy = z.yx;
			z = CVector4(z.y, z.x, z.z, z.w);

		if (z.x - z.z < 0.0) // z.xz = z.zx;
			z = CVector4(z.z, z.y, z.x, z.w);

		z.x = fabs(z.x);
		z = z * fractal->transformCommon.scale2
				- fractal->transformCommon.offset100 * (fractal->transformCommon.scale2 - 1.0);

		aux.DE *= fractal->transformCommon.scale2;
	}

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		z += fractal->mandelbox.offset;
		double m;
		double rr = z.Dot(z);

		if (rr < fractal->transformCommon.minR0 * fractal->transformCommon.maxR2d1)
		{
			//if (!fractal->transformCommon.functionEnabledFalse)
				//	m =fractal->mandelbox.foldingSphericalFixed / (fractal->transformCommon.minR0 * fractal->transformCommon.scale);

			m =1. / (fractal->transformCommon.minR0);

			//else m = 2.0f *fractal->mandelbox.foldingSphericalFixed/ (fractal->transformCommon.minR0 * fractal->transformCommon.scale)- rr;

			z *= m;
			aux.DE *= m;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			m = fractal->transformCommon.maxR2d1 / rr;
			z *= m;
			aux.DE *= m;
		}
		z -= fractal->mandelbox.offset; // + fractal->transformCommon.additionConstant000;
	}

	if (aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		z = fabs(z);
		z += fractal->transformCommon.offset000;

		if (fractal->transformCommon.rotationEnabledFalse
				&& aux.i >= fractal->transformCommon.startIterationsR
				&& aux.i < fractal->transformCommon.stopIterationsR)
		{
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}

		double t;
		t = z.x - z.y;
		t = fractal->transformCommon.additionConstant0555.x
				* (t - sqrt(t * t) * fractal->transformCommon.constantMultiplier111.x);
		z.x = z.x - t;
		z.y = z.y + t;

		t = z.x - z.z;
		t = fractal->transformCommon.additionConstant0555.y
				* (t - sqrt(t * t) * fractal->transformCommon.constantMultiplier111.y);
		z.x = z.x - t;
		z.z = z.z + t;

		t = z.y - z.z;
		t = fractal->transformCommon.additionConstant0555.z
				* (t - sqrt(t * t) * fractal->transformCommon.constantMultiplier111.z);
		z.y = z.y - t;
		z.z = z.z + t;

		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

		double sc1 = fractal->transformCommon.scale3 - 1.0;
		double sc2 = sc1 / fractal->transformCommon.scale3;
		z.z = z.z - fractal->transformCommon.offset1105.z * sc2;
		z.z = -fabs(z.z) + fractal->transformCommon.offset1105.z * sc2;

		z.x = fractal->transformCommon.scale3 * z.x - fractal->transformCommon.offset1105.x * sc1;
		z.y = fractal->transformCommon.scale3 * z.y - fractal->transformCommon.offset1105.y * sc1;
		z.z = fractal->transformCommon.scale3 * z.z;

		aux.DE = aux.DE * fractal->transformCommon.scale3 + fractal->analyticDE.offset0;
	}



	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box offset
	{
		z.x = sign(z.x) * fractal->transformCommon.additionConstantA000.x + z.x;
		z.y = sign(z.y) * fractal->transformCommon.additionConstantA000.y + z.y;
		z.z = sign(z.z) * fractal->transformCommon.additionConstantA000.z + z.z;
	}


}
