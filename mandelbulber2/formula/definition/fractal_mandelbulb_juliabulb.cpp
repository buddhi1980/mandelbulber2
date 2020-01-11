/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * mandelbulb juliabulb hybrid 3D
 * constructed from Mandelbulb, Msltoe - Julia Bulb Eiffie & Msltoe - Sym4 Mod formulas.
 * note: an Offset Radius of 0.1 can sometimes improve the DE statistic
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbJuliabulb::cFractalMandelbulbJuliabulb() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Juliabulb";
	internalName = "mandelbulb_juliabulb";
	internalID = fractal::mandelbulbJuliabulb;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbJuliabulb::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
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
	}

	// sym4
	if (fractal->transformCommon.functionEnabled && aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;
		CVector4 temp = z;
		double tempL = temp.Length();
		// if (tempL < 1e-21)
		//	tempL = 1e-21;
		z *= fractal->transformCommon.scale3D111;

		aux.DE *= fabs(z.Length() / tempL);

		if (fabs(z.x) < fabs(z.z)) swap(z.x, z.z);
		if (fabs(z.x) < fabs(z.y)) swap(z.x, z.y);
		if (fabs(z.y) < fabs(z.z)) swap(z.y, z.z);

		if (z.x * z.z < 0.0) z.z = -z.z;
		if (z.x * z.y < 0.0) z.y = -z.y;

		temp.x = z.x * z.x - z.y * z.y - z.z * z.z;
		temp.y = 2.0 * z.x * z.y;
		temp.z = 2.0 * z.x * z.z;

		z = temp + fractal->transformCommon.offsetF000;
	}

	// sym3 msltoe eiffie
	if (fractal->transformCommon.functionEnabledEFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		aux.r = z.Length();
		double psi = fabs(fmod(atan2(z.z, z.y) + M_PI + M_PI_8, M_PI_4) - M_PI_8);
		double lengthYZ = sqrt(z.y * z.y + z.z * z.z);

		z.y = cos(psi) * lengthYZ;
		z.z = sin(psi) * lengthYZ;
		aux.DE = aux.DE * 2.0 * aux.r;

		CVector4 z2 = z * z;
		double rr = z2.x + z2.y + z2.z;
		double m = 1.0 - z2.z / rr;
		CVector4 temp;
		temp.x = (z2.x - z2.y) * m;
		temp.y = 2.0 * z.x * z.y * m * fractal->transformCommon.scale; // scaling y;;
		temp.z = 2.0 * z.z * sqrt(z2.x + z2.y);
		temp.w = z.w;
		z = temp + fractal->transformCommon.additionConstant000;
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
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
	// radial offset
	double lengthTempZ = -z.Length();
	// if (lengthTempZ > -1e-21)
	//	lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
	// scale
	z *= fractal->transformCommon.scale1;
	aux.DE *= fabs(fractal->transformCommon.scale1);
}
