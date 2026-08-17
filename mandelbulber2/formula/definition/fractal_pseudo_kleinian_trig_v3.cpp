/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Pseudo Kleinian Trig, amoser's complex sine formula
 * @reference https://fractalforums.org/index.php?topic=5591.0;topicseen
 */

#include "all_fractal_definitions.h"
#include "src/fractal.h"

cFractalPseudoKleinianTrigV3::cFractalPseudoKleinianTrigV3() : cAbstractFractal()
{
	nameInComboBox = "Pseudo Kleinian - Trig V3";
	internalName = "pseudo_kleinian_trig_v3";
	internalID = fractal::pseudoKleinianTrigV3;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalPseudoKleinianTrigV3::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{ // info amoser's complex sine formula, DE by amoser
	CVector4 oldZ = z;
	double temp = 0.0;

	// pi scale
	if (fractal->transformCommon.functionEnabledTFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		temp = M_PI * fractal->transformCommon.scaleC1;
		z *= temp;
		aux.DE *= fabs(temp);
	}

	// sphere inversion (Pre-Trig)
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		z += fractal->mandelbox.offset;
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
		// Combine the magnitude-based inversion
		double invRR = 1.0 / z.Dot(z);
		z *= invRR;
		aux.DE *= invRR;

		z -= fractal->mandelbox.offset + fractal->transformCommon.additionConstant000;
	}

	// 1. Fold & Offset (Pre-Trig)
	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
		z -= fractal->transformCommon.offsetA000;
	}

	// 2. Trigonometry & Hyperbolics
	double sx, cx, sy, cy;
	// Use sincos to compute pairs in half the time
	sx = sin(z.x);
	cx = cos(z.x);
	sy = sin(z.y);
	cy = cos(z.y);

	double ez = exp(z.z);
	double invEz = 1.0 / (ez);
	double shz = 0.5f * (ez - invEz);
	double chz = 0.5f * (ez + invEz);

	// 3. Pre-compute components for 'z' update
	double sxchz = sx * chz;
	double sychz = sy * chz;
	double cxcy_shz = (cx * cy) * shz;
	z = CVector4(sxchz, sychz, cxcy_shz, 0.0);

	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	// 4. Regulator
	if (fractal->transformCommon.functionEnabledHFalse)
	{
		// native_rsqrt is 3x faster
		double reg = 1.0 / sqrt(1.0 + (sx * sx * sy * sy));
		z *= reg;
		aux.DE *= reg;
	}

	// 5. Transform & Stretch
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		z *= fractal->transformCommon.scale1;
		aux.DE *= fabs(fractal->transformCommon.scale1);
	}
	z -= fractal->transformCommon.offsetF000;

	// Use Identity: sqrt(chz^2 + shz^2) = sqrt(0.5 * (ez^2 + invEz^2))
	double stretch =1.0;
	if (fractal->transformCommon.functionEnabledNFalse)
	{
		double ez2 = ez * ez;
		double invEz2 = invEz * invEz;
		stretch = fractal->transformCommon.scaleF1 * sqrt(0.5f * (ez2 + invEz2));
		stretch = max(fractal->transformCommon.scaleA1, stretch);
	}

	// 6. Distance Estimation update
	aux.DE = aux.DE * fractal->analyticDE.scale1 * stretch + fractal->analyticDE.offset0;

	if (fractal->analyticDE.enabled)
	{
		double cos_x2 = cx * cx;
		double cos_y2 = cy * cy;

		double sinh_z2 = shz * shz;
		double A = cos_x2 + cos_y2;
		double B = cos_x2 * cos_y2;

		// Combined math multiplication multipliers
		double S = fractal->transformCommon.scaleG1 * (sinh_z2 * (2.0 + A - B) + A + B) * FRAC_1_3;
	//	S = max(S, 0.0); // nnnnnnnnn 0.5f

		aux.DE *= S;
		aux.DE0 = max(aux.DE0, aux.DE);

		if (aux.i >= fractal->analyticDE.startIterationsA
			&& aux.i < fractal->analyticDE.stopIterationsA)
		{
			aux.dist = 1.0f * 1.0 / sqrt(aux.DE0);  // scale?
		}
	}
	else
	{
		aux.pseudoKleinianDE = -fractal->transformCommon.offsetR0; // pK DE
	}

	// color
	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
		&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addCol = fabs(z.x * z.y) * fractal->foldColor.difs0;
		addCol += fractal->foldColor.difs0000.x * stretch
			+ fractal->foldColor.difs0000.y * fabs(z.z)
			+ fractal->foldColor.difs0000.w * fabs(oldZ.z - z.z);
		if (fractal->foldColor.difs0000.z != 0.0)
			addCol += fractal->foldColor.difs0000.z * (oldZ - z).Length();

		if (!fractal->foldColor.auxColorEnabledBFalse)
		{
			aux.color += addCol;
		}
		else
		{
			if ((fractal->foldColor.int0 + aux.i) % fractal->foldColor.int2 == 0)
				aux.color += addCol;
		}
	}
}
