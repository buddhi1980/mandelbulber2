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

cFractalMandelnest::cFractalMandelnest() : cAbstractFractal()
{
	nameInComboBox = "Mandelnest";
	internalName = "mandelnest";
	internalID = fractal::mandelnest;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelnest::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double Power = fractal->bulb.power;
	double shift = fractal->transformCommon.offset0;

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		if (aux.pos_neg < 0.0) shift += 1.0;
	}

	shift *= M_PI;


	double r = aux.r;

//if (fractal->transformCommon.functionEnabledFalse && aux->pos_neg < 0.0f)

	double rN =  1.0 / r;
	aux.DE *= rN;

	z.x = (cos(shift + Power * acos(z.x * rN)));
	z.y = (cos(shift + Power * acos(z.y * rN)));
	z.z = (cos(shift + Power * acos(z.z * rN)));

	if (!fractal->transformCommon.functionEnabledAFalse)
	{
		rN = 1.0 / z.Length();
		z *= rN;
		aux.DE *= rN;
	}

	z *=  pow(r, Power - fractal->transformCommon.offset1);

	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		if (!fractal->transformCommon.functionEnabledFalse)
		{
			z += fractal->transformCommon.offsetF000;
		}
		else
		{
			z.x += sign(z.x) * fractal->transformCommon.offsetF000.x;
			z.y += sign(z.y) * fractal->transformCommon.offsetF000.y;
			z.z += sign(z.z) * fractal->transformCommon.offsetF000.z;
		}
	}


	aux.pos_neg *= -1.0f;
	//z+=aux.c;
	r = z.Length();

	aux.DE = aux.DE * Power * r + 1.0;
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		if (fractal->transformCommon.functionEnabledBFalse)
			aux.DE = max(aux.DE, fractal->analyticDE.offset2);

		aux.dist = 0.5 * log(r) * r / aux.DE;
		aux.dist = min(aux.dist, fractal->analyticDE.offset1);
	}
	/*const double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	const double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	const double th = th0 * fractal->bulb.power;
	const double ph = ph0 * fractal->bulb.power;
	const double cth = cos(th);
	aux.DE = (rp * aux.DE) * fractal->bulb.power + 1.0;
	rp *= aux.r;
	z.x = cth * cos(ph) * rp;
	z.y = cth * sin(ph) * rp;
	z.z = sin(th) * rp;*/
}
