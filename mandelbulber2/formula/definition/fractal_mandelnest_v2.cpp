/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2019 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Mandelnest by Jeannoc, adapted by pupukuusikko
 * https://fractalforums.org/share-a-fractal/22/mandelbrot-3d-mandelnest/4028/
 */

#include "all_fractal_definitions.h"

cFractalMandelnestV2::cFractalMandelnestV2() : cAbstractFractal()
{
	nameInComboBox = "Mandelnest V2";
	internalName = "mandelnest_v2";
	internalID = fractal::mandelnestV2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelnestV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double Power = fractal->bulb.power;
	CVector4 shift = fractal->transformCommon.offset000 * M_PI;
	CVector4 dual = fractal->transformCommon.scale3D111;

	CVector4 limit = fractal->transformCommon.offsetA000;
	z = z + fabs(z - limit) - fabs(z + limit);
	//	z = fabs(z + limit) - fabs(z - limit) - z;

	// double r = aux.r;
	double r = z.Length();
	double rN = fractal->transformCommon.scale1 / r;
	aux.DE *= fabs(rN);

	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	// CVector4 limit = fractal->transformCommon.offsetA000;
	// z = z + fabs(z - limit) - fabs(z + limit);
	//	z = fabs(z + limit) - fabs(z - limit) - z;

	CVector4 temp = z * rN;
	if (!fractal->transformCommon.functionEnabledBxFalse) temp.x = asin(temp.x);
	else temp.x = acos(temp.x);
	if (!fractal->transformCommon.functionEnabledByFalse) temp.y = asin(temp.y);
	else temp.y = acos(temp.y);
	if (!fractal->transformCommon.functionEnabledBzFalse) temp.z = asin(temp.z);
	else temp.z = acos(temp.z);

	temp = shift + Power * dual * temp;

	if (!fractal->transformCommon.functionEnabledCxFalse) z.x = sin(temp.x);
	else z.x = cos(temp.x);
	if (!fractal->transformCommon.functionEnabledCyFalse) z.y = sin(temp.y);
	else z.y = cos(temp.y);
	if (!fractal->transformCommon.functionEnabledCzFalse) z.z = sin(temp.z);
	else z.z = cos(temp.z);

	if (!fractal->transformCommon.functionEnabledAFalse)
	{
		rN = 1.0 / z.Length();
		z *= rN;
		aux.DE *= rN;
	}

	z *= pow(r, Power - fractal->transformCommon.offset1);

	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
			z += fractal->transformCommon.offsetF000;

	r = z.Length();

	aux.DE = aux.DE * Power * r + 1.0;
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		if (fractal->transformCommon.functionEnabledBFalse)
			aux.DE = max(aux.DE, fractal->analyticDE.offset2);
	// aux.dist
		if (fractal->transformCommon.functionEnabledDFalse)
		{
			aux.DE0 = 0.5 * log(r) * r / aux.DE;

			if (aux.DE < fractal->transformCommon.startIterationsE)
				aux.dist = min(aux.dist, aux.DE0);
			else
				aux.dist = min(aux.DE0, fractal->analyticDE.offset1);


			/*if (!fractal->transformCommon.functionEnabledEFalse)
				aux.dist = min(aux.DE0, fractal->analyticDE.offset1);
			else
				aux.dist = min(aux.dist, aux.DE0);*/
		}
	}
}
