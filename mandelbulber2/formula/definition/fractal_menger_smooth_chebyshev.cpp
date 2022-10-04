/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Smooth Mod1, based on :
 * http://www.fractalforums.com/fragmentarium/help-t22583/
 */

#include "all_fractal_definitions.h"

cFractalMengerSmoothChebyshev::cFractalMengerSmoothChebyshev() : cAbstractFractal()
{
	nameInComboBox = "Menger - Smooth Chebyshev";
	internalName = "menger_smooth_chebyshev";
	internalID = fractal::mengerSmoothChebyshev;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerSmoothChebyshev::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	//chebyshev rotation
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		if (fractal->transformCommon.functionEnabledSwFalse) swap(z.x, z.y);
		double tmp = 0.0;
		double F = z.x / z.y;
		if (z.y == 0.0) tmp = (z.x > 0.0 ? 0.0: 4.0);
		else if (fabs(F) < 1.0)
		{
			if (z.y > 0.0) tmp = 2.0 - F;
			else tmp = 6.0 - F;
		}
		else
		{
			F = z.y / z.x;
			if (z.x > 0.0) tmp = fmod(F, 8.0);
			else tmp = 4.0 + F;
		}

		tmp = tmp + fractal->transformCommon.offset1;

		double Length2 = max(fabs(z.x), fabs(z.y));

		double C = tmp - 8.0 * floor(tmp / 8.0);

		C = fabs(C - 4.0) - 2.0;
		z.x = clamp(C, - 1.0, 1.0) * Length2;

		double S = tmp - 2.0;
		S = S - 8.0 * floor(S / 8.0);
		S = fabs(S - 4.0) - 2.0;
		z.y = clamp(S, - 1.0, 1.0) * Length2;
		aux.DE *= SQRT_1_2 * 2.0;
	}

	if (fractal->transformCommon.functionEnabled)
	{
		z = CVector4(sqrt(z.x * z.x + fractal->transformCommon.offset0),
			sqrt(z.y * z.y + fractal->transformCommon.offset0),
			sqrt(z.z * z.z + fractal->transformCommon.offset0), z.w);
	}
	if (fractal->transformCommon.functionEnabledFFalse)
	{
		z = fabs(z);
		double s = fractal->transformCommon.offset;
		z += CVector4(s, s, s, 0.0);
	}

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);


	double t;
	//double ScaleP5 = fractal->transformCommon.scale05;
	//CVector4 OffsetC = fractal->transformCommon.constantMultiplier221;
	double OffsetS = fractal->transformCommon.offset0005;
	// if OffsetS * fractal->transformCommon.constantMultiplier111 =0

	double col = 0.0;
	if (z.x < z.y) col += fractal->foldColor.difs0000.x;

	t = z.x - z.y;
	t = fractal->transformCommon.scale05
			* (t - sqrt(t * t + OffsetS * fractal->transformCommon.constantMultiplier111.x));
	z.x = z.x - t;
	z.y = z.y + t;

		if (z.x < z.z) col += fractal->foldColor.difs0000.y;


	t = z.x - z.z;
	t = fractal->transformCommon.scale05
			* (t - sqrt(t * t + OffsetS * fractal->transformCommon.constantMultiplier111.y));
	z.x = z.x - t;
	z.z = z.z + t;

		if (z.y < z.z)  col += fractal->foldColor.difs0000.z;

	t = z.y - z.z;
	t = fractal->transformCommon.scale05
			* (t - sqrt(t * t + OffsetS * fractal->transformCommon.constantMultiplier111.z));
	z.y = z.y - t;
	z.z = z.z + t;

	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
					&& aux.i < fractal->foldColor.stopIterationsA)
	{
		aux.color += col;
	}


	t = fractal->transformCommon.constantMultiplier221.z * FRAC_1_3;
	z.z -= t;
	z.z = -sqrt(z.z * z.z + fractal->transformCommon.offset0005);
	z.z += t;

	z.x = fractal->transformCommon.scale3 * z.x - fractal->transformCommon.constantMultiplier221.x;
	z.y = fractal->transformCommon.scale3 * z.y - fractal->transformCommon.constantMultiplier221.y;
	z.z = fractal->transformCommon.scale3 * z.z;

	aux.DE *= fractal->transformCommon.scale3;

	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box offset
	{
		z.x += sign(z.x) * fractal->transformCommon.additionConstantA000.x;
		z.y += sign(z.y) * fractal->transformCommon.additionConstantA000.y;
		z.z += sign(z.z) * fractal->transformCommon.additionConstantA000.z;
	}
}
