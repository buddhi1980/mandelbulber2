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

cFractalEiffieMsltoe::cFractalEiffieMsltoe() : cAbstractFractal()
{
	nameInComboBox = "Msltoe - Julia Bulb Eiffie";
	internalName = "eiffie_msltoe";
	internalID = fractal::eiffieMsltoe;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalEiffieMsltoe::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
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
	/*aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	// aux.DE *= fabs(fractal->transformCommon.scale1);

	if (fractal->analyticDE.enabledFalse)
	{ // analytic DE adjustment
		aux.DE *= fabs(fractal->transformCommon.scale1) * fractal->analyticDE.scale1;
	}
	else
	{
		aux.DE *= fabs(fractal->transformCommon.scale1);
	}*/
	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) + 1.0;
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
}
