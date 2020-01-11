/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * rotation variation v1. Rotation angles vary linearly between iteration parameters.
 * After the stop iteration, alpha = alpha + variable alpha.
 */

#include "all_fractal_definitions.h"

cFractalTransfRotationVaryV1::cFractalTransfRotationVaryV1() : cAbstractFractal()
{
	nameInComboBox = "T>Rotation VaryV1";
	internalName = "transf_rotation_vary_v1";
	internalID = fractal::transfRotationVaryV1;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfRotationVaryV1::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempVC = CVector4(fractal->transformCommon.rotation, 0.0); // constant to be varied
	int iterationRange =
		fractal->transformCommon.stopIterations - fractal->transformCommon.startIterations250;

	if (aux.i >= fractal->transformCommon.startIterations250 && iterationRange != 0)
	{
		if (aux.i < fractal->transformCommon.stopIterations)
		{
			int currentIteration = (aux.i - fractal->transformCommon.startIterations250);
			double dCurrentIteration = (double)currentIteration;
			double dIterationRange = (double)iterationRange;
			tempVC += fractal->transformCommon.offset000 * dCurrentIteration / dIterationRange;
		}
		else
		{
			tempVC += fractal->transformCommon.offset000;
		}
	}

	tempVC *= M_PI_180;

	z = z.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), tempVC.x);
	z = z.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), tempVC.y);
	z = z.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), tempVC.z);
}
