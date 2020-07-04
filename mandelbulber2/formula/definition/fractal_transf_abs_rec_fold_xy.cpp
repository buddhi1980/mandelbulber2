/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * based of darkbeams recfold transform from Mandelbulb3D.
 */

#include "all_fractal_definitions.h"

cFractalTransfAbsRecFoldXY::cFractalTransfAbsRecFoldXY() : cAbstractFractal()
{
	nameInComboBox = "T>Abs Rec FoldXY";
	internalName = "transf_abs_rec_fold_xy";
	internalID = fractal::transfAbsRecFoldXY;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAbsRecFoldXY::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double foldX = fractal->transformCommon.offset1;
	double foldY = fractal->transformCommon.offsetA1;

	double t;
	z.x = fabs(z.x);
	z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		t = z.x;
		z.x = z.y;
		z.y = t;
	}
	t = z.x;
	z.x = z.x + z.y - fractal->transformCommon.offset0;
	z.y = t - z.y - fractal->transformCommon.offsetA0;
	if (fractal->transformCommon.functionEnabledBxFalse) z.x = -fabs(z.x);
	if (fractal->transformCommon.functionEnabledBx) z.y = -fabs(z.y);

	t = z.x;
	z.x = z.x + z.y;
	z.y = t - z.y;
	z.x *= 0.5;
	z.y *= 0.5;
	if (fractal->transformCommon.functionEnabledAx) z.x = foldX - fabs(z.x + foldX);
	if (fractal->transformCommon.functionEnabledAxFalse) z.y = foldY - fabs(z.y + foldY);

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
