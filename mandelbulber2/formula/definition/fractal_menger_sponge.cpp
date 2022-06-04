/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Sponge formula created by Knighty
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/kaleidoscopic-(escape-time-ifs)/
 */

#include "all_fractal_definitions.h"

cFractalMengerSponge::cFractalMengerSponge() : cAbstractFractal()
{
	nameInComboBox = "Menger sponge";
	internalName = "menger_sponge";
	internalID = fractal::mengerSponge;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalMengerSponge::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z = fabs(z);

	double temp;
	double col = 0.0;
	if (z.x < z.y)
	{
		temp = z.y;
		z.y = z.x;
		z.x = temp;
		col += fractal->foldColor.difs0000.x;
	}
	if (z.x < z.z)
	{
		temp = z.z;
		z.z = z.x;
		z.x = temp;
		col += fractal->foldColor.difs0000.y;
	}
	if (z.y < z.z)
	{
		temp = z.z;
		z.z = z.y;
		z.y = temp;
		col += fractal->foldColor.difs0000.z;
	}
	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
					&& aux.i < fractal->foldColor.stopIterationsA)
	{
		aux.color += col;
	}

	z *= fractal->transformCommon.scale3;

	z.x -= 2.0;
	z.y -= 2.0;
	if (z.z > 1.0) z.z -= 2.0;

	aux.DE *= fractal->transformCommon.scale3;
}
