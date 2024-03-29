/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2021 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * amazing surf based on Mandelbulber3D. Formula proposed by Kali, with features added by
 * DarkBeam
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_amazing_surf_klein_v2.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 AmazingSurfKleinV2Iteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	// polyfold
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux->i >= fractal->transformCommon.startIterationsP
			&& aux->i < fractal->transformCommon.stopIterationsP1)
	{
		z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledCy) z.y = fabs(z.y);
		REAL psi = M_PI_F / fractal->transformCommon.int8Y;
		psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0f * psi) - psi);
		REAL len = native_sqrt(z.x * z.x + z.y * z.y);
		z.x = native_cos(psi) * len;
		z.y = native_sin(psi) * len;
		z += fractal->transformCommon.offsetF000;
	}

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

	if (aux->i < fractal->transformCommon.int8X)
	{
		REAL4 oldZ = z;
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;

		if (fractal->transformCommon.functionEnabledIFalse
				&& aux->i >= fractal->transformCommon.startIterationsI
				&& aux->i < fractal->transformCommon.stopIterationsI)
		{
			REAL tt = z.x;
			z.x = z.y;
			z.y = tt;
		}

		if (fractal->transformCommon.functionEnabledJFalse
				&& aux->i >= fractal->transformCommon.startIterationsJ
				&& aux->i < fractal->transformCommon.stopIterationsJ)
		{
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		}
		else
		{
			REAL tt = z.y;
			z.y = z.z;
			z.z = tt;
		}
		if (fractal->transformCommon.functionEnabledSwFalse
				&& aux->i >= fractal->transformCommon.startIterationsH
				&& aux->i < fractal->transformCommon.stopIterationsH)
		{
			REAL tt = z.x;
			z.x = z.y;
			z.y = tt;
		}
		REAL4 zCol = z;

		REAL rr = dot(z, z);
		REAL MinRR = fractal->transformCommon.minR0;
		REAL dividend = rr < MinRR ? MinRR : min(rr, 1.0f);

		// scale
		REAL useScale = 1.0f;
		useScale = (aux->actualScaleA + fractal->transformCommon.scale1) / dividend;
		z *= useScale;
		aux->DE = aux->DE * fabs(useScale) + fractal->analyticDE.offset1;
		if (fractal->transformCommon.functionEnabledKFalse
				&& aux->i >= fractal->transformCommon.startIterationsK
				&& aux->i < fractal->transformCommon.stopIterationsK)
		{
			// update actualScaleA for next iteration
			REAL vary = fractal->transformCommon.scaleVary0
									* (fabs(aux->actualScaleA) - fractal->transformCommon.scaleC1);
			aux->actualScaleA -= vary;
		}

		if (fractal->transformCommon.rotation2EnabledFalse)
		{
			z = Matrix33MulFloat4(fractal->transformCommon.rotationMatrix, z);
		}
		if (aux->i >= fractal->transformCommon.startIterationsO
				&& aux->i < fractal->transformCommon.stopIterationsO)
			z += fractal->transformCommon.additionConstantA000;

		if (fractal->foldColor.auxColorEnabledFalse)
		{
			REAL colorAdd = 0.0f;
			if (zCol.x != oldZ.x)
				colorAdd += fractal->foldColor.difs0000.x
										* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
			if (zCol.y != oldZ.y)
				colorAdd += fractal->foldColor.difs0000.y
										* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
			if (zCol.z != oldZ.z)
				colorAdd += fractal->foldColor.difs0000.z
										* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);
			colorAdd += fractal->foldColor.difs0000.w * useScale;
			aux->color += colorAdd;
		}
	}
	else
	{
		if (fractal->transformCommon.functionEnabled)
		{
			z = fabs(z + fractal->transformCommon.offset110)
					- fabs(z - fractal->transformCommon.offset110) - z;

			z *= fractal->transformCommon.scale2;
			aux->DE *= fractal->transformCommon.scale2;

			z += fractal->transformCommon.offsetA000;
		}
	}
	return z;
}