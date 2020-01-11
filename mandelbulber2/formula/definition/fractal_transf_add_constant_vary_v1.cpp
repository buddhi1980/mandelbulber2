/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Adds c constant to z vector. C addition constant varies based on iteration parameters.
 */

#include "all_fractal_definitions.h"

cFractalTransfAddConstantVaryV1::cFractalTransfAddConstantVaryV1() : cAbstractFractal()
{
	nameInComboBox = "T>Add Constant VaryV1";
	internalName = "transf_add_constant_vary_v1";
	internalID = fractal::transfAddConstantVaryV1;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddConstantVaryV1::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempVC = fractal->transformCommon.additionConstant000; // constant to be varied
	if (aux.i < fractal->transformCommon.startIterations250)
	{
		;
	}
	if (aux.i >= fractal->transformCommon.startIterations250
			&& aux.i < fractal->transformCommon.stopIterations
			&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250
					!= 0))
	{
		int iterationRange =
			fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250;
		int currentIteration = (aux.i - fractal->transformCommon.startIterations250);
		tempVC += fractal->transformCommon.offset000 * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterations)
	{
		tempVC = (tempVC + fractal->transformCommon.offset000);
	}
	z += tempVC;
}
