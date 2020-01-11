/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * transfDIFSGridIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and  Buddhi
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSGrid::cFractalTransfDIFSGrid() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Grid";
	internalName = "transf_difs_grid";
	internalID = fractal::transfDIFSGrid;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSGrid::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;

	double size = fractal->transformCommon.scale1;
	double grid = 0.0;

	zc.z /= fractal->transformCommon.scaleF1;

	if (fractal->transformCommon.rotationEnabled)
	{
		zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
	}

	double xFloor = fabs(zc.x - size * floor(zc.x / size + 0.5f));
	double yFloor = fabs(zc.y - size * floor(zc.y / size + 0.5f));
	double gridXY = min(xFloor, yFloor);

	if (!fractal->transformCommon.functionEnabledJFalse)
		grid = sqrt(gridXY * gridXY + zc.z * zc.z);
	else
		grid = max(fabs(gridXY), fabs(zc.z));

	aux.dist = min(aux.dist, (grid - fractal->transformCommon.offset0005) / (aux.DE + 1.0));
}
