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

	if (!fractal->transformCommon.functionEnabledCFalse)
		temp = -fractal->transformCommon.offset05;
	else
		temp = fractal->transformCommon.offset0005 - fractal->transformCommon.offset05;

	if (!fractal->transformCommon.functionEnabledJFalse)
		torD = sqrt(T1 * T1 + zc.z * zc.z) + temp;
	else
		torD = max(fabs(T1), fabs(zc.z)) + temp;
	temp = torD;
	if (fractal->transformCommon.functionEnabledCFalse)
	{
		torD = max(fabs(torD) - fractal->transformCommon.offset0005, 0.0);
		T1 = torD;
	}

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		torD = max(torD, -zc.x);
	}
	if (fractal->transformCommon.functionEnabledBFalse)
	{
		torD = max(torD, -zc.y);
	}

	double colDist = aux.dist;


	aux.dist = min(aux.dist, torD / (aux.DE + fractal->analyticDE.offset0));

	if (fractal->foldColor.auxColorEnabledFalse && colDist != aux.dist
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addCol = fractal->foldColor.difs0000.y;
		if (fractal->transformCommon.functionEnabledCFalse
				&& T1 >= temp + fractal->transformCommon.offset0005)
			addCol = fractal->foldColor.difs0000.z;

		if (!fractal->foldColor.auxColorEnabledBFalse)
		{
			aux.color = addCol;
		}
		else
		{
			aux.color += addCol + fractal->foldColor.difs0000.x;
		}
	}
}
