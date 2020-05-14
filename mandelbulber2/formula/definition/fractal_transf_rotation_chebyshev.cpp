/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * testing log
 */

#include "all_fractal_definitions.h"

cFractalTransfRotationChebyshev::cFractalTransfRotationChebyshev() : cAbstractFractal()
{
	nameInComboBox = "T>Rotation Chebyshev";
	internalName = "transf_rotation_chebyshev";
	internalID = fractal::transfRotationChebyshev;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfRotationChebyshev::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	//chebyshev
	if (fractal->transformCommon.functionEnabledAx
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		//chebyshev
		double tmp = 0.0;
		if (fractal->transformCommon.functionEnabledBxFalse) swap(z.x, z.y);
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

		tmp = tmp + fractal->transformCommon.offset111.x;
		double Length2 = max(fabs(z.x), fabs(z.y));

		double C = tmp - 8.0 * floor(tmp / 8.0);
		C = fabs(C - 4.0) - 2.0;
		z.x = clamp(C, - 1.0, 1.0) * Length2;

		double S = tmp - 2.0;
		S = S - 8.0 * floor(S / 8.0);
		S = fabs(S - 4.0) - 2.0;
		z.y = clamp(S, - 1.0, 1.0) * Length2;
	}

	if (fractal->transformCommon.functionEnabledAyFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		double tmp = 0.0;
		if (fractal->transformCommon.functionEnabledByFalse) swap(z.y, z.z);
		double F = z.y / z.z;
		if (z.z == 0.0) tmp =(z.y > 0.0 ? 0.0: 4.0);
		if (fabs(F) < 1.0)
		{
			if (z.z > 0.0) tmp = 2.0 - F;
			else tmp = 6.0 - F;
		}
		else
		{
			F = z.z / z.y;
			if (z.y > 0.0) tmp = fmod(F, 8.0);
			else tmp = 4.0 + F;
		}

		tmp = tmp + fractal->transformCommon.offset111.y;
		double Length2 = max(fabs(z.y), fabs(z.z));

		double C = tmp - 8.0 * floor(tmp / 8.0);
		C = fabs(C - 4.0) - 2.0;
		z.y = clamp(C, - 1.0, 1.0) * Length2;

		double S = tmp - 2.0;
		S = S - 8.0 * floor(S / 8.0);
		S = fabs(S - 4.0) - 2.0;
		z.z = clamp(S, - 1.0, 1.0) * Length2;
	}

	if (fractal->transformCommon.functionEnabledAzFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		double tmp = 0.0;
		if (fractal->transformCommon.functionEnabledBzFalse) swap(z.z, z.x);
		double F = z.z / z.x;
		if (z.x == 0.0) tmp =(z.z > 0.0 ? 0.0: 4.0);
		if (fabs(F) < 1.0)
		{
			if (z.x > 0.0) tmp = 2.0 - F;
			else tmp = 6.0 - F;
		}
		else
		{
			F = z.x / z.z;
			if (z.z > 0.0) tmp = fmod(F, 8.0);
			else tmp = 4.0 + F;
		}

		tmp = tmp + fractal->transformCommon.offset111.z;
		double Length2 = max(fabs(z.x), fabs(z.z));

		double C = tmp - 8.0 * floor(tmp / 8.0);
		C = fabs(C - 4.0) - 2.0;
		z.z = clamp(C, - 1.0, 1.0) * Length2;

		double S = tmp - 2.0;
		S = S - 8.0 * floor(S / 8.0);
		S = fabs(S - 4.0) - 2.0;
		z.x = clamp(S, - 1.0, 1.0) * Length2;
	}

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
