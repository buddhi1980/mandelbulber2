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
	CVector4 pc = aux.const_c;

	if (!fractal->transformCommon.functionEnabledAxFalse) pc.x = 1.0 / pc.x;
	if (!fractal->transformCommon.functionEnabledAyFalse) pc.y = 1.0 / pc.y;
	if (!fractal->transformCommon.functionEnabledAzFalse) pc.z = 1.0 / pc.z;

	z.x += pc.x * fractal->transformCommon.constantMultiplier111.x;
	z.y += pc.y * fractal->transformCommon.constantMultiplier111.y;
	z.z += pc.z * fractal->transformCommon.constantMultiplier111.z;

}
