/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * DIFSMultiV1
 * http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalDIFSMultiV1::cFractalDIFSMultiV1() : cAbstractFractal()
{
	nameInComboBox = "DIFS Multi V1";
	internalName = "difs_multi_v1";
	internalID = fractal::dIFSMultiV1;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalDIFSMultiV1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	CVector4 oldZ = z;
	CVector4 boxFold = fractal->transformCommon.additionConstantA111;

	// abs
	if (fractal->transformCommon.functionEnabledAx
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
		z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledAy
			&& aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
		z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledAzFalse
			&& aux.i >= fractal->transformCommon.startIterationsZ
			&& aux.i < fractal->transformCommon.stopIterationsZ)
		z.z = fabs(z.z);
	if (fractal->transformCommon.functionEnabledFalse)
	{
		// xy box fold
		if (fractal->transformCommon.functionEnabledAFalse
				&& aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA)
		{
			z.x -= boxFold.x;
			z.y -= boxFold.y;
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
			int poly = fractal->transformCommon.int6;
			double psi = fabs(fmod(atan(z.y / z.x) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
			double len = sqrt(z.x * z.x + z.y * z.y);
			z.x = cos(psi) * len;
			z.y = sin(psi) * len;
		}

		// diag fold1
		if (fractal->transformCommon.functionEnabledCxFalse
				&& aux.i >= fractal->transformCommon.startIterationsCx
				&& aux.i < fractal->transformCommon.stopIterationsCx)
			if (z.y > z.x) swap(z.x, z.y);

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

		// diag fold2
		if (fractal->transformCommon.functionEnabledCyFalse
				&& aux.i >= fractal->transformCommon.startIterationsCy
				&& aux.i < fractal->transformCommon.stopIterationsCy)
			if (z.y > z.x) swap(z.x, z.y);
	}

	// reverse offset part 1
	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z.x -= fractal->transformCommon.offsetE2;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y -= fractal->transformCommon.offsetF2;

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		useScale = aux.actualScaleA + fractal->transformCommon.scale2;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;
		// else

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
		z.x += fractal->transformCommon.offsetE2;

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z.y += fractal->transformCommon.offsetF2;

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
	CVector4 zc = oldZ;

	if (fractal->transformCommon.functionEnabledFFalse) zc = z;

	// box
	if (fractal->transformCommon.functionEnabledM && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		CVector4 boxSize = fractal->transformCommon.additionConstant111;
		CVector4 bxV = zc;
		double bxD = 0.0;
		bxV = fabs(bxV) - boxSize;
		bxD = max(bxV.x, max(bxV.y, bxV.z)); // infnorm
		if (bxD > 0.0)
		{
			bxV.x = max(bxV.x, 0.0);
			bxV.y = max(bxV.y, 0.0);
			bxV.z = max(bxV.z, 0.0);
			aux.DE0 = bxV.Length();
		}
		aux.dist = min(aux.dist, aux.DE0 / aux.DE);
	}
	// sphere
	if (fractal->transformCommon.functionEnabledMFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		aux.DE0 = zc.Length() - fractal->transformCommon.offsetR1;
		aux.dist = min(aux.dist, aux.DE0 / aux.DE);
	}
	// cylinder
	if (fractal->transformCommon.functionEnabledOFalse
			&& aux.i >= fractal->transformCommon.startIterationsO
			&& aux.i < fractal->transformCommon.stopIterationsO)
	{
		double cylD = 0.0;
		//- fractal->transformCommon.scale0 * aux.i;

		if (fractal->transformCommon.functionEnabledSwFalse) swap(zc.x, zc.z);

		double cylR = sqrt(zc.x * zc.x + zc.y * zc.y);
		double cylH = fabs(zc.z) - fractal->transformCommon.offsetA1;

		double cylRm = cylR - fractal->transformCommon.radius1;

		if (!fractal->transformCommon.functionEnabledXFalse)
		{
			if (!fractal->transformCommon.functionEnabledzFalse) cylR = cylRm;
			cylR = max(cylR, 0.0);
			double cylHm = max(cylH, 0.0);
			cylD = sqrt(cylR * cylR + cylHm * cylHm);
		}
		else
			cylD = sqrt(cylRm * cylRm + cylH * cylH);

		aux.DE0 = min(max(cylRm, cylH) - fractal->transformCommon.offsetR0, 0.0) + cylD;
		aux.dist = min(aux.dist, aux.DE0 / aux.DE);
	}

	// torus
	if (fractal->transformCommon.functionEnabledTFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		// swap axis
		if (fractal->transformCommon.functionEnabledJFalse) swap(zc.x, zc.z);

		double T1 = sqrt(zc.y * zc.y + zc.x * zc.x) - fractal->transformCommon.offsetT1;
		aux.DE0 = sqrt(T1 * T1 + zc.z * zc.z) - fractal->transformCommon.offset05;
		aux.dist = min(aux.dist, aux.DE0 / aux.DE);
	}

	// prism
	if (fractal->transformCommon.functionEnabledNFalse
			&& aux.i >= fractal->transformCommon.startIterationsN
			&& aux.i < fractal->transformCommon.stopIterationsN)
	{
		// swap axis
		if (fractal->transformCommon.functionEnabledEFalse) swap(zc.x, zc.z);

		aux.DE0 = max(fabs(zc.x) - fractal->transformCommon.offset2,
			max(fabs(zc.y) * SQRT_3_4 + zc.z * 0.5, -zc.z) - fractal->transformCommon.offsetA05);

		aux.dist = min(aux.dist, aux.DE0 / aux.DE);
	}

	// aux.dist tweaks
	if (fractal->analyticDE.enabledFalse)
		aux.dist = aux.dist * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	// aux.color
	if (fractal->foldColor.auxColorEnabled)
	{
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			colorAdd += fractal->foldColor.difs0000.x * fabs(z.x * z.y); // fabs(zc.x * zc.y)
			colorAdd += fractal->foldColor.difs0000.y * max(z.x, z.y);	 // max(z.x, z.y);
			colorAdd += fractal->foldColor.difs0000.z * (z.x * z.x + z.y * z.y);
			colorAdd += fractal->foldColor.difs0000.w * fabs(zc.x * zc.y);
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
