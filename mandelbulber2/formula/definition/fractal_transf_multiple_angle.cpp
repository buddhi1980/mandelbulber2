/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * multiple angle
 */

#include "all_fractal_definitions.h"

cFractalTransfMultipleAngle::cFractalTransfMultipleAngle() : cAbstractFractal()
{
	nameInComboBox = "T>Multiple angle";
	internalName = "transf_multiple_angle";
	internalID = fractal::transfMultipleAngle;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfMultipleAngle::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double th0 = asin(z.z / aux.r);
	double ph0 = atan2(z.y, z.x);
	double th = th0 * fractal->transformCommon.multiplication;
	double ph = ph0 * fractal->transformCommon.multiplication;
	double cth = cos(th);
	z = CVector4(cth * cos(ph), cth * sin(ph), sin(th), 0.0) * aux.r;
}
