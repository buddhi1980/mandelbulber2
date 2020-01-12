/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Adds Cpixel constant to z vector, with symmetry
 */

#include "all_fractal_definitions.h"

cFractalTransfAddCpixelSymmetrical::cFractalTransfAddCpixelSymmetrical() : cAbstractFractal()
{
	nameInComboBox = "T>Add Cpixel - Symmetrical";
	internalName = "transf_add_cpixel_symmetrical";
	internalID = fractal::transfAddCpixelSymmetrical;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddCpixelSymmetrical::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempFAB = aux.const_c;
	if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
	if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
	if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

	tempFAB *= fractal->transformCommon.constantMultiplier111;

	z.x += sign(z.x) * tempFAB.x;
	z.y += sign(z.y) * tempFAB.y;
	z.z += sign(z.z) * tempFAB.z;
}
