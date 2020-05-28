/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDeLinearCube
 *
 */

#include "all_fractal_definitions.h"

cFractalTransfDELinearCube::cFractalTransfDELinearCube() : cAbstractFractal()
{
	nameInComboBox = "T>DE Linear Cube";
	internalName = "transf_de_linear_cube";
	internalID = fractal::transfDELinearCube;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDELinearCube::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double R;
	if (!fractal->transformCommon.functionEnabledFalse)
	{
		R = max(max(fabs(z.x), fabs(z.y)), fabs(z.z));
	}
	else
	{
		R = z.Length();
	}
	aux.dist =
		(fractal->transformCommon.scale1 * R / aux.DE) - fractal->transformCommon.offset0 / 100.0;

	// aux.color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux.color = fractal->foldColor.difs1 * aux.DE / R;
	}
}
