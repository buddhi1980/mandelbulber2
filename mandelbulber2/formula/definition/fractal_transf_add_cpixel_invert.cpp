/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Adds Cpixel constant to z vector
 */

#include "all_fractal_definitions.h"

cFractalTransfAddCpixelInvert::cFractalTransfAddCpixelInvert() : cAbstractFractal()
{
	nameInComboBox = "T>Add Cpixel Invert";
	internalName = "transf_add_cpixel_invert";
	internalID = fractal::transfAddCpixelInvert;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddCpixelInvert::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 cv = aux.const_c;

	if (!fractal->transformCommon.functionEnabledAxFalse) cv.x = 1.0 / cv.x;
	if (!fractal->transformCommon.functionEnabledAyFalse) cv.y = 1.0 / cv.y;
	if (!fractal->transformCommon.functionEnabledAzFalse) cv.z = 1.0 / cv.z;

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		cv.x = min(fabs(cv.x), fractal->transformCommon.scale1);
		cv.y = min(fabs(cv.y), fractal->transformCommon.scale1);
		cv.z = min(fabs(cv.z), fractal->transformCommon.scale1);
	}

	if (fractal->transformCommon.functionEnabledBFalse)
	{
		cv.x *= sign(aux.const_c.x);
		cv.y *= sign(aux.const_c.y);
		cv.z *= sign(aux.const_c.z);
	}

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		cv.x *= sign(z.x);
		cv.y *= sign(z.y);
		cv.z *= sign(z.z);
	}

	z.x += cv.x * fractal->transformCommon.constantMultiplier111.x;
	z.y += cv.y * fractal->transformCommon.constantMultiplier111.y;
	z.z += cv.z * fractal->transformCommon.constantMultiplier111.z;

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	// aux->color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux.color += fabs(cv.x * cv.y) * fractal->foldColor.difs0000.x;
		aux.color += (cv.x * cv.x + cv.y * cv.y) * fractal->foldColor.difs0000.y;
		aux.color += fabs(cv.z) * fractal->foldColor.difs0000.z;
	}
}
