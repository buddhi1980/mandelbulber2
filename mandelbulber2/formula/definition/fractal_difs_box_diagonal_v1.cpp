/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * DIFSBoxDiagonalV1Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 *
 */

#include "all_fractal_definitions.h"

cFractalDIFSBoxDiagonalV1::cFractalDIFSBoxDiagonalV1() : cAbstractFractal()
{
	nameInComboBox = "DIFS Box Diagonal V1";
	internalName = "difs_box_diagonal_v1";
	internalID = fractal::dIFSBoxDiagonalV1;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalDIFSBoxDiagonalV1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	CVector4 oldZ = z;
	CVector4 boxFold = fractal->transformCommon.additionConstantA111;

	// diag 1
	if (aux.i >= fractal->transformCommon.startIterationsCx
			&& aux.i < fractal->transformCommon.stopIterationsCx)
		if (z.x > z.y) swap(z.x, z.y);

	// abs and boxfoldxy
	if (aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
		z.x = fabs(z.x);
	if (aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
		z.y = fabs(z.y);

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z.x -= boxFold.x;
		z.y -= boxFold.y;
	}

	// abs offsets
	if (fractal->transformCommon.functionEnabledCFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		double xOffset = fractal->transformCommon.offsetC0;
		if (z.x < xOffset) z.x = fabs(z.x - xOffset) + xOffset;
	}
	if (fractal->transformCommon.functionEnabledDFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		double yOffset = fractal->transformCommon.offsetD0;
		if (z.y < yOffset) z.y = fabs(z.y - yOffset) + yOffset;
	}

	// reverse offset part 1
	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z.x -= fractal->transformCommon.offsetE0;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y -= fractal->transformCommon.offsetF0;

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		useScale = aux.actualScaleA + fractal->transformCommon.scale2;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;

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

	// reverse offset part 2
	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z.x += fractal->transformCommon.offsetE0;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y += fractal->transformCommon.offsetF0;

	// offset
	z += fractal->transformCommon.offset001;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	double colorDist = aux.dist;
	CVector4 zc = z;
	if (fractal->transformCommon.functionEnabledYFalse) zc = oldZ;
	// box
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		CVector4 boxSize = fractal->transformCommon.additionConstant111;
		// curve
		if (fractal->transformCommon.functionEnabledTFalse
				&& aux.i >= fractal->transformCommon.startIterationsT
				&& aux.i < fractal->transformCommon.stopIterationsT)
		{
			double absZ = fabs(z.z);
			absZ *= absZ;
			boxSize.x += fractal->transformCommon.scale0 * absZ;
			boxSize.y += fractal->transformCommon.scale0 * absZ;
		}
		// pyramid
		if (fractal->transformCommon.functionEnabledMFalse
				&& aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
		{
			boxSize.x -= fractal->transformCommon.scaleA0 * zc.z;
			boxSize.y -= fractal->transformCommon.scaleA0 * zc.z;
		}
		zc = fabs(zc) - boxSize;
		zc.x = max(zc.x, 0.0);
		zc.y = max(zc.y, 0.0);
		zc.z = max(zc.z, 0.0);
		double zcd = zc.Length();

		aux.dist = min(aux.dist, zcd / aux.DE);
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
