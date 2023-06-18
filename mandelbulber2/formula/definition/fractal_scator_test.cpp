/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * quadratic iteration in real scator algebra
 * Use stop at maximum iteration (at maxiter)for the image to rendered correctly
 * @reference
 * https://luz.izt.uam.mx/drupal/en/fractals/hun
 * @author Manuel Fernandez-Guasti
 */

#include "all_fractal_definitions.h"

cFractalScatorTest::cFractalScatorTest() : cAbstractFractal()
{
	nameInComboBox = "Scator Test";
	internalName = "scator_test";
	internalID = fractal::scatorTest;
	DEType = deltaDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalScatorTest::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	 Q_UNUSED(aux);

	double x2 = z.x * z.x; //+ 1e-061
	double y2 = z.y * z.y * fractal->transformCommon.scaleB1;
	double z2 = z.z * z.z * fractal->transformCommon.scaleC1;

	double newx = x2 + y2 + z2 + (y2 * z2) * fractal->transformCommon.scaleA1 / x2;
	double newy = 2.0 * z.x * z.y * (1.0 + z2 / x2);
	double newz = 2.0 * z.x * z.z * (1.0 + y2 / x2);

	z.x = newx;
	z.y = newy;
	z.z = newz;

	z += aux.const_c;
	aux.DE = aux.DE * z.Length() / aux.r + fractal->analyticDE.offset1;
	aux.DE *= fractal->analyticDE.scale1;

	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
					&& aux.i < fractal->foldColor.stopIterationsA)
	{
		aux.color += fractal->foldColor.difs0000.x;
	}

}
