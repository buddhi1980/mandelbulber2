/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * CollatzIteration formula
 * @reference https://mathr.co.uk/blog/2016-04-10_collatz_fractal.html
 *            https://en.wikipedia.org/wiki/Collatz_conjecture#Iterating_on_real_or_complex_numbers
 */

#include "all_fractal_definitions.h"

cFractalCollatz::cFractalCollatz() : cAbstractFractal()
{
	nameInComboBox = "Collatz";
	internalName = "collatz";
	internalID = fractal::collatz;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalCollatz::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	CVector4 xV = CVector4(1.0, 1.0, 1.0, 0.0);
	CVector4 temp = xV + z * 2.0;
	temp *= z.RotateAroundVectorByAngle(xV.GetXYZ(), M_PI);
	z = xV + z * 4.0 - temp;
	z /= 4.0;
	aux.DE = aux.DE * 4.0 + 1.0;
}
