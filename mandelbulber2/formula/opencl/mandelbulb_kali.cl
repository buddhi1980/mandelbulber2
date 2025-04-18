/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2024 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * mandelbulb Kali modification
 * @reference http://www.fractalforums.com/theory/mandelbulb-variant/

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_mandelbulb_kali.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 MandelbulbKaliIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	REAL th0 = acos(z.z / aux->r) + fractal->bulb.betaAngleOffset; // MUST keep exception catch
	REAL ph0 = atan(z.y / z.x) + fractal->bulb.alphaAngleOffset;
	th0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleA1;
	REAL sinth = native_sin(th0);
	z = aux->r * (REAL4){sinth * native_cos(ph0), native_sin(ph0) * sinth, native_cos(th0), 0.0f};

	if (z.x * z.x + z.y * z.y == 0.0f)
	{
		z.y = z.z * z.z;
		z.z = 0.0f;
	}

	th0 =
		acos(z.z / aux->r) + fractal->transformCommon.betaAngleOffset; // MUST keep exception catch ??;
	ph0 = atan(z.y / z.x);
	ph0 *= fractal->transformCommon.pwr8 * fractal->transformCommon.scaleB1;
	REAL zp = pow(aux->r, fractal->transformCommon.pwr8);
	sinth = native_sin(th0);
	z = zp * (REAL4){sinth * native_cos(ph0), native_sin(ph0) * sinth, native_cos(th0), 0.0f};

	if (fractal->analyticDE.enabledFalse)
	{ // analytic log DE adjustment
		aux->DE = pow(aux->r, fractal->transformCommon.pwr8 - fractal->analyticDE.offset1) * aux->DE
								* fractal->transformCommon.pwr8 * fractal->analyticDE.scale1
							+ fractal->analyticDE.offset2;
	}
	else // default, i.e. scale1 & offset1 & offset2 = 1.0f
	{
		aux->DE =
			pow(aux->r, fractal->transformCommon.pwr8 - 1.0f) * fractal->transformCommon.pwr8 * aux->DE
			+ 1.0f;
	}
	return z;
}