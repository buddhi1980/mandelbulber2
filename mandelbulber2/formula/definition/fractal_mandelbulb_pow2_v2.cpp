/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MandelbulbPow2 v2  buffalo , Quick Dudley types and makin3D-2
 * @reference http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/
 * and http://www.fractalgallery.co.uk/ and https://www.facebook.com/david.makin.7
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbPow2V2::cFractalMandelbulbPow2V2() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Pow2 V2";
	internalName = "mandelbulb_pow2_v2";
	internalID = fractal::mandelbulbPow2V2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbPow2V2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// Q_UNUSED(aux);

	aux.DE = aux.DE * aux.r * 2.0;
	CVector4 c = aux.const_c;
	//  pre-abs
	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (fractal->transformCommon.functionEnabledCxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledCyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledCzFalse) z.z = fabs(z.z);
	}
	// swap axis
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		CVector4 v = z;
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
		z = v;
	}

	// Pow2
	CVector4 oldZ = z;

	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 zz = z * z;
		if (fractal->transformCommon.functionEnabledDFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD)
			zz *= fractal->transformCommon.constantMultiplierA111;
		CVector4 Scale2 = CVector4(2.0, 2.0, 2.0, 1.0);
		if (fractal->transformCommon.functionEnabledEFalse
				&& aux.i >= fractal->transformCommon.startIterationsE
				&& aux.i < fractal->transformCommon.stopIterationsE)
			Scale2 = fractal->transformCommon.constantMultiplier222;

		CVector4 v4 = z;
		switch (fractal->combo4.combo4)
		{
			case multi_combo4_type1:
			default:
				if (fractal->transformCommon.functionEnabledXFalse)
					v4.x = fabs(zz.x - zz.y - zz.z) * -Scale2.x * 0.5;
				else
					v4.x = (zz.x - zz.y - zz.z) * -Scale2.x * 0.5;
				oldZ = fabs(oldZ);
				v4.y = oldZ.x * oldZ.y * -Scale2.y;
				v4.z = oldZ.x * oldZ.z * -Scale2.z;
				break;
			// lkmitch/quick dudley type
			case multi_combo4_type2:
				v4.x = zz.x - Scale2.x * oldZ.y * oldZ.z;
				v4.y = zz.z + Scale2.y * oldZ.x * oldZ.y;
				v4.z = zz.y - Scale2.z * oldZ.x * oldZ.z;
				break;
			// makin 3D-2 type
			case multi_combo4_type3:
				v4.x = zz.x + Scale2.x * oldZ.y * oldZ.z;
				v4.y = -zz.y - Scale2.y * oldZ.x * oldZ.z;
				v4.z = -zz.z + Scale2.z * oldZ.x * oldZ.y;
				aux.DE += 1.0;
				break;
			// buffalo V2
			case multi_combo4_type4:
				oldZ = fabs(oldZ);
				v4.x = (zz.x - zz.y - zz.z) * Scale2.x * 0.5 - oldZ.x;
				v4.y = oldZ.x * oldZ.y * Scale2.y - oldZ.y;
				v4.z = oldZ.x * oldZ.z * Scale2.z - oldZ.z;
				break;
		}
		z = v4;
	}

	//  post abs
	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		if (fractal->transformCommon.functionEnabledBxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledByFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledBzFalse) z.z = fabs(z.z);
	}

	// offset or juliaC
	if (aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		z += fractal->transformCommon.offset000;
	}

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
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
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	// rotation
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
