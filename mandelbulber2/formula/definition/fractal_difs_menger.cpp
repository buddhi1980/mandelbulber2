/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * DifsMengerIteration  fragmentarium code, mdifs by knighty (jan 2012)
 */

#include "all_fractal_definitions.h"

cFractalDIFSMenger::cFractalDIFSMenger() : cAbstractFractal()
{
	nameInComboBox = "DIFS Menger";
	internalName = "difs_menger";
	internalID = fractal::dIFSMenger;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalDIFSMenger::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	CVector4 oldZ = z;
	CVector4 boxFold = fractal->transformCommon.additionConstantA111;

	// abs z
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
	// folds
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
		// diag fold2
		if (fractal->transformCommon.functionEnabledCyFalse
				&& aux.i >= fractal->transformCommon.startIterationsCy
				&& aux.i < fractal->transformCommon.stopIterationsCy)
			if (z.x > z.y) swap(z.x, z.y);
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
	double mengD = 0.0;

	// menger sponge
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		CVector4 ones = CVector4(1.0, 1.0, 1.0, 0.0);
		double rr = 0.0;
		double sz = fractal->transformCommon.scale05;
		CVector4 szVect = CVector4(sz, sz, sz, 1.0);
		int count = fractal->transformCommon.int8X; // Menger Sponge
		zc *= (1.0 / szVect);
		aux.pseudoKleinianDE = 1.0;

		for (int k = 0; k < count && rr < 10.0; k++)
		{
			zc = fabs(zc + fractal->transformCommon.offsetA000);

			if (zc.y > zc.x) swap(zc.y, zc.x);
			if (zc.y > zc.z) swap(zc.y, zc.z);
			double factor = 1.0 / fractal->transformCommon.offset3;
			if (!fractal->transformCommon.functionEnabledJFalse)
			{
				if (zc.y < factor) zc.y = fabs(zc.y - factor) + factor;
			}
			else
			{
				zc.y = fabs(zc.y - factor) + factor;
			}

			zc -= ones;
			zc *= fractal->transformCommon.scale3;
			aux.pseudoKleinianDE *= fractal->transformCommon.scale3;
			zc += ones;

			rr = zc.Length();
		}

		zc = fabs(zc) - ones;

		CVector4 bxV = zc;

		mengD = max(bxV.x, max(bxV.y, bxV.z));
		if (mengD > 0.0)
		{
			bxV.x = max(bxV.x, 0.0);
			bxV.y = max(bxV.y, 0.0);
			bxV.z = max(bxV.z, 0.0);
			mengD = bxV.Length();
		}
		mengD = mengD * sz;
		mengD /= aux.pseudoKleinianDE;

		aux.dist = min(aux.dist, mengD / aux.DE);
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
