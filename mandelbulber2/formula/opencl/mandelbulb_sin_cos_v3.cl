/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2022 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Mandelbulb fractal.
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_mandelbulb_sin_cos_v3.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 MandelbulbSinCosV3Iteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL temp;
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux->i >= fractal->transformCommon.startIterationsP
			&& aux->i < fractal->transformCommon.stopIterationsP1)
	{
		if (fractal->transformCommon.functionEnabledxFalse)
		{
			z.x = sign(z.x) * (fractal->transformCommon.offset000.x - fabs(z.x));
		}
		if (fractal->transformCommon.functionEnabledyFalse)
		{
			z.y = sign(z.y) * (fractal->transformCommon.offset000.y - fabs(z.y));
		}
		if (fractal->transformCommon.functionEnabledzFalse)
		{
			z.z = sign(z.z) * (fractal->transformCommon.offset000.z - fabs(z.z));
		}

		REAL r1;
		REAL phi;
		if (!fractal->transformCommon.functionEnabledSwFalse)
			phi = atan2(z.y, z.x);
		else
			phi = atan2(z.x, z.y);

		REAL t1 = fabs(native_cos(fractal->transformCommon.constantMultiplierA111.x * phi)
									 * fractal->transformCommon.constantMultiplierA111.y);
		if (fractal->transformCommon.functionEnabledCxFalse)
			t1 = pow(t1, fractal->transformCommon.constantMultiplierB111.x);

		REAL t2 = fabs(native_sin(fractal->transformCommon.constantMultiplierA111.x * phi)
									 * fractal->transformCommon.constantMultiplierA111.z);
		if (fractal->transformCommon.functionEnabledCyFalse)
			t2 = pow(t2, fractal->transformCommon.constantMultiplierB111.y);

		if (!fractal->transformCommon.functionEnabledOFalse)
			r1 = t1 + t2;
		else
			r1 = pow(t1 + t2, fractal->transformCommon.constantMultiplierB111.z);

		if (fractal->transformCommon.functionEnabledRFalse) r1 = 1.0f / r1;

		r1 = r1 * fractal->transformCommon.minR0;

		aux->r = aux->r * fractal->transformCommon.radius1 + r1;
		aux->DE = aux->DE * fractal->transformCommon.radius1 + r1;
	}

	// cartesian to polar
	REAL th = z.z / aux->r;
	if (!fractal->transformCommon.functionEnabledBFalse)
	{
		if (!fractal->transformCommon.functionEnabledAFalse)
			th = asin(th);
		else
			th = acos(th);
	}
	else
	{
		temp = acos(th);
		th = temp + (asin(th) - temp) * fractal->transformCommon.scale1;
	}
	REAL ph;
	if (!fractal->transformCommon.functionEnabledXFalse)
		ph = atan2(z.y, z.x);
	else
		ph = atan2(z.x, z.y);

	th = (th + fractal->bulb.betaAngleOffset) * fractal->bulb.power;
	ph = (ph + fractal->bulb.alphaAngleOffset) * fractal->bulb.power;
	REAL rp = pow(aux->r, fractal->bulb.power - 1.0f);
	aux->DE = rp * aux->DE * fractal->bulb.power + 1.0f;
	rp *= aux->r;

	// polar to cartesian
	REAL cth = native_cos(th);
	REAL sth = native_sin(th);

	REAL4 trg;
	if (!fractal->transformCommon.functionEnabledFFalse)
	{
		trg = (REAL4){cth * native_cos(ph), cth * native_sin(ph), sth, 0.0f};
	}
	else
	{
		trg = (REAL4){sth * native_sin(ph), sth * native_cos(ph), cth, 0.0f};
	}

	if (fractal->transformCommon.functionEnabledAx
			&& aux->i >= fractal->transformCommon.startIterationsT
			&& aux->i < fractal->transformCommon.stopIterationsT)
	{
		if (!fractal->transformCommon.functionEnabledDFalse)
		{
			z = trg;
		}
		else
		{
			temp = trg.z;
			trg.z = trg.y;
			trg.y = temp;
			z = trg;
		}
		z *= rp;
	}

	if (fractal->transformCommon.functionEnabledGFalse
			&& aux->i >= fractal->transformCommon.startIterationsG
			&& aux->i < fractal->transformCommon.stopIterationsG)
	{
		z += (trg * rp - z) * fractal->transformCommon.scale3D111;
	}

	if (fractal->transformCommon.functionEnabledJFalse
			&& aux->i >= fractal->transformCommon.startIterationsJ
			&& aux->i < fractal->transformCommon.stopIterationsJ)
	{
		z.x = native_cos(ph);
		z.y = native_sin(ph);
		z.z = sth;

		if (fractal->transformCommon.functionEnabledBxFalse)
			z.x *= 1.0f + (cth - 1.0f) * fractal->transformCommon.vec111.x;
		if (fractal->transformCommon.functionEnabledByFalse)
			z.y *= 1.0f + (cth - 1.0f) * fractal->transformCommon.vec111.y;
		if (fractal->transformCommon.functionEnabledBzFalse)
			z.z *= 1.0f + (cth - 1.0f) * fractal->transformCommon.vec111.z;

		z *= rp;
	}

	z += fractal->transformCommon.offsetA000;
	z += aux->const_c * fractal->transformCommon.constantMultiplier111;
	z.z *= fractal->transformCommon.scaleA1;

	if (fractal->analyticDE.enabledFalse)
	{
		aux->DE = aux->DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		aux->DE0 = length(z);
		if (aux->DE0 > 1.0f)
			aux->DE0 = 0.5f * log(aux->DE0) * aux->DE0 / (aux->DE);
		else
			aux->DE0 = 0.0f; // 0.01f artifacts in openCL

		if (aux->i >= fractal->transformCommon.startIterationsO
				&& aux->i < fractal->transformCommon.stopIterationsO)
			aux->dist = min(aux->dist, aux->DE0);
		else
			aux->dist = aux->DE0;
	}
	return z;
}