/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Testing transform2
 *https://fractalforums.org/fractal-mathematics-and-new-theories/28/fake-3d-mandelbrot-set/1787/msg17940#msg17940
 */

#include "fractal_definitions.h"

cFractalTestingTransform2::cFractalTestingTransform2() : cAbstractFractal()
{
	nameInComboBox = "Testing Transform2";
	internalName = "testing_transform2";
	internalID = fractal::testingTransform2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingTransform2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// double colorAdd = 0.0;
	aux.DE = 2.0 * aux.DE * aux.r + 1.0;

	//  pre-abs
	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (fractal->transformCommon.functionEnabledCxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledCyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledCzFalse) z.z = fabs(z.z);
	}

	CVector4 ZZ = z * z;
	double rr = z.Dot(z);
	double theta = atan2(z.z, sqrt(ZZ.x + ZZ.y));
	double phi = atan2(z.y, z.x);
	double thetatemp = theta;

	/*if(usespin)if(spin)
		{
			theta = -theta;
	if(theta > 0.0)
	{
		theta = -pi1p5 + theta;
	}
	else
	{
		double alpha = pi0p5 + theta;
		theta = M_PI + alpha;
	}
	theta = theta + M_PI;
	}*/

	double phi_pow = 2.0 * phi + M_PI;
	double theta_pow = theta + M_PI + M_PI / 2.0; // piAdd;+ M_PI / 2.0
	if (fractal->transformCommon.functionEnabledFalse) theta_pow = theta + M_PI / 4.0;
	// theta_pow = theta + thetatemp + M_PI / 2.0;
	if (fractal->transformCommon.functionEnabledAFalse) theta_pow = theta + thetatemp + M_PI;

	double rn_sin_theta_pow = rr * sin(theta_pow);
	z.x = rn_sin_theta_pow * cos(phi_pow); //  + jx
	z.y = rn_sin_theta_pow * sin(phi_pow); // + jy
	z.z = rr * cos(theta_pow);						 //  + jz

	// z+=(Julia ? JuliaC : pos);

	/*if(usespin)
	{
		if(spin)
		{
			theta += thetatemp - piAdd;
					if(theta > pi1p5) spin = !spin;
		else
		{
		if(theta < pi0p5) spin = !spin;
		}
		}
		else
	{
			if(theta > pi0p25) spin = !spin;
			 else if(theta < -pi0p25) spin = !spin;
		}
	}*/

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
