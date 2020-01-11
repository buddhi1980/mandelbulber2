/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * iteration weight. Influence fractal based on the weight of
 * Z values after different iterations
 */

#include "all_fractal_definitions.h"

cFractalTransfIterationWeight::cFractalTransfIterationWeight() : cAbstractFractal()
{
	nameInComboBox = "T>Iteration Weight";
	internalName = "transf_iteration_weight";
	internalID = fractal::transfIterationWeight;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfIterationWeight::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zA = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
	CVector4 zB = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

	z = (z * fractal->transformCommon.scale) + (zA * fractal->transformCommon.offset)
			+ (zB * fractal->transformCommon.offset0);
	aux.DE *= fractal->transformCommon.scale;
}
