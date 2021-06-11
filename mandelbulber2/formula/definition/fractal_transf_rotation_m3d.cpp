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

	// double temp = fractal->transformCommon.angleDegC;
	// double sinan = fractal->transformCommon.sinC;
	// double cosan = fractal->transformCommon.cosC;
	// double sinan = sin(temp);
	// double cosan = cos(temp);
	double temp = z.x;
	z.x = z.x * fractal->transformCommon.cosC - z.y * fractal->transformCommon.sinC;
	z.y = temp * fractal->transformCommon.sinC + z.y * fractal->transformCommon.cosC;

	// z.x = z.x * cosan - z.y * sinan;
	// z.y = temp * sinan + z.y * cosan;

	//temp = fractal->transformCommon.angleDegB;
	// sinan = fractal->transformCommon.sinB;
	// cosan = fractal->transformCommon.cosB;
	// sinan = sin(temp);
	// cosan = cos(temp);
	temp = z.z;
	z.z = z.z * fractal->transformCommon.cosB - z.x * fractal->transformCommon.sinB;
	z.x = temp * fractal->transformCommon.sinB + z.x * fractal->transformCommon.cosB;

	// z.z = z.z * cosan - z.x * sinan;
	// z.x = temp * sinan + z.x * cosan;

	// temp = fractal->transformCommon.angleDegA;
	// sinan = fractal->transformCommon.sinA;
	// cosan = fractal->transformCommon.cosA;
	// cosan = cos(temp);
	temp = z.y;
	z.y = z.y * fractal->transformCommon.cosA - z.z * fractal->transformCommon.sinA;
	z.z = temp * fractal->transformCommon.sinA + z.z * fractal->transformCommon.cosA;

	// z.y = z.y * cosan - z.z * sinan;
	// z.z = temp * sinan + z.z * cosan;
}
