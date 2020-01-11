/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * abs add conditional2.
 */

#include "all_fractal_definitions.h"

cFractalTransfAbsAddConditional2::cFractalTransfAbsAddConditional2() : cAbstractFractal()
{
	nameInComboBox = "T>Abs Add Conditional2";
	internalName = "transf_abs_add_conditional2";
	internalID = fractal::transfAbsAddConditional2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAbsAddConditional2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 Offset = fractal->transformCommon.offset111;
	CVector4 OffsetA = fractal->transformCommon.offsetA111;

	if (fractal->transformCommon.functionEnabled) OffsetA = Offset;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (z.x < OffsetA.x) z.x = (fabs(z.x) - Offset.x) + Offset.x;
	}
	if (fractal->transformCommon.functionEnabledy)
	{
		if (z.y < OffsetA.y) z.y = (fabs(z.y) - Offset.y) + Offset.y;
	}
	if (fractal->transformCommon.functionEnabledz)
	{
		if (z.z < OffsetA.z) z.z = (fabs(z.z) - Offset.z) + Offset.z;
	}

	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}
