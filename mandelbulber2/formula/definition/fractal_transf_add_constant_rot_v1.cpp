/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Adds c constant to z vector
 * This formula contains aux.pos_neg
 */

#include "all_fractal_definitions.h"

cFractalTransfAddConstantRotV1::cFractalTransfAddConstantRotV1() : cAbstractFractal()
{
	nameInComboBox = "T>Add Constant Rot V1";
	internalName = "transf_add_constant_rot_v1";
	internalID = fractal::transfAddConstantRotV1;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddConstantRotV1::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 rotadd = fractal->transformCommon.additionConstantA000;
	rotadd = fractal->transformCommon.rotationMatrix.RotateVector(rotadd);

	if (!fractal->transformCommon.functionEnabledFalse)
	{
		z += aux.pos_neg * rotadd;
	}
	else // iter controls
	{
		if (aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA)
			z.x += aux.pos_neg * rotadd.x;
		if (aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
			z.y += aux.pos_neg * rotadd.y;
		if (aux.i >= fractal->transformCommon.startIterationsC
				&& aux.i < fractal->transformCommon.stopIterationsC)
			z.z += aux.pos_neg * rotadd.z;
	}
	// update for next
	aux.pos_neg *= fractal->transformCommon.scaleNeg1;

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
