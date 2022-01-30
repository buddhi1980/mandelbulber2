/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2019 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Mandelnest refer Jeannot
 * https://fractalforums.org/share-a-fractal/22/mandelbrot-3d-mandelnest/4028/
 * also https://www.shadertoy.com/view/4lKfzy
 */

#include "all_fractal_definitions.h"

cFractalMandelnest4d::cFractalMandelnest4d() : cAbstractFractal()
{
	nameInComboBox = "Mandelnest 4D";
	internalName = "mandelnest4d";
	internalID = fractal::mandelnest4d;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelnest4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledCFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		if (fractal->transformCommon.functionEnabledEFalse)
		{
			z = CVector4(z.x, z.y, z.z, z.Length());
			aux.DE += 0.5;
		}
		if (fractal->transformCommon.functionEnabledFFalse)
		{
			z = CVector4(z.x + z.y + z.z, -z.x - z.y + z.z, -z.x + z.y - z.z, z.x - z.y - z.z);
			aux.DE *= z.Length() / aux.r;
		}
		if (fractal->transformCommon.addCpixelEnabledFalse) aux.const_c = z;
	}

	if (fractal->transformCommon.functionEnabledBFalse)
	{
		z = z + fabs(z - fractal->transformCommon.offsetA0000)
				- fabs(z + fractal->transformCommon.offsetA0000);
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledAwFalse) z.w = fabs(z.w);
	}

	double r = z.Length();
	double rN = fractal->transformCommon.scale1 / r;
	CVector4 temp = z * rN;
	aux.DE *= fabs(rN);

	if (!fractal->transformCommon.functionEnabledBxFalse) temp.x = asin(temp.x);
	else temp.x = acos(temp.x);
	if (!fractal->transformCommon.functionEnabledByFalse) temp.y = asin(temp.y);
	else temp.y = acos(temp.y);
	if (!fractal->transformCommon.functionEnabledBzFalse) temp.z = asin(temp.z);
	else temp.z = acos(temp.z);
	if (!fractal->transformCommon.functionEnabledBwFalse) temp.w = asin(temp.w);
	else temp.w = acos(temp.w);

	double Power = fractal->transformCommon.pwr8;
	temp = (fractal->transformCommon.offset0000 * M_PI)
			+ Power * fractal->transformCommon.scale1111 * temp;

	if (!fractal->transformCommon.functionEnabledCxFalse) z.x = sin(temp.x);
	else z.x = cos(temp.x);
	if (!fractal->transformCommon.functionEnabledCyFalse) z.y = sin(temp.y);
	else z.y = cos(temp.y);
	if (!fractal->transformCommon.functionEnabledCzFalse) z.z = sin(temp.z);
	else z.z = cos(temp.z);
	if (!fractal->transformCommon.functionEnabledw) z.w = sin(temp.w);
	else z.w = cos(temp.w);

	if (!fractal->transformCommon.functionEnabledAFalse)
	{
		rN = 1.0 / z.Length();
		z *= rN;
		aux.DE *= rN;
	}

	z *= pow(r, Power - fractal->transformCommon.offset1);

	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
			z += fractal->transformCommon.additionConstant0000;

	if (fractal->transformCommon.addCpixelEnabledFalse) z += aux.const_c;

	r = z.Length();
	aux.DE = aux.DE * Power * r + 1.0;

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
	// aux.dist
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		if (r > 1.0)
			aux.DE0 = 0.5 * log(r) * r / (aux.DE);
		else
			aux.DE0 = 0.0;

		if (aux.i <= fractal->transformCommon.startIterationsE)
			aux.dist = min(aux.DE0, fractal->analyticDE.offset1);
		else
			aux.dist = min(aux.dist, aux.DE0); // hybrid
	}
}
