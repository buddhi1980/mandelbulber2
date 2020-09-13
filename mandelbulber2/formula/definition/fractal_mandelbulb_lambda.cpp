/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * lambdabulb
 * based on fractalrebels code http://www.fractalforums.com/mandelbulb-renderings/lambdabulb/
 * and http://bugman123.com/Hypercomplex/ LambdaBulb
 * z=c(z-z^p)
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbLambda::cFractalMandelbulbLambda() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Lambda";
	internalName = "mandelbulb_lambda";
	internalID = fractal::mandelbulbLambda;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbLambda::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse
				&& aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
			z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse
				&& aux.i >= fractal->transformCommon.startIterationsY
				&& aux.i < fractal->transformCommon.stopIterationsY)
			z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse
				&& aux.i >= fractal->transformCommon.startIterationsZ
				&& aux.i < fractal->transformCommon.stopIterationsZ)
			z.z = fabs(z.z);
	}

	CVector4 z1 = z;
	CVector4 lc = fractal->transformCommon.offset001;
	double th0 = fractal->bulb.betaAngleOffset;
	double ph0 = fractal->bulb.alphaAngleOffset;
	double Pwr = fractal->bulb.power;
	// if (aux.r < 1e-021) aux.r = 1e-021;
	if (fractal->transformCommon.functionEnabledAFalse) z1.z = -z1.z;
	th0 += asin(z1.z / aux.r);
	ph0 += atan2(z1.y, z1.x);
	double rp = pow(aux.r, Pwr - fractal->transformCommon.offset0);
	const double th = th0 * Pwr;
	const double ph = ph0 * Pwr;
	double costh = cos(th);
	z1 = rp * CVector4(costh * cos(ph), sin(ph) * costh, sin(th), 0.0);

	// aux.DE = (rp * aux.DE) * Pwr + 1.0;
	// rp *= aux.r;

	z1 = z - z1;

	// vec3 triMul(vec3 a, vec3 b) non-trig

	if (lc.x > -1e-021 && lc.x < 1e-021) lc.x = (lc.x > 0) ? 1e-021 : -1e-021;
	if (lc.y > -1e-021 && lc.y < 1e-021) lc.y = (lc.y > 0) ? 1e-021 : -1e-021;

	double r1 = sqrt(z1.x * z1.x + z1.y * z1.y);
	double r2 = sqrt(lc.x * lc.x + lc.y * lc.y);
	double a = r1 * r2;
	if (fractal->transformCommon.functionEnabledBFalse) z1.z = -z1.z;
	a = 1.0 - z1.z * lc.z / a;
	z.x = a * (z1.x * lc.x - z1.y * lc.y);
	z.y = a * (lc.x * z1.y + z1.x * lc.y);
	z.z = r2 * z1.z + r1 * lc.z;

	if (fractal->transformCommon.functionEnabledKFalse)
	{
		if (fractal->transformCommon.functionEnabledDFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD)
			swap(z.x, z.y);
		if (fractal->transformCommon.functionEnabledEFalse
				&& aux.i >= fractal->transformCommon.startIterationsE
				&& aux.i < fractal->transformCommon.stopIterationsE)
			swap(z.x, z.z);

		// swap
		if (fractal->transformCommon.functionEnabledBxFalse) z.x = -z.x;
		if (fractal->transformCommon.functionEnabledByFalse) z.y = -z.y;
		if (fractal->transformCommon.functionEnabledBzFalse) z.z = -z.z;
	}

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	double bias = fractal->transformCommon.scaleC1;
	aux.DE = max(aux.DE * bias, pow(aux.r, Pwr - 1.0) * aux.DE * Pwr + 1.0);
	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
