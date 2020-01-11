/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * ABoxModKali V3, a formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/
 */

#include "all_fractal_definitions.h"

cFractalAboxModKaliV3::cFractalAboxModKaliV3() : cAbstractFractal()
{
	nameInComboBox = "Abox - Mod Kali-V3";
	internalName = "abox_mod_kali_v3";
	internalID = fractal::aboxModKaliV3;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionIFS;
}

void cFractalAboxModKaliV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterations15)
	{
		z = fractal->transformCommon.offset111 - fabs(z);
		double rr = z.Dot(z);
		double MinR = fractal->transformCommon.minR0;
		double dividend = rr < MinR ? MinR : min(rr, 1.0);
		double m = -fractal->transformCommon.scale015 / dividend;
		z = z * m;
		aux.DE = aux.DE * fabs(m) + 1.0;
	}

	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z += fractal->transformCommon.offsetA111;
	}

	// TODO add the options
}
