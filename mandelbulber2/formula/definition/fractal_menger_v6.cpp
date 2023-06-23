/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MengerV6Iteration
 * Based on code by blepfo 2020-05-28 https://www.shadertoy.com/view/wsjfzd
 * https://fractalforums.org/fragmentarium/17/simple-menger-kifs-from-shadertoy/5076
 */

#include "all_fractal_definitions.h"

cFractalMengerV6::cFractalMengerV6() : cAbstractFractal()
{
	nameInComboBox = "Menger V6";
	internalName = "menger_v6";
	internalID = fractal::mengerV6;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerV6::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double t;

	if (fractal->transformCommon.functionEnabledAFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z += fractal->transformCommon.additionConstant000;

		if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x) + fractal->transformCommon.offset000.x;
		if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y) + fractal->transformCommon.offset000.y;
		if (fractal->transformCommon.functionEnabledAz) z.z = fabs(z.z) + fractal->transformCommon.offset000.z;

		if (fractal->transformCommon.functionEnabledx)
			z.x = fractal->transformCommon.offsetA000.x - fabs(fractal->transformCommon.offsetA000.x - z.x);
		if (fractal->transformCommon.functionEnabledy)
			z.y = fractal->transformCommon.offsetA000.y - fabs(fractal->transformCommon.offsetA000.y - z.y);
		if (fractal->transformCommon.functionEnabledz)
			z.z = fractal->transformCommon.offsetA000.z - fabs(fractal->transformCommon.offsetA000.z - z.z);
	}

	// folds
	if (fractal->transformCommon.functionEnabledFalse)
	{
		// polyfold
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			z.y = fabs(z.y);
			double psi = M_PI / fractal->transformCommon.int6;
			psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
			t = sqrt(z.x * z.x + z.y * z.y);
			z.x = cos(psi) * t;
			z.y = sin(psi) * t;
		}
		// abs offsets
		if (fractal->transformCommon.functionEnabledCFalse)
		{
			double xOffset = fractal->transformCommon.offsetC0;
			if (z.x < xOffset) z.x = fabs(z.x - xOffset) + xOffset;
		}
		if (fractal->transformCommon.functionEnabledDFalse)
		{
			double yOffset = fractal->transformCommon.offsetD0;
			if (z.y < yOffset) z.y = fabs(z.y - yOffset) + yOffset;
		}
	}

	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		CVector4 n;
		double col = 0.0;
		z.y *= fractal->transformCommon.scaleA1;
		z *= 0.5;

		for (int k = 0; k < fractal->transformCommon.int8X; k++)
		{
			z *= fractal->transformCommon.scale3;
			aux.DE *= fabs(fractal->transformCommon.scale3);
			CVector4 Offset1 = fractal->transformCommon.offset222;
			z.y = z.y - (2.0 * max(z.y, 0.0)) + Offset1.y;
			z.x = -(z.x - (2.0 * max(z.x, 0.0)) + Offset1.x);

			t = fractal->transformCommon.cosA;
			n = CVector4{t * fractal->transformCommon.sinB, fractal->transformCommon.sinA, t * fractal->transformCommon.cosB, 0.0};

			t = n.Length();
			if (t == 0.0) t = 1e-21;
			n /= t;
			t = z.Dot(n) * 2.0;
			z -= max(t, 0.0) * n;

			z.z += Offset1.z;
			t = cos(fractal->transformCommon.angle45 * M_PI_180);
			n = CVector4{t * fractal->transformCommon.sinC, sin(-fractal->transformCommon.angle45 * M_PI_180), t * fractal->transformCommon.cosC, 0.0};
			t = n.Length();
			if (t == 0.0) t = 1e-21;
			n /= t;
			t = z.Dot(n) * 2.0;
			z -= max(t, 0.0) * n;

			t = max((z.x + z.y), 0.0);
			z.y = z.y - t;
			z.x = z.x - t + fractal->transformCommon.offset2;

			if (fractal->foldColor.auxColorEnabledFalse
					&& k >= fractal->foldColor.startIterationsA
							&& k < fractal->foldColor.stopIterationsA)
			{
				if (!fractal->transformCommon.functionEnabledOFalse)
				{
					if (z.z > -1.0) col += fractal->foldColor.difs0000.y; //
				}
				else
				{
					if (z.y < 1.0) col += fractal->foldColor.difs0000.y; //
				}

				if (z.x > -fractal->transformCommon.scaleA3 && z.y > -fractal->foldColor.difs1)
					col += fractal->foldColor.difs0000.z; // x y

				if (z.x > -fractal->transformCommon.scaleA3 && z.z > -fractal->foldColor.difs1)
					col += fractal->foldColor.difs0000.w; // x z

				z.x = z.x - (2.0 * max(z.x, 0.0)) + fractal->transformCommon.offsetA1;
				z.x = z.x - (2.0 * max(z.x, 0.0)) + fractal->transformCommon.offsetT1;

				if (z.x + z.y < 0.0f) col += fractal->foldColor.difs0000.x; //diag;

				aux.color = col;
			}
			else
			{
				z.x = z.x - (2.0 * max(z.x, 0.0)) + fractal->transformCommon.offsetA1;
				z.x = z.x - (2.0 * max(z.x, 0.0)) + fractal->transformCommon.offsetT1;
			}
			t = max((z.x + z.y), 0.0);
			z.x -= t;
			z.y -= t;

			if (fractal->transformCommon.functionEnabledRFalse
						&& k >= fractal->transformCommon.startIterationsR
						&& k < fractal->transformCommon.stopIterationsR)
			{
				z = fractal->transformCommon.rotationMatrix2.RotateVector(z);
			}
		}

		CVector4 edgeDist = fabs(z) - CVector4{1.0, 1.0, 1.0, 0.0};
		edgeDist.x = max(edgeDist.x, 0.0);
		edgeDist.y = max(edgeDist.y, 0.0);
		edgeDist.z = max(edgeDist.z, 0.0);
		t = edgeDist.Length(); // + min(max(edgeDist.x, max(edgeDist.y, edgeDist.z)));

		t /= aux.DE;

		if (!fractal->analyticDE.enabledFalse)
			aux.dist = t;
		else
			aux.dist = min(aux.dist, t);
	}
}
