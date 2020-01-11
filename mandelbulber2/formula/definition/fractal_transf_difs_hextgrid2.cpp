/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * transfDIFSHextgrid2Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and  darkbeams optimized verion @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 * "Beautiful iso-surface made of a hexagonal grid of tubes.
 * Taken from K3DSurf forum, posted by user abdelhamid belaid."
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSHextgrid2::cFractalTransfDIFSHextgrid2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Hextgrid2";
	internalName = "transf_difs_hextgrid2";
	internalID = fractal::transfDIFSHextgrid2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSHextgrid2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;

	double size = fractal->transformCommon.scale1;
	double hexD = 0.0;

	if (fractal->transformCommon.rotationEnabled)
	{
		zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
	}

	zc.z /= fractal->transformCommon.scaleF1;

	double cosPi6 = cos(M_PI / 6.0);
	double yFloor = fabs(zc.y - size * floor(zc.y / size + 0.5));
	double xFloor = fabs(zc.x - size * 1.5 / cosPi6 * floor(zc.x / size / 1.5 * cosPi6 + 0.5));
	double gridMax = max(yFloor, xFloor * cosPi6 + yFloor * sin(M_PI / 6.0));
	double gridMin = min(gridMax - size * 0.5, yFloor);
	if (!fractal->transformCommon.functionEnabledJFalse)
		hexD = sqrt(gridMin * gridMin + zc.z * zc.z);
	else
		hexD = max(fabs(gridMin), fabs(zc.z));

	aux.dist = min(aux.dist, (hexD - fractal->transformCommon.offset0005) / (aux.DE + 1.0));
}
