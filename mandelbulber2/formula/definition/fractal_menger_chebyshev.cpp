/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Sponge formula modified by Mclarekin
 * from code by Knighty
 * Chebyshev rotation from eiffie
 */

#include "all_fractal_definitions.h"

cFractalMengerChebyshev::cFractalMengerChebyshev() : cAbstractFractal()
{
	nameInComboBox = "Menger - Chebyshev";
	internalName = "menger_chebyshev";
	internalID = fractal::mengerChebyshev;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalMengerChebyshev::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	//chebyshev rotation
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		if (fractal->transformCommon.functionEnabledSwFalse) swap(z.x, z.y);
		double tmp = 0.0;
		double F = z.x / z.y;
		if (z.y == 0.0) tmp =(z.x > 0.0 ? 0.0: 4.0);
		if (fabs(F) < 1.0)
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

	z = fabs(z);

	// rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (z.x < z.y) swap(z.y, z.x);
	if (z.x < z.z) swap(z.z, z.x);
	if (z.y < z.z) swap(z.z, z.y);

	z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	z = z * fractal->transformCommon.scale3 - fractal->transformCommon.offset222;
	aux.DE = aux.DE * fractal->transformCommon.scale3 + fractal->analyticDE.offset0;
	if (z.z < -1.0) z.z += 2.0;

	// boxoffset
	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box offset
	{
		CVector4 temp = z;
		z.x = sign(z.x) * fractal->transformCommon.additionConstantA000.x + z.x;
		z.y = sign(z.y) * fractal->transformCommon.additionConstantA000.y + z.y;
		z.z = sign(z.z) * fractal->transformCommon.additionConstantA000.z + z.z;

		if (fractal->transformCommon.functionEnabledFalse)
		{
			double tempL = temp.Length();
			// if (tempL < 1e-21) tempL = 1e-21;
			double avgScale = z.Length() / tempL;
			aux.DE = aux.DE * avgScale + 1.0;
		}
	}


}
