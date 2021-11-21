/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * trafassel
 *  https://fractalforums.org/fractal-mathematics-and-new-theories/28/fake-3d-mandelbrot-set/1787/msg17956#msg17956
 */

#include "all_fractal_definitions.h"

cFractalTestingLog::cFractalTestingLog() : cAbstractFractal()
{
	nameInComboBox = "Testing Log";
	internalName = "testing_log";
	internalID = fractal::testingLog;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingLog::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	const double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	const double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	const double th = th0 * fractal->bulb.power;
	const double ph = ph0 * fractal->bulb.power;
	const double cth = cos(th);
	aux.DE = (rp * aux.DE) * fractal->bulb.power + 1.0;
	rp *= aux.r;
	z.x = cth * cos(ph) * rp;
	z.y = cth * sin(ph) * rp;
	z.z = sin(th) * rp;




	/*aux.DE = aux.DE * aux.r * 2.0f;

	// Preparation operations
	double fac_eff = 0.6666666666;
	double offset = 1.0e-10;
	CVector4 c = CVector4{0.0, 0.0, 0.0, 0.0};

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	if (fractal->transformCommon.juliaMode)
	{
		c = fractal->transformCommon.constantMultiplier100;
	}
	else
	{
		if (!fractal->transformCommon.functionEnabledCFalse) c = aux.const_c;
		else c = z;

		c *= fractal->transformCommon.constantMultiplier100;
	}

	// Converting the diverging (x,y,z) back to the variable
	// that can be used for the (converging) Newton method calculation
	double sq_r = fractal->transformCommon.scale / (aux.r * aux.r + offset);
	double x1 = z.x * sq_r + fractal->transformCommon.vec111.x;
	double y1 = -z.y * sq_r + fractal->transformCommon.vec111.y;
	double z1 = -z.z * sq_r + fractal->transformCommon.vec111.z;

	double x2 = x1 * x1;
	double y2 = y1 * y1;
	double z2 = z1 * z1;

	// Calculate the inverse power of t=(x,y,z),
	// and use it for the Newton method calculations for t^power + c = 0
	// i.e. t(n+1) = 2*t(n)/3 - c/2*t(n)^2

	sq_r = x2 + y2 + z2;
	sq_r = 1.0 / (3.0 * sq_r * sq_r + offset);
	double r_xy = x2 + y2;
	double h1 = 1.0 - z2 / r_xy;

	double tmpx = h1 * (x2 - y2) * sq_r;
	double tmpy = -2.0 * h1 * x1 * y1 * sq_r;
	double tmpz = 2.0 * z1 * sqrt(r_xy) * sq_r;

	double r_2xy = sqrt(tmpx * tmpx + tmpy * tmpy);
	double r_2cxy = sqrt(c.x * c.x + c.y * c.y);
	double h2 = 1.0 - c.z * tmpz / (r_2xy * r_2cxy);

	double tmp2x = (c.x * tmpx - c.y * tmpy) * h2;
	double tmp2y = (c.y * tmpx + c.x * tmpy) * h2;
	double tmp2z = r_2cxy * tmpz + r_2xy * c.z;

	x1 = fac_eff * x1 - tmp2x;
	y1 = fac_eff * y1 - tmp2y;
	z1 = fac_eff * z1 - tmp2z;

	// Below the hack that provides a divergent value of (x,y,z) to Mandelbulber
	// although the plain Newton method does always converge

	double diffx = (x1 - fractal->transformCommon.vec111.x);
	double diffy = (y1 - fractal->transformCommon.vec111.y);
	double diffz = (z1 - fractal->transformCommon.vec111.z);

	sq_r = fractal->transformCommon.scale1 / (diffx * diffx + diffy * diffy + diffz * diffz + offset);
	z.x = diffx * sq_r;
	z.y = -diffy * sq_r;
	z.z = -diffz * sq_r;

	double dd = z.Length() / aux.r;
	dd = dd + (aux.r * 2.0 - dd) * fractal->transformCommon.scaleA1;
	aux.DE *= dd;*/


	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}

	if (fractal->analyticDE.enabled)
	{
		aux.DE0 = z.Length();

		aux.DE0 = 0.5 * log(aux.DE0) * aux.DE0 / (aux.DE);
		if (!fractal->transformCommon.functionEnabledYFalse)
			aux.dist = aux.DE0;
		else
			aux.dist = min(aux.dist, aux.DE0);
	}

	//if (fractal->transformCommon.functionEnabledXFalse)
	//	z = zc;

}
