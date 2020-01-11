/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * mix integer powers 1234
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbPower1234::cFractalMandelbulbPower1234() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Power 1234";
	internalName = "mandelbulb_power1234";
	internalID = fractal::mandelbulbPower1234;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbPower1234::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	if (fractal->transformCommon.functionEnabledFalse)
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
	double de1 = 0.0;
	double de2 = 0.0;
	double de3 = 0.0;
	double de4 = 0.0;
	CVector4 zer0s = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 newZ1 = zer0s;
	CVector4 newZ2 = zer0s;
	CVector4 newZ3 = zer0s;
	CVector4 newZ4 = zer0s;

	double th0 = fractal->bulb.betaAngleOffset;
	double ph0 = fractal->bulb.alphaAngleOffset;

	if (!fractal->transformCommon.functionEnabledxFalse)
	{
		th0 += asin(z.z / r);
		ph0 += atan2(z.y, z.x);
	}
	else
	{
		th0 += acos(z.z / r);
		ph0 += atan(z.y / z.x);
	}

	CVector4 w8ts = fractal->transformCommon.offset1111;
	double rp = r;

	if (fractal->transformCommon.functionEnabledx) // 0ne
	{
		newZ1 = z * w8ts.x;
		de1 = w8ts.x;
	}

	if (fractal->transformCommon.functionEnabledy) // two
	{
		de2 = (rp * aux.DE * 2.0 + 1.0) * w8ts.y;
		rp *= r;
		double th = th0 * 2.0 * fractal->transformCommon.scaleA1;
		double ph = ph0 * 2.0 * fractal->transformCommon.scaleB1;

		if (!fractal->transformCommon.functionEnabledxFalse)
		{
			double costh = cos(th);
			newZ2 = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}
		else
		{
			double sinth = sin(th);
			newZ2 = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		newZ2 *= w8ts.y;
	}

	if (fractal->transformCommon.functionEnabledz) // three
	{
		rp = r * r;
		de3 = (rp * aux.DE * 3.0 + 1.0) * w8ts.z;
		rp *= r;
		double th = th0 * 3.0 * fractal->transformCommon.scaleA1;
		double ph = ph0 * 3.0 * fractal->transformCommon.scaleB1;

		if (!fractal->transformCommon.functionEnabledxFalse)
		{
			double costh = cos(th);
			newZ3 = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}
		else
		{
			double sinth = sin(th);
			newZ3 = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		newZ3 *= w8ts.z;
	}

	if (fractal->transformCommon.functionEnabledw) // four
	{
		rp = r * r * r;
		de4 = (rp * aux.DE * 4.0 + 1.0) * w8ts.w;
		rp *= r;
		double th = th0 * 4.0 * fractal->transformCommon.scaleA1;
		double ph = ph0 * 4.0 * fractal->transformCommon.scaleB1;

		if (!fractal->transformCommon.functionEnabledxFalse)
		{
			double costh = cos(th);
			newZ4 = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}
		else
		{
			double sinth = sin(th);
			newZ4 = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		newZ4 *= w8ts.w;
	}
	z = newZ1 + newZ2 + newZ3 + newZ4;
	aux.DE = fabs(de1 + de2 + de3 + de4);

	if (fractal->transformCommon.functionEnabledKFalse)
	{
		if (fractal->transformCommon.functionEnabledDFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD)
			swap(z.x, z.y);
		if (fractal->transformCommon.functionEnabledEFalse
				&& aux.i >= fractal->transformCommon.startIterationsE
				&& aux.i < fractal->transformCommon.stopIterationsE)
			swap(z.x, z.z);

		// swap
		if (fractal->transformCommon.functionEnabledBxFalse) z.x = -z.x;
		if (fractal->transformCommon.functionEnabledByFalse) z.y = -z.y;
		if (fractal->transformCommon.functionEnabledBzFalse) z.z = -z.z;
	}

	z += fractal->transformCommon.additionConstant000;

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
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
