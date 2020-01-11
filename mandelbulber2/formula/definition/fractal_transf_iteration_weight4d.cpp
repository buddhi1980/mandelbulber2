/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * iteration weight 4D
 */

#include "all_fractal_definitions.h"

cFractalTransfIterationWeight4d::cFractalTransfIterationWeight4d() : cAbstractFractal()
{
	nameInComboBox = "T>Iteration Weight 4D";
	internalName = "transf_iteration_weight4d";
	internalID = fractal::transfIterationWeight4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfIterationWeight4d::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zA4 = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
	CVector4 zB4 = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

	z = (z * fractal->transformCommon.scale) + (zA4 * fractal->transformCommon.offset)
			+ (zB4 * fractal->transformCommon.offset0);
	aux.DE *= fractal->transformCommon.scale;
}
