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
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingTransform::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{ // piriform
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		z.y = fabs(z.y);
		double psi = M_PI / fractal->transformCommon.int2;
		psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
		z += fractal->transformCommon.offsetA000;
	}

	if (aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR1)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	z += fractal->transformCommon.offset000;
	z *= fractal->transformCommon.scale1;
	aux.DE *= fractal->transformCommon.scale1;

	CVector4 zc = z;

	// swap axis
	if (fractal->transformCommon.functionEnabledSFalse)
	{
		double temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}

	//double u = pow(zc.x, fractal->transformCommon.int2);
	double u = pow(zc.x, fractal->transformCommon.scale2);


	double r = u * zc.x + zc.y * zc.y + zc.z * zc.z + fractal->transformCommon.offsetB0;
	r = (r < 0.0f) ? 0.0f : sqrt(r);
	double t = u + fractal->transformCommon.offsetC0;
	t = (t < 0.0f) ? 0.0f : sqrt(t);
	t = r - t;

	if (fractal->transformCommon.offset0 > 0.0)
		t = min(t, fabs(zc.x) - fractal->transformCommon.offset0);

	// z.z clip
	if (!fractal->transformCommon.functionEnabledCFalse)
		t = max(t, fractal->transformCommon.offsetA0 - zc.x);

	if (fractal->transformCommon.functionEnabledKFalse)
	{
		if (!fractal->transformCommon.functionEnabledIFalse)
			t = sqrt(t * t + zc.z * zc.z);
		else t = max(fabs(t), fabs(zc.z));

		t -= fractal->transformCommon.offset0005;
		aux.DE += 1.0;
	}

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	if (aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
				t = min(aux.dist, t / aux.DE);

	double colDist = aux.dist;
	aux.dist = t;

	// aux.color
	if (aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addColor = 0.0;
		if (aux.dist == colDist) addColor += fractal->foldColor.difs0000.x;
		if (aux.dist != colDist) addColor += fractal->foldColor.difs0000.y;
		aux.color += addColor;
	}

	//aux.dist *= fractal->transformCommon.scaleA1 / (aux.DE + fractal->analyticDE.offset0);

	if (fractal->transformCommon.functionEnabledZcFalse
			&& aux.i >= fractal->transformCommon.startIterationsZc
			&& aux.i < fractal->transformCommon.stopIterationsZc)
		z = zc;
}
