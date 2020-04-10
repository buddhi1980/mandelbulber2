/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Testing transform2
 *
 */

#include "all_fractal_definitions.h"

cFractalTestingTransform2::cFractalTestingTransform2() : cAbstractFractal()
{
	nameInComboBox = "Testing Transform2";
	internalName = "testing_transform2";
	internalID = fractal::testingTransform2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingTransform2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 p = z;
	double dd = aux.DE;
	double m = 0.0;
		double tp = 1.0;
	CVector4 signs = z;
	signs.x = sign(z.x);
	signs.y = sign(z.y);
	signs.z = sign(z.z);

	z = fabs(z);
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	z -= fractal->transformCommon.offset000;
	double trr = z.Dot(z);
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (trr < fractal->transformCommon.scale1)
		{
			tp = 1.0 / trr;
			tp = min(tp, fractal->transformCommon.scale4);
		}
		else
		{
			tp = fractal->transformCommon.scale1;
		}
	}
	if (fractal->transformCommon.functionEnabledCFalse)
	{
		tp = trr + sin(trr * M_PI * fractal->transformCommon.scale2) * fractal->transformCommon.scaleC1 + fractal->transformCommon.scaleC1;
		tp = min(max(1.0 / tp, fractal->transformCommon.scale1), fractal->transformCommon.scale4);

	}
	/*	if (trr < fractal->transformCommon.scale4)
		{
			tp = fractal->transformCommon.scale4 / fractal->transformCommon.scale1;
		}
		else if (trr < fractal->transformCommon.scale1)
		{
			tp = 1.0 / fractal->transformCommon.scale1 / trr;
		}
	}*/

	if (fractal->transformCommon.functionEnabledJFalse)
	{
		tp = min(max(1.0 / trr, fractal->transformCommon.scale1), fractal->transformCommon.scale4);

	}

	if (fractal->transformCommon.functionEnabledMFalse)
	{
		tp = 1.0 / trr + fractal->transformCommon.scale1;
		tp = min(tp, fractal->transformCommon.scale4);
	}


	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
		z += fractal->transformCommon.offset000;

	z *= tp;
	aux.DE *= tp;
	z *= signs;

	if (fractal->transformCommon.functionEnabledBFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		double rr = p.Dot(p);
		if (rr < 1.0)
		{
			p += fractal->mandelbox.offset;
			if (rr < fractal->transformCommon.scale025)
				m = fractal->transformCommon.scale025;
			else m = rr;
			m = 1.0 / m;
			p *= m;
			dd *= m;
			p -= fractal->mandelbox.offset;
		}


		z = p + (z - p) * fractal->transformCommon.scaleA1;
		aux.DE = dd + (aux.DE - dd) * fractal->transformCommon.scaleA1;

	}
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux.color += tp * fractal->mandelbox.color.factorSp1;
		aux.color += m * fractal->mandelbox.color.factorSp2;
	}


	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
