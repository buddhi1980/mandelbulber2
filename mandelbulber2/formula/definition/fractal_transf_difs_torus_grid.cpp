/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSTorusGridIteration
 * based on http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSTorusGrid::cFractalTransfDIFSTorusGrid() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Torus Grid";
	internalName = "transf_difs_torus_grid";
	internalID = fractal::transfDIFSTorusGrid;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSTorusGrid::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;

	if (fractal->transformCommon.rotationEnabled)
	{
		zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
	}

	zc.z /= fractal->transformCommon.scaleF1;

	double size = fractal->transformCommon.offset2;

	if (!fractal->transformCommon.functionEnabledGFalse)
	{
		zc.x = fabs(zc.x - size * floor(zc.x / size + 0.5));
		zc.y = fabs(zc.y - size * floor(zc.y / size + 0.5));
	}
	else
	{
		double tx = fractal->transformCommon.int3X;
		double ty = fractal->transformCommon.int3Y;
		zc.x = zc.x - size * clamp(round(zc.x / size), -tx, tx);
		zc.y = zc.y - size * clamp(round(zc.y / size), -ty, ty);
	}

	double torD = sqrt(zc.y * zc.y + zc.x * zc.x) - fractal->transformCommon.offsetT1;

	if (!fractal->transformCommon.functionEnabledJFalse)
		torD = sqrt(torD * torD + zc.z * zc.z);
	else
		torD = max(fabs(torD), fabs(zc.z));

	aux.dist = min(aux.dist, torD - fractal->transformCommon.offset0005 / (aux.DE + 1.0));
}
