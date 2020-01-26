/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * knotv1
 * Based on DarkBeam formula from this thread:
 * http://www.fractalforums.com/new-theories-and-research/not-fractal-but-funny-trefoil-knot-routine
 */

#include "all_fractal_definitions.h"

cFractalKnotV1::cFractalKnotV1() : cAbstractFractal()
{
	nameInComboBox = "Knot V1";
	internalName = "knot_v1";
	internalID = fractal::knotV1;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalKnotV1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double polyfoldOrder = fractal->transformCommon.int2;

	if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	z += fractal->transformCommon.offset000;

	CVector4 zc = z;
	zc.z *= fractal->transformCommon.scaleA1;
	double mobius =
		(1.0 * fractal->transformCommon.intA1 + fractal->transformCommon.intB1 / polyfoldOrder)
		* atan2(zc.y, zc.x);

	zc.x = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.offsetA2;
	double temp = zc.x;
	double c = cos(mobius);
	double s = sin(mobius);
	zc.x = c * zc.x + s * zc.z;
	zc.z = -s * temp + c * zc.z;

	double m = 1.0 * polyfoldOrder / M_PI_2x;
	double angle1 = floor(0.5 + m * (M_PI_2 - atan2(zc.x, zc.z))) / m;

	temp = zc.x;
	c = cos(angle1);
	s = sin(angle1);
	zc.x = c * zc.x + s * zc.z;
	zc.z = -s * temp + c * zc.z;

	zc.x -= fractal->transformCommon.offset05;

	double len = sqrt(zc.x * zc.x + zc.z * zc.z);

	if (fractal->transformCommon.functionEnabledCFalse) len = min(len, max(fabs(zc.x), fabs(zc.z)));

	aux.DE0 = len - fractal->transformCommon.offset01;

	if (fractal->transformCommon.functionEnabledJFalse)
	{
		if (fractal->transformCommon.functionEnabledDFalse)
			aux.DE0 = min(aux.dist, len - fractal->transformCommon.offset01);
		if (fractal->transformCommon.functionEnabledKFalse) aux.DE0 /= aux.DE;
		if (fractal->transformCommon.functionEnabledEFalse) z = zc;
	}
	double colorDist = aux.dist;
	aux.dist = aux.DE0;

	// aux.color
	if (fractal->foldColor.auxColorEnabled)
	{
		double colorAdd = 0.0;
		double ang = (M_PI - 2.0 * fabs(atan(zc.x / zc.z))) * 2.0 / M_PI;

		if (fmod(ang, 2.0) < 1.0) colorAdd += fractal->foldColor.difs0000.x;
		colorAdd += fractal->foldColor.difs0000.y * fabs(ang);
		colorAdd += fractal->foldColor.difs0000.z * fabs(ang * zc.x);
		colorAdd += fractal->foldColor.difs0000.w * angle1;

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
