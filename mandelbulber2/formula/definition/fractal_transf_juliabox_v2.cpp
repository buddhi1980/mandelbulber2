/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * T>Juliabox V2
 * Folds, scaling, rotation and offsets
 * from Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 */

#include "all_fractal_definitions.h"

cFractalTransfJuliaboxV2::cFractalTransfJuliaboxV2() : cAbstractFractal()
{
	nameInComboBox = "T>Juliabox V2";
	internalName = "transf_juliabox_v2";
	internalID = fractal::transfJuliaboxV2;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalTransfJuliaboxV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	double colorAdd = 0.0;

	CVector4 oldZ = z;
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabled)
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
	}
	CVector4 zCol = z;

	// offset1
	if (aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
			z += fractal->transformCommon.offsetA000;

	// swap
	if (fractal->transformCommon.functionEnabledSwFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
	  z = CVector4(z.z, z.y, z.x, z.w);
	}

	// spherical fold
	double rrCol = 0.0;
	double m = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);
		z += fractal->mandelbox.offset;
		rrCol = rr;
		if (rr < fractal->transformCommon.minR0)
			m = fractal->transformCommon.maxMinR0factor;
		else if (rr < fractal->transformCommon.maxR2d1)
			m = fractal->transformCommon.maxR2d1 / rr;
		z *= m;
		aux.DE *= m;
		z -= fractal->mandelbox.offset;
	}

	// scale
	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		if (!fractal->transformCommon.functionEnabledKFalse)
		{
			z *= fractal->transformCommon.scale1;
			aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + fractal->analyticDE.offset0;
		}
		else
		{
			double tempVC = fractal->transformCommon.scale1; // constant to be varied

			if (aux.i >= fractal->transformCommon.startIterations
					&& aux.i < fractal->transformCommon.stopIterations
					&& (fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations
							!= 0))
			{
				int iterationRange =
					fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations;
				int currentIteration = (aux.i - fractal->transformCommon.startIterations);
				tempVC += fractal->transformCommon.offset0 * (1.0 * currentIteration) / iterationRange;
			}
			if (aux.i >= fractal->transformCommon.stopIterations)
			{
				tempVC = (tempVC + fractal->transformCommon.offset0);
			}
			z *= tempVC;
			aux.DE = aux.DE * fabs(tempVC) + fractal->analyticDE.offset0;
		}
	}

	if (aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
				z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
		z += fractal->transformCommon.additionConstantA000;

	if (fractal->transformCommon.rotation2EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (aux.i >= fractal->foldColor.startIterationsA
				&& aux.i < fractal->foldColor.stopIterationsA)
		{
			zCol = fabs(zCol - oldZ);
			if (zCol.x > 0.0)
				colorAdd += fractal->foldColor.difs0000.x * zCol.x;
			if (zCol.y > 0.0)
				colorAdd += fractal->foldColor.difs0000.y * zCol.y;
			if (zCol.z > 0.0)
				colorAdd += fractal->foldColor.difs0000.z * zCol.z;
		}
		if (aux.i >= fractal->transformCommon.startIterationsK
				&& aux.i < fractal->transformCommon.stopIterationsK)
		{
			if (rrCol < fractal->transformCommon.maxR2d1)
				colorAdd +=
					fractal->foldColor.difs0000.w * (fractal->transformCommon.maxR2d1 - rrCol);

			colorAdd += fractal->mandelbox.color.factorSp1 * m;
		}

		aux.color += colorAdd;
	}
}
