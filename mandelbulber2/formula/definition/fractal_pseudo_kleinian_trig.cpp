/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Pseudo Kleinian Trig, Knighty // info amoser's complex sine formula, DE by Pupukuusikko
 * @reference https://fractalforums.org/index.php?topic=5591.0;topicseen
 * added Knighty pseudo kleinian DE
 */

#include "all_fractal_definitions.h"

cFractalPseudoKleinianTrig::cFractalPseudoKleinianTrig() : cAbstractFractal()
{
	nameInComboBox = "Pseudo Kleinian - Trig";
	internalName = "pseudo_kleinian_trig";
	internalID = fractal::pseudoKleinianTrig;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalPseudoKleinianTrig::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	// info amoser's complex sine formula, DE by Pupukuusikko

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
	CVector4 z_new = (CVector4){sxchz, sychz, cxcy_shz, 0.0};

	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z_new = fractal->transformCommon.rotationMatrix.RotateVector(z_new);

	// 4. Regulator
	if (fractal->transformCommon.functionEnabledHFalse)
	{
		// native_rsqrt is 3x faster than 1.0/sqrt
		z_new *= 1.0 / sqrt(1.0 + (sx * sx * sy * sy));
	}

	// 5. Transform & Stretch
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		z = z_new * fractal->transformCommon.scale1;
		aux.DE *= fabs(fractal->transformCommon.scale1);
	}

	z -= fractal->transformCommon.offsetF000;


	// Use Identity: sqrt(chz^2 + shz^2) = sqrt(0.5 * (ez^2 + invEz^2))
	double stretch = fractal->transformCommon.scaleF1 * sqrt(0.5 * (ez * ez + invEz * invEz));

	if (fractal->transformCommon.functionEnabledOFalse)
	{
		stretch = max(fractal->transformCommon.scaleA1, stretch);
	}

	// 6. Distance Estimation update
	aux.DE = aux.DE * fractal->analyticDE.scale1 * stretch + fractal->analyticDE.offset0;

	double colDist = aux.dist;
	if (aux.i >= fractal->analyticDE.startIterationsA
			&& aux.i < fractal->analyticDE.stopIterationsA)
	{
		// 7. Divergent Branches DE
		if (!fractal->transformCommon.functionEnabledPFalse)
		{
			aux.temp1000 = min(aux.temp1000, 1.0 /(aux.DE));
			aux.dist = aux.temp1000 - fractal->transformCommon.offsetB0;
		}
		else
		{
			// knighty pk
			double tx = z.x - fractal->transformCommon.offsetD0;
			double ty = z.y - fractal->transformCommon.offsetD0;
			double rxy = sqrt(tx * tx + ty * ty) - fractal->transformCommon.offsetC0;

			// Use native_recip for 1/length(z)
			double dst = 1.0 / z.Length();
			dst = max(rxy - fractal->transformCommon.offsetR0,
				fabs(rxy * z.z - fractal->transformCommon.offsetA0) * dst);

			dst = (dst / aux.DE) - fractal->transformCommon.offsetB0;
			aux.dist = min(aux.dist, dst);
		}
	}



	// color
	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
		&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addCol = fractal->foldColor.difs0000.x + aux.i * fractal->foldColor.difs0

			+ fractal->foldColor.difs0000.y * fabs(z.z)

			+ (oldZ - z).Length() * fractal->foldColor.difs0000.z

			+ fabs(oldZ.z - z.z) * fractal->foldColor.difs0000.w;



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
