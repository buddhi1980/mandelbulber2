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

	double rxz = sqrt(zc.x * zc.x + zc.z * zc.z);
	double ang = atan2(zc.z, zc.x);
	double y = zc.y;

	for(int n = 0; n < fractal->transformCommon.int1; n++)
	{
	zc = CVector4(rxz, y, ang + M_PI_2x * n, 0.0);

	zc.x -= fractal->transformCommon.offsetA2;

	double ra = zc.z * fractal->transformCommon.int2 / fractal->transformCommon.int3;
	double raz = zc.z *fractal->transformCommon.int6 / fractal->transformCommon.int3;

	zc.x = zc.x - (fractal->transformCommon.offset1 * cos(ra) + fractal->transformCommon.offsetA2);
	zc.y = zc.y - (fractal->transformCommon.offset1 * sin(raz) + fractal->transformCommon.offsetA2);

	double twist = sqrt(zc.x * zc.x +  zc.y * zc.y) - fractal->transformCommon.offset01;
	if (fractal->transformCommon.functionEnabledCFalse) twist = min(twist, max(abs(zc.x), abs(zc.z)));

	aux.dist = min(aux.dist, twist);
	}
	// aux.color
	if (fractal->foldColor.auxColorEnabled)
	{
		double colorAdd = 0.0;
		double ang = (M_PI - 2.0 * fabs(atan(zc.x / zc.z))) * 2.0 / M_PI;

		if (fmod(ang, 2.0) < 1.0) colorAdd += fractal->foldColor.difs0000.x;
		colorAdd += fractal->foldColor.difs0000.y * fabs(ang);
		colorAdd += fractal->foldColor.difs0000.z * fabs(ang * zc.x);
//		colorAdd += fractal->foldColor.difs0000.w * angle1;

		colorAdd += fractal->foldColor.difs1;
		if (fractal->foldColor.auxColorEnabledA)
		{
//			if (colorDist != aux.dist) aux.color += colorAdd;
		}
		else
			aux.color += colorAdd;
	}

	// DE tweak
	if (fractal->analyticDE.enabledFalse) aux.dist = aux.dist * fractal->analyticDE.scale1;
}
