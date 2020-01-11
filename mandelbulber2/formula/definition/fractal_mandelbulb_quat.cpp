/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * mandelbulb Quaternion
 *
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbQuat::cFractalMandelbulbQuat() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Quat";
	internalName = "mandelbulb_quat";
	internalID = fractal::mandelbulbQuat;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbQuat::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		CVector4 c = aux.const_c;
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

	// Quaternion fold
	if (aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		aux.r = z.Length();
		z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, 0.0);
		aux.DE = aux.DE * 2.0 * aux.r;
		double tempL = z.Length();
		z *= fractal->transformCommon.constantMultiplier122;
		// if (tempL < 1e-21) tempL = 1e-21;
		CVector3 tempAvgScale = CVector3(z.x, z.y / 2.0, z.z / 2.0);
		double avgScale = tempAvgScale.Length() / tempL;
		double tempAux = aux.DE * avgScale;
		aux.DE = aux.DE + (tempAux - aux.DE) * fractal->analyticDE.scale1;
		z += fractal->transformCommon.offset000;
		// if (!fractal->analyticDE.enabledFalse)
		//	aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;
		// else
		//	aux.DE =
		//		aux.DE * fabs(aux.actualScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
	// sym4
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;
		CVector4 temp = z;
		double tempL = temp.Length();
		// if (tempL < 1e-21)
		//	tempL = 1e-21;
		z *= fractal->transformCommon.scale3D111;

		aux.DE *= z.Length() / tempL;

		if (fabs(z.x) < fabs(z.z)) swap(z.x, z.z);
		if (fabs(z.x) < fabs(z.y)) swap(z.x, z.y);
		if (fabs(z.y) < fabs(z.z)) swap(z.y, z.z);

		if (z.x * z.z < 0.0) z.z = -z.z;
		if (z.x * z.y < 0.0) z.y = -z.y;

		temp.x = z.x * z.x - z.y * z.y - z.z * z.z;
		temp.y = 2.0 * z.x * z.y;
		temp.z = 2.0 * z.x * z.z;

		z = temp + fractal->transformCommon.offsetF000;
		// radial offset
		double lengthTempZ = -z.Length();
		// if (lengthTempZ > -1e-21)
		//	lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;

		// scale
		z *= fractal->transformCommon.scale1;
		aux.DE *= fabs(fractal->transformCommon.scale1);
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	// mandelbulb multi
	if (aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		aux.r = z.Length();
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

		double th0 = fractal->bulb.betaAngleOffset;
		double ph0 = fractal->bulb.alphaAngleOffset;

		CVector3 v;
		switch (fractal->sinTan2Trig.orderOfZYX)
		{
			case multi_OrderOfZYX_zyx:
			default: v = CVector3(z.z, z.y, z.x); break;
			case multi_OrderOfZYX_zxy: v = CVector3(z.z, z.x, z.y); break;
			case multi_OrderOfZYX_yzx: v = CVector3(z.y, z.z, z.x); break;
			case multi_OrderOfZYX_yxz: v = CVector3(z.y, z.x, z.z); break;
			case multi_OrderOfZYX_xzy: v = CVector3(z.x, z.z, z.y); break;
			case multi_OrderOfZYX_xyz: v = CVector3(z.x, z.y, z.z); break;
		}

		if (fractal->sinTan2Trig.asinOrAcos == multi_asinOrAcos_asin)
			th0 += asin(v.x / aux.r);
		else
			th0 += acos(v.x / aux.r);

		if (fractal->sinTan2Trig.atan2OrAtan == multi_atan2OrAtan_atan2)
			ph0 += atan2(v.y, v.z);
		else
			ph0 += atan(v.y / v.z);

		double rp = pow(aux.r, fractal->bulb.power - 1.0);
		double th = th0 * fractal->bulb.power * fractal->transformCommon.scaleA1;
		double ph = ph0 * fractal->bulb.power * fractal->transformCommon.scaleB1;

		aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;
		rp *= aux.r;

		if (fractal->transformCommon.functionEnabledxFalse)
		{ // cosine mode
			double sinth = th;
			if (fractal->transformCommon.functionEnabledyFalse) sinth = th0;
			sinth = sin(sinth);
			z = rp * CVector4(sinth * sin(ph), cos(ph) * sinth, cos(th), 0.0);
		}
		else
		{ // sine mode
			double costh = th;
			if (fractal->transformCommon.functionEnabledzFalse) costh = th0;
			costh = cos(costh);
			z = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);
		}

		if (fractal->transformCommon.functionEnabledKFalse)
		{
			if (fractal->transformCommon.functionEnabledDFalse) swap(z.x, z.y);
			if (fractal->transformCommon.functionEnabledEFalse) swap(z.x, z.z);

			// swap
			if (fractal->transformCommon.functionEnabledBxFalse) z.x = -z.x;
			if (fractal->transformCommon.functionEnabledByFalse) z.y = -z.y;
			if (fractal->transformCommon.functionEnabledBzFalse) z.z = -z.z;
		}
	}
	z += fractal->transformCommon.additionConstant000;
}
