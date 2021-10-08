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
	// Q_UNUSED(aux);
	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	double temp = z.z;
	double ang = sqrt(z.x * z.x + z.y * z.y);
	ang = fractal->transformCommon.angleDegA + ang * fractal->transformCommon.scaleA0
			+ temp * fractal->transformCommon.scaleB0;
	double cosA = cos(ang);
	double sinB = sin(ang);
	temp = z.x;
	z.x = z.y * cosA + z.x * sinB;
	z.y = temp * cosA - z.y * sinB;
}
