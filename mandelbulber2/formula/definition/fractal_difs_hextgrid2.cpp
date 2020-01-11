/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * DIFSHextgrid2Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and  darkbeams optimized verion @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 * "Beautiful iso-surface made of a hexagonal grid of tubes.
 * Taken from K3DSurf forum, posted by user abdelhamid belaid."
 */

#include "all_fractal_definitions.h"

cFractalDIFSHextgrid2::cFractalDIFSHextgrid2() : cAbstractFractal()
{
	nameInComboBox = "DIFS Hextgrid2";
	internalName = "difs_hextgrid2";
	internalID = fractal::dIFSHextgrid2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalDIFSHextgrid2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;

	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		z += fractal->transformCommon.offset000;
		double rr = z.Dot(z);
		z *= fractal->transformCommon.scaleG1 / rr;
		aux.DE *= (fractal->transformCommon.scaleG1 / rr);
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleA1;
		aux.DE *= fractal->transformCommon.scaleA1;
	}

	if (fractal->transformCommon.functionEnabledCFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		double Tp = fractal->transformCommon.offset2;
		z.x = fmod(fabs(z.x) + Tp, 2.0 * Tp) - Tp;
		Tp = fractal->transformCommon.offsetA2;
		z.y = fmod(fabs(z.y) + Tp, 2.0 * Tp) - Tp;
	}

	// scale
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double useScale = aux.actualScaleA + fractal->transformCommon.scale2;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale);
		// scale vary
		if (fractal->transformCommon.functionEnabledKFalse
				&& aux.i >= fractal->transformCommon.startIterationsK
				&& aux.i < fractal->transformCommon.stopIterationsK)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			aux.actualScaleA -= vary;
		}
	}

	// offset
	z += fractal->transformCommon.offset001;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	// DE
	double colorDist = aux.dist;
	CVector4 zc = z;

	// Hextgrid2
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		double size = fractal->transformCommon.scale1;
		double hexD = 0.0;
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
		hexD -= fractal->transformCommon.offset0005;

		aux.dist = min(aux.dist, hexD / (aux.DE + 1.0));
	}

	// aux.color
	if (fractal->foldColor.auxColorEnabled)
	{
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			colorAdd += fractal->foldColor.difs0000.x * fabs(z.x * z.y);
			colorAdd += fractal->foldColor.difs0000.y * max(z.x, z.y);
		}
		colorAdd += fractal->foldColor.difs1;
		if (fractal->foldColor.auxColorEnabledA)
		{
			if (colorDist != aux.dist) aux.color += colorAdd;
		}
		else
			aux.color += colorAdd;
	}
}
