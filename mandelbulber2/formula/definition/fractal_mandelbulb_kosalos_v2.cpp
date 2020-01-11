/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * based on formula by kosalos
 * https://fractalforums.org/fractal-mathematics-and-new-theories/28
 * /julia-sets-and-altering-the-iterate-afterwards/2871/msg16342#msg16342
 * This formula contains aux.const_c, and uses aux.c for oldZ
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbKosalosV2::cFractalMandelbulbKosalosV2() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Kosalos V2";
	internalName = "mandelbulb_kosalos_v2";
	internalID = fractal::mandelbulbKosalosV2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbKosalosV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double power = fractal->bulb.power;

	CVector4 diffVec = CVector4(0.001, 0.001, 0.001, 0.0) + aux.c - z;
	aux.c = z;
	double diffLen = diffVec.Length(); // > 3.16e-5
	double thetaTweak =
		fractal->transformCommon.scaleA1 * 0.01 / (diffLen + fractal->transformCommon.offsetA0 * 0.01);

	if (!fractal->transformCommon.functionEnabledxFalse)
	{
		thetaTweak = (1.0 - thetaTweak);
	}
	else // mode2
	{
		thetaTweak = thetaTweak + (1.0 - thetaTweak) * fractal->transformCommon.scaleB1;
	}

	double theta;
	if (!fractal->transformCommon.functionEnabledAzFalse)
	{
		double xyL = sqrt(z.x * z.x + z.y * z.y);
		theta = atan2(xyL * thetaTweak, z.z);
	}
	else
	{
		theta = asin(z.z / aux.r * thetaTweak);
	}
	theta = (theta + fractal->bulb.betaAngleOffset) * power;

	double phi = atan2(z.y, z.x) * power;
	double pwr = pow(aux.r, power);

	if (!fractal->transformCommon.addCpixelEnabledFalse) // z = old z + new z
	{
		if (!fractal->transformCommon.functionEnabledzFalse)
		{
			double ss = sin(theta) * pwr;
			z.x += ss * cos(phi);
			z.y += ss * sin(phi);
			z.z += pwr * cos(theta);
		}
		else
		{
			double cs = cos(theta) * pwr;
			z.x += cs * sin(phi);
			z.y += cs * cos(phi);
			z.z += pwr * sin(theta);
		}
		aux.DE += (pow(aux.r, power - 1.0) * power * aux.DE);
		z.z += fractal->transformCommon.offset0;
	}
	else // z = f(z) + c
	{
		if (!fractal->transformCommon.functionEnabledzFalse)
		{
			double ss = sin(theta * power) * pwr;
			z.x = ss * cos(phi);
			z.y = ss * sin(phi);
			z.z = pwr * cos(theta * power);
		}
		else
		{
			double cs = cos(theta * power) * pwr;
			z.x += cs * sin(phi);
			z.y += cs * cos(phi);
			z.z += pwr * sin(theta * power);
		}
		aux.DE = (pow(aux.r, power - 1.0) * power * aux.DE);
		z.z += fractal->transformCommon.offset0;
		c *= fractal->transformCommon.constantMultiplierC111;
		if (!fractal->transformCommon.functionEnabledyFalse)
			z += c;
		else
			z += CVector4(c.y, c.x, c.z, 0.0);
	}
	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
