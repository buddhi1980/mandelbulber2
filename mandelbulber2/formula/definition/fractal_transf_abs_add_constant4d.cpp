/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * abs add  constant,  z = abs( z + pre-offset) + post-offset
 */

#include "all_fractal_definitions.h"

cFractalTransfAbsAddConstant4d::cFractalTransfAbsAddConstant4d() : cAbstractFractal()
{
	nameInComboBox = "T>Abs Add Constant 4D";
	internalName = "transf_abs_add_constant4d";
	internalID = fractal::transfAbsAddConstant4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAbsAddConstant4d::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	z += fractal->transformCommon.additionConstant0000;

	if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);
	if (fractal->transformCommon.functionEnabled) z.w = fabs(z.w);

	z += fractal->transformCommon.offset0000;
}
