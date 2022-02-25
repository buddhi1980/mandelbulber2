/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Buddhi Mandelbox SphericalFoldSmooth
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalFoldSmooth::cFractalTransfSphericalFoldSmooth() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Fold Smooth";
	internalName = "transf_spherical_fold_smooth";
	internalID = fractal::transfSphericalFoldSmooth;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalFoldSmooth::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{


	CVector4 oldZ = z;
	double m = 1.0;
	double t = 1.0;
	z += fractal->transformCommon.offset000;
	// spherical fold
	double rrCol = z.Dot(z);

	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = rrCol;
		if (rr < fractal->transformCommon.minR2p25)
			m = fractal->transformCommon.maxMinR2factor;
		else if (rr < fractal->transformCommon.maxR2d1)
			m = fractal->transformCommon.maxR2d1 / rr;
		z *= m;
		aux.DE *= m;
	}

	if (fractal->transformCommon.functionEnabledCy
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		double rr = z.Dot(z);

		double rk1 = SmoothConditionALessB(rr, fractal->transformCommon.minR2p25, fractal->transformCommon.scaleA3);
		double sm1 = (fractal->transformCommon.maxMinR2factor * rk1) + (1.0f - rk1);

		t = 1.0;
		if (aux.i >= fractal->transformCommon.startIterationsY
				&& aux.i < fractal->transformCommon.stopIterationsY)
		{
			double rk2 = SmoothConditionALessB(rr, fractal->transformCommon.maxR2d1, fractal->transformCommon.scaleB3);
			double rk21 = (1.0f - rk1) * rk2;
			t = (1.0f - rk21) + (fractal->transformCommon.maxR2d1 / rr * rk21);
		}
		t = sm1 * t;
		z = z * t;
		aux.DE = aux.DE * t;
	}

	z -= fractal->transformCommon.offset000;
	double rrCol2 = z.Dot(z);

	/*if (fractal->mandelbox.mainRotationEnabled) z = fractal->mandelbox.mainRot.RotateVector(z);
	z = z * fractal->mandelbox.scale;

	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;*/

	//REAL rrCol2 = dot(z, z);
	//REAL rr = dot(z, z);
	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colorAdd = 0.0f;
		oldZ = fabs(oldZ - z);

		//colorAdd += rrCol1 * fractal->foldColor.difs0000.x;
		//colorAdd += rrCol2 * fractal->foldColor.difs0000.y;
		//colorAdd += fractal->foldColor.difs0000.z;
		colorAdd += oldZ.x * fractal->foldColor.difs0000.x;
		colorAdd += oldZ.y * fractal->foldColor.difs0000.y;
		colorAdd += oldZ.z * fractal->foldColor.difs0000.z;
		colorAdd += t * m * fractal->foldColor.difs0000.w;

		if (!fractal->foldColor.auxColorEnabled)
			aux.color = colorAdd;
		else
			aux.color += colorAdd;
	}



	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
