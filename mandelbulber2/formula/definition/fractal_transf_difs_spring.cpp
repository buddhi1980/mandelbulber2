/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSSpring::cFractalTransfDIFSSpring() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Spring";
	internalName = "transf_difs_spring";
	internalID = fractal::transfDIFSSpring;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSSpring::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;

	zc *= fractal->transformCommon.scale1;
	aux.DE *= fractal->transformCommon.scale1;
	zc += fractal->transformCommon.offset000;

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		double temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}

	double T = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.minR05;
	int int8x = fractal->transformCommon.int8X;
	int int2 = fractal->transformCommon.int8Y;

	double pit1 = fractal->transformCommon.offset01;
	double pit2 = fractal->transformCommon.offsetp01;
	double ht1 = int8x * pit1;
	double ht2 = int2 * pit2;

	double L = fabs(zc.z) - ht1 - ht2;
	double P;
	if (fabs(zc.z) < ht1) P = pit1;
	else P = pit2;
	zc.z += atan2(zc.y, zc.x) / M_PI * P;
	zc.z = (zc.z - P * 2.0 * floor(zc.z / (P * 2.0))) - P;

	// L = max(max(T, L), 0.0);
	// L = sqrt(T * T + L * L);

	double dd;
	if (!fractal->transformCommon.functionEnabledJFalse)
		dd = sqrt(T * T + zc.z * zc.z);
	else dd = max(fabs(T), fabs(zc.z));

	dd = max(L, dd);
	if (!fractal->analyticDE.enabledFalse)
		aux.dist = (dd - fractal->transformCommon.offset0005) / (aux.DE + 1.0);
	else
		aux.dist = min(aux.dist, (dd - fractal->transformCommon.offset0005) / (aux.DE + 1.0));

	aux.DE0 = (dd - fractal->transformCommon.offset0005); // temp testing

	if (fractal->transformCommon.functionEnabledYFalse) z = zc;

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (P == pit1) aux.color = fractal->foldColor.difs0000.x;
		else aux.color = fractal->foldColor.difs0000.y;
	}
}
