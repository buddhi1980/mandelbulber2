/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Mandelbar or Tricorn
 * ref: https://en.wikipedia.org/wiki/Tricorn_(mathematics)
 */

#include "all_fractal_definitions.h"

cFractalMandelbar::cFractalMandelbar() : cAbstractFractal()
{
	nameInComboBox = "Mandelbar";
	internalName = "mandelbar";
	internalID = fractal::mandelbar;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbar::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	aux.DE = aux.DE * 2.0 * aux.r;
	CVector4 dd = fractal->transformCommon.constantMultiplier122;
	dd.x = z.x * z.x * dd.x - z.y * z.y - z.z * z.z;
	dd.y = -dd.y * z.x * z.y;
	dd.z = dd.z * z.x * z.z;
	z = dd;

	// offset (Julia)
	z += fractal->transformCommon.additionConstant000;

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
