/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Sponge formula created by Knighty,
 * Modulus Operation applied by mancoast
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/escape-time-algorithm-defined-using-the-quotient-and-remainder-functions/
 */

#include "all_fractal_definitions.h"

cFractalModulusMengerSponge::cFractalModulusMengerSponge() : cAbstractFractal()
{
	nameInComboBox = "Menger - Modulus";
	internalName = "modulus_menger_sponge";
	internalID = fractal::modulusMengerSponge;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalModulusMengerSponge::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);

	z.x = fabs(z.x);
	z.y = fabs(z.y);
	z.z = fabs(z.z);

	if (z.x - z.y < 0.0) swap(z.x, z.y);
	if (z.x - z.z < 0.0) swap(z.x, z.z);
	if (z.y - z.z < 0.0) swap(z.y, z.z);

	z *= fractal->transformCommon.scale3;

	z.x -= 2.0;
	z.y -= 2.0;

	if (fmod(z.z, M_PI) > 2.0) z.z -= 2.0;

	aux.DE *= fractal->transformCommon.scale3;
}
