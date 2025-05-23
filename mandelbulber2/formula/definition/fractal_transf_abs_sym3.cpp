/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * abs sym3 based on code by msltoe
 */

#include "all_fractal_definitions.h"

cFractalTransfAbsSym3::cFractalTransfAbsSym3() : cAbstractFractal()
{
	nameInComboBox = "T>Abs Sym3";
	internalName = "transf_abs_sym3";
	internalID = fractal::transfAbsSym3;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAbsSym3::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);
		z += fractal->transformCommon.offset000;
	}

	if (aux.i >= fractal->transformCommon.startIterationsK
			&& aux.i < fractal->transformCommon.stopIterationsK)
	{
		if (fractal->transformCommon.functionEnabledBx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledBy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledBz) z.z = fabs(z.z);
		CVector4 pfive = fractal->transformCommon.additionConstant0555;
		double t;
		if (fractal->transformCommon.functionEnabledAx)
		{
			t = z.x - z.z;
			t = pfive.x * (t - fabs(t));
			z.x = z.x - t;
			z.z = z.z + t;
		}
		if (fractal->transformCommon.functionEnabledAy)
		{
			t = z.x - z.y;
			t = pfive.y * (t - fabs(t));
			z.x = z.x - t;
			z.y = z.y + t;
		}
		if (fractal->transformCommon.functionEnabledAz)
		{
			t = z.y - z.z;
			t = pfive.z * (t - fabs(t));
			z.y = z.y - t;
			z.z = z.z + t;
		}
		z += fractal->transformCommon.offsetA000;
	}

	if (fractal->transformCommon.functionEnabledBFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		if (fractal->transformCommon.functionEnabledCx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledCy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledCx) z.z = fabs(z.z);
		z += fractal->transformCommon.offsetF000;
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
}
