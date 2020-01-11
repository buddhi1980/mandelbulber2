/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * abs add  constant,  z = fabs( z + pre-offset) + post-offset
 */

#include "all_fractal_definitions.h"

cFractalTransfAbsAddConstant::cFractalTransfAbsAddConstant() : cAbstractFractal()
{
	nameInComboBox = "T>Abs Add Constant";
	internalName = "transf_abs_add_constant";
	internalID = fractal::transfAbsAddConstant;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAbsAddConstant::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);

	z += fractal->transformCommon.offsetA000;
}
