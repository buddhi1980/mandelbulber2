/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MengerV2Iteration
 * Based on a fractal proposed by Buddhi, with a DE outlined by Knighty:
 * http://www.fractalforums.com/3d-fractal-generation/revenge-of-the-half-eaten-menger-sponge/
 */

#include "all_fractal_definitions.h"

cFractalMengerV2::cFractalMengerV2() : cAbstractFractal()
{
	nameInComboBox = "Menger V2";
	internalName = "menger_v2";
	internalID = fractal::mengerV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
		CVector4 ptFive = CVector4(0.5, 0.5, 0.5, 0.0);
		CVector4 onePtFive = CVector4(1.5, 1.5, 1.5, 0.0);
		zc = zc * 0.5 + ptFive;
		CVector4 pp = fabs(zc - ptFive) - ptFive;
		double modOff = fractal->transformCommon.offset3;
		aux.DE += fractal->transformCommon.offsetA0;
		aux.DE0 = max(pp.x, max(pp.y, pp.z));
		int count = fractal->transformCommon.int8X;
		for (int k = 0; k < count && rr < 10.0; k++)
		{
			double pax = fmod(modOff * zc.x * aux.DE, modOff);
			double pay = fmod(modOff * zc.y * aux.DE, modOff);
			double paz = fmod(modOff * zc.z * aux.DE, modOff);
			CVector4 pa = CVector4(pax, pay, paz, 0.0);

			pp = ptFive - fabs(pa - onePtFive) + fractal->transformCommon.offsetA000;
			rr = pp.Dot(pp);
			// rotation
			if (fractal->transformCommon.functionEnabledRFalse
					&& k >= fractal->transformCommon.startIterationsR
					&& k < fractal->transformCommon.stopIterationsR)
			{
				pp = fractal->transformCommon.rotationMatrix.RotateVector(pp);
			}
			double d;
			d = min(max(pp.x, pp.z), min(max(pp.x, pp.y), max(pp.y, pp.z))) / aux.DE;
			aux.DE0 = max(d, aux.DE0);

			if (fractal->transformCommon.functionEnabledEFalse)
			{
				d = max(d, (fractal->transformCommon.offset1 - pp.Length())) / aux.DE;
				aux.DE0 = max(d, aux.DE0);
			}
			aux.DE *= fractal->transformCommon.scale3;
		}
		if (!fractal->transformCommon.functionEnabledAFalse)
			aux.dist = aux.DE0;
		else
		{
			// Use this to crop to a sphere:
			double e;
			if (!fractal->transformCommon.functionEnabledBFalse)
				e = z.Length();
			else
				e = zc.Length();
			e = clamp(e - 1.0, 0.0, 100.0);
			aux.dist = max(aux.DE0, e);
		}
		aux.dist *= fractal->analyticDE.scale1;
	}
}
