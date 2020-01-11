/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Vicsek
 */

#include "all_fractal_definitions.h"

cFractalVicsek::cFractalVicsek() : cAbstractFractal()
{
	nameInComboBox = "Vicsek";
	internalName = "vicsek";
	internalID = fractal::vicsek;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalVicsek::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;

	// octo
	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		if (z.x + z.y < 0.0) z = CVector4(-z.y, -z.x, z.z, z.w);

		if (z.x + z.z < 0.0) // z.xz = -z.zx;
			z = CVector4(-z.z, z.y, -z.x, z.w);

		if (z.x - z.y < 0.0) // z.xy = z.yx;
			z = CVector4(z.y, z.x, z.z, z.w);

		if (z.x - z.z < 0.0) // z.xz = z.zx;
			z = CVector4(z.z, z.y, z.x, z.w);

		z.x = fabs(z.x);
		z = z * fractal->transformCommon.scale2
				- fractal->transformCommon.offset100 * (fractal->transformCommon.scale2 - 1.0);

		aux.DE *= fractal->transformCommon.scale2;
	}

	// spherical fold
	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);
		rrCol = rr; // test ooooooooooooooooooooooooooooooooo
		// if (r2 < 1e-21) r2 = 1e-21;
		if (rr < fractal->transformCommon.minR2p25)
		{
			double tglad_factor1 = fractal->transformCommon.maxMinR2factor;
			// double tglad_factor1 = fractal->transformCommon.maxR2d1 /fractal->transformCommon.minR2p25;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
			aux.color += fractal->mandelbox.color.factorSp1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
		z *= fractal->transformCommon.scale08;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale08);
	}

	// Vicsek
	if (fractal->transformCommon.functionEnabledM
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		zCol = z; // test ooooooooooooooooooooooooooooooooo
		z = fabs(z);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		z *= fractal->transformCommon.scale3;
		aux.DE *= fractal->transformCommon.scale3;

		CVector4 limit = fractal->transformCommon.offset111;
		if (z.x > limit.x * 0.5) z.x -= limit.x * fractal->transformCommon.scaleA1;
		if (z.y > limit.y * 0.5) z.y -= limit.y;
		if (z.z > limit.z) z.z -= 2.0 * limit.z;
		z.x += fractal->transformCommon.offset0;
		zCol = z; // test ooooooooooooooooooooooooooooooooo
	}

	// 45 rot XY
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		double xTemp = SQRT_1_2 * (z.x - z.y);
		z.y = SQRT_1_2 * (z.y + z.x);
		z.x = xTemp;
	}

	// iter weight
	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector4 zA = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
		CVector4 zB = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offsetA0)
				+ (zB * fractal->transformCommon.offsetB0);
		aux.DE *= fractal->transformCommon.scale1;
	}

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	// aux color
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
