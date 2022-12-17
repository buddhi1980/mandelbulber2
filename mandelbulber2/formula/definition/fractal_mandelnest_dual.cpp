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

cFractalMandelnestDual::cFractalMandelnestDual() : cAbstractFractal()
{
	nameInComboBox = "Mandelnest Dual";
	internalName = "mandelnest_dual";
	internalID = fractal::mandelnestDual;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelnestDual::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double Power = fractal->transformCommon.scale5;

	// Dual +
	CVector4 zp = z;
	double M0 = zp.Length();
	zp.x = asin(zp.x / M0); // dual+
	zp.y = asin(zp.y / M0);
	zp.z = asin(zp.z / M0);
	M0 = pow(M0, Power);
	zp *= Power;

	zp.x = sin(zp.x);
	zp.y = sin(zp.y);
	zp.z = sin(zp.z);
	double M1 = zp.Length();
	zp = zp * M0 / M1;
	zp += fractal->transformCommon.offsetA000;
	zp += aux.const_c * fractal->transformCommon.constantMultiplierA111;

	// Dual -
	CVector4 zm = z;
	M0 = zm.Length();
	zm.x = -asin(zm.x / M0); // dual-
	zm.y = -asin(zm.y / M0);
	zm.z = -asin(zm.z / M0);
	M0 = pow(M0, Power);
	zm *= Power;

	zm.x = sin(zm.x);
	zm.y = sin(zm.y);
	zm.z = sin(zm.z);
	M1 = zm.Length();
	zm = zm * M0 / M1;
	zm += fractal->transformCommon.offsetA000;
	zm += aux.const_c * fractal->transformCommon.constantMultiplierA111;

	// Dual+ OR dual-
	M0 = zp.Length();
	M1 = zm.Length();
	if (M0<M1)
	{
		z = zp;
	}
	else
	{
		z = zm;
	}

	aux.DE = pow(aux.r, Power - 1.0f) * aux.DE * Power + 1.0;

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		if (fractal->transformCommon.functionEnabledBFalse)
			aux.DE = max(aux.DE, fractal->analyticDE.offset2);

		if (fractal->transformCommon.functionEnabledDFalse)
		{
					double r = z.Length();
			aux.dist = 0.5 * log(r) * r / aux.DE;
			aux.dist = min(aux.dist, fractal->analyticDE.offset1);
		}
	}




	/*double Power = fractal->bulb.power;
	CVector4 shift = fractal->transformCommon.offset000 * M_PI;
	CVector4 dual = fractal->transformCommon.scale3D111;



	double r = z.Length();
	double rN = fractal->transformCommon.scale1 / r;
	aux.DE *= fabs(rN);

	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

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

	// aux.dist
		if (fractal->transformCommon.functionEnabledDFalse)
		{
			aux.DE0 = 0.5 * log(r) * r / aux.DE;

			if (aux.i <= fractal->transformCommon.startIterationsE)
				aux.dist = min(aux.DE0, fractal->analyticDE.offset1);
			else
				aux.dist = min(aux.dist, aux.DE0); // hybrid
		}
	}*/
}
