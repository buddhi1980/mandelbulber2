/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Pseudo Kleinian Mod7, Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */

#include "all_fractal_definitions.h"

cFractalPseudoKleinianMod7::cFractalPseudoKleinianMod7() : cAbstractFractal()
{
	nameInComboBox = "Pseudo Kleinian - Mod 7";
	internalName = "pseudo_kleinian_mod7";
	internalID = fractal::pseudoKleinianMod7;
	DEType = analyticDEType;
	DEFunctionType = pseudoKleinianDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionPseudoKleinian;
	coloringFunction = coloringFunctionDefault;
}

void cFractalPseudoKleinianMod7::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double oldZz = z.z;

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		double dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		double t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z)
		{
			t = z.y;
			z.y = z.z;
			z.z = t;
		}
		z -= fractal->transformCommon.constantMultiplier000 * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x)
		{
			t = z.z;
			z.z = z.x;
			z.x = t;
		}
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
		CVector4 tempA = CVector4(0.0, 0.0, 0.0, 0.0);
		CVector4 tempB = CVector4(0.0, 0.0, 0.0, 0.0);

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

	//	z += fractal->transformCommon.offsetA000;
	}

	// Pseudo kleinian
	double k = 1.0;
	CVector4 cSize = fractal->transformCommon.additionConstant0777;
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 tempZ = z;
		if (z.x > cSize.x) tempZ.x = cSize.x;
		if (z.x < -cSize.x) tempZ.x = -cSize.x;
		if (z.y > cSize.y) tempZ.y = cSize.y;
		if (z.y < -cSize.y) tempZ.y = -cSize.y;
		if (z.z > cSize.z) tempZ.z = cSize.z;
		if (z.z < -cSize.z) tempZ.z = -cSize.z;
		z = tempZ * 2.0 - z;
		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= -k * fractal->transformCommon.scaleB1;
		z.z = -z.z;
		aux.DE *= (k + fractal->analyticDE.tweak005) * fabs(fractal->transformCommon.scaleB1);
		aux.pseudoKleinianDE = fractal->analyticDE.scale1; // pK DE
	}

	z += fractal->transformCommon.additionConstant000;

	// color
	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
		&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colorAdd = 0.0;
		colorAdd += fractal->foldColor.difs0000.x * k;
		colorAdd += fractal->foldColor.difs0000.y * fabs(z.z);
		colorAdd += fractal->foldColor.difs0000.z * fabs(z.z - oldZz);

		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			double Size = 2.0 * cSize.x * fractal->transformCommon.constantMultiplier111.x;
			double bb = ((z.x + Size) / Size) + fractal->transformCommon.additionConstantP000.x;
			bb = fabs(bb - round(bb)) * fractal->transformCommon.constantMultiplierC111.x;
			double dd = ((aux.const_c.x + Size) / Size) + fractal->transformCommon.additionConstantP000.x;
			dd = fabs(dd - round(dd)) * fractal->transformCommon.constantMultiplierC111.x;

			Size = 2.0 * cSize.y * fractal->transformCommon.constantMultiplier111.y;
			double cc = ((z.y + Size) / Size) + fractal->transformCommon.additionConstantP000.y;
			cc = fabs(cc - round(cc)) * fractal->transformCommon.constantMultiplierC111.y;
			double ee = ((aux.const_c.y + Size) / Size) + fractal->transformCommon.additionConstantP000.y;
			ee = fabs(ee - round(ee)) * fractal->transformCommon.constantMultiplierC111.y;


			if (!fractal->transformCommon.functionEnabledOFalse)
			{
				bb = bb + cc;
				dd = dd + ee;
			}
			else
			{
				bb = bb * bb + cc * cc;
				dd = dd * dd + ee * ee;
			}

			if (fractal->transformCommon.functionEnabledAFalse)
			{	Size = 2.0 * cSize.z * fractal->transformCommon.constantMultiplier111.z;
				double aa = ((z.z + Size) / Size) + fractal->transformCommon.additionConstantP000.z;
				aa = fabs(aa - round(aa)) * fractal->transformCommon.constantMultiplierC111.z;
				bb = bb + aa;
				double ff = ((aux.const_c.z + Size) / Size) + fractal->transformCommon.additionConstantP000.z;
				ff = fabs(ff - round(ff)) * fractal->transformCommon.constantMultiplierC111.z;
				dd = dd + ff;
			}
			bb = dd * (1.0 - fractal->foldColor.difs1) + bb * fractal->foldColor.difs1; // mix

			colorAdd += fractal->foldColor.difs0000.w * bb;
		}
		aux.color += colorAdd;
	}
}
