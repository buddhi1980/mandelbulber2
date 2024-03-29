/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Fractal formula created by Buddhi

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_mandelbulb4.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 Mandelbulb4Iteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL rp = pow(aux->r, fractal->bulb.power - 1.0f);
	aux->DE = rp * aux->DE * fractal->bulb.power + 1.0f;

	REAL angZ = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	REAL angY = atan2(z.z, z.x) + fractal->bulb.betaAngleOffset;
	REAL angX = atan2(z.z, z.y) + fractal->bulb.gammaAngleOffset;

	matrix33 rotM;
	rotM.m1 = (float3){1.0f, 0.0f, 0.0f};
	rotM.m2 = (float3){0.0f, 1.0f, 0.0f};
	rotM.m3 = (float3){0.0f, 0.0f, 1.0f};
	rotM = RotateX(rotM, angX * (fractal->bulb.power - 1.0f));
	rotM = RotateY(rotM, angY * (fractal->bulb.power - 1.0f));
	rotM = RotateZ(rotM, angZ * (fractal->bulb.power - 1.0f));

	z = Matrix33MulFloat4(rotM, z) * rp;
	return z;
}