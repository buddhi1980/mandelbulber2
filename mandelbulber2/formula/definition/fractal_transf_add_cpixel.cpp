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

cFractalTransfAddCpixel::cFractalTransfAddCpixel() : cAbstractFractal()
{
	nameInComboBox = "T>Add Cpixel";
	internalName = "transf_add_cpixel";
	internalID = fractal::transfAddCpixel;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddCpixel::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += aux.const_c * fractal->transformCommon.constantMultiplier111;
}
