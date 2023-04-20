/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * T>Dot Fold
 */

#include "all_fractal_definitions.h"

cFractalTransfDotFold::cFractalTransfDotFold() : cAbstractFractal()
{
	nameInComboBox = "T>Dot Fold";
	internalName = "transf_dot_fold";
	internalID = fractal::transfDotFold;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDotFold::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double t;
	CVector4 n;

	if (!fractal->transformCommon.functionEnabledFalse)
	{
		t = fractal->transformCommon.cosA;
		n = CVector4(t * fractal->transformCommon.sinB,
			fractal->transformCommon.sinA,
				t * fractal->transformCommon.cosB, 0.0);
		n += fractal->transformCommon.offset000;
	}
	else
	{
		n = fractal->transformCommon.offset000;
	}
	t = n.Length();
	if (t == 0.0) t = 1e-21;
	n /= t;

	if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	t = z.Dot(n);

	z = (z - (2.0 * max(t, 0.0) * n));

		// scale
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fractal->transformCommon.scale1 + fractal->analyticDE.offset0;

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}
