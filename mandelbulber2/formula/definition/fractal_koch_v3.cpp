/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * KochV3 Baird Delta
 * formula coded by Knighty in fragmentarium:
 * based on formula by Eric Baird
 * https://www.researchgate.net/publication/262600735_The_Koch_curve_in_three_dimensions
 */

#include "all_fractal_definitions.h"

cFractalKochV3::cFractalKochV3() : cAbstractFractal()
{
	nameInComboBox = "Koch V3";
	internalName = "koch_v3";
	internalID = fractal::kochV3;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalKochV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double beta = fractal->transformCommon.angle72 * M_PI / 360.0;
	double tc = tan(beta);
	double tsq = sqrt(3.0 * tc * tc - 1.0) * 0.25;

	CVector4 fl1 = (CVector4(1.0, 0.0, -2.0 * tsq, 0.0));
	fl1 = fl1 / sqrt(1.0 + 4.0 * tsq * tsq);
	tc = cos(beta);

	double BDscale = fractal->transformCommon.scale4 * tc * tc;

	// Sierpinski triangle symmetry + fold about xy, plane plus folds inserted
	if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAz) z.z = fabs(z.z);

	// folds
	if (fractal->transformCommon.functionEnabledFalse)
	{
		// diagonal
		if (fractal->transformCommon.functionEnabledCxFalse)
			if (z.y > z.x) swap(z.x, z.y);
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
			if (z.x < xOffset) z.x = fabs(z.x + xOffset) - xOffset;
		}
	}

	// double Pid6 = M_PI / 6.0;
	double CPid6 = SQRT_3_4; //cos(Pi / 6);
	double SPid6 = -0.5; // sin(Pi / 6);
	double t = 2.0 * min(0.0, z.x * CPid6 + z.y * SPid6);
	z.x -= t * CPid6;
	z.y -= t * SPid6;
	z.y = fabs(z.y);

	// Koch curve fold
	z.x -= 0.5;
	z.z -= tsq;
	t = 2.0 * min(0.0, z.Dot(fl1));
	z -= t * fl1;
	z.x += 0.5;
	z.z += tsq;
	z.x -= 1.0;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	// scale
	z *= BDscale;
	aux.DE *= BDscale;
	z.x += 1.0;

	aux.dist = (z.Length() - 3.0) / aux.DE;
}
