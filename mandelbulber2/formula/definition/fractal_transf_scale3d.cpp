/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * scale 3d. Scale each axis individually
 */

#include "all_fractal_definitions.h"

cFractalTransfScale3d::cFractalTransfScale3d() : cAbstractFractal()
{
	nameInComboBox = "T>Scale 3D";
	internalName = "transf_scale3d";
	internalID = fractal::transfScale3d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfScale3d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z *= fractal->transformCommon.scale3D111;

	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * z.Length() / aux.r + 1.0;
	else
		aux.DE = aux.DE * z.Length() / aux.r * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
