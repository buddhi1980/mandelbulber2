/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * diagonal fold conditional
 */

#include "all_fractal_definitions.h"

cFractalTransfDiagonalFold::cFractalTransfDiagonalFold() : cAbstractFractal()
{
	nameInComboBox = "T>Diagonal Fold";
	internalName = "transf_diagonal_fold";
	internalID = fractal::transfDiagonalFold;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDiagonalFold::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (!fractal->transformCommon.functionEnabledAxFalse)
	{
		if (fractal->transformCommon.functionEnabledAx)
		{
			if (z.x > z.y) swap(z.y, z.x);
		}
		if (fractal->transformCommon.functionEnabledAyFalse)
		{
			if (z.y > z.z) swap(z.z, z.y);
		}
		if (fractal->transformCommon.functionEnabledAzFalse)
		{
			if (z.z > z.x) swap(z.x, z.z);
		}
	}
	else
	{
		if (fractal->transformCommon.functionEnabledAx
				&& aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA)
		{
			if (z.x > z.y) swap(z.y, z.x);
		}
		if (fractal->transformCommon.functionEnabledAyFalse
				&& aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			if (z.y > z.z) swap(z.z, z.y);
		}
		if (fractal->transformCommon.functionEnabledAzFalse
				&& aux.i >= fractal->transformCommon.startIterationsC
				&& aux.i < fractal->transformCommon.stopIterationsC)
		{
			if (z.z > z.x) swap(z.x, z.z);
		}
	}

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}
