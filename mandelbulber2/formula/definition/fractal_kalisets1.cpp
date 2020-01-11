/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Based on Kalisets1 and KaliDucks, from Mandelbulb 3D, and refer Formula proposed by Kali, with
 * features added by DarkBeam.
 *
 * <b>M3D notes:</b>
 * Try out julia and low R_bailout values of 2 down to 1!
 * You might have to cutoff at z=0 or so, to see something.
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/very-simple-formula-for-fractal-patterns
 */

#include "all_fractal_definitions.h"

cFractalKalisets1::cFractalKalisets1() : cAbstractFractal()
{
	nameInComboBox = "Kalisets1 Mod";
	internalName = "kalisets1";
	internalID = fractal::kalisets1;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalKalisets1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	z = fabs(z);
	double sqs = (z.x * z.x + z.y * z.y + z.z * z.z + 1e-21); // sph inv
	double m;
	double minR = fractal->transformCommon.minR0; //  KaliDucks

	if (sqs < minR)
		m = 1.0 / sqrt(minR);
	else
		m = fractal->transformCommon.scale / sqs; // kalisets

	z = z * m;
	aux.DE = aux.DE * fabs(m) + 1.0;

	if (fractal->transformCommon.addCpixelEnabledFalse)
		z += c * fractal->transformCommon.constantMultiplier111;

	if (fractal->transformCommon.juliaMode) z += fractal->transformCommon.juliaC;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
}
