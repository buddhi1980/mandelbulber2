/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Sponge v5, based on :
 * http://www.fractalforums.com/fragmentarium/help-t22583/
 */

#include "all_fractal_definitions.h"

cFractalMengerV5::cFractalMengerV5() : cAbstractFractal()
{
	nameInComboBox = "Menger - V5";
	internalName = "menger_v5";
	internalID = fractal::mengerV5;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalMengerV5::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
		if (z.x + z.z < 0.0) z = CVector4(-z.z, z.y, -z.x, z.w);
		if (z.x - z.y < 0.0) z = CVector4(z.y, z.x, z.z, z.w);
		if (z.x - z.z < 0.0) z = CVector4(z.z, z.y, z.x, z.w);

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

		if (rr < fractal->transformCommon.minR0)
		{
			m = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR0;
			z *= m;
			aux.DE *= m;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			m = fractal->transformCommon.maxR2d1 / rr;
			z *= m;
			aux.DE *= m;
		}
		z -= fractal->mandelbox.offset;
	}
		// menger sponge
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

		t = z.x - z.z;
		t = fractal->transformCommon.additionConstant0555.x
				* (t - fabs(t) * fractal->transformCommon.constantMultiplier111.x);
		z.x = z.x - t;
		z.z = z.z + t;

		t = z.x - z.y;
		t = fractal->transformCommon.additionConstant0555.y
				* (t - fabs(t) * fractal->transformCommon.constantMultiplier111.y);
		z.x = z.x - t;
		z.y = z.y + t;

		t = z.z - z.y;
		t = fractal->transformCommon.additionConstant0555.z
				* (t - fabs(t) * fractal->transformCommon.constantMultiplier111.z);
		z.z = z.z - t;
		z.y = z.y + t;


		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

		double useScale = fractal->transformCommon.scale3;
		if (fractal->transformCommon.functionEnabledXFalse
				&& aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			useScale += aux.actualScaleA;
			// update actualScale for next iteration
			double vary = fractal->transformCommon.scaleVary0
					* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
			aux.actualScaleA = -vary;
		}
		aux.DE = aux.DE * useScale + fractal->analyticDE.offset0;

		double sc1 = useScale - 1.0;
		double sc2 = sc1 / useScale;
		z.y -= fractal->transformCommon.offset1105.z * sc2;
		z.y = fractal->transformCommon.offset1105.z * sc2 - fabs(z.y);
		z.x = useScale * z.x - fractal->transformCommon.offset1105.x * sc1;
		z.z = useScale * z.z - fractal->transformCommon.offset1105.y * sc1;
		z.y = useScale * z.y;
	}

	// box offset
	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box offset
	{
		z.x = sign(z.x) * fractal->transformCommon.additionConstantA000.x + z.x;
		z.y = sign(z.y) * fractal->transformCommon.additionConstantA000.y + z.y;
		z.z = sign(z.z) * fractal->transformCommon.additionConstantA000.z + z.z;
	}

	if (fractal->transformCommon.functionEnabledYFalse)
	{
		if (!fractal->transformCommon.functionEnabledzFalse)
		{
			CVector4 q = fabs(z);
			aux.DE0 = max(max(q.x, q.y), q.z);
		}
		else aux.DE0 = z.Length();

		aux.dist = min(aux.dist, (aux.DE0 - fractal->transformCommon.offset0) / aux.DE);
	}
}
