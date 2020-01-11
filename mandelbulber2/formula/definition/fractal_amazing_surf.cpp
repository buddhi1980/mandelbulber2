/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * amazing surf from Mandelbulber3D. Formula proposed by Kali, with features added by DarkBeam
 *
 * This formula has a c.x c.y SWAP
 *
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalAmazingSurf::cFractalAmazingSurf() : cAbstractFractal()
{
	nameInComboBox = "Amazing Surf";
	internalName = "amazing_surf";
	internalID = fractal::amazingSurf;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionAmazingSurf;
}

void cFractalAmazingSurf::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// update aux.actualScale
	aux.actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	CVector4 c = aux.const_c;
	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
				- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
				- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	// no z fold

	double rr = z.Dot(z);
	if (fractal->transformCommon.functionEnabledFalse) // force cylinder fold
		rr -= z.z * z.z;

	double sqrtMinR = sqrt(fractal->transformCommon.minR05);
	double dividend = rr < sqrtMinR ? sqrtMinR : min(rr, 1.0);

	/*if (rr < sqrtMinR)
	{
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < 1.0)
	{
		aux.color += fractal->mandelbox.color.factorSp2;
	}*/
	// use aux.actualScale
	double m = aux.actualScale / dividend;

	z *= (m - 1.0) * fractal->transformCommon.scale1 + 1.0;
	// z *= m * fractal->transformCommon.scale1 + 1.0 * (1.0 - fractal->transformCommon.scale1);
	aux.DE = aux.DE * fabs(m) + 1.0;

	if (fractal->transformCommon.addCpixelEnabledFalse)
		z += CVector4(c.y, c.x, c.z, c.w) * fractal->transformCommon.constantMultiplier111;

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}
