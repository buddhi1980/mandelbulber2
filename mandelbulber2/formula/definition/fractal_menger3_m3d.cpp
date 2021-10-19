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

	CRotationMatrix rotM;
	rotM.RotateX(fractal->transformCommon.rotation.x * M_PI_180);
	rotM.RotateY(fractal->transformCommon.rotation.y * M_PI_180);
	rotM.RotateZ(fractal->transformCommon.rotation.z * M_PI_180);
	z = rotM.RotateVector(z);

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

	CRotationMatrix rotM2;
	rotM2.RotateX(fractal->transformCommon.rotation2.x * M_PI_180);
	rotM2.RotateY(fractal->transformCommon.rotation2.y * M_PI_180);
	rotM2.RotateZ(fractal->transformCommon.rotation2.z * M_PI_180);
	z = rotM2.RotateVector(z);

/*	if (fractal->transformCommon.angleDegC != 0.0)
	{
		t = z.x;
		z.x = z.x * fractal->transformCommon.cosC - z.y * fractal->transformCommon.sinC;
		z.y = t * fractal->transformCommon.sinC + z.y * fractal->transformCommon.cosC;
	}

	if (fractal->transformCommon.angleDegB != 0.0)
	{
		t = z.z;
		z.z = z.z * fractal->transformCommon.cosB - z.x * fractal->transformCommon.sinB;
		z.x = t * fractal->transformCommon.sinB + z.x * fractal->transformCommon.cosB;
	}

	if (fractal->transformCommon.angleDegA != 0.0)
	{
		t = z.y;
		z.y = z.y * fractal->transformCommon.cosA - z.z * fractal->transformCommon.sinA;
		z.z = t * fractal->transformCommon.sinA + z.z * fractal->transformCommon.cosA;
	}*/

	double sc1 = fractal->transformCommon.scale3 - 1.0;
	double sc2 = 0.5 * sc1 / fractal->transformCommon.scale3;
	z.z = z.z - fractal->transformCommon.offset111.z * sc2;
	z.z = -sqrt(z.z * z.z);
	z.z = z.z + fractal->transformCommon.offset111.z * sc2;

	z.x = fractal->transformCommon.scale3 * z.x - fractal->transformCommon.offset111.x * sc1;
	z.y = fractal->transformCommon.scale3 * z.y - fractal->transformCommon.offset111.y * sc1;
	z.z = fractal->transformCommon.scale3 * z.z;

	aux.DE *= fractal->transformCommon.scale3;


	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
