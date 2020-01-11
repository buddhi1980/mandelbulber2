/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Formula invented by Benesi
 * @reference http://www.fractalforums.com/index.php?action=profile;u=1170
 */

#include "all_fractal_definitions.h"

cFractalBenesi::cFractalBenesi() : cAbstractFractal()
{
	nameInComboBox = "Benesi - Pwr2";
	internalName = "benesi";
	internalID = fractal::benesi;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalBenesi::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	CVector4 c = aux.const_c;
	aux.DE = aux.DE * 2.0 * aux.r;
	double r1 = z.y * z.y + z.z * z.z;
	double newx;
	if (c.x < 0.0 || z.x < sqrt(r1))
	{
		newx = z.x * z.x - r1;
	}
	else
	{
		newx = -z.x * z.x + r1;
	}
	r1 = -1.0 / sqrt(r1) * 2.0 * fabs(z.x);
	double newy = r1 * (z.y * z.y - z.z * z.z);
	double newz = r1 * 2.0 * z.y * z.z;

	z.x = newx;
	z.y = newy;
	z.z = newz;
}
