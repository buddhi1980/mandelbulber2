/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsCylinderIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSCylinder::cFractalTransfDIFSCylinder() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Cylinder";
	internalName = "transf_difs_cylinder";
	internalID = fractal::transfDIFSCylinder;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSCylinder::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse) swap(zc.x, zc.z);

	double cylR = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.radius1;
	double cylH = fabs(zc.z) - fractal->transformCommon.offsetA1;

	cylR = max(cylR, 0.0);
	cylH = max(cylH, 0.0);
	double cylD = sqrt(cylR * cylR + cylH * cylH);
	cylD = min(max(cylR, cylH), 0.0) + cylD;

	aux.dist = min(aux.dist, cylD / (aux.DE + 1.0));
}
