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
	double a = fractal->transformCommon.intA1;
	double b = fractal->transformCommon.intB1;
	double polyfoldOrder = fractal->transformCommon.int2;

	if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	z += fractal->transformCommon.offset000;
	//z *= fractal->transformCommon.scale1;
	//z += fractal->transformCommon.offset000;

	//aux.DE *= fractal->transformCommon.scale1;

	CVector4 zc = z;

	zc.z *= fractal->transformCommon.scaleA1;
	double mobius = double(a + b / polyfoldOrder) * atan2(zc.y, zc.x);

	zc.x = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.offsetA2;
	double temp = zc.x;
	double c = cos(mobius);
	double s = sin(mobius);
	zc.x = c * zc.x + s * zc.z;
	zc.z = -s * temp + c * zc.z;

	double m = float(polyfoldOrder) / M_PI_2x;
	double angle1 = floor(0.5 + m * (M_PI_2 - atan2(zc.x, zc.z))) / m;

	temp = zc.y;
	c = cos(fractal->transformCommon.offset0);
	s = sin(fractal->transformCommon.offset0);
	zc.y = c * zc.y + s * zc.z;
	zc.z = -s * temp + c * zc.z;

	temp = zc.x;
	c = cos(angle1);
	s = sin(angle1);
	zc.x = c * zc.x + s * zc.z;
	zc.z = -s * temp + c * zc.z;

	zc.x -= fractal->transformCommon.offset05;

	double len = sqrt(zc.x * zc.x + zc.z * zc.z);

	if (fractal->transformCommon.functionEnabledCFalse) len = min(len, max(abs(zc.x), abs(zc.z)));

	if (fractal->transformCommon.functionEnabledEFalse) z = zc;
	if (!fractal->transformCommon.functionEnabledDFalse)
		aux.DE0 = len - fractal->transformCommon.offset01;
	else
		aux.DE0 = min(aux.dist, len - fractal->transformCommon.offset01);
	aux.dist = aux.DE0 / aux.DE;
}
