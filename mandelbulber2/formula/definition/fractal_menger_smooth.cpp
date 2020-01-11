/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Smooth
 * http://www.fractalforums.com/fragmentarium/help-t22583/
 */

#include "all_fractal_definitions.h"

cFractalMengerSmooth::cFractalMengerSmooth() : cAbstractFractal()
{
	nameInComboBox = "Menger - Smooth";
	internalName = "menger_smooth";
	internalID = fractal::mengerSmooth;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerSmooth::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double sc1 = fractal->transformCommon.scale3 - 1.0; // 3 - 1 = 2, 2/3 = 0.6667;
	double sc2 = sc1 / fractal->transformCommon.scale3; //  8 - 1 = 7, 7/8 = 0.89ish;
	double OffsetS = fractal->transformCommon.offset0005;

	// the closer to origin the greater the effect of OffsetSQ
	z =
		CVector4(sqrt(z.x * z.x + OffsetS), sqrt(z.y * z.y + OffsetS), sqrt(z.z * z.z + OffsetS), z.w);

	double t;
	CVector4 OffsetC = fractal->transformCommon.offset1105;

	t = z.x - z.y;
	t = 0.5 * (t - sqrt(t * t + OffsetS));
	z.x = z.x - t;
	z.y = z.y + t;

	t = z.x - z.z;
	t = 0.5 * (t - sqrt(t * t + OffsetS));
	z.x = z.x - t;
	z.z = z.z + t;

	t = z.y - z.z;
	t = 0.5 * (t - sqrt(t * t + OffsetS));
	z.y = z.y - t;
	z.z = z.z + t;

	z.z = z.z - OffsetC.z * sc2; // sc2 reduces C.z
	z.z = -sqrt(z.z * z.z + OffsetS);
	z.z = z.z + OffsetC.z * sc2;

	z.x = fractal->transformCommon.scale3 * z.x - OffsetC.x * sc1; // sc1 scales up C.x
	z.y = fractal->transformCommon.scale3 * z.y - OffsetC.y * sc1;
	z.z = fractal->transformCommon.scale3 * z.z;

	aux.DE *= fractal->transformCommon.scale3;

	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
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
