/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * 4D boxbulb, Formula based on Mandelbox (ABox). Extended to 4 dimensions and with variable scale
 * parameter.
 * This formula contains aux.color and aux.actualScale

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_testing4d.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 Testing4dIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL colorAdd = 0.0f;
	REAL rrCol = 0.0f;
	REAL4 zCol = z;
	REAL4 oldZ = z;

	// parabolic.w = paraOffset + iter *slope + (iter *iter *scale)
	REAL paraAddP0 = 0.0f;
	if (fractal->Cpara.enabledParabFalse)
	{
		REAL parabScale = 0.0f;
		if (fractal->Cpara.parabScale != 0.0f)
			parabScale = aux->i * aux->i * 0.001f * fractal->Cpara.parabScale;
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux->i * fractal->Cpara.parabSlope) + (parabScale);
		z.w += paraAddP0;
	}

	// sinusoidal w
	REAL sinAdd = 0.0f;
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		sinAdd =
			native_sin((aux->i + fractal->transformCommon.offset0) / fractal->transformCommon.scaleA1)
			* fractal->transformCommon.scaleC1;
		z.w += sinAdd;
	}

	/*	REAL4 temp = fractal->transformCommon.offset0000;
		REAL4 temp2 = temp * temp;

		z.x += ((8.0f * temp.x * temp2.x) / ((z.x * z.x) + (4.0f * temp2.x)) - 2.0f * temp.x) *
		sign(z.x) * fractal->transformCommon.scale1; z.y += ((8.0f * temp.y * temp2.y) / ((z.y * z.y) +
		(4.0f * temp2.y)) - 2.0f * temp.y) * sign(z.y) * fractal->transformCommon.scale1; z.z += ((8.0f
		* temp.z * temp2.z) / ((z.z * z.z) + (4.0f * temp2.z)) - 2.0f * temp.z) * sign(z.z) *
		fractal->transformCommon.scale1;
		z.w += ((8.0f * temp.w * temp2.w) / ((z.w * z.w) + (4.0f * temp2.w)) - 2.0f * temp.w) *
		sign(z.w) * fractal->transformCommon.scale1*/

	if (aux->i >= fractal->transformCommon.startIterationsB
			&& aux->i < fractal->transformCommon.stopIterationsB)
	{
		oldZ = z;
		z.x = fabs(z.x + fractal->transformCommon.offset1111.x)
					- fabs(z.x - fractal->transformCommon.offset1111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.offset1111.y)
					- fabs(z.y - fractal->transformCommon.offset1111.y) - z.y;
		z.z = fabs(z.z + fractal->transformCommon.offset1111.z)
					- fabs(z.z - fractal->transformCommon.offset1111.z) - z.z;
		z.w = fabs(z.w + fractal->transformCommon.offset1111.w)
					- fabs(z.w - fractal->transformCommon.offset1111.w) - z.w;
		zCol = z;
	}
	REAL4 temp = fractal->transformCommon.offset0000;
	REAL4 temp2 = temp * temp;
	// if (z.w < 1e-016f) z.w = 1e-016f;
	if (z.w < 1e-21f && z.w > -1e-21f) z.w = (z.w > 0) ? 1e-21f : -1e-21f;
	z.x += ((8.0f * temp.x * temp2.x) / ((z.x * z.x) + (4.0f * temp2.x)) - 2.0f * temp.x) * sign(z.x)
				 * fractal->transformCommon.scale1;
	z.y += ((8.0f * temp.y * temp2.y) / ((z.y * z.y) + (4.0f * temp2.y)) - 2.0f * temp.y) * sign(z.y)
				 * fractal->transformCommon.scale1;
	z.z += ((8.0f * temp.z * temp2.z) / ((z.z * z.z) + (4.0f * temp2.z)) - 2.0f * temp.z) * sign(z.z)
				 * fractal->transformCommon.scale1;
	z.w += ((8.0f * temp.w * temp2.w) / ((z.w * z.w) + (4.0f * temp2.w)) - 2.0f * temp.w) * sign(z.w)
				 * fractal->transformCommon.scale1;

	// r power
	if (aux->i >= fractal->transformCommon.startIterationsS
			&& aux->i < fractal->transformCommon.stopIterationsS)
	{
		REAL rr = dot(z, z);
		rrCol = rr;
		if (fractal->mandelboxVary4D.rPower != 1.0f) rr = pow(rr, fractal->mandelboxVary4D.rPower);

		// spherical fold
		// z += fractal->transformCommon.offset0000;
		if (rr < fractal->transformCommon.minR2p25)
		{
			z *= fractal->transformCommon.maxMinR2factor;
			aux->DE *= fractal->transformCommon.maxMinR2factor;
			// colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rr);
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			z *= fractal->transformCommon.maxR2d1 / rr;
			aux->DE *= fractal->transformCommon.maxR2d1 / rr;
			// colorAdd += fractal->mandelbox.color.factorSp2 * (rr - fractal->transformCommon.minR2p25);
		}
		/*else
			colorAdd += fractal->mandelbox.color.factorSp2
							* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);*/

		// z -= fractal->transformCommon.offset0000;
	}

	// scale
	REAL useScale = 1.0f;
	if (aux->i >= fractal->transformCommon.startIterationsC
			&& aux->i < fractal->transformCommon.stopIterationsC)
	{

		useScale = aux->actualScaleA + fractal->transformCommon.scale;

		z *= useScale;
		aux->DE = aux->DE * fabs(useScale);

		if (aux->i >= fractal->transformCommon.startIterationsX
				&& aux->i < fractal->transformCommon.stopIterationsX)
		{
			// update actualScale for next iteration
			REAL vary = fractal->transformCommon.scaleVary0
									* (fabs(aux->actualScaleA) - fractal->transformCommon.scaleB1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux->actualScaleA = -vary;
			else
				aux->actualScaleA = aux->actualScaleA - vary;
		}
	}

	// 6 plane rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux->i >= fractal->transformCommon.startIterationsR
			&& aux->i < fractal->transformCommon.stopIterationsR)
	{
		REAL4 tp;
		if (fractal->transformCommon.rotation44a.x != 0)
		{
			tp = z;
			REAL alpha = fractal->transformCommon.rotation44a.x * M_PI_180_F;
			z.x = tp.x * native_cos(alpha) + tp.y * native_sin(alpha);
			z.y = tp.x * -native_sin(alpha) + tp.y * native_cos(alpha);
		}
		if (fractal->transformCommon.rotation44a.y != 0)
		{
			tp = z;
			REAL beta = fractal->transformCommon.rotation44a.y * M_PI_180_F;
			z.y = tp.y * native_cos(beta) + tp.z * native_sin(beta);
			z.z = tp.y * -native_sin(beta) + tp.z * native_cos(beta);
		}
		if (fractal->transformCommon.rotation44a.z != 0)
		{
			tp = z;
			REAL gamma = fractal->transformCommon.rotation44a.z * M_PI_180_F;
			z.x = tp.x * native_cos(gamma) + tp.z * native_sin(gamma);
			z.z = tp.x * -native_sin(gamma) + tp.z * native_cos(gamma);
		}
		if (fractal->transformCommon.rotation44b.x != 0)
		{
			tp = z;
			REAL delta = fractal->transformCommon.rotation44b.x * M_PI_180_F;
			z.x = tp.x * native_cos(delta) + tp.w * native_sin(delta);
			z.w = tp.x * -native_sin(delta) + tp.w * native_cos(delta);
		}
		if (fractal->transformCommon.rotation44b.y != 0)
		{
			tp = z;
			REAL epsilon = fractal->transformCommon.rotation44b.y * M_PI_180_F;
			z.y = tp.y * native_cos(epsilon) + tp.w * native_sin(epsilon);
			z.w = tp.y * -native_sin(epsilon) + tp.w * native_cos(epsilon);
		}
		if (fractal->transformCommon.rotation44b.z != 0)
		{
			tp = z;
			REAL zeta = fractal->transformCommon.rotation44b.z * M_PI_180_F;
			z.z = tp.z * native_cos(zeta) + tp.w * native_sin(zeta);
			z.w = tp.z * -native_sin(zeta) + tp.w * native_cos(zeta);
		}
	}
	z += fractal->transformCommon.additionConstant0000;

	/*REAL4 temp = fractal->transformCommon.offset0000;
	REAL4 temp2 = temp * temp;

	z.x += ((8.0f * temp.x * temp2.x) / ((z.x * z.x) + (4.0f * temp2.x)) - 2.0f * temp.x) * sign(z.x)
				 * fractal->transformCommon.scale1;
	z.y += ((8.0f * temp.y * temp2.y) / ((z.y * z.y) + (4.0f * temp2.y)) - 2.0f * temp.y) * sign(z.y)
				 * fractal->transformCommon.scale1;
	z.z += ((8.0f * temp.z * temp2.z) / ((z.z * z.z) + (4.0f * temp2.z)) - 2.0f * temp.z) * sign(z.z)
				 * fractal->transformCommon.scale1;
	z.w += ((8.0f * temp.w * temp2.w) / ((z.w * z.w) + (4.0f * temp2.w)) - 2.0f * temp.w) * sign(z.w)
				 * fractal->transformCommon.scale1;*/

	if (aux->i >= fractal->transformCommon.startIterationsA
			&& aux->i < fractal->transformCommon.stopIterationsA)
	{
		aux->r = length(z);
		aux->DE = aux->r * aux->DE * 16.0f * fractal->analyticDE.scale1
								* native_sqrt(fractal->foldingIntPow.zFactor * fractal->foldingIntPow.zFactor + 2.0f
															+ fractal->analyticDE.offset2)
								/ SQRT_3_F
							+ fractal->analyticDE.offset1;

		z = z * 2.0f;
		REAL x2 = z.x * z.x;
		REAL y2 = z.y * z.y;
		REAL z2 = z.z * z.z;
		REAL temp = 1.0f - z2 / (x2 + y2);
		REAL4 zTemp;
		zTemp.x = (x2 - y2) * temp;
		zTemp.y = 2.0f * z.x * z.y * temp;
		zTemp.z = -2.0f * z.z * native_sqrt(x2 + y2);
		zTemp.w = z.w;
		z = zTemp;
		z.z *= fractal->foldingIntPow.zFactor;
	}

	// color updated v2.13 & mode2 v2.14
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd +=
					fractal->mandelbox.color.factor.x * (fabs(zCol.x) - fractal->mandelbox.color.factor4D.x);
			if (zCol.y != oldZ.y)
				colorAdd +=
					fractal->mandelbox.color.factor.y * (fabs(zCol.y) - fractal->mandelbox.color.factor4D.y);
			if (zCol.z != oldZ.z)
				colorAdd +=
					fractal->mandelbox.color.factor.z * (fabs(zCol.z) - fractal->mandelbox.color.factor4D.z);
			if (zCol.w != oldZ.w)
				colorAdd +=
					fractal->mandelbox.color.factor.z * (fabs(zCol.w) - fractal->mandelbox.color.factor4D.w);
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
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor4D.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor4D.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor4D.z;
			if (zCol.w != oldZ.w) colorAdd += fractal->mandelbox.color.factor4D.w;
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1;
			else if (rrCol < fractal->transformCommon.maxR2d1)
				colorAdd += fractal->mandelbox.color.factorSp2;
		}
		aux->color += colorAdd;
	}
	return z;
}