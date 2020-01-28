/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * knotv2
 * knot thingy by knighty (2012). Based on an idea by DarkBeam from fractalforums
 *(http://www.fractalforums.com/new-theories-and-research/not-fractal-but-funny-trefoil-knot-routine/30/)
 */

#include "all_fractal_definitions.h"

cFractalKnotV2::cFractalKnotV2() : cAbstractFractal()
{
	nameInComboBox = "Knot V2";
	internalName = "knot_v2";
	internalID = fractal::knotV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalKnotV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	z += fractal->transformCommon.offset000;

	CVector4 zc = z;
	double tempA = zc.y;
	double tempB = zc.z;
	if (fractal->transformCommon.functionEnabledJFalse) swap(tempA, tempB);

	zc.z *= fractal->transformCommon.scaleA1;
	double rxz = sqrt(zc.x * zc.x + tempA * tempA);
	double ang = atan2(tempA, zc.x);
	double t = tempB;

	double colorDist = 0.0;
	for(int n = 0; n < fractal->transformCommon.int3; n++)
	{
		zc = CVector4(rxz, t, ang + M_PI_2x * n, 0.0);

		zc.x -= fractal->transformCommon.offsetA2;

		double ra = zc.z * fractal->transformCommon.int3X / fractal->transformCommon.int3Z;
		double raz = zc.z *fractal->transformCommon.int8Y / fractal->transformCommon.int3Z;

		zc.x = zc.x - (fractal->transformCommon.offset1 * cos(ra) + fractal->transformCommon.offsetA2);
		zc.y = zc.y - (fractal->transformCommon.offset1 * sin(raz) + fractal->transformCommon.offsetA2);

		aux.DE0 = sqrt(zc.x * zc.x +  zc.y * zc.y) - fractal->transformCommon.offset01;

		if (fractal->transformCommon.functionEnabledKFalse) aux.DE0 /= aux.DE;

		if (!fractal->transformCommon.functionEnabledDFalse) aux.dist = min(aux.dist, aux.DE0);
		else aux.dist = aux.DE0;
	}
	if (fractal->transformCommon.functionEnabledEFalse) z = zc;

	// aux.color
	if (fractal->foldColor.auxColorEnabled)
	{
		double colorAdd = 0.0;

		if (fmod(ang, 2.0) < 1.0) colorAdd += fractal->foldColor.difs0000.x;
		colorAdd += fractal->foldColor.difs0000.y * (zc.x);
		colorAdd += fractal->foldColor.difs0000.z * (zc.y);
		colorAdd += fractal->foldColor.difs0000.w * (zc.z);

		colorAdd += fractal->foldColor.difs1;
		if (fractal->foldColor.auxColorEnabledA)
		{
			if (colorDist != aux.dist) aux.color += colorAdd;
		}
		else
			aux.color += colorAdd;
	}

	// DE tweak
	if (fractal->analyticDE.enabledFalse) aux.dist = aux.dist * fractal->analyticDE.scale1;
}
