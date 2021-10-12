/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2021 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * ABoxMod15,
 * The Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * Variations from DarkBeam, Buddhi and mclarekin
 * This formula has a c.x c.y SWAP
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_abox_mod15.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 AboxSmoothIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL4 c = aux->const_c;
	REAL colorAdd = 0.0f;

	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux->i >= fractal->transformCommon.startIterationsI
			&& aux->i < fractal->transformCommon.stopIterations1)
	{
		z += fractal->transformCommon.offset000;
		REAL rr = dot(z, z);
		z *= fractal->transformCommon.scaleG1 / rr;
		aux->DE *= (fractal->transformCommon.scaleG1 / rr);
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleA1;
		aux->DE *= fractal->transformCommon.scaleA1;
	}

	REAL4 oldZ = z;
	if (aux->i >= fractal->transformCommon.startIterationsB
			&& aux->i < fractal->transformCommon.stopIterationsB)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabled)
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
	}
	REAL4 zCol = z;

	// offset1
	if (aux->i >= fractal->transformCommon.startIterationsM
			&& aux->i < fractal->transformCommon.stopIterationsM)
		z += fractal->transformCommon.offsetA000;

	// spherical fold
	REAL rrCol = 0.0f;
	REAL m = 1.0f;
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux->i >= fractal->transformCommon.startIterationsS
			&& aux->i < fractal->transformCommon.stopIterationsS)
	{
		REAL rr = dot(z, z);
		rrCol = rr;
		if (rr < fractal->transformCommon.minR2p25)
			m = fractal->transformCommon.maxMinR2factor;
		else if (rr < fractal->transformCommon.maxR2d1)
			m = fractal->transformCommon.maxR2d1 / rr;
		z *= m;
		aux->DE *= m;
	}

	if (fractal->transformCommon.functionEnabledCy
			&& aux->i >= fractal->transformCommon.startIterationsX
			&& aux->i < fractal->transformCommon.stopIterationsX)
	{
		REAL rr = dot(z, z);
		REAL rk1 = SmoothConditionALessB(rr, fractal->transformCommon.minR2p25, fractal->transformCommon.scaleA3);
		REAL sm1 = (fractal->transformCommon.maxMinR2factor * rk1) + (1.0f - rk1);
		z = z * sm1;
		aux->DE = aux->DE * sm1;
		//aux->color += rk1 * fractal->mandelbox.color.factorSp1;
		if (aux->i >= fractal->transformCommon.startIterationsY
				&& aux->i < fractal->transformCommon.stopIterationsY)
		{
			REAL rk2 = SmoothConditionALessB(rr, fractal->transformCommon.maxR2d1, fractal->transformCommon.scaleB3);
			REAL rk21 = (1.0f - rk1) * rk2;
			REAL sm2 = (1.0f - rk21) + (fractal->transformCommon.maxR2d1 / rr * rk21);
			z = z * sm2;
			aux->DE = aux->DE * sm2;
			//aux->color += rk21 * fractal->mandelbox.color.factorSp2;
		}
	}







	// scale
	if (aux->i >= fractal->transformCommon.startIterationsE
			&& aux->i < fractal->transformCommon.stopIterationsE)
	{
		REAL useScale = 1.0f;

		useScale = (aux->actualScaleA + fractal->transformCommon.scale2);
		z *= useScale;
		aux->DE = aux->DE * fabs(useScale) + fractal->analyticDE.offset1;
		if (fractal->transformCommon.functionEnabledKFalse)
		{
			// update actualScaleA for next iteration
			REAL vary = fractal->transformCommon.scaleVary0
									* (fabs(aux->actualScaleA) - fractal->transformCommon.scaleC1);
			aux->actualScaleA = -vary;
		}
	}

	if (fractal->transformCommon.rotation2EnabledFalse
			&& aux->i >= fractal->transformCommon.startIterationsC
			&& aux->i < fractal->transformCommon.stopIterationsC)
	{
		z = Matrix33MulFloat4(fractal->transformCommon.rotationMatrix, z);
	}

	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux->i >= fractal->transformCommon.startIterationsG
			&& aux->i < fractal->transformCommon.stopIterationsG)
	{
		z += c * fractal->transformCommon.constantMultiplier111;
	}

	if (aux->i >= fractal->transformCommon.startIterationsF
			&& aux->i < fractal->transformCommon.stopIterationsF)
		z += fractal->transformCommon.additionConstantA000;

	if (aux->i >= fractal->transformCommon.startIterationsR
			&& aux->i < fractal->transformCommon.stopIterationsR)
		z = Matrix33MulFloat4(fractal->transformCommon.rotationMatrix2, z);

	if (fractal->transformCommon.functionEnabledNFalse
			&& aux->i >= fractal->transformCommon.startIterationsN
			&& aux->i < fractal->transformCommon.stopIterationsN)
	{
		REAL foldX = fractal->transformCommon.offset1;
		REAL foldY = fractal->transformCommon.offsetA1;

		REAL t;
		z.x = fabs(z.x);
		z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAFalse)
		{
			t = z.x;
			z.x = z.y;
			z.y = t;
		}
		t = z.x;
		z.x = z.x + z.y - fractal->transformCommon.offset0;
		z.y = t - z.y - fractal->transformCommon.offsetA0;
		if (fractal->transformCommon.functionEnabledBxFalse
				&& aux->i >= fractal->transformCommon.startIterationsO
				&& aux->i < fractal->transformCommon.stopIterationsO)
			z.x = -fabs(z.x);
		if (fractal->transformCommon.functionEnabledBx
				&& aux->i >= fractal->transformCommon.startIterationsP
				&& aux->i < fractal->transformCommon.stopIterationsP)
			z.y = -fabs(z.y);

		t = z.x;
		z.x = z.x + z.y;
		z.y = t - z.y;
		z.x *= 0.5f;
		z.y *= 0.5f;
		if (fractal->transformCommon.functionEnabledAx
				&& aux->i >= fractal->transformCommon.startIterationsD
				&& aux->i < fractal->transformCommon.stopIterationsD)
			z.x = foldX - fabs(z.x + foldX);
		if (fractal->transformCommon.functionEnabledAxFalse
				&& aux->i >= fractal->transformCommon.startIterationsRV
				&& aux->i < fractal->transformCommon.stopIterationsRV)
			z.y = foldY - fabs(z.y + foldY);
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (aux->i >= fractal->foldColor.startIterationsA
				&& aux->i < fractal->foldColor.stopIterationsA)
		{
			zCol = fabs(zCol - oldZ);
			if (zCol.x > 0.0f) colorAdd += fractal->foldColor.difs0000.x * zCol.x;
			if (zCol.y > 0.0f) colorAdd += fractal->foldColor.difs0000.y * zCol.y;
			if (zCol.z > 0.0f) colorAdd += fractal->foldColor.difs0000.z * zCol.z;
		}

		if (rrCol > fractal->transformCommon.maxR2d1)
			colorAdd +=
				fractal->foldColor.difs0000.w * (rrCol - fractal->transformCommon.maxR2d1) / 100.0f;

		colorAdd += fractal->mandelbox.color.factorSp1 * m;

		aux->color += colorAdd;
	}
	return z;
}