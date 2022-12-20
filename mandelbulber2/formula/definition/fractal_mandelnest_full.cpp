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

cFractalMandelnestFull::cFractalMandelnestFull() : cAbstractFractal()
{
	nameInComboBox = "Mandelnest Full";
	internalName = "mandelnest_full";
	internalID = fractal::mandelnestFull;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelnestFull::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double Power = fractal->bulb.power;
	aux.DE = pow(aux.r, Power - 1.0f) * aux.DE * Power + 1.0;
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

	if (!fractal->transformCommon.functionEnabledAFalse)
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
	if (!fractal->transformCommon.functionEnabledAFalse)
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
	if (fractal->transformCommon.functionEnabledAFalse)
		z += aux.const_c * fractal->transformCommon.constantMultiplierA111;

	if (fractal->transformCommon.functionEnabledFalse)
	{
		zp.x = sign(aux.const_c.x);
		zp.y = sign(aux.const_c.y);
		zp.z = sign(aux.const_c.z);
		z -= fractal->transformCommon.offset000 * zp;
	}

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
