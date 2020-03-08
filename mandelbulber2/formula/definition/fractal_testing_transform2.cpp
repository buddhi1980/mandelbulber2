/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Testing transform2
 *https://fractalforums.org/fractal-mathematics-and-new-theories/28/fake-3d-mandelbrot-set/1787/msg17940#msg17940
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
	//if (!fractal->transformCommon.functionEnabledJFalse) // temp
	{
		CVector4 signs = z;
		signs.x = sign(z.x);
		signs.y = sign(z.y);
		signs.z = sign(z.z);
		signs.w = sign(z.w);

		z = fabs(z);
		CVector4 tt = fractal->mandelbox.offset;
		z -= tt;

		double trr = z.Dot(z);
		double tp = min(max(1.0 / trr, 1.0), 1.0 / fractal->transformCommon.minR2p25);

		z += tt;
		z *= tp;
		aux.DE *= tp;
		z *= signs;
	}
	//else
	{
		double r2 = p.Dot(p);
		p += fractal->mandelbox.offset;
		double R = fractal->transformCommon.sqtR;
		if (fractal->transformCommon.functionEnabledAFalse) R *= R * R * R;

		if (r2 < R)
		{
			p *= fractal->transformCommon.mboxFactor1;
			dd *= fractal->transformCommon.mboxFactor1;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
				aux.color += fractal->mandelbox.color.factorSp1;
			}
		}
		else if (r2 < 1.0)
		{
			p *= 1.0 / r2;
			dd *= 1.0 / r2;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
				aux.color += fractal->mandelbox.color.factorSp2;
			}
		}
		p -= fractal->mandelbox.offset;
	}

	z = p + (z - p) * fractal->transformCommon.scale1;
	aux.DE = dd + (aux.DE  - dd) * fractal->transformCommon.scale1;

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
