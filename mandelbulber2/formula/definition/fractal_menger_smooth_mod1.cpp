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

cFractalMengerSmoothMod1::cFractalMengerSmoothMod1() : cAbstractFractal()
{
	nameInComboBox = "Menger - Smooth Mod1";
	internalName = "menger_smooth_mod1";
	internalID = fractal::mengerSmoothMod1;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerSmoothMod1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double t; // temp

	if (fractal->transformCommon.functionEnabled)
	{
		z = CVector4(sqrt(z.x * z.x + fractal->transformCommon.offset0),
			sqrt(z.y * z.y + fractal->transformCommon.offset0),
			sqrt(z.z * z.z + fractal->transformCommon.offset0), z.w);
	}
	if (fractal->transformCommon.functionEnabledFFalse)
	{
		z = fabs(z);
		t = fractal->transformCommon.offset;
		z += CVector4(t, t, t, 0.0);
	}

	double OffsetS = fractal->transformCommon.offset0005;

	t = z.x - z.y;
	t = fractal->transformCommon.scale05
			* (t - sqrt(t * t + OffsetS * fractal->transformCommon.constantMultiplier111.x));
	z.x = z.x - t;
	z.y = z.y + t;

	t = z.x - z.z;
	t = fractal->transformCommon.scale05
			* (t - sqrt(t * t + OffsetS * fractal->transformCommon.constantMultiplier111.y));
	z.x = z.x - t;
	z.z = z.z + t;

	t = z.y - z.z;
	t = fractal->transformCommon.scale05
			* (t - sqrt(t * t + OffsetS * fractal->transformCommon.constantMultiplier111.z));
	z.y = z.y - t;
	z.z = z.z + t;

	t = fractal->transformCommon.constantMultiplier221.z * FRAC_1_3;
	z.z -= t;
	z.z = -sqrt(z.z * z.z + OffsetS + fractal->transformCommon.offsetA0);
	z.z += t;

	z.x = fractal->transformCommon.scale3 * z.x - fractal->transformCommon.constantMultiplier221.x;
	z.y = fractal->transformCommon.scale3 * z.y - fractal->transformCommon.constantMultiplier221.y;
	z.z = fractal->transformCommon.scale3 * z.z;

	aux.DE *= fractal->transformCommon.scale3;

	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->transformCommon.functionEnabledxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box offset
	{
		z.x += sign(z.x) * fractal->transformCommon.additionConstantA000.x;
		z.y += sign(z.y) * fractal->transformCommon.additionConstantA000.y;
		z.z += sign(z.z) * fractal->transformCommon.additionConstantA000.z;
	}

	if (fractal->transformCommon.functionEnabledzFalse)
	{
		CVector4 zA = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
		CVector4 zB = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

		z = (z * fractal->transformCommon.scale1) + (zA * fractal->transformCommon.offsetA0)
				+ (zB * fractal->transformCommon.offsetB0);
		aux.DE *= fractal->transformCommon.scale1;
	}
}
