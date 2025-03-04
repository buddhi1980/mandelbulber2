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
	double temp = 0.0;
	// swap axis
	if (fractal->transformCommon.functionEnabledSFalse)
	{
		temp = zc.x;
		zc.x = zc.y;
		zc.y = temp;
	}

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}

	double xyR = sqrt(zc.x * zc.x + zc.y * zc.y)
			- fractal->transformCommon.radius1
			+ fractal->transformCommon.offsetB0;

	double cylR = xyR;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		cylR = fabs(cylR) - fractal->transformCommon.offset0;
		if (fractal->transformCommon.functionEnabledAFalse)
			cylR = max(cylR, xyR);
	}

	double cylH = fabs(zc.z)
			- fractal->transformCommon.offsetA1
			+ fractal->transformCommon.offsetB0;

	cylR = max(cylR, 0.0);
	cylH = max(cylH, 0.0);
	double cylD = sqrt(cylR * cylR + cylH * cylH);
	cylD = min(max(cylR, cylH), 0.0) + cylD;

	double colDist = aux.dist;
	aux.dist = min(aux.dist,
			cylD / (aux.DE + fractal->analyticDE.offset0)
				- fractal->transformCommon.offsetB0);

	if (fractal->foldColor.auxColorEnabledFalse && colDist != aux.dist
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addCol = fractal->foldColor.difs0000.y;
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (xyR < -fractal->transformCommon.offset0
					- fractal->transformCommon.offsetB0)
				addCol = fractal->foldColor.difs0000.z;
			if (fractal->transformCommon.offsetA1
					+ fractal->transformCommon.offsetB0
					- fractal->foldColor.difs0 < fabs(zc.z))
				addCol = fractal->foldColor.difs0000.w;
		}
		if (!fractal->foldColor.auxColorEnabledBFalse)
			aux.color = addCol;
		else
		{
			aux.colorHybrid += addCol + fractal->foldColor.difs0000.x;
			aux.color = aux.colorHybrid; // aux.color default 1
		}
	}
}
