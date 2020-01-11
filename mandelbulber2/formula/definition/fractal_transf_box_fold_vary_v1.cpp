/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Box Fold VaryV1. Varies folding limit based on iteration conditions
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfBoxFoldVaryV1::cFractalTransfBoxFoldVaryV1() : cAbstractFractal()
{
	nameInComboBox = "T>Box Fold VaryV1";
	internalName = "transf_box_fold_vary_v1";
	internalID = fractal::transfBoxFoldVaryV1;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBoxFoldVaryV1::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	double limit = fractal->mandelbox.foldingLimit;
	double tempVC = limit; // constant to be varied

	if (aux.i >= fractal->transformCommon.startIterations250
			&& aux.i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					!= 0))
	{
		int iterationRange =
			fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250;
		int currentIteration = (aux.i - fractal->transformCommon.startIterations250);
		tempVC += fractal->transformCommon.offset * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset);
	}

	limit = tempVC;
	double value = fractal->transformCommon.scale2 * limit;

	if (z.x > limit)
	{
		z.x = value - z.x;
	}
	else if (z.x < -limit)
	{
		z.x = -value - z.x;
	}
	if (z.y > limit)
	{
		z.y = value - z.y;
		;
	}
	else if (z.y < -limit)
	{
		z.y = -value - z.y;
	}
	double zLimit = limit * fractal->transformCommon.scale1;
	double zValue = value * fractal->transformCommon.scale1;
	if (z.z > zLimit)
	{
		z.z = zValue - z.z;
	}
	else if (z.z < -zLimit)
	{
		z.z = -zValue - z.z;
	}
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor.z;
	}
	aux.DE *= fractal->analyticDE.scale1;
}
