/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * rotation
 */

#include "all_fractal_definitions.h"

cFractalTransfRotationM3d::cFractalTransfRotationM3d() : cAbstractFractal()
{
	nameInComboBox = "T>Rotation M3D";
	internalName = "transf_rotation_m3d";
	internalID = fractal::transfRotationM3d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfRotationM3d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double temp = 0.0;
	if (fractal->transformCommon.angleDegC != 0.0)
	{
		temp = z.x;
		z.x = z.x * fractal->transformCommon.cosC - z.y * fractal->transformCommon.sinC;
		z.y = temp * fractal->transformCommon.sinC + z.y * fractal->transformCommon.cosC;
	}

	if (fractal->transformCommon.angleDegB != 0.0)
	{
		temp = z.z;
		z.z = z.z * fractal->transformCommon.cosB - z.x * fractal->transformCommon.sinB;
		z.x = temp * fractal->transformCommon.sinB + z.x * fractal->transformCommon.cosB;
	}

	if (fractal->transformCommon.angleDegA != 0.0)
	{
		temp = z.y;
		z.y = z.y * fractal->transformCommon.cosA - z.z * fractal->transformCommon.sinA;
		z.z = temp * fractal->transformCommon.sinA + z.z * fractal->transformCommon.cosA;
	}
}
