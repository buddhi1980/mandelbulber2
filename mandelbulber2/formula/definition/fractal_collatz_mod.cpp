/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * CollatzModIteration formula
 * @reference https://mathr.co.uk/blog/2016-04-10_collatz_fractal.html
 *            https://en.wikipedia.org/wiki/Collatz_conjecture#Iterating_on_real_or_complex_numbers
 */

#include "all_fractal_definitions.h"

cFractalCollatzMod::cFractalCollatzMod() : cAbstractFractal()
{
	nameInComboBox = "Collatz - Mod";
	internalName = "collatz_mod";
	internalID = fractal::collatzMod;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalCollatzMod::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	CVector4 constantMult = CVector4(fractal->transformCommon.constantMultiplierB111.x,
		fractal->transformCommon.constantMultiplierB111.y,
		fractal->transformCommon.constantMultiplierB111.z, 0.0);

	z = constantMult + fractal->transformCommon.scale4 * z
			- (fractal->transformCommon.constantMultiplier111 + fractal->transformCommon.scale2 * z)
					* z.RotateAroundVectorByAngle(fractal->transformCommon.constantMultiplier111.GetXYZ(),
						M_PI * fractal->transformCommon.scale1); // * cPI ;

	z *= fractal->transformCommon.scale025;

	aux.DE = aux.DE * 4.0 * fractal->analyticDE.scale1 + 1.0;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		c = CVector4(c.z, c.y, c.x, c.w);
		z += c * fractal->transformCommon.constantMultiplierA111;
	}
}
