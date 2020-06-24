/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * based on formula by pupukuusikko
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/a-new-3d-mandelbrot-variant-mandelcup/
 */

#include "all_fractal_definitions.h"

cFractalMandelcup::cFractalMandelcup() : cAbstractFractal()
{
	nameInComboBox = "Mandelcup";
	internalName = "mandelcup";
	internalID = fractal::mandelcup;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelcup::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	if (fractal->transformCommon.functionEnabledNFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse
				&& aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
			z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse
				&& aux.i >= fractal->transformCommon.startIterationsY
				&& aux.i < fractal->transformCommon.stopIterationsY)
			z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse
				&& aux.i >= fractal->transformCommon.startIterationsZ
				&& aux.i < fractal->transformCommon.stopIterationsZ)
			z.z = fabs(z.z);
	}

	double r = aux.r;
	aux.DE = r * 2.0 * aux.DE + 1.0;
	double Scale = fractal->transformCommon.scale1;
	double Shape = fractal->transformCommon.offset0;
	double temp = 1.0;

	// stereographic projection, modified a bit
	if (fractal->transformCommon.functionEnabledx)
	{
		z.x /= r;
		z.y /= r;
	}
	if (fractal->transformCommon.functionEnabledz) z.z = (z.z / r) + Shape;

	if (fractal->transformCommon.functionEnabledBFalse)
	{
		z.z *= z.z;
		Shape *= Shape;
	}
	if (fractal->transformCommon.functionEnabledy)
	{
		z.x /= z.z;
		z.y /= z.z;
	}
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		temp = 1.0 / (z.z * z.z * 1.0);
		z.x *= temp;
		z.y *= temp;
	}
	if (fractal->transformCommon.functionEnabledEFalse)
	{
		z.x *= z.z;
		z.y *= z.z;
	}

	// complex multiplication
	temp = z.x * z.x - z.y * z.y;
	z.y = 2.0 * z.x * z.y;
	z.x = temp;
	temp = Scale * (1.0 + (Shape * Shape));

	z.x *= temp;
	z.y *= temp;

	// inverse stereographic
	double mag1 = z.x * z.x + z.y * z.y - 1.0;
	double mag2 = 1.0 / (mag1 + 2.0);
	z.x = z.x * 2.0 * mag2;
	z.y = z.y * 2.0 * mag2;
	z.z = mag1 * mag2;

	z *= r * r;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}

	z += fractal->transformCommon.additionConstant000;

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
