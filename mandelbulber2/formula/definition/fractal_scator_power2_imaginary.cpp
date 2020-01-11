/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * quadratic iteration in imaginary scator algebra
 * Use stop at maximum iteration (at maxiter)for the image to rendered correctly
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * ix-possibly-the-holy-grail-fractal-%28in-fff-lore%29
 * https://luz.izt.uam.mx/drupal/en/fractals/ix
 * @author Manuel Fernandez-Guasti
 */

#include "all_fractal_definitions.h"

cFractalScatorPower2Imaginary::cFractalScatorPower2Imaginary() : cAbstractFractal()
{
	nameInComboBox = "Scator Power 2 Imaginary";
	internalName = "scator_power2_imaginary";
	internalID = fractal::scatorPower2Imaginary;
	DEType = deltaDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalScatorPower2Imaginary::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	Q_UNUSED(aux);

	double x2 = z.x * z.x; // + 1e-061;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;

	double newx = x2 - y2 - z2 + (y2 * z2) / x2;
	double newy = 2.0 * z.x * z.y * (1.0 - z2 / x2);
	double newz = 2.0 * z.x * z.z * (1.0 - y2 / x2);

	z.x = newx;
	z.y = newy;
	z.z = newz;
}
