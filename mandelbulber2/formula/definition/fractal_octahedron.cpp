/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * eiffie and
 */

#include "all_fractal_definitions.h"

cFractalOctahedron::cFractalOctahedron() : cAbstractFractal()
{
	nameInComboBox = "Octahedron";
	internalName = "octahedron";
	internalID = fractal::octahedron;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalOctahedron::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double b = 1.0;
	double d;
	double limit1 = fractal->transformCommon.offset0;
	double limit2 = fractal->transformCommon.offsetA0;
	double sizer1 = fractal->transformCommon.scale1;
	double sizer2 = fractal->transformCommon.scaleA1;
	double sub = fractal->transformCommon.offsetB0;
	CVector4 a;

	for (int i = 0; i < fractal->transformCommon.int3X; i++)
	{
		a = fabs(z);
		z.x -= sign(z.x) * max(double(sign(a.x - max(a.y, a.z))), limit1) * b * sizer1;
		z.y -= sign(z.y) * max(double(sign(a.y - max(a.z, a.x))), limit1) * b * sizer1;
		z.z -= sign(z.z) * max(double(sign(a.z - max(a.x, a.y))), limit1) * b * sizer1;
		b *= b;
	}

	 d = z.Length() - sub;
	for (int i = 0; i < fractal->transformCommon.int3Y; i++)
	{
		 a = fabs(z);
		z.x -= sign(z.x) * max(double(sign(a.x - min(a.y, a.z))), limit2) * b * sizer2;
		z.y -= sign(z.y) * max(double(sign(a.y - min(a.z, a.x))), limit2) * b * sizer2;
		z.z -= sign(z.z) * max(double(sign(a.z - min(a.x, a.y))), limit2) * b * sizer2;
		b *= b;
	}

	a = fabs(z);
	double d2;

	if (!fractal->transformCommon.functionEnabledFalse)
		d2 = max(a.x, max(a.y, a.z)) - b;
	else
		d2 = a.Length() - b;

	 d2 = max(d2, -d);
	 aux.dist = d2;

	//if (!fractal->transformCommon.functionEnabledAFalse) aux.dist = d2;
	//else aux.dist = max(aux.dist, d2);


}
