/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MsltoeSym3Mod based on the formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/15/
 */

#include "all_fractal_definitions.h"

cFractalMsltoeSym3Mod::cFractalMsltoeSym3Mod() : cAbstractFractal()
{
	nameInComboBox = "Msltoe - Sym3 Mod";
	internalName = "msltoe_sym3_mod";
	internalID = fractal::msltoeSym3Mod;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMsltoeSym3Mod::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	aux.DE = aux.DE * 2.0 * aux.r;
	CVector4 temp = z;
	if (fabs(z.y) < fabs(z.z)) // then swap
	{
		z.y = temp.z; // making z.y furthest away from axis
		z.z = temp.y;
	}
	if (z.y > z.z) // then change sign of z.x and z.z
	{
		z.x = -z.x;
		z.z = -z.z;
	}
	CVector4 z2 = z * z; // squares
	// sum of squares
	double v3 = (z2.x + z2.y + z2.z)
							+ fractal->transformCommon.scale0 * fractal->transformCommon.scale0 * z2.y * z2.z;
	// if (v3 < 1e-21 && v3 > -1e-21) v3 = (v3 > 0) ? 1e-21 : -1e-21;
	double zr = 1.0 - z2.z / v3;
	temp.x = (z2.x - z2.y) * zr;
	temp.y = 2.0 * z.x * z.y * zr * fractal->transformCommon.scale; // scaling temp.y
	temp.z = 2.0 * z.z * sqrt(z2.x + z2.y);

	z = temp + fractal->transformCommon.additionConstant000;

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
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;

	if (fractal->transformCommon.functionEnabledFalse // quaternion fold
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;
		z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, z.w);

		if (fractal->analyticDE.enabledFalse)
		{
			CVector4 temp2 = z;
			double tempL = temp2.Length();
			z *= CVector4(1.0, 2.0, 2.0, 1.0);
			// if (tempL < 1e-21) tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			// aux.DE *= avgScale * fractal->transformCommon.scaleA1;
			double tempAux = aux.DE * avgScale;
			aux.DE = aux.DE + (tempAux - aux.DE) * fractal->analyticDE.scale1;
		}
		else
		{
			z *= CVector4(1.0, 2.0, 2.0, 1.0);
		}
	}
}
