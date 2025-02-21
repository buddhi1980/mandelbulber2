/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSTorus::cFractalTransfDIFSTorus() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Torus";
	internalName = "transf_difs_torus";
	internalID = fractal::transfDIFSTorus;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSTorus::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;
	double temp = 0.0;
	double torD;
	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}
	if (fractal->transformCommon.functionEnabledSFalse)
	{
		temp = zc.y;
		zc.y = zc.z;
		zc.z = temp;
	}

	double T1 = sqrt(zc.y * zc.y + zc.x * zc.x) - fractal->transformCommon.offsetT1;

	if (!fractal->transformCommon.functionEnabledJFalse)
		torD = sqrt(T1 * T1 + zc.z * zc.z) - fractal->transformCommon.offset05;
	else
		torD = max(fabs(T1), fabs(zc.z)) - fractal->transformCommon.offset05;
	double colDist = aux.dist;
	aux.dist = min(aux.dist, torD / (aux.DE + fractal->analyticDE.offset0));

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		aux.dist = max(aux.dist, -zc.x);
	}
	if (fractal->transformCommon.functionEnabledBFalse)
	{
		aux.dist = max(aux.dist, -zc.y);
	}


	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (colDist != aux.dist) aux.color += fractal->foldColor.difs0000.x;
	}
}
