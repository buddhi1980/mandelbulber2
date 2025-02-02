/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * KochIteration
 * Based on Knighty's Kaleidoscopic IFS 3D Fractals, described here:
 * http://www.fractalforums.com/3d-fractal-generation/kaleidoscopic-%28escape-time-ifs%29/
 */

#include "all_fractal_definitions.h"

cFractalKoch::cFractalKoch() : cAbstractFractal()
{
	nameInComboBox = "Koch";
	internalName = "koch";
	internalID = fractal::koch;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalKoch::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double col = 0.0;
	double temp = 0.0;
	z.x = fabs(z.x);
	z.y = fabs(z.y);
	if (z.y > z.x)
	{
		temp = z.x;
		z.x = z.y;
		z.y = temp;
		col += fractal->foldColor.difs0000.x;
	}

	double YOff = FRAC_1_3 * fractal->transformCommon.scale1;
	z.y = YOff - fabs(z.y - YOff);

	z.x += FRAC_1_3;
	if (z.z > z.x)
	{
		temp = z.x;
		z.x = z.z;
		z.z = temp;
		col += fractal->foldColor.difs0000.y;
	}
	z.x -= FRAC_1_3;

	z.x -= FRAC_1_3;
	if (z.z > z.x)
	{
		temp = z.x;
		z.x = z.z;
		z.z = temp;
		col += fractal->foldColor.difs0000.z;
	}
	z.x += FRAC_1_3;

	CVector4 Offset = fractal->transformCommon.offset100;
	z = fractal->transformCommon.scale3 * (z - Offset) + Offset;
	aux.DE = aux.DE * fractal->transformCommon.scale3;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	aux.dist = fabs(z.Length() - Offset.Length());
	aux.dist = aux.dist / aux.DE;

	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
					&& aux.i < fractal->foldColor.stopIterationsA)
	{
		aux.color += col;
	}

}
