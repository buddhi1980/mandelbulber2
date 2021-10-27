/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * sin and cos
 */

#include "all_fractal_definitions.h"

cFractalTransfSincos::cFractalTransfSincos() : cAbstractFractal()
{
	nameInComboBox = "T>SinCos";
	internalName = "transf_sincos";
	internalID = fractal::transfSincos;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSincos::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	double ang = sqrt(z.x * z.x + z.y * z.y) * fractal->transformCommon.scaleA0
			+ z.z * fractal->transformCommon.scaleB0
			+ fractal->transformCommon.angleDegA;
	double cosA = cos(ang);
	double sinB = sin(ang);
	double temp = z.x;
	z.x = z.x * cosA - z.y * sinB;
	z.y = temp * sinB + z.y * cosA;

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}
