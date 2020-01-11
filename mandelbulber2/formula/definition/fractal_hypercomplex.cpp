/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * 3D Mandelbrot formula invented by David Makin
 * http://www.fractalgallery.co.uk/ and https://www.facebook.com/david.makin.7
 * @reference
 *
 http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/msg7235/#msg7235
 */

#include "all_fractal_definitions.h"

cFractalHypercomplex::cFractalHypercomplex() : cAbstractFractal()
{
	nameInComboBox = "Hypercomplex";
	internalName = "hypercomplex";
	internalID = fractal::hypercomplex;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalHypercomplex::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	aux.DE = aux.DE * 2.0 * aux.r;
	double newx = z.x * z.x - z.y * z.y - z.z * z.z - z.w * z.w;
	double newy = 2.0 * z.x * z.y - 2.0 * z.w * z.z;
	double newz = 2.0 * z.x * z.z - 2.0 * z.y * z.w;
	double neww = 2.0 * z.x * z.w - 2.0 * z.y * z.z;
	z.x = newx;
	z.y = newy;
	z.z = newz;
	z.w = neww;
}
