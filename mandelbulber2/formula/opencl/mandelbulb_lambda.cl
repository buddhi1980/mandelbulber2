/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * lambdabulb
 * based on fractalrebels code http://www.fractalforums.com/mandelbulb-renderings/lambdabulb/
 * and http://bugman123.com/Hypercomplex/ LambdaBulb
 * z=c(z-z^p)

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_mandelbulb_lambda.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 MandelbulbLambdaIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse
				&& aux->i >= fractal->transformCommon.startIterationsX
				&& aux->i < fractal->transformCommon.stopIterationsX)
			z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse
				&& aux->i >= fractal->transformCommon.startIterationsY
				&& aux->i < fractal->transformCommon.stopIterationsY)
			z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse
				&& aux->i >= fractal->transformCommon.startIterationsZ
				&& aux->i < fractal->transformCommon.stopIterationsZ)
			z.z = fabs(z.z);
	}

	REAL4 z1 = z;
	REAL4 lc = fractal->transformCommon.offset001;
	REAL th0 = fractal->bulb.betaAngleOffset;
	REAL ph0 = fractal->bulb.alphaAngleOffset;
	REAL Pwr = fractal->bulb.power;
	// if (aux->r < 1e-006f) aux->r = 1e-006f;
	if (fractal->transformCommon.functionEnabledAFalse) z1.z = -z1.z;
	th0 += asin(z1.z / aux->r);
	ph0 += atan2(z1.y, z1.x);
	REAL rp = pow(aux->r, Pwr - fractal->transformCommon.offset0);
	REAL th = th0 * Pwr;
	REAL ph = ph0 * Pwr;
	REAL costh = native_cos(th);
	z1 = rp * (REAL4){costh * native_cos(ph), native_sin(ph) * costh, native_sin(th), 0.0f};

	// aux->DE = (rp * aux->DE) * Pwr + 1.0f;
	// rp *= aux->r;

	z1 = z - z1;

	// vec3 triMul(vec3 a, vec3 b) non-trig

	if (lc.x > -1e-006f && lc.x < 1e-006f) lc.x = (lc.x > 0) ? 1e-006f : -1e-006f;
	if (lc.y > -1e-006f && lc.y < 1e-006f) lc.y = (lc.y > 0) ? 1e-006f : -1e-006f;

	REAL r1 = native_sqrt(z1.x * z1.x + z1.y * z1.y);
	REAL r2 = native_sqrt(lc.x * lc.x + lc.y * lc.y);
	REAL a = r1 * r2;
	if (fractal->transformCommon.functionEnabledBFalse) z1.z = -z1.z;
	a = 1.0f - z1.z * lc.z / a;
	z.x = a * (z1.x * lc.x - z1.y * lc.y);
	z.y = a * (lc.x * z1.y + z1.x * lc.y);
	z.z = r2 * z1.z + r1 * lc.z;

	if (fractal->transformCommon.functionEnabledKFalse)
	{
		if (fractal->transformCommon.functionEnabledDFalse
				&& aux->i >= fractal->transformCommon.startIterationsD
				&& aux->i < fractal->transformCommon.stopIterationsD)
		{
			REAL temp = z.x;
			z.x = z.y;
			z.y = temp;
		}
		if (fractal->transformCommon.functionEnabledEFalse
				&& aux->i >= fractal->transformCommon.startIterationsE
				&& aux->i < fractal->transformCommon.stopIterationsE)
		{
			REAL temp = z.x;
			z.x = z.z;
			z.z = temp;
		}

		// swap
		if (fractal->transformCommon.functionEnabledBxFalse) z.x = -z.x;
		if (fractal->transformCommon.functionEnabledByFalse) z.y = -z.y;
		if (fractal->transformCommon.functionEnabledBzFalse) z.z = -z.z;
	}

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux->i >= fractal->transformCommon.startIterationsR
			&& aux->i < fractal->transformCommon.stopIterationsR)
	{
		z = Matrix33MulFloat4(fractal->transformCommon.rotationMatrix, z);
	}

	REAL bias = fractal->transformCommon.scaleC1;
	aux->DE = max(aux->DE * bias, pow(aux->r, Pwr - 1.0f) * aux->DE * Pwr + 1.0f);
	aux->DE = aux->DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	return z;
}