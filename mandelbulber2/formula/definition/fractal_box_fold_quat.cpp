/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * BoxFold Quaternion
 * This formula contains aux.color, aux.pos_neg and aux.actualScale
 * Sometimes Delta DE Linear works best.
 */

#include "all_fractal_definitions.h"

cFractalBoxFoldQuat::cFractalBoxFoldQuat() : cAbstractFractal()
{
	nameInComboBox = "Box Fold Quat";
	internalName = "box_fold_quat";
	internalID = fractal::boxFoldQuat;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalBoxFoldQuat::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;
	// tglad fold
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		zCol = z;
	}

	// spherical fold
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{

		double rr = z.Dot(z);
		rrCol = rr;
		z += fractal->mandelbox.offset;

		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < fractal->transformCommon.minR2p25)
		{
			double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
		}
		z -= fractal->mandelbox.offset;
	}
	// scale
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{

		double useScale = aux.actualScaleA + fractal->transformCommon.scale;

		z *= useScale;
		aux.DE = aux.DE * fabs(useScale);

		if (aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			// update actualScale for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
	}

	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsTM
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale;
		z = (fabs(z + fractal->transformCommon.additionConstantA111)
				 - fabs(z - fractal->transformCommon.additionConstantA111) - z);
		tempXZ = (z.y + z.x) * SQRT_1_2;
		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	}

	// octo
	if (fractal->transformCommon.functionEnabledDFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)

	{
		if (z.x + z.y < 0.0) z = CVector4(-z.y, -z.x, z.z, z.w);

		if (z.x + z.z < 0.0) // z.xz = -z.zx;
			z = CVector4(-z.z, z.y, -z.x, z.w);

		if (z.x - z.y < 0.0) // z.xy = z.yx;
			z = CVector4(z.y, z.x, z.z, z.w);

		if (z.x - z.z < 0.0) // z.xz = z.zx;
			z = CVector4(z.z, z.y, z.x, z.w);

		z.x = fabs(z.x);
		z = z * fractal->transformCommon.scaleA2
				- fractal->transformCommon.offset100 * (fractal->transformCommon.scaleA2 - 1.0);

		aux.DE *= fabs(fractal->transformCommon.scaleA2);
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{ // fabs() and menger fold
		z = fabs(z + fractal->transformCommon.additionConstantA000);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		// menger scales and offsets
		z *= fractal->transformCommon.scale2;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplier111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplier111.y;
		z.z -= 2.0 * fractal->transformCommon.constantMultiplier111.z;
		aux.DE *= fractal->transformCommon.scale2;
	}

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;

		if (fractal->analyticDE.enabledFalse)
		{
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
		}
		z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, z.w);

		double tempL = z.Length();
		z *= fractal->transformCommon.constantMultiplier122;
		// if (tempL < 1e-21) tempL = 1e-21;
		CVector4 tempAvgScale = CVector4(z.x, z.y / 2.0, z.z / 2.0, z.w);
		double avgScale = tempAvgScale.Length() / tempL;
		double tempAux = aux.DE * avgScale;
		aux.DE = aux.DE + (tempAux - aux.DE) * fractal->transformCommon.scaleA1;

		if (fractal->transformCommon.functionEnabledAxFalse)
		{
			CVector4 offsetAlt = aux.pos_neg * fractal->transformCommon.additionConstant000;
			z += offsetAlt;
			aux.pos_neg *= -fractal->transformCommon.scale1;
		}
		else
		{
			z += fractal->transformCommon.additionConstant000;
		}
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->mandelbox.color.factor.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->mandelbox.color.factor.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->mandelbox.color.factor.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

		if (rrCol < fractal->transformCommon.maxR2d1)
		{
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
										+ fractal->mandelbox.color.factorSp2
												* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
			else
				colorAdd += fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
		}

		aux.color += colorAdd;
	}
}
