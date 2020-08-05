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

cFractalTransfAddNorm::cFractalTransfAddNorm() : cAbstractFractal()
{
	nameInComboBox = "T>Add Norm";
	internalName = "transf_add_norm";
	internalID = fractal::transfAddNorm;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddNorm::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.offset000;
	CVector4 zNorm = z / aux.r;
	CVector4 rotadd = fractal->transformCommon.rotationMatrix.RotateVector(zNorm);
	z += fractal->transformCommon.scale1 * rotadd;
	z -= fractal->transformCommon.offset000;
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * z.Length() / aux.r * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
