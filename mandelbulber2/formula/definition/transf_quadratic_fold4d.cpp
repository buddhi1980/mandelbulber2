/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfQuadraticFold4d::cFractalTransfQuadraticFold4d() : cAbstractFractal()
{
	nameInComboBox = "T>Quadratic Fold 4D";
	internalName = "transf_quadratic_fold4d";
	internalID = fractal::transfQuadraticFold4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfQuadraticFold4d::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 a = fractal->transformCommon.offset1111;
	if (fractal->transformCommon.functionEnabledAx)
	{
		z.x = 1.0 - fabs(z.x);
		a.x = a.x - 1.0;
		double bx = 1.0 - a.x;
		if (z.x > 0.0) z.x = a.x * z.x * z.x + bx * z.x;
	} // x = 0 -> newx = 0 and x = 1 -> newx = 1

	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		z.y = 1.0 - fabs(z.y);
		a.y = a.y - 1.0;
		double by = 1.0 - a.y;
		if (z.y > 0.0) z.y = a.y * z.y * z.y + by * z.y;
	} // y = 0 -> newy = 0 and y = 1 -> newy = 1

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		z.z = 1.0 - fabs(z.z);
		a.z = a.z - 1.0;
		double bz = 1.0 - a.z;
		if (z.z > 0.0) z.z = a.z * z.z * z.z + bz * z.z;
	}

	if (fractal->transformCommon.functionEnabledAwFalse)
	{
		z.w = 1.0 - fabs(z.w);
		a.w = a.w - 1.0;
		double bw = 1.0 - a.w;
		if (z.w > 0.0) z.w = a.w * z.w * z.w + bw * z.w;
	}

	z += fractal->transformCommon.offset0000;

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
