/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Fractal formula created by Robert Pancoast

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_coastalbrot.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 CoastalbrotIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	Q_UNUSED(fractal);

	REAL temp = aux->r;
	temp = pow(temp, 7.7f);
	aux->DE = temp * aux->DE * 7.7f;
	temp *= aux->r;

	z.x = native_sin(native_sin(native_sin(M_PI_F / 3 + z.x * M_PI_F)));
	z.y = native_sin(native_sin(native_sin(M_PI_F / 3 + z.y * M_PI_F)));
	z.z = native_sin(native_sin(native_sin(M_PI_F / 3 + z.z * M_PI_F)));

	z *= temp;
	return z;
}