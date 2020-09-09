/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Adds Cpixel 4D constant to z vector
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
	CVector4 t = aux.const_c;
	if (fractal->transformCommon.functionEnabledEFalse) t = aux.c;

	if (fractal->transformCommon.functionEnabledAFalse)
	{ // c.w = rad
		t = CVector4(t.x, t.y, t.z, 0.0);
		t = CVector4(t.x, t.y, t.z, t.Length());
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{ // quadray
		t = CVector4(t.x + t.y + t.z, -t.x - t.y + t.z, -t.x + t.y - t.z, t.x - t.y - t.z);
	}

	if (fractal->transformCommon.functionEnabledBFalse) t = fabs(t);

	if (fractal->transformCommon.functionEnabledCFalse)
		t = fabs(t - fractal->transformCommon.offsetA0000);


	if (fractal->transformCommon.functionEnabledDFalse)
	{
		t.x = sign(z.x) * t.x;
		t.y = sign(z.y) * t.y;
		t.z = sign(z.z) * t.z;
		t.w = sign(z.w) * t.w;
	}
	t = t * fractal->transformCommon.scale1111;
	if (fractal->transformCommon.functionEnabled) z+= t;
	if (fractal->transformCommon.addCpixelEnabledFalse) aux.const_c = t;
	//else  z += t;

	aux.c = t;

	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
