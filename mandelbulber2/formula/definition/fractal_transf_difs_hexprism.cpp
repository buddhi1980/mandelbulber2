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

cFractalTransfDIFSHexprism::cFractalTransfDIFSHexprism() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Hexprism";
	internalName = "transf_difs_hexprism";
	internalID = fractal::transfDIFSHexprism;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSHexprism::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double lenX = fractal->transformCommon.offset1;
	double lenY = fractal->transformCommon.offsetA1;
	CVector4 zc = fabs(z);

	// swap axis
	if (fractal->transformCommon.functionEnabledSFalse)
	{
		double temp = zc.x;
		zc.x = zc.y;
		zc.y = temp;
	}

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		double temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}

	if (fractal->transformCommon.rotation2EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
	}

	CVector4 k = CVector4(-SQRT_3_4, 0.5, SQRT_1_3, 0.0);

	double tp = 2.0 * min(k.x * zc.x + k.y * zc.y, 0.0);
	zc.x -= tp * k.x;
	zc.y -= tp * k.y;

	double dx = zc.x - clamp(zc.x, -k.z * lenX, k.z * lenX);
	double dy = zc.y - lenX;

	tp = sqrt(dx * dx + dy * dy);
	dx = tp * sign(zc.y - lenX) + fractal->transformCommon.offsetB0;
	dy = zc.z - lenY + fractal->transformCommon.offsetB0;
	tp = dx;
	double colIn = 0.0;
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		colIn = dx + fractal->transformCommon.offset0;
		dx = fabs(dx) - fractal->transformCommon.offset0;

		if (fractal->transformCommon.functionEnabledBFalse)
			dx = max(dx, tp);
	}

	double maxdx = max(dx, 0.0);
	double maxdy = max(dy, 0.0);

	tp = sqrt(maxdx * maxdx + maxdy * maxdy);
	aux.DE0 = min(max(dx, dy), 0.0) + tp;

	double colDist = aux.dist;
	aux.dist = min(aux.dist, aux.DE0 / (aux.DE + fractal->analyticDE.offset0)
				- fractal->transformCommon.offsetB0);

	if (fractal->foldColor.auxColorEnabledFalse && colDist != aux.dist
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addCol = fractal->foldColor.difs0000.y;
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (colIn < maxdx) addCol = fractal->foldColor.difs0000.z;
			if (lenY - fractal->foldColor.difs0 < zc.z)
				addCol = fractal->foldColor.difs0000.w;
		}
		if (!fractal->foldColor.auxColorEnabledBFalse)
		{
			aux.color = addCol;
		}
		else
		{
			aux.color += addCol + fractal->foldColor.difs0000.x; // aux.color default 1
		}
	}
}
