/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * scale - offsetV2  abs
 */

#include "all_fractal_definitions.h"

cFractalTransfScaleOffsetV2::cFractalTransfScaleOffsetV2() : cAbstractFractal()
{
	nameInComboBox = "T>Scale OffsetV2";
	internalName = "transf_scale_offset_v2";
	internalID = fractal::transfScaleOffsetV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfScaleOffsetV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.additionConstantA000;

	if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);

	z -= fractal->transformCommon.offsetA000;

	z *= fractal->transformCommon.scale1;
	z += fractal->transformCommon.additionConstant000;
	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset0;
}
