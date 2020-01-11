/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * RiemannBulbMsltoe Mod2
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * another-way-to-make-my-riemann-sphere-'bulb'-using-a-conformal-transformation/
 */

#include "all_fractal_definitions.h"

cFractalRiemannBulbMsltoeMod2::cFractalRiemannBulbMsltoeMod2() : cAbstractFractal()
{
	nameInComboBox = "Riemann - Bulb Msltoe Mod2";
	internalName = "riemann_bulb_msltoe_mod2";
	internalID = fractal::riemannBulbMsltoeMod2;
	DEType = deltaDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 1.5;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalRiemannBulbMsltoeMod2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double radius2 = fractal->transformCommon.minR05;
	double rr = z.x * z.x + z.y * z.y + z.z * z.z; // r2 or point radius squared
	if (rr < radius2 * radius2)
	{
		if (fractal->transformCommon.functionEnabled)
			// smooth inside
			z *= radius2 * ((rr * 0.1) + 0.4) * 1.18 * fractal->transformCommon.scaleA1 / rr;
		else
		{
			z *= fractal->transformCommon.constantMultiplier111;
		}
	} // if internal smooth function disabled, then z = z * scale, default vect3(1,1,1)
	else
	{
		// 1st scale variable, default vect3 (1.7, 1.7, 1.7),
		z *= fractal->transformCommon.constantMultiplier222;
		double shift = fractal->transformCommon.offset1;
		// r1 = length^2,  //  + 1e-061
		double r1 = z.x * z.x + (z.y - shift) * (z.y - shift) + z.z * z.z;
		// inversions by length^2
		z.x = z.x / r1;
		z.y = (z.y - shift) / r1;
		z.z = z.z / r1;
		// 2nd scale variable , default = double (3.0)
		z *= fractal->transformCommon.scale3;
		// y offset variable, default = double (1.9);
		z.y = z.y + fractal->transformCommon.offset105;
		if (fractal->transformCommon.functionEnabledx)
		{
			z.x = z.x - round(z.x); // periodic cubic tiling,
			z.z = z.z - round(z.z);
		}
		if (fractal->transformCommon.functionEnabledxFalse)
		{
			z.x = fabs(sin(M_PI * z.x * fractal->transformCommon.scale1));
			z.z = fabs(sin(M_PI * z.z * fractal->transformCommon.scale1));
		}
	}
}
