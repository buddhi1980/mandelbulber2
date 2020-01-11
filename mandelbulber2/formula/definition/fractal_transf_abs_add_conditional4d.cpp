/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * abs add conditional4D
 */

#include "all_fractal_definitions.h"

cFractalTransfAbsAddConditional4d::cFractalTransfAbsAddConditional4d() : cAbstractFractal()
{
	nameInComboBox = "T>Abs Add Conditional 4D";
	internalName = "transf_abs_add_conditional4d";
	internalID = fractal::transfAbsAddConditional4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAbsAddConditional4d::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledx)
	{
		z.x = sign(z.x)
					* (fractal->transformCommon.offset1111.x - fabs(z.x)
						 + fabs(z.x) * fractal->transformCommon.scale0000.x);
	}

	if (fractal->transformCommon.functionEnabledy)
	{
		z.y = sign(z.y)
					* (fractal->transformCommon.offset1111.y - fabs(z.y)
						 + fabs(z.y) * fractal->transformCommon.scale0000.y);
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		z.z = sign(z.z)
					* (fractal->transformCommon.offset1111.z - fabs(z.z)
						 + fabs(z.z) * fractal->transformCommon.scale0000.z);
	}

	if (fractal->transformCommon.functionEnabledw)
	{
		z.w = sign(z.w)
					* (fractal->transformCommon.offset1111.w - fabs(z.w)
						 + fabs(z.w) * fractal->transformCommon.scale0000.w);
	}
	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}
