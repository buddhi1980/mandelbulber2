/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Sponge code by Knighty
 */

#include "all_fractal_definitions.h"

cFractalMenger3::cFractalMenger3() : cAbstractFractal()
{
	nameInComboBox = "Menger3";
	internalName = "menger3";
	internalID = fractal::menger3;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalMenger3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 Offset = fractal->transformCommon.offset111;
	double Scale = fractal->transformCommon.scale3;

	z = fabs(z);
	z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (z.x - z.y < 0.0) swap(z.y, z.x);
	if (z.y - z.z < 0.0) swap(z.z, z.y);
	if (z.x - z.y < 0.0) swap(z.x, z.y);

	z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	z.z = fabs(z.z - FRAC_1_3 * Offset.z) + FRAC_1_3 * Offset.z;
	z = z * Scale - Offset * (Scale - 1.0);
	aux.DE = aux.DE * Scale;

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
