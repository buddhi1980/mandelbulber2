/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Middle Mod
 */

#include "all_fractal_definitions.h"

cFractalMengerMiddleMod::cFractalMengerMiddleMod() : cAbstractFractal()
{
	nameInComboBox = "Menger - Middle Mod";
	internalName = "menger_middle_mod";
	internalID = fractal::mengerMiddleMod;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalMengerMiddleMod::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		double rr = 1.0;
		z += fractal->transformCommon.offset000;
		rr = z.Dot(z);
		z *= fractal->transformCommon.maxR2d1 / rr;
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleB1;
		// double r = sqrt(rr);
		aux.DE *= (fractal->transformCommon.maxR2d1 / rr) * fractal->analyticDE.scale1
							* fractal->transformCommon.scaleB1;
	}

	// fabs() and menger fold
	z = fabs(z + fractal->transformCommon.additionConstantA000);
	if (z.x - z.y < 0.0) swap(z.y, z.x);
	if (z.x - z.z < 0.0) swap(z.z, z.x);
	if (z.y - z.z < 0.0) swap(z.z, z.y);

	if (fractal->mandelbox.mainRotationEnabled && aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC) // rotation
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box fold
	{
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
		double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}
	if (fractal->transformCommon.functionEnabledByFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{ // fabsBoxFold
		CVector4 tempA, tempB;

		if (fractal->transformCommon.functionEnabledx)
			tempA.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
		if (fractal->transformCommon.functionEnabledAx)
			tempB.x = fabs(z.x - fractal->transformCommon.additionConstantA111.x);
		z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale3D111.x);

		if (fractal->transformCommon.functionEnabledy)
			tempA.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
		if (fractal->transformCommon.functionEnabledAy)
			tempB.y = fabs(z.y - fractal->transformCommon.additionConstantA111.y);
		z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale3D111.y);

		if (fractal->transformCommon.functionEnabledz)
			tempA.z = fabs(z.z + fractal->transformCommon.additionConstant111.z);
		if (fractal->transformCommon.functionEnabledAz)
			tempB.z = fabs(z.z - fractal->transformCommon.additionConstantA111.z);
		z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale3D111.z);

		if (aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			z *= fractal->transformCommon.scaleA1;
			aux.DE *= fabs(fractal->transformCommon.scaleA1);
		}
	}
	// menger scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		useScale = aux.actualScaleA + fractal->transformCommon.scale3;

		z *= useScale;

		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fabs(useScale) + 1.0;
		else // testing for log
			aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

		if (fractal->transformCommon.functionEnabledFFalse
				&& aux.i >= fractal->transformCommon.startIterationsY
				&& aux.i < fractal->transformCommon.stopIterationsY)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	// menger offsets
	z.x -= 2.0 * fractal->transformCommon.constantMultiplier111.x;
	z.y -= 2.0 * fractal->transformCommon.constantMultiplier111.y;
	if (fractal->transformCommon.functionEnabled)
	{
		if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
	}
	else
	{
		z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
	}

	if (fractal->transformCommon.addCpixelEnabledFalse) // addCpixel options
	{
		switch (fractal->mandelbulbMulti.orderOfXYZC)
		{
			case multi_OrderOfXYZ_xyz:
			default: aux.c = CVector4(aux.c.x, aux.c.y, aux.c.z, aux.c.w); break;
			case multi_OrderOfXYZ_xzy: aux.c = CVector4(aux.c.x, aux.c.z, aux.c.y, aux.c.w); break;
			case multi_OrderOfXYZ_yxz: aux.c = CVector4(aux.c.y, aux.c.x, aux.c.z, aux.c.w); break;
			case multi_OrderOfXYZ_yzx: aux.c = CVector4(aux.c.y, aux.c.z, aux.c.x, aux.c.w); break;
			case multi_OrderOfXYZ_zxy: aux.c = CVector4(aux.c.z, aux.c.x, aux.c.y, aux.c.w); break;
			case multi_OrderOfXYZ_zyx: aux.c = CVector4(aux.c.z, aux.c.y, aux.c.x, aux.c.w); break;
		}
		z += aux.c * fractal->transformCommon.constantMultiplierC111;
	}
}
