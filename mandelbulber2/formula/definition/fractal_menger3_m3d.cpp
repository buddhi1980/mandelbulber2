/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Sponge code by Knighty
 */

#include "all_fractal_definitions.h"

cFractalMenger3M3d::cFractalMenger3M3d() : cAbstractFractal()
{
	nameInComboBox = "Menger3 - M3D";
	internalName = "menger3_m3d";
	internalID = fractal::menger3M3d;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalMenger3M3d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z = fabs(z);
	z += fractal->transformCommon.offset000;

	z = fractal->transformCommon.rotationMatrixXYZ.RotateVector(z);

	double t;
	t = z.x - z.y;
	t = 0.5 * (t - sqrt(t * t));
	z.x = z.x - t;
	z.y = z.y + t;

	t = z.x - z.z;
	t = 0.5 * (t - sqrt(t * t));
	z.x = z.x - t;
	z.z = z.z + t;

	t = z.y - z.z;
	t = 0.5 * (t - sqrt(t * t));
	z.y = z.y - t;
	z.z = z.z + t;

	z = fractal->transformCommon.rotationMatrix2XYZ.RotateVector(z);

	double sc1 = fractal->transformCommon.scale3 - 1.0;
	double sc2 = 0.5 * sc1 / fractal->transformCommon.scale3;
	z.z = z.z - fractal->transformCommon.offset111.z * sc2;
	z.z = -fabs(z.z) + fractal->transformCommon.offset111.z * sc2;

	z.x = fractal->transformCommon.scale3 * z.x - fractal->transformCommon.offset111.x * sc1;
	z.y = fractal->transformCommon.scale3 * z.y - fractal->transformCommon.offset111.y * sc1;
	z.z = fractal->transformCommon.scale3 * z.z;

	aux.DE *= fractal->transformCommon.scale3;

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
