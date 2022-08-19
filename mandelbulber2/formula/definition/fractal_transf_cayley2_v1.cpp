/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * transfCayley2V1 based on Cayley2IFS
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfCayley2V1::cFractalTransfCayley2V1() : cAbstractFractal()
{
	nameInComboBox = "T>Cayley2 V1";
	internalName = "transf_cayley2_v1";
	internalID = fractal::transfCayley2V1;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfCayley2V1::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		double xTemp = SQRT_1_2 * (z.x - z.y);
		z.y = SQRT_1_2 * (z.y + z.x);
		z.x = xTemp;
	}

	if (fractal->transformCommon.functionEnabledM)
	{
		if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR1)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	double mx = z.x * z.x;
	double my = z.y * z.y;
	double m = fractal->transformCommon.scaleA2 * mx * my + mx * mx + my * my;
	double n = m + fractal->transformCommon.scale4 * z.x * z.y + 1.0;
	z.y = 2.0 * (my - mx) / n;
	z.x = fractal->transformCommon.scale2 * (m - 1.0) / n;
	z.z *= fractal->transformCommon.scale1;

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
