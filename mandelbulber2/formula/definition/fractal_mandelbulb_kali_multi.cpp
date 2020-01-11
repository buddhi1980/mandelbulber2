/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * based on mandelbulb Kali multi
 * @reference http://www.fractalforums.com/theory/mandelbulb-variant/
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbKaliMulti::cFractalMandelbulbKaliMulti() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Kali Multi";
	internalName = "mandelbulb_kali_multi";
	internalID = fractal::mandelbulbKaliMulti;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbKaliMulti::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}
	double costh;
	double sinth;
	double th0 = fractal->bulb.betaAngleOffset + 1e-061; // MUST keep exception catch
	double ph0 = fractal->bulb.alphaAngleOffset;
	CVector4 v;
	switch (fractal->mandelbulbMulti.orderOfXYZ)
	{
		case multi_OrderOfXYZ_xyz:
		default: v = CVector4(z.x, z.y, z.z, z.w); break;
		case multi_OrderOfXYZ_xzy: v = CVector4(z.x, z.z, z.y, z.w); break;
		case multi_OrderOfXYZ_yxz: v = CVector4(z.y, z.x, z.z, z.w); break;
		case multi_OrderOfXYZ_yzx: v = CVector4(z.y, z.z, z.x, z.w); break;
		case multi_OrderOfXYZ_zxy: v = CVector4(z.z, z.x, z.y, z.w); break;
		case multi_OrderOfXYZ_zyx: v = CVector4(z.z, z.y, z.x, z.w); break;
	}

	if (fractal->mandelbulbMulti.acosOrAsin == multi_acosOrAsin_acos)
		th0 += acos(v.x / aux.r);
	else
		th0 += asin(v.x / aux.r);
	if (fractal->mandelbulbMulti.atanOrAtan2 == multi_atanOrAtan2_atan)
		ph0 += atan(v.y / v.z);
	else
		ph0 += atan2(v.y, v.z);

	th0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleA1;

	if (fractal->transformCommon.functionEnabledBxFalse)
	{
		costh = cos(th0);
		z = aux.r * CVector4(costh * sin(ph0), cos(ph0) * costh, sin(th0), 0.0);
	}
	else
	{ // cosine mode default
		sinth = sin(th0);
		z = aux.r * CVector4(sinth * cos(ph0), sin(ph0) * sinth, cos(th0), 0.0);
	}

	if (fractal->transformCommon.functionEnabledxFalse)
	{
		switch (fractal->mandelbulbMulti.orderOfXYZ2)
		{
			case multi_OrderOfXYZ_xyz:
			default: v = CVector4(z.x, z.y, z.z, z.w); break;
			case multi_OrderOfXYZ_xzy: v = CVector4(z.x, z.z, z.y, z.w); break;
			case multi_OrderOfXYZ_yxz: v = CVector4(z.y, z.x, z.z, z.w); break;
			case multi_OrderOfXYZ_yzx: v = CVector4(z.y, z.z, z.x, z.w); break;
			case multi_OrderOfXYZ_zxy: v = CVector4(z.z, z.x, z.y, z.w); break;
			case multi_OrderOfXYZ_zyx: v = CVector4(z.z, z.y, z.x, z.w); break;
		}
		if (fractal->mandelbulbMulti.acosOrAsinA == multi_acosOrAsin_acos)
			th0 = acos(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						+ 1e-061; // MUST keep exception catch
		else
			th0 += asin(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						 + 1e-061; // MUST keep exception catch;

		if (fractal->mandelbulbMulti.atanOrAtan2A == multi_atanOrAtan2_atan)
			ph0 += atan(v.y / v.z);
		else
			ph0 += atan2(v.y, v.z);
	}
	else
	{
		switch (fractal->mandelbulbMulti.orderOfXYZ)
		{
			case multi_OrderOfXYZ_xyz:
			default: v = CVector4(z.x, z.y, z.z, z.w); break;
			case multi_OrderOfXYZ_xzy: v = CVector4(z.x, z.z, z.y, z.w); break;
			case multi_OrderOfXYZ_yxz: v = CVector4(z.y, z.x, z.z, z.w); break;
			case multi_OrderOfXYZ_yzx: v = CVector4(z.y, z.z, z.x, z.w); break;
			case multi_OrderOfXYZ_zxy: v = CVector4(z.z, z.x, z.y, z.w); break;
			case multi_OrderOfXYZ_zyx: v = CVector4(z.z, z.y, z.x, z.w); break;
		}

		if (fractal->mandelbulbMulti.acosOrAsin == multi_acosOrAsin_acos)
			th0 = acos(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						+ 1e-061; // MUST keep exception catch ??;
		else
			th0 += asin(v.x / aux.r) + fractal->transformCommon.betaAngleOffset
						 + 1e-061; // MUST keep exception catch ??;

		if (fractal->mandelbulbMulti.atanOrAtan2 == multi_atanOrAtan2_atan)
			ph0 += atan(v.y / v.z);
		else
			ph0 += atan2(v.y, v.z);
	}

	ph0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleB1 * 0.5; // 0.5 retain
	double zp = pow(aux.r, fractal->transformCommon.pwr8);

	if (fractal->transformCommon.functionEnabledzFalse)
	{ // sine mode
		costh = cos(th0);
		z = zp * CVector4(costh * sin(ph0), cos(ph0) * costh, sin(th0), 0.0);
	}
	else
	{ // cosine mode default
		sinth = sin(th0);
		z = zp * CVector4(sinth * cos(ph0), sin(ph0) * sinth, cos(th0), 0.0);
	}

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		aux.DE = pow(aux.r, fractal->transformCommon.pwr8 - fractal->analyticDE.offset1) * aux.DE
							 * fractal->transformCommon.pwr8 * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset2;
	}
	else // default, i.e. scale1 & offset1 & offset2 = 1.0
	{
		aux.DE =
			pow(aux.r, fractal->transformCommon.pwr8 - 1.0) * fractal->transformCommon.pwr8 * aux.DE
			+ 1.0;
	}
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
}
