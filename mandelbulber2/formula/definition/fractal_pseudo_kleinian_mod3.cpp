/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Pseudo Kleinian Mod2, Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */

#include "all_fractal_definitions.h"

cFractalPseudoKleinianMod3::cFractalPseudoKleinianMod3() : cAbstractFractal()
{
	nameInComboBox = "Pseudo Kleinian - Mod 3";
	internalName = "pseudo_kleinian_mod3";
	internalID = fractal::pseudoKleinianMod3;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionPseudoKleinian;
	coloringFunction = coloringFunctionDefault;
}

void cFractalPseudoKleinianMod3::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	CVector4 oldZ = z;
	CVector4 zCol = z;
	double rrCol = 0.0;
	double colorAdd = 0.0;



	z.x -= sign(z.x) * fractal->transformCommon.constantMultiplier000.x;
	z.y -= sign(z.y) * fractal->transformCommon.constantMultiplier000.y;
	z.z -= sign(z.z) * fractal->transformCommon.constantMultiplier000.z;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->transformCommon.benesiT1EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, 0.0);

		CVector4 tempZ = z;
		double tempL = tempZ.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, 0.0);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, 0.0);

		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		oldZ = z;
		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);
		zCol = z;
		/*if (fractal->foldColor.auxColorEnabledFalse)
		{
			if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
		}*/
		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, 0.0);
	}

	double k;
	// Pseudo kleinian
	CVector4 cSize = fractal->transformCommon.additionConstant0777;
	if (fractal->transformCommon.functionEnabledAy
			&& aux.i >= fractal->transformCommon.startIterationsC
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
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		oldZ = z;
		z = fabs(z + fractal->transformCommon.offsetA000)
				- fabs(z - fractal->transformCommon.offsetA000) - z;
		zCol = z;
	}
	if (fractal->transformCommon.addCpixelEnabledFalse) // symmetrical addCpixel
	{
		CVector4 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.offsetF000;
		z.x -= sign(z.x) * tempFAB.x;
		z.y -= sign(z.y) * tempFAB.y;
		z.z -= sign(z.z) * tempFAB.z;
	}



	double colorDist = aux.dist;
	CVector4 zz = z * z;
	double d1 = sqrt(min(min( zz.x + zz.y, zz.y + zz.z), zz.z + zz.x));
	if (fractal->transformCommon.functionEnabledKFalse) d1 = sqrt(zz.x + zz.y);
	d1 -= fractal->transformCommon.offsetR0;

	double d2 = fabs(z.z);
	aux.DE0 = d2;
	if( d1 < d2) aux.DE0 = d1;

	//if (!fractal->transformCommon.functionEnabledEFalse)

	aux.DE0 = 0.5 * (aux.DE0 - fractal->transformCommon.offset0) / aux.DE;

	if (fractal->transformCommon.functionEnabledDFalse)
		aux.DE0 = min(aux.dist, aux.DE0);

	aux.dist = aux.DE0;

	/*aux.pseudoKleinianDE = fractal->analyticDE.scale1; // pK DE
	// aux.pseudoKleinianZZ = fractal->transformCommon.scale0; // pK z.z * z.z * scale0
	double rxy = sqrt(z.x * z.x + z.y * z.y);

	double r = z.Length();
	aux.dist = max(rxy - aux.pseudoKleinianDE, fabs(rxy * z.z) / r) / aux.DE;*/

	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
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
					colorAdd +=
						fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
						+ fractal->mandelbox.color.factorSp2
								* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
				else
					colorAdd +=
						fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			}
		}
		else
		{
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;

			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1;
			else if (rrCol < fractal->transformCommon.maxR2d1)
				colorAdd += fractal->mandelbox.color.factorSp2;
		}
		aux.color += colorAdd;
	}
}
