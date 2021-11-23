/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Based on a DarkBeam fold formula adapted by Knighty
 * MandalayBox  Fragmentarium /Examples/ Knighty Collection
 */

#include "all_fractal_definitions.h"

cFractalTestingTransform::cFractalTestingTransform() : cAbstractFractal()
{
	nameInComboBox = "Testing Transform";
	internalName = "testing_transform";
	internalID = fractal::testingTransform;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingTransform::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 signs ;
	signs.x = sign(aux.const_c.x);
	signs.y = sign(aux.const_c.y);
	signs.z = sign(aux.const_c.z);
	signs.w = 0.0;


	double t = 1.0;

	// spherical fold
	double rrCol = z.Dot(z);
	double m = 1.0;
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
	double rrCol2 = z.Dot(z);

	/*if (fractal->mandelbox.mainRotationEnabled) z = fractal->mandelbox.mainRot.RotateVector(z);
	z = z * fractal->mandelbox.scale;

	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;*/





	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
