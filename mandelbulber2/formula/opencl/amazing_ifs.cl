/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2021 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * amazing surf Mod4 based on Mandelbulber3D. Formula proposed by Kali, with features added by
 * DarkBeam
 * This formula has a c.x c.y SWAP
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_amazing_ifs.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 AmazingIfsIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL colorDist = aux->DE;

	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux->i >= fractal->transformCommon.startIterationsX
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

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}
	z += fractal->transformCommon.offsetA000;

	// polyfold
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux->i >= fractal->transformCommon.startIterationsP
			&& aux->i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		REAL psi = M_PI_F / fractal->transformCommon.int6;
		psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0f * psi) - psi);
		REAL len = native_sqrt(z.x * z.x + z.y * z.y);
		z.x = native_cos(psi) * len;
		z.y = native_sin(psi) * len;
	}

	REAL4 oldZ = z;

	if (fractal->transformCommon.functionEnabledCx)
		z.x = fabs(z.x + fractal->transformCommon.additionConstant0555.x)
					- fabs(z.x - fractal->transformCommon.additionConstant0555.x) - z.x;
	if (fractal->transformCommon.functionEnabledCy)
		z.y = fabs(z.y + fractal->transformCommon.additionConstant0555.y)
					- fabs(z.y - fractal->transformCommon.additionConstant0555.y) - z.y;
	if (fractal->transformCommon.functionEnabledCzFalse)
		z.z = fabs(z.z + fractal->transformCommon.additionConstant0555.z)
					- fabs(z.z - fractal->transformCommon.additionConstant0555.z) - z.z;
	REAL4 zCol = z;

	if (fractal->transformCommon.functionEnabledIFalse)
	{
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			if (!fractal->transformCommon.functionEnabledxFalse)
				z.x = fabs(z.x);
			else
				z.x = sign(z.x) * fabs(z.x);
			z.x -= fractal->transformCommon.additionConstantP000.x;
		}
		if (fractal->transformCommon.functionEnabledByFalse)
		{
			if (!fractal->transformCommon.functionEnabledyFalse)
				z.y = fabs(z.y);
			else
				z.y = sign(z.y) * fabs(z.y);
			z.y -= fractal->transformCommon.additionConstantP000.y;
		}
		if (fractal->transformCommon.functionEnabledBzFalse)
		{
			if (!fractal->transformCommon.functionEnabledzFalse)
				z.z = fabs(z.z);
			else
				z.z = sign(z.z) * fabs(z.z);
			z.z -= fractal->transformCommon.additionConstantP000.z;
		}
	}

	z += fractal->transformCommon.additionConstantA000;

	REAL rr = dot(z, z);
	REAL rrCol = rr;
	REAL MinRR = fractal->transformCommon.minR2p25;
	REAL dividend = rr < MinRR ? MinRR : min(rr, 1.0f);

	// scale
	REAL useScale = 1.0f;
	if (aux->i >= fractal->transformCommon.startIterationsB
			&& aux->i < fractal->transformCommon.stopIterationsB)
	{
		useScale = (aux->actualScaleA + fractal->transformCommon.scale2) / dividend;
		z *= useScale;
		aux->DE = aux->DE * fabs(useScale) + fractal->analyticDE.offset0;
		if (fractal->transformCommon.functionEnabledKFalse)
		{
			// update actualScaleA for next iteration
			REAL vary = fractal->transformCommon.scaleVary0
									* (fabs(aux->actualScaleA) - fractal->transformCommon.scaleC1);
			aux->actualScaleA -= vary;
		}
	}

	z = Matrix33MulFloat4(fractal->transformCommon.rotationMatrix2, z);

	if (fractal->transformCommon.functionEnabledGFalse)
	{
		aux->DE0 = length(z) / aux->DE;
		if (!fractal->analyticDE.enabledFalse && aux->i >= fractal->transformCommon.startIterationsC
				&& aux->i < fractal->transformCommon.stopIterationsC)
			aux->DE0 = min(aux->dist, aux->DE0);
		aux->dist = aux->DE0;
	}

	// aux->color
	if (fractal->foldColor.auxColorEnabled)
	{
		REAL colorAdd = 0.0f;
		if (fractal->foldColor.auxColorEnabledA)
			if (colorDist != aux->DE) colorAdd += fractal->foldColor.difs1;

		if (fractal->foldColor.auxColorEnabledFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd += fractal->foldColor.difs0000.x
										* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
			if (zCol.y != oldZ.y)
				colorAdd += fractal->foldColor.difs0000.y
										* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
			if (zCol.z != oldZ.z)
				colorAdd += fractal->foldColor.difs0000.z
										* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);
			if (rrCol > fractal->transformCommon.minR2p25)
				colorAdd +=
					fractal->foldColor.difs0000.w * (rrCol - fractal->transformCommon.minR2p25) / 100.0f;
		}
		aux->color += colorAdd;
	}
	return z;
}