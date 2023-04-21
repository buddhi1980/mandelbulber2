/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MengerV3Iteration
 * Based on a fractal proposed by Buddhi, with a DE outlined by Knighty:
 * http://www.fractalforums.com/3d-fractal-generation/revenge-of-the-half-eaten-menger-sponge/
 */

#include "all_fractal_definitions.h"

cFractalMengerV3::cFractalMengerV3() : cAbstractFractal()
{
	nameInComboBox = "Menger V3";
	internalName = "menger_v3";
	internalID = fractal::mengerV3;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double t;
	// abs z
	if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAz) z.z = fabs(z.z);
	// folds
	if (fractal->transformCommon.functionEnabledFalse)
	{
		// polyfold
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			z.x = fabs(z.x);
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

	// scale
	z *= fractal->transformCommon.scale1;
	aux.DE *= fractal->transformCommon.scale1;

	// DE
	CVector4 zc = z;

	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		int k = 0.0;
		double e = 0.0;
		double rr = 0.0;
		CVector4 one = CVector4(1.0, 1.0, 1.0, 0.0);

		zc += one;
		double modOff = fractal->transformCommon.offset3;
		aux.DE += fractal->analyticDE.offset0;
		int count = fractal->transformCommon.int8X;
		for (k = 0; k < count && rr < 10.0; k++)
		{
			zc += fractal->transformCommon.offset000;
			if (fractal->transformCommon.functionEnabledRFalse
						&& k >= fractal->transformCommon.startIterationsR
						&& k < fractal->transformCommon.stopIterationsR)
			{
				zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
				zc += fractal->transformCommon.offsetA000;
			}

			t = 0.5 * modOff;
			CVector4 pp = z;
			pp.x = fmod(zc.x * aux.DE, modOff) - t;
			pp.y = fmod(zc.y * aux.DE, modOff) - t;
			pp.z = fmod(zc.z * aux.DE, modOff) - t;

			rr = pp.Dot(pp);

			aux.DE0 = max(aux.DE0, (fractal->transformCommon.offset1 - pp.Length()) / aux.DE);
			aux.DE *= fractal->transformCommon.scale3;
		}
		if (!fractal->transformCommon.functionEnabledAFalse)
		{
			// Use this to crop to a sphere:

			if (!fractal->transformCommon.functionEnabledBFalse)
				e = z.Length();
			else
				e = zc.Length();
			e = clamp(e - fractal->transformCommon.offset2, 0.0, 100.0);
			t = max(aux.DE0, e);
		}
		else
		{
			t = aux.DE0;
		}
		t = t * fractal->analyticDE.scale1;
		double colDist = aux.dist;
		if (!fractal->analyticDE.enabledFalse)
			aux.dist = t;
		else
			aux.dist = min(aux.dist, t);

		if (fractal->foldColor.auxColorEnabledFalse)
		{
			double colorAdd = 0.0;
			if (colDist != aux.dist) colorAdd = fractal->foldColor.difs0000.x;
			if (t <= e) colorAdd = fractal->foldColor.difs0000.y;

			aux.color += colorAdd;
		}
	}
}
