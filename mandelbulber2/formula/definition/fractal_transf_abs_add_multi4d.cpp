/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * abs Multi - Multiple parameters for abs functions
 */

#include "all_fractal_definitions.h"

cFractalTransfAbsAddMulti4d::cFractalTransfAbsAddMulti4d() : cAbstractFractal()
{
	nameInComboBox = "T>Abs Add Multi 4D";
	internalName = "transf_abs_add_multi4d";
	internalID = fractal::transfAbsAddMulti4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAbsAddMulti4d::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempA = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 tempB = CVector4(0.0, 0.0, 0.0, 0.0);

	if (fractal->transformCommon.functionEnabledAx)
		tempA.x = fabs(z.x + fractal->transformCommon.additionConstant0000.x);

	if (fractal->transformCommon.functionEnabledx)
		tempB.x = fabs(z.x - fractal->transformCommon.offset0000.x);

	z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale1111.x);

	if (fractal->transformCommon.functionEnabledAy)
		tempA.y = fabs(z.y + fractal->transformCommon.additionConstant0000.y);

	if (fractal->transformCommon.functionEnabledy)
		tempB.y = fabs(z.y - fractal->transformCommon.offset0000.y);

	z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale1111.y);

	if (fractal->transformCommon.functionEnabledAz)
		tempA.z = fabs(z.z + fractal->transformCommon.additionConstant0000.z);

	if (fractal->transformCommon.functionEnabledz)
		tempB.z = fabs(z.z - fractal->transformCommon.offset0000.z);

	z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale1111.z);

	if (fractal->transformCommon.functionEnabledAw)
		tempA.w = fabs(z.w + fractal->transformCommon.additionConstant0000.w);

	if (fractal->transformCommon.functionEnabledz)
		tempB.w = fabs(z.w - fractal->transformCommon.offset0000.w);

	z.w = tempA.w - tempB.w - (z.w * fractal->transformCommon.scale1111.w);

	// post-offset
	z += fractal->transformCommon.offsetA0000;

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
