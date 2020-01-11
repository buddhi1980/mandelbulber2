/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Msltoe_Julia_Bulb_Mod3
 * @reference
 * http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/msg14320/#msg14320
 */

#include "all_fractal_definitions.h"

cFractalMsltoeSym3Mod3::cFractalMsltoeSym3Mod3() : cAbstractFractal()
{
	nameInComboBox = "Msltoe - Julia Bulb Mod3";
	internalName = "msltoe_sym3_mod3";
	internalID = fractal::msltoeSym3Mod3;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMsltoeSym3Mod3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	aux.DE = aux.DE * 2.0 * aux.r;
	CVector4 z1 = z;
	double psi = atan2(z.z, z.y) + M_PI * 2.0;
	double psi2 = 0;
	while (psi > M_PI_8)
	{
		psi -= M_PI_4;
		psi2 -= M_PI_4; // M_PI_4 = pi/4
	}
	double cs = cos(psi2);
	double sn = sin(psi2);
	z1.y = z.y * cs - z.z * sn;
	z1.z = z.y * sn + z.z * cs;
	z.y = z1.y;
	z.z = z1.z;
	CVector4 zs = z * z;
	double zs2 = zs.x + zs.y;
	// if (zs2 < 1e-21)
	//	zs2 = 1e-21;
	double zs3 =
		(zs2 + zs.z) + fractal->transformCommon.scale0 * fractal->transformCommon.scale0 * zs.y * zs.z;
	double zsd = (1.0 - zs.z / zs3);

	z1.x = (zs.x - zs.y) * zsd;
	z1.y = (2.0 * z.x * z.y) * zsd * fractal->transformCommon.scale; // scaling y;
	z1.z = 2.0 * z.z * sqrt(zs2);
	z.x = z1.x;
	z.y = z1.y * cs + z1.z * sn;
	z.z = -z1.y * sn + z1.z * cs;
	z += fractal->transformCommon.additionConstant000;
	if (fractal->transformCommon.addCpixelEnabledFalse) // symmetrical addCpixel
	{
		CVector4 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;
		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}
	double lengthTempZ = -z.Length(); // spherical offset
	// if (lengthTempZ > -1e-21)
	//	lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE *= fabs(fractal->transformCommon.scale1);

	if (fractal->transformCommon.functionEnabledFalse // quaternion fold
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;
		z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, z.w);
		if (fractal->analyticDE.enabledFalse)
		{
			CVector4 temp = z;
			double tempL = temp.Length();
			z *= CVector4(1.0, 2.0, 2.0, 1.0);
			// if (tempL < 1e-21)
			//	tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			aux.DE *= avgScale;
		}
		else
		{
			z *= CVector4(1.0, 2.0, 2.0, 1.0);
		}
	}
}
