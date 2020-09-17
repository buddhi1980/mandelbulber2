/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Create and add Cpixel 4D constant to z vector
 */

#include "all_fractal_definitions.h"

cFractalTransfAddCpixel4d::cFractalTransfAddCpixel4d() : cAbstractFractal()
{
	nameInComboBox = "T>Add Cpixel 4D";
	internalName = "transf_add_cpixel4d";
	internalID = fractal::transfAddCpixel4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddCpixel4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		CVector4 t = aux.const_c;

		if (fractal->transformCommon.functionEnabledAFalse)
		{ // c.w = rad
			t = CVector4(t.x, t.y, t.z, 0.0);
			t = CVector4(t.x, t.y, t.z, t.Length());
		}

		if (fractal->transformCommon.functionEnabledBFalse)
		{ // quadray
			t = CVector4(t.x + t.y + t.z, -t.x - t.y + t.z, -t.x + t.y - t.z, t.x - t.y - t.z);
		}

		if (fractal->transformCommon.functionEnabledFFalse) t = fabs(t);
		t = t - fractal->transformCommon.offsetA0000;
		if (fractal->transformCommon.functionEnabledCFalse) t = fabs(t);

		aux.const_c = t * fractal->transformCommon.scale1111;
		aux.c = aux.const_c;
	}

	if (fractal->transformCommon.functionEnabledDFalse)
	{
		aux.c.x *= sign(z.x);
		aux.c.y *= sign(z.y);
		aux.c.z *= sign(z.z);
		aux.c.w *= sign(z.w);
	}

	if (fractal->transformCommon.addCpixelEnabledFalse) z += aux.c;

	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
