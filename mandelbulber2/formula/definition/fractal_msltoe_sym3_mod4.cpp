/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MsltoeJuliaBulb Eiffie. Refer post by Eiffie    Reply #69 on: January 27, 2015
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/60/
 */

#include "all_fractal_definitions.h"

cFractalMsltoeSym3Mod4::cFractalMsltoeSym3Mod4() : cAbstractFractal()
{
	nameInComboBox = "Msltoe - Julia Bulb Mod4";
	internalName = "msltoe_sym3_mod4";
	internalID = fractal::msltoeSym3Mod4;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMsltoeSym3Mod4::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	aux.DE = aux.DE * 2.0 * aux.r;
	double psi = M_PI / fractal->transformCommon.int8Y;
	psi = fabs(fmod(atan2(z.z, z.y) + M_PI + psi, 2.0 * psi) - psi);
	double len = sqrt(z.y * z.y + z.z * z.z);
	z.y = cos(psi) * len;
	z.z = sin(psi) * len;

	CVector4 z2 = z * z;
	double rr = z2.x + z2.y + z2.z;
	double m = 1.0 - z2.z / rr;
	CVector4 temp;
	temp.x = (z2.x - z2.y) * m;
	temp.y = 2.0 * z.x * z.y * m * fractal->transformCommon.scale; // scaling y;
	temp.z = 2.0 * z.z * sqrt(z2.x + z2.y);
	temp.w = z.w;
	z = temp + fractal->transformCommon.additionConstantNeg100;

	if (fractal->transformCommon.addCpixelEnabledFalse)
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
	double lengthTempZ = -z.Length();
	// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;

	if (fractal->transformCommon.functionEnabledFalse // quaternion fold
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;
		z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, z.w);
		if (!fractal->analyticDE.enabled)
		{
			z *= CVector4(1.0, 2.0, 2.0, 1.0);
		}
		else
		{
			CVector4 temp = z;
			double tempL = temp.Length();
			z *= CVector4(1.0, 2.0, 2.0, 1.0);
			// if (tempL < 1e-21)
			//	tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			aux.DE *= avgScale;
		}
	}
	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale1);
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset0;
}
