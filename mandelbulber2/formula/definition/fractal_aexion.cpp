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

cFractalAexion::cFractalAexion() : cAbstractFractal()
{
	nameInComboBox = "Aexion";
	internalName = "aexion";
	internalID = fractal::aexion;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelAlreadyHas;
	defaultBailout = 10000.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalAexion::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 temp = z;
	double t;
	if (aux.i == 0)
	{
		t = fractal->aexion.cadd;
		CVector4 cadd = CVector4(t, t, t, t);
		temp.x = z.x + z.y + z.z;
		temp.y = -z.x - z.y + z.z;
		temp.z = -z.x + z.y - z.z;
		temp.w = z.x - z.y - z.z;
		z = fabs(temp) + cadd;
		aux.const_c = z;
	}
	t = 2.0 * z.w * z.z;
	temp.x = z.x * z.x - z.y * z.y;
	temp.y = t - temp.x;
	temp.x += t;
	t = 2.0 * z.x * z.y;
	temp.z = z.z * z.z - z.w * z.w;
	temp.w = t - temp.z;
	temp.z += t;
	z = temp + aux.const_c;

	if (fractal->analyticDE.enabled)
	{
		double de1 = 1.1 * aux.r;
		de1 = de1 + (z.Length() / aux.r - de1) * fractal->analyticDE.scale1;
		aux.DE = de1 * 2.0 * aux.DE + fractal->analyticDE.offset1;
	}
}
