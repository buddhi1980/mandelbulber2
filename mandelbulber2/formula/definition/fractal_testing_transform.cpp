/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Testing
 * Based on a fractal proposed by Buddhi, with a DE outlined by Knighty:
 * http://www.fractalforums.com/3d-fractal-generation/revenge-of-the-half-eaten-menger-sponge/
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
	double a = fractal->transformCommon.offset1;
	double b = fractal->transformCommon.offsetA1;
	double polyfoldOrder = fractal->transformCommon.offset2;

	double mobius = ((a + b) / polyfoldOrder) * atan2(z.y, z.x);

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

	aux.dist = sqrt(z.x * z.x + z.z * z.z) - fractal->transformCommon.offset05;
}
