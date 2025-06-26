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

cFractalKochV5::cFractalKochV5() : cAbstractFractal()
{
	nameInComboBox = "Koch V5";
	internalName = "koch_v5";
	internalID = fractal::kochV5;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalKochV5::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAx
		&& aux.i >= fractal->transformCommon.startIterationsCx
		&& aux.i < fractal->transformCommon.stopIterationsCx)
		 z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAy
		&& aux.i >= fractal->transformCommon.startIterationsCy
		&& aux.i < fractal->transformCommon.stopIterationsCy)
		 z.y = fabs(z.y);
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
			double psi = M_PI / fractal->transformCommon.int8X;
			psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
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
				// double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR0;
				z *= fractal->transformCommon.maxMinR0factor;
				aux.DE *= fractal->transformCommon.maxMinR0factor;
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
	z = z - fractal->transformCommon.offsetA000;
	double YOff = FRAC_1_3 * fractal->transformCommon.scale1;
	z.y = YOff - fabs(z.y - YOff);

	if (fractal->transformCommon.functionEnabledAFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z.x += FRAC_1_3;
		if (z.z > z.x) swap(z.x, z.z);
		z.x -= FRAC_1_3;
	}
	if (fractal->transformCommon.functionEnabledBFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x -= FRAC_1_3;
		if (z.z > z.x) swap(z.x, z.z);
		z.x += FRAC_1_3;
	}


		z = z - fractal->transformCommon.offset000;

	CVector4 Offset = fractal->transformCommon.offset100;
	z = fractal->transformCommon.scale2 * (z + Offset);
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale2);

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	z -= Offset;

	double d;
	if (!fractal->transformCommon.functionEnabledFFalse)
	{
		d = fabs(z.Length() - Offset.Length());
	}
	else
	{
		double e = fractal->transformCommon.offset1;
		CVector4 c = aux.const_c;
		if (!fractal->transformCommon.functionEnabledEFalse)
		{
			CVector4 f = fabs(c) - CVector4(e, e, e, 0.0);
			e = max(f.x, max(f.y, f.z));
		}
		else
		{
			e = clamp(c.Length() - e, 0.0, 100.0); // sphere
		}
		d = fabs(z.z - Offset.z);
		d = max(d, e);
	}
	aux.dist = d / aux.DE;
}
