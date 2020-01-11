/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Makin3D-2 found through the another shot at the holy grail topic at ff
 * http://www.fractalgallery.co.uk/ and https://www.facebook.com/david.makin.7
 * @reference http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/
 */

#include "all_fractal_definitions.h"

cFractalMakin3d2::cFractalMakin3d2() : cAbstractFractal()
{
	nameInComboBox = "Makin3D-2";
	internalName = "makin3d2";
	internalID = fractal::makin3d2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMakin3d2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double newx = x2 + 2.0 * z.y * z.z;
	double newy = -y2 - 2.0 * z.x * z.z;
	double newz = -z2 + 2.0 * z.x * z.y;
	z.x = newx;
	z.y = newy;
	z.z = newz;
}
