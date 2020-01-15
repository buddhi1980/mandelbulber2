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

cFractalTestingTransform::cFractalTestingTransform() : cAbstractFractal()
{
	nameInComboBox = "Testing Transform";
	internalName = "testing_transform";
	internalID = fractal::testingTransform;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingTransform::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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

	z.z *= fractal->transformCommon.scaleA1;
	double mobius = double(a + b / polyfoldOrder) * atan2(zc.y, zc.x);

	zc.x = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.offsetA2;
	double temp = zc.x;
	double c = cos(mobius);
	double s = sin(mobius);
	zc.x = c * zc.x + s * zc.z;
	zc.z = -s * temp + c * zc.z;

	double m = polyfoldOrder / M_PI_2x;
	double angle1 = floor(0.5 + m * (M_PI_2 - atan2(zc.x, zc.z))) / m;

	temp = zc.y;
	c = cos(fractal->transformCommon.offsetT1);
	s = sin(fractal->transformCommon.offsetT1);
	zc.y = c * zc.y + s * zc.z;
	zc.z = -s * temp + c * zc.z;

	temp = zc.x;
	c = cos(angle1);
	s = sin(angle1);
	zc.x = c * zc.x + s * zc.z;
	zc.z = -s * temp + c * zc.z;

	zc.x -= fractal->transformCommon.offsetR1;

	double len = sqrt(zc.x * zc.x + zc.z * zc.z);

	if (fractal->transformCommon.functionEnabledCFalse) len = min(len, max(abs(zc.x), abs(zc.z)));

	/*z.z *= fractal->transformCommon.scaleA1;
	double mobius = double(a + b / polyfoldOrder) * atan2(z.y, z.x);

	z.x = sqrt(z.x * z.x + z.y * z.y) - fractal->transformCommon.offsetA2;
	double temp = z.x;
	double c = cos(mobius);
	double s = sin(mobius);
	z.x = c * z.x + s * z.z;
	z.z = -s * temp + c * z.z;

	double m = polyfoldOrder / M_PI_2x;
	double angle1 = floor(0.5 + m * (M_PI_2 - atan2(z.x, z.z))) / m;

	temp = z.y;
	c = cos(fractal->transformCommon.offsetT1);
	s = sin(fractal->transformCommon.offsetT1);
	z.y = c * z.y + s * z.z;
	z.z = -s * temp + c * z.z;

	temp = z.x;
	c = cos(angle1);
	s = sin(angle1);
	z.x = c * z.x + s * z.z;
	z.z = -s * temp + c * z.z;

	z.x -= fractal->transformCommon.offsetR1;

	double len = sqrt(z.x * z.x + z.z * z.z);

	if (fractal->transformCommon.functionEnabledCFalse) len = min(len, max(abs(z.x), abs(z.z)));*/

	if (fractal->transformCommon.functionEnabledEFalse) z = zc;
	if (!fractal->transformCommon.functionEnabledDFalse)
		aux.dist =  len - fractal->transformCommon.offset05;
	else
		aux.dist = min(aux.dist, len - fractal->transformCommon.offset05) / aux.DE;
}
