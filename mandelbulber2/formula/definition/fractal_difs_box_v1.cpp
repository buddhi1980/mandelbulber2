/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * DIFSBoxV1Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * DIFS = direct distance iterated function (primitives) system
 */

#include "all_fractal_definitions.h"

cFractalDIFSBoxV1::cFractalDIFSBoxV1() : cAbstractFractal()
{
	nameInComboBox = "DIFS Box V1";
	internalName = "difs_box_v1";
	internalID = fractal::dIFSBoxV1;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalDIFSBoxV1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	CVector4 oldZ = z;
	CVector4 boxFold = fractal->transformCommon.additionConstantA111;

	// abs
	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
		z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledAyFalse
			&& aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
		z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledAzFalse
			&& aux.i >= fractal->transformCommon.startIterationsZ
			&& aux.i < fractal->transformCommon.stopIterationsZ)
		z.z = fabs(z.z);
	// xy box fold
	if (fractal->transformCommon.functionEnabledCx
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z.x = fabs(z.x + boxFold.x) - fabs(z.x - boxFold.x) - z.x;
		z.y = fabs(z.y + boxFold.y) - fabs(z.y - boxFold.y) - z.y;
	}
	// xyz box fold
	if (fractal->transformCommon.functionEnabledBFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
		z -= boxFold;
	// polyfold
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP)
	{
		z.x = fabs(z.x);
		double psi = M_PI / fractal->transformCommon.int6;
		psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}

	// diag 1
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsCx
			&& aux.i < fractal->transformCommon.stopIterationsCx)
		if (z.x > z.y) swap(z.x, z.y);

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

	// diag 2
	if (fractal->transformCommon.functionEnabledCyFalse
			&& aux.i >= fractal->transformCommon.startIterationsCy
			&& aux.i < fractal->transformCommon.stopIterationsCy)
		if (z.x > z.y) swap(z.x, z.y);

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

	// offset2
	z += fractal->transformCommon.offset001;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	double colorDist = aux.dist;
	// DE
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
			double absZZ = zc.z * zc.z * fractal->transformCommon.scale0;
			boxSize.x += absZZ;
			boxSize.y += absZZ;
		}

		// pyramid
		if (fractal->transformCommon.functionEnabledMFalse
				&& aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
		{
			double subZ = fractal->transformCommon.scaleA0 * zc.z;
			boxSize.x -= subZ;
			boxSize.y -= subZ;
		}

		zc = fabs(zc) - boxSize;
		zc.x = max(zc.x, 0.0);
		zc.y = max(zc.y, 0.0);
		zc.z = max(zc.z, 0.0);
		double zcd = zc.Length();

		aux.dist = min(aux.dist, zcd / aux.DE);

		if (!fractal->transformCommon.functionEnabledEFalse)
			aux.dist = min(aux.dist, zcd / aux.DE);
		else
			aux.dist = min(aux.dist, zcd / aux.DE) - fractal->transformCommon.offsetB0;
	}

	// aux.color
	if (fractal->foldColor.auxColorEnabled && colorDist != aux.dist
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			zc = fabs(zc);
			colorAdd += fractal->foldColor.difs0000.x * zc.x * zc.y;
			colorAdd += fractal->foldColor.difs0000.y * max(zc.x, zc.y);
		}

		if (fractal->foldColor.auxColorEnabledA)
		{
			aux.color = colorAdd + aux.i * fractal->foldColor.difs1 + fractal->foldColor.difs0;
		}
		else
		{
			aux.color += colorAdd;
		}
	}
}
