/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Pseudo Kleinian std DE Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */

#include "all_fractal_definitions.h"

cFractalPseudoKleinianStdDE::cFractalPseudoKleinianStdDE() : cAbstractFractal()
{
	nameInComboBox = "Pseudo Kleinian - stdDE";
	internalName = "pseudo_kleinian_std_de";
	internalID = fractal::pseudoKleinianStdDE;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalPseudoKleinianStdDE::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.benesiT1EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 tempZ = z;
		double tempL = tempZ.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
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
		aux.DE = aux.DE * avgScale + 1.0;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	}
	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		CVector4 tempA, tempB;

		if (fractal->transformCommon.functionEnabledAx)
			tempA.x = fabs(z.x + fractal->transformCommon.offsetF000.x);

		if (fractal->transformCommon.functionEnabledx)
			tempB.x = fabs(z.x - fractal->transformCommon.offset000.x);

		z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale3D111.x);

		if (fractal->transformCommon.functionEnabledAy)
			tempA.y = fabs(z.y + fractal->transformCommon.offsetF000.y);

		if (fractal->transformCommon.functionEnabledy)
			tempB.y = fabs(z.y - fractal->transformCommon.offset000.y);

		z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale3D111.y);

		if (fractal->transformCommon.functionEnabledAz)
			tempA.z = fabs(z.z + fractal->transformCommon.offsetF000.z);

		if (fractal->transformCommon.functionEnabledz)
			tempB.z = fabs(z.z - fractal->transformCommon.offset000.z);

		z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale3D111.z);

		z += fractal->transformCommon.offsetA000;
	}
	if (fractal->transformCommon.functionEnabled && aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		z = fabs(z);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		z *= fractal->transformCommon.scale3;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplierA111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplierA111.y;
		if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplierA111.z;
		aux.DE *= fractal->transformCommon.scale3 * fractal->transformCommon.scaleA1;

		z += fractal->transformCommon.additionConstantA000;
	}

	aux.DE *= fractal->transformCommon.scaleB1;
	double k;
	// Pseudo kleinian
	CVector4 cSize = fractal->transformCommon.additionConstant0777;
	if (fractal->transformCommon.functionEnabledAy
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 tempZ = z; //  correct c++ version.

		if (z.x > cSize.x) tempZ.x = cSize.x;
		if (z.x < -cSize.x) tempZ.x = -cSize.x;
		if (z.y > cSize.y) tempZ.y = cSize.y;
		if (z.y < -cSize.y) tempZ.y = -cSize.y;
		if (z.z > cSize.z) tempZ.z = cSize.z;
		if (z.z < -cSize.z) tempZ.z = -cSize.z;

		z *= fractal->transformCommon.scaleB1;
		z = tempZ * 2.0 - z;

		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	if (fractal->transformCommon.functionEnabledAyFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		//  variation from openCL
		if (z.x > cSize.x) z.x = cSize.x * 2.0 - z.x;
		if (z.x < -cSize.x) z.x = -cSize.x * 2.0 - z.x;
		if (z.y > cSize.y) z.y = cSize.y * 2.0 - z.y;
		if (z.y < -cSize.y) z.y = -cSize.y * 2.0 - z.y;
		if (z.z > cSize.z) z.z = cSize.z * 2.0 - z.z;
		if (z.z < -cSize.z) z.z = -cSize.z * 2.0 - z.z;
		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	z += fractal->transformCommon.additionConstant000;

	aux.dist = sqrt(z.x * z.x + z.y * z.y) / aux.DE;
}
