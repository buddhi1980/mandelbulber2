/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * KochV2Iteration
 * Based on Knighty's Kaleidoscopic IFS 3D Fractals, described here:
 * http://www.fractalforums.com/3d-fractal-generation/kaleidoscopic-%28escape-time-ifs%29/
 */

#include "all_fractal_definitions.h"

cFractalKochV2::cFractalKochV2() : cAbstractFractal()
{
	nameInComboBox = "Koch V2";
	internalName = "koch_v2";
	internalID = fractal::kochV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalKochV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	if (fractal->transformCommon.functionEnabledCx)
		if (z.y > z.x) swap(z.x, z.y);

	// folds
	if (fractal->transformCommon.functionEnabledFalse)
	{
		// diagonal2
		if (fractal->transformCommon.functionEnabledCxFalse)
			if (z.x > z.y) swap(z.x, z.y);
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

		if (fractal->transformCommon.functionEnabledGFalse)
		{
			z += fractal->mandelbox.offset;
			double rr = z.Dot(z);

			if (rr < fractal->transformCommon.minR0)
			{
				double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR0;
				z *= tglad_factor1;
				aux.DE *= tglad_factor1;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
				z *= tglad_factor2;
				aux.DE *= tglad_factor2;
			}
			z -= fractal->mandelbox.offset;
		}
	}

	double YOff = FRAC_1_3 * fractal->transformCommon.scale1;
	z.y = YOff - fabs(z.y - YOff);

	z.x += FRAC_1_3;
	if (z.z > z.x) swap(z.x, z.z);
	z.x -= FRAC_1_3;

	z.x -= FRAC_1_3;
	if (z.z > z.x) swap(z.x, z.z);
	z.x += FRAC_1_3;

	CVector4 Offset = fractal->transformCommon.offset100;
	z = fractal->transformCommon.scale3 * (z - Offset) + Offset;
	aux.DE = aux.DE * fractal->transformCommon.scale3;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	z += fractal->transformCommon.offset000;

	if (!fractal->transformCommon.functionEnabledFFalse)
	{
		aux.dist = fabs(z.Length() - Offset.Length());
		aux.dist = aux.dist / aux.DE;
	}
	else
	{
		double e = fractal->transformCommon.offset1;
		if (!fractal->transformCommon.functionEnabledEFalse)
		{
			CVector4 f = fabs(aux.c) - CVector4(e, e, e, 0.0);
			e = max(f.x, max(f.y, f.z));
		}
		else
		{
			e = clamp(aux.c.Length() - e, 0.0, 100.0); // circle
		}
		aux.dist = fabs(z.z - Offset.z);
		aux.dist = aux.dist / aux.DE;
		aux.dist = max(aux.dist, e);
	}
}
