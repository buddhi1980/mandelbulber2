/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2019 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Aexion's Quadray Sets from FractalForums
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/quadray-sets/msg31458/#msg31458
 */

#include "all_fractal_definitions.h"

cFractalAexion4dV2::cFractalAexion4dV2() : cAbstractFractal()
{
	nameInComboBox = "Aexion4D_V2";
	internalName = "aexion4d_v2";
	internalID = fractal::aexion4dV2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelAlreadyHas;
	defaultBailout = 10000.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalAexion4dV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 temp = z;
	double t;
	if (aux.i == 0)
	{
		t = fractal->aexion.cadd;
		CVector4 cadd = CVector4(t, t, t, t);
		temp = CVector4(z.x + z.y + z.z, -z.x - z.y + z.z, -z.x + z.y - z.z, z.x - z.y - z.z);
		temp = fabs(temp) + cadd + fractal->transformCommon.offsetA0000;
		if (fractal->transformCommon.functionEnabledFalse) temp = fabs(temp);
		aux.const_c = temp * fractal->transformCommon.scale1111;
		t = temp.Length();
		if (!fractal->transformCommon.functionEnabledAFalse) z = CVector4(0.0, 0.0, 0.0, 0.0);
		else
		{
			z = temp;
			aux.DE *= t / aux.r * fractal->analyticDE.scale1;
		}
		aux.r = t;
	}
	aux.DE = aux.DE * 2.0 * aux.r + 1.0;
	t = 2.0 * z.w * z.z;
	temp.x = z.x * z.x - z.y * z.y;
	temp.y = t - temp.x;
	temp.x += t;
	t = 2.0 * z.x * z.y;
	temp.z = z.z * z.z - z.w * z.w;
	temp.w = t - temp.z;
	temp.z += t;

	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fractal->transformCommon.scale1 + fractal->analyticDE.offset0;

	z = temp + aux.const_c;
}
