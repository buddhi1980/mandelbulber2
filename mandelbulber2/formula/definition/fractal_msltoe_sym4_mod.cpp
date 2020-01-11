/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MsltoeSym4Mod  Based on the formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/15/
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalMsltoeSym4Mod::cFractalMsltoeSym4Mod() : cAbstractFractal()
{
	nameInComboBox = "Msltoe - Sym4 Mod";
	internalName = "msltoe_sym4_mod";
	internalID = fractal::msltoeSym4Mod;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMsltoeSym4Mod::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	CVector4 oldZ = z;
	aux.DE = aux.DE * 2.0 * aux.r;

	if (fabs(z.x) < fabs(z.z) * fractal->transformCommon.constantMultiplier111.x) swap(z.x, z.z);
	if (fabs(z.x) < fabs(z.y) * fractal->transformCommon.constantMultiplier111.y) swap(z.x, z.y);
	if (fabs(z.y) < fabs(z.z) * fractal->transformCommon.constantMultiplier111.z) swap(z.y, z.z);

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor.z;
	}
	if (z.x * z.z < 0.0) z.z = -z.z;
	if (z.x * z.y < 0.0) z.y = -z.y;

	z *= fractal->transformCommon.scale3D111;
	aux.DE *= z.Length() / aux.r;

	CVector4 temp = z;
	temp.x = z.x * z.x - z.y * z.y - z.z * z.z;
	temp.y = 2.0 * z.x * z.y;
	temp.z = 2.0 * z.x * z.z;

	z = temp + fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.rotationEnabled)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	double lengthTempZ = -z.Length();
	// if (lengthTempZ > -1e-21)
	//	lengthTempZ = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
	z *= fractal->transformCommon.scale1;
	aux.DE *= fabs(fractal->transformCommon.scale1);

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
}
