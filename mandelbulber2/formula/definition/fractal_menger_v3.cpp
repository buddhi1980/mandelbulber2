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
	// abs z
	if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	// folds
	if (fractal->transformCommon.functionEnabledFalse)
	{
		// polyfold
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			z.x = fabs(z.x);
			int poly = fractal->transformCommon.int6;
			double psi = fabs(fmod(atan(z.y / z.x) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
			double len = sqrt(z.x * z.x + z.y * z.y);
			z.x = cos(psi) * len;
			z.y = sin(psi) * len;
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
	aux.DE *= fabs(fractal->transformCommon.scale1);

	// DE
	CVector4 zc = z;

	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		double rr = 0.0;
		CVector4 one = CVector4(1.0, 1.0, 1.0, 0.0);
		swap(zc.y, zc.z);
		zc += one;
		double modOff = fractal->transformCommon.offset3;
		aux.DE += fractal->analyticDE.offset0;
		int count = fractal->transformCommon.int8X;
		for (int k = 0; k < count && rr < 10.0; k++)
		{
			double pax = fmod(zc.x * aux.DE, modOff) - 0.5 * modOff;
			double pay = fmod(zc.y * aux.DE, modOff) - 0.5 * modOff;
			double paz = fmod(zc.z * aux.DE, modOff) - 0.5 * modOff;
			CVector4 pp = CVector4(pax, pay, paz, 0.0);

			pp += fractal->transformCommon.offsetA000;
			rr = pp.Dot(pp);

			// rotation
			if (fractal->transformCommon.functionEnabledRFalse
					&& k >= fractal->transformCommon.startIterationsR
					&& k < fractal->transformCommon.stopIterationsR)
			{
				pp = fractal->transformCommon.rotationMatrix.RotateVector(pp);
			}
			aux.DE0 = max(aux.DE0, (fractal->transformCommon.offset1 - pp.Length()) / aux.DE);
			aux.DE *= fractal->transformCommon.scale3;
		}
		if (!fractal->transformCommon.functionEnabledAFalse)
		{
			// Use this to crop to a sphere:
			double e;
			if (!fractal->transformCommon.functionEnabledBFalse)
				e = z.Length();
			else
				e = zc.Length();
			e = clamp(e - fractal->transformCommon.offset2, 0.0, 100.0);
			aux.dist = max(aux.DE0, e);
		}
		else
		{
			aux.dist = aux.DE0;
		}
		aux.dist *= fractal->analyticDE.scale1;
	}
}
