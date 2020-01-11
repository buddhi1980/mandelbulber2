/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MandelbulbAtan22Iteration
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbAtan2Power2::cFractalMandelbulbAtan2Power2() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Atan2 Power 2";
	internalName = "mandelbulb_atan2_power2";
	internalID = fractal::mandelbulbAtan2Power2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbAtan2Power2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r;

	if (fractal->buffalo.preabsx) z.x = fabs(z.x);
	if (fractal->buffalo.preabsy) z.y = fabs(z.y);
	if (fractal->buffalo.preabsz) z.z = fabs(z.z);
	// bitwise ??
	CVector4 zz = z * z;
	double lenXY = sqrt(zz.x + zz.y);
	double theta1 = atan2(lenXY, z.z);
	double theta2 = -atan2(lenXY, -z.z);
	double phi = atan2(z.y, z.x) * fractal->transformCommon.scale1;

	double rr = z.Dot(z);
	double sinth = sin(2.0 * theta1);
	z.x = sinth * cos(2.0 * phi);
	z.y = sinth * sin(2.0 * phi);
	z.z = -cos(theta1 + theta2);
	z *= rr;

	z.x = fractal->buffalo.absx ? fabs(z.x) : z.x;
	z.y = fractal->buffalo.absy ? fabs(z.y) : z.y;
	z.z = fractal->buffalo.absz ? fabs(z.z) : z.z;

	z += fractal->transformCommon.additionConstantA000;
}
