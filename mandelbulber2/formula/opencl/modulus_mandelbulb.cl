/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Classic Mandelbulb fractal with a Modulus Operation applied by mancoast
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/escape-time-algorithm-defined-using-the-quotient-and-remainder-functions/

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_modulus_mandelbulb.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 ModulusMandelbulbIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	// if (aux->r < 1e-21f) aux->r = 1e-21f;
	REAL th0 = asin(z.z / aux->r) + fractal->bulb.betaAngleOffset;
	REAL ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	REAL rp = pow(aux->r, fractal->bulb.power - 1.0f);
	REAL th = th0 * fractal->bulb.power;
	REAL ph = ph0 * fractal->bulb.power;
	REAL cth = native_cos(th);
	aux->DE = (rp * aux->DE) * fractal->bulb.power + 1.0f;
	rp *= aux->r;
	z.x = cth * native_cos(ph) * rp;
	z.y = fmod(cth * native_sin(ph) * rp, 2.0f);
	z.z = native_sin(th) * rp;
	return z;
}