/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * FoldCutCube
 * based on formula coded by Darkbeam in fragmentarium:
 */

#include "all_fractal_definitions.h"

cFractalFoldCutCube::cFractalFoldCutCube() : cAbstractFractal()
{
	nameInComboBox = "Fold Cut Cube";
	internalName = "fold_cut_cube";
	internalID = fractal::foldCutCube;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalFoldCutCube::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (!fractal->transformCommon.functionEnabledDFalse
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		CVector4 zc = fabs(z);
		if (zc.y > zc.x) swap(zc.x, zc.y);
		if (zc.z > zc.x) swap(zc.x, zc.z);
		if (zc.y > zc.x) swap(zc.x, zc.y);
		aux.dist = 1.0 - zc.x;
	}

	z *= fractal->transformCommon.scale015;
	aux.DE *= fractal->transformCommon.scale015;
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	z = fabs(z);

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
		if (fractal->transformCommon.functionEnabledSFalse)
		{
			double rr = z.Dot(z);
			// z += fractal->mandelbox.offset;
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
			// z -= fractal->mandelbox.offset;
		}
	}

	z -= fractal->transformCommon.offset111;
	if (z.y > z.x) swap(z.x, z.y);
	if (z.z > z.x) swap(z.x, z.z);
	if (z.y > z.x) swap(z.x, z.y);

	z -= fractal->transformCommon.offset100;

	if (z.y > z.x) swap(z.x, z.y);
	if (z.z > z.x) swap(z.x, z.z);
	if (z.y > z.x) swap(z.x, z.y);

	if (!fractal->transformCommon.functionEnabledDFalse)
	{
		double divT = 1.0 + aux.i;
		divT = fractal->transformCommon.offset05 / divT;
		aux.dist = fabs(min(divT - aux.dist, z.x / aux.DE));
	}
	else
		aux.dist = min(aux.dist, z.x / aux.DE);

	if (fractal->transformCommon.rotation2EnabledFalse)
		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	// DE tweak
	if (fractal->analyticDE.enabledFalse) aux.dist = aux.dist * fractal->analyticDE.scale1;
}
