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

cFractalTransfAbsAddConstantV2::cFractalTransfAbsAddConstantV2() : cAbstractFractal()
{
	nameInComboBox = "T>Abs Add Constant V2";
	internalName = "transf_abs_add_constant_v2";
	internalID = fractal::transfAbsAddConstantV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAbsAddConstantV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x) + fractal->transformCommon.offset000.x;
	if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y) + fractal->transformCommon.offset000.y;
	if (fractal->transformCommon.functionEnabledAz) z.z = fabs(z.z) + fractal->transformCommon.offset000.z;

	if (fractal->transformCommon.functionEnabledx)
		z.x = fractal->transformCommon.offsetA000.x - fabs(fractal->transformCommon.offsetA000.x - z.x);
	if (fractal->transformCommon.functionEnabledy)
		z.y = fractal->transformCommon.offsetA000.y - fabs(fractal->transformCommon.offsetA000.y - z.y);
	if (fractal->transformCommon.functionEnabledz)
		z.z = fractal->transformCommon.offsetA000.z - fabs(fractal->transformCommon.offsetA000.z - z.z);

	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
		aux.const_c = z;
}
