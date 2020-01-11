/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical fold varyV1 MBox type
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalFoldVaryV1::cFractalTransfSphericalFoldVaryV1() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Fold VaryV1";
	internalName = "transf_spherical_fold_vary_v1";
	internalID = fractal::transfSphericalFoldVaryV1;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalFoldVaryV1::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double r2 = z.Dot(z);
	double tempVCf = fractal->mandelbox.foldingSphericalFixed; // constant to be varied
	double tempVCm = fractal->mandelbox.foldingSphericalMin;

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		int iterationRange =
			fractal->transformCommon.stopIterationsA - fractal->transformCommon.startIterationsA;
		int currentIteration = (aux.i - fractal->transformCommon.startIterationsA);
		tempVCf += fractal->transformCommon.offset * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterationsA)
	{
		tempVCf = (tempVCf + fractal->transformCommon.offset);
	}
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{

		int iterationRange =
			fractal->transformCommon.stopIterationsB - fractal->transformCommon.startIterationsB;
		int currentIteration = (aux.i - fractal->transformCommon.startIterationsB);
		tempVCm += fractal->transformCommon.offset0 * (1.0 * currentIteration) / iterationRange;
	}
	if (aux.i >= fractal->transformCommon.stopIterationsB)
	{
		tempVCm = tempVCm + fractal->transformCommon.offset0;
	}

	z += fractal->mandelbox.offset;

	tempVCm *= tempVCm;
	tempVCf *= tempVCf;

	if (r2 < tempVCm)
	{
		z *= tempVCf / tempVCm;
		aux.DE *= tempVCf / tempVCm;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (r2 < tempVCf)
	{
		double tglad_factor2 = tempVCf / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
	z -= fractal->mandelbox.offset;
}
