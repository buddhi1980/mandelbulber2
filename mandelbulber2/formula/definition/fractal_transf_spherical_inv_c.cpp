/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * inverted sphere z & c- A transform from M3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 * updated v2.12
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalInvC::cFractalTransfSphericalInvC() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Invert C";
	internalName = "transf_spherical_inv_c";
	internalID = fractal::transfSphericalInvC;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelAlreadyHas;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalInvC::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double rSqrL;
	CVector4 tempC;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		tempC = aux.c;
		tempC *= fractal->transformCommon.constantMultiplier111;
		rSqrL = tempC.Dot(tempC);
		// if (rSqrL < 1e-21) rSqrL = 1e-21;
		rSqrL = 1.0 / rSqrL;
		tempC *= rSqrL;
		aux.c = tempC;
	}
	else
	{
		tempC = aux.const_c;
		tempC *= fractal->transformCommon.constantMultiplier111;
		rSqrL = tempC.Dot(tempC);
		// if (rSqrL < 1e-21) rSqrL = 1e-21;
		rSqrL = 1.0 / rSqrL;
		tempC *= rSqrL;
	}

	if (fractal->transformCommon.functionEnabledAwFalse)
	{
		rSqrL = z.Dot(z);
		// if (rSqrL < 1e-21) rSqrL = 1e-21;
		rSqrL = 1.0 / rSqrL;
		z *= rSqrL;
		aux.DE *= rSqrL;
		z += tempC;
	}
	z += tempC;
}
