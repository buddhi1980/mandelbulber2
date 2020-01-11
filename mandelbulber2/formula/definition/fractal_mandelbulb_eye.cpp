/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * 3D Version of the 2D Eye Fractal created by biberino, modified mclarekin
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbEye::cFractalMandelbulbEye() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Eye";
	internalName = "mandelbulb_eye";
	internalID = fractal::mandelbulbEye;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 2.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbEye::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r;

	// pre abs
	if (fractal->buffalo.preabsx) z.x = fabs(z.x);
	if (fractal->buffalo.preabsy) z.y = fabs(z.y);
	if (fractal->buffalo.preabsz) z.z = fabs(z.z);

	// bulb
	CVector4 zz = z * z;
	double rr = zz.x + zz.y + zz.z;
	double temp = sqrt(zz.x + zz.y);
	double theta1 = atan2(temp, z.z) * fractal->transformCommon.scaleB1;
	double theta2 = atan2(temp, -z.z) * fractal->transformCommon.scaleC1;
	temp = theta1 + theta2;

	double phi1 = atan2(z.y, z.x);
	double phi2 = 0.0;
	if (!fractal->transformCommon.functionEnabledFalse)
		phi2 = atan2(-z.y, z.x);
	else
		phi2 = M_PI - phi1;
	phi1 *= fractal->transformCommon.scale1;
	phi1 += phi2;

	temp = rr * sin(theta1 + theta2);
	z.x = temp * cos(phi1);
	z.y = temp * sin(phi1);
	z.z = rr * cos(theta1 + theta2);

	// post abs
	z.x = fractal->buffalo.absx ? fabs(z.x) : z.x;
	z.y = fractal->buffalo.absy ? fabs(z.y) : z.y;
	z.z = fractal->buffalo.absz ? fabs(z.z) : z.z;

	// offset
	z += fractal->transformCommon.additionConstantA000;

	// analyticDE controls
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
