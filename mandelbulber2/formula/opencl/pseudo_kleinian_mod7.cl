/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2024 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Pseudo Kleinian Mod7, Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_pseudo_kleinian_mod7.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 PseudoKleinianMod7Iteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL oldZz = z.z;

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux->i >= fractal->transformCommon.startIterationsP
			&& aux->i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		REAL dot1 = (z.x * -SQRT_3_4_F + z.y * 0.5f) * fractal->transformCommon.scale;
		REAL t = max(0.0f, dot1);
		z.x -= t * -SQRT_3_F;
		z.y = fabs(z.y - t);

		if (z.y > z.z)
		{
			t = z.y;
			z.y = z.z;
			z.z = t;
		}
		z -= fractal->transformCommon.constantMultiplier000 * (REAL4){SQRT_3_4_F, 1.5f, 1.5f, 0.0f};
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x)
		{
			t = z.z;
			z.z = z.x;
			z.x = t;
		}
		if (z.x > 0.0f)
		{
			z.y = max(0.0f, z.y);
			z.z = max(0.0f, z.z);
		}
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux->i >= fractal->transformCommon.startIterationsR
			&& aux->i < fractal->transformCommon.stopIterationsR)
		z = Matrix33MulFloat4(fractal->transformCommon.rotationMatrix, z);

	if (fractal->transformCommon.benesiT1EnabledFalse
			&& aux->i >= fractal->transformCommon.startIterations
			&& aux->i < fractal->transformCommon.stopIterationsT1)
	{
		REAL tempXZ = z.x * SQRT_2_3_F - z.z * SQRT_1_3_F;
		z = (REAL4){(tempXZ - z.y) * SQRT_1_2_F, (tempXZ + z.y) * SQRT_1_2_F,
			z.x * SQRT_1_3_F + z.z * SQRT_2_3_F, z.w};

		REAL4 tempZ = z;
		REAL tempL = length(tempZ);
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21f) tempL = 1e-21f;
		REAL avgScale = length(z) / tempL;
		aux->DE = aux->DE * avgScale + 1.0f;

		tempXZ = (z.y + z.x) * SQRT_1_2_F;

		z = (REAL4){z.z * SQRT_1_3_F + tempXZ * SQRT_2_3_F, (z.y - z.x) * SQRT_1_2_F,
			z.z * SQRT_2_3_F - tempXZ * SQRT_1_3_F, z.w};
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux->i >= fractal->transformCommon.startIterationsD
			&& aux->i < fractal->transformCommon.stopIterationsTM1)
	{
		REAL tempXZ = z.x * SQRT_2_3_F - z.z * SQRT_1_3_F;
		z = (REAL4){(tempXZ - z.y) * SQRT_1_2_F, (tempXZ + z.y) * SQRT_1_2_F,
			z.x * SQRT_1_3_F + z.z * SQRT_2_3_F, z.w};

		REAL4 temp = z;
		REAL tempL = length(temp);
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21f) tempL = 1e-21f;
		REAL avgScale = length(z) / tempL;
		aux->DE = aux->DE * avgScale + 1.0f;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2_F;

		z = (REAL4){z.z * SQRT_1_3_F + tempXZ * SQRT_2_3_F, (z.y - z.x) * SQRT_1_2_F,
			z.z * SQRT_2_3_F - tempXZ * SQRT_1_3_F, z.w};
	}

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux->i >= fractal->transformCommon.startIterationsF
			&& aux->i < fractal->transformCommon.stopIterationsF)
	{
		REAL4 tempA = (REAL4){0.0f, 0.0f, 0.0f, 0.0f};
		REAL4 tempB = (REAL4){0.0f, 0.0f, 0.0f, 0.0f};

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
	REAL k = 1.0f;
	REAL4 cSize = fractal->transformCommon.additionConstant0777;
	if (aux->i >= fractal->transformCommon.startIterationsC
			&& aux->i < fractal->transformCommon.stopIterationsC)
	{
		REAL4 tempZ = z;
		if (z.x > cSize.x) tempZ.x = cSize.x;
		if (z.x < -cSize.x) tempZ.x = -cSize.x;
		if (z.y > cSize.y) tempZ.y = cSize.y;
		if (z.y < -cSize.y) tempZ.y = -cSize.y;
		if (z.z > cSize.z) tempZ.z = cSize.z;
		if (z.z < -cSize.z) tempZ.z = -cSize.z;
		z = tempZ * 2.0f - z;
		k = max(fractal->transformCommon.minR05 / dot(z, z), 1.0f);
		z *= -k * fractal->transformCommon.scaleB1;
		z.z = -z.z;
		aux->DE *= (k + fractal->analyticDE.tweak005) * fabs(fractal->transformCommon.scaleB1);
		aux->pseudoKleinianDE = fractal->analyticDE.scale1; // pK DE
	}

	z += fractal->transformCommon.additionConstant000;

	// color
	if (fractal->foldColor.auxColorEnabledFalse && aux->i >= fractal->foldColor.startIterationsA
			&& aux->i < fractal->foldColor.stopIterationsA)
	{
		REAL colorAdd = 0.0f;
		colorAdd += fractal->foldColor.difs0000.x * k;
		colorAdd += fractal->foldColor.difs0000.y * fabs(z.z);
		colorAdd += fractal->foldColor.difs0000.z * fabs(z.z - oldZz);

		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			REAL Size = 2.0f * cSize.x * fractal->transformCommon.constantMultiplier111.x;
			REAL bb = ((z.x + Size) / Size) + fractal->transformCommon.additionConstantP000.x;
			bb = fabs(bb - round(bb)) * fractal->transformCommon.constantMultiplierC111.x;
			REAL dd = ((aux->const_c.x + Size) / Size) + fractal->transformCommon.additionConstantP000.x;
			dd = fabs(dd - round(dd)) * fractal->transformCommon.constantMultiplierC111.x;

			Size = 2.0f * cSize.y * fractal->transformCommon.constantMultiplier111.y;
			REAL cc = ((z.y + Size) / Size) + fractal->transformCommon.additionConstantP000.y;
			cc = fabs(cc - round(cc)) * fractal->transformCommon.constantMultiplierC111.y;
			REAL ee = ((aux->const_c.y + Size) / Size) + fractal->transformCommon.additionConstantP000.y;
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
			{
				Size = 2.0f * cSize.z * fractal->transformCommon.constantMultiplier111.z;
				REAL aa = ((z.z + Size) / Size) + fractal->transformCommon.additionConstantP000.z;
				aa = fabs(aa - round(aa)) * fractal->transformCommon.constantMultiplierC111.z;
				bb = bb + aa;
				REAL ff =
					((aux->const_c.z + Size) / Size) + fractal->transformCommon.additionConstantP000.z;
				ff = fabs(ff - round(ff)) * fractal->transformCommon.constantMultiplierC111.z;
				dd = dd + ff;
			}
			bb = dd * (1.0f - fractal->foldColor.difs1) + bb * fractal->foldColor.difs1; // mix

			colorAdd += fractal->foldColor.difs0000.w * bb;
		}
		aux->color += colorAdd;
	}
	return z;
}