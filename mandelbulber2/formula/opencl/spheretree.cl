/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2023 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * formula by TGlad, extras by sabine62
 * https://fractalforums.org/fractal-mathematics-and-new-theories/28/new-sphere-tree/3557/msg22100#msg22100

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_spheretree.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 SpheretreeIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL Dd;
	if (!fractal->transformCommon.functionEnabledByFalse)
		Dd = 1.0f;
	else
		Dd = aux->DE;
	REAL4 ColV = (REAL4){0.0f, 0.0f, 0.0f, 0.0f};
	REAL4 oldZ = z;
	REAL t = fractal->transformCommon.minR06;
	REAL4 t1 = (REAL4){SQRT_3_4_F, -0.5f, 0.0f, 0.0f};
	REAL4 t2 = (REAL4){-SQRT_3_4_F, -0.5f, 0.0f, 0.0f};
	REAL4 n1 = (REAL4){-0.5f, -SQRT_3_4_F, 0.0f, 0.0f};
	REAL4 n2 = (REAL4){-0.5f, SQRT_3_4_F, 0.0f, 0.0f};
	REAL innerScale = SQRT_3_F / (1.0f + SQRT_3_F);
	REAL innerScaleB = innerScale * innerScale * 0.25f;

	// for (int i = 0; i < fractal->transformCommon.int8X && dot(z, z) < 0.5f; i++)
	for (int n = 0; n < fractal->transformCommon.int8X; n++)
	{
		if (!fractal->transformCommon.functionEnabledBxFalse)
		{
			REAL4 zB = z - (REAL4){0.0f, 0.0f, innerScale * 0.5f, 0.0f};
			if (dot(zB, zB) < innerScaleB) break; // definitely inside
		}

		REAL maxH = 0.4f * fractal->transformCommon.scaleG1;
		if (n == 0) maxH = -100.0f;

		REAL4 zC = z - (REAL4){0.0f, 0.0f, t, 0.0f};
		if (z.z > maxH && dot(zC, zC) > t * t) break; // definitely outside

		REAL4 zD = z - (REAL4){0.0f, 0.0f, 0.5f, 0.0f};
		REAL invSC = fractal->transformCommon.scaleF1 / dot(z, z);

		if (z.z < maxH && dot(zD, zD) > 0.25f)
		{
			// needs a sphere inverse
			Dd *= invSC;
			z *= invSC;
			ColV.x += 1.0f;
		}
		else
		{
			// stretch onto a plane at zero
			ColV.y += 1.0f;
			Dd *= invSC;
			z *= invSC;
			z.z -= 1.0f;
			z.z *= -1.0f;
			z *= SQRT_3_F;
			Dd *= SQRT_3_F;
			z.z += 1.0f;

			// and rotate it a twelfth of a revolution pi/6
			REAL a = M_PI_F / fractal->transformCommon.scale6;
			REAL cosA = native_cos(a);
			REAL sinA = native_sin(a);
			REAL xx = z.x * cosA + z.y * sinA;
			REAL yy = -z.x * sinA + z.y * cosA;
			z.x = xx;
			z.y = yy;
		}

		// now modolu the space so we move to being in just the central hexagon, inner radius 0.5f
		REAL h = z.z * fractal->transformCommon.scaleE1;
		REAL x = dot(z, -1.0f * n2) * fractal->transformCommon.scaleA2 / SQRT_3_F;
		REAL y = dot(z, -1.0f * n1) * fractal->transformCommon.scaleA2 / SQRT_3_F;
		x = x - floor(x);
		y = y - floor(y);

		if (x + y > 1.0f)
		{
			x = 1.0f - x;
			y = 1.0f - y;
			ColV.z += 1.0f;
		}

		z = x * t1 - y * t2;
		// fold the space to be in a kite
		REAL l0 = dot(z, z);
		REAL4 zt1 = z - t1;
		REAL4 zt2 = z + t2;
		REAL l1 = dot(zt1, zt1);
		REAL l2 = dot(zt2, zt2);

		if (l1 < l0 && l1 < l2)
		{
			z -= t1 * (2.0f * dot(z, t1) - 1.0f);
		}
		else if (l2 < l0 && l2 < l1)
		{
			z -= t2 * (2.0f * dot(z, t2) + 1.0f);
		}
		z.z = h;
		z *= fractal->transformCommon.scaleD1;
		Dd *= fractal->transformCommon.scaleD1;
		z += fractal->transformCommon.offset000;
	}
	// aux->DE
	aux->DE = Dd;
	REAL4 len = z - (REAL4){0.0f, 0.0f, 0.5f, 0.0f};
	REAL d = (length(len) - 0.5f);
	// d = (native_sqrt(d + 1.0f) - 1.0f) * 2.0f;
	ColV.w = d;
	d /= fractal->analyticDE.scale1 * 2.22f * aux->DE;

	if (!fractal->transformCommon.functionEnabledXFalse)
		aux->dist = min(aux->dist, d);
	else
		aux->dist = d;

	if (fractal->analyticDE.enabledFalse) z = oldZ;

	// aux->color
	if (fractal->foldColor.auxColorEnabled)
	{
		aux->color += ColV.x * fractal->foldColor.difs0000.x + ColV.y * fractal->foldColor.difs0000.y
									+ ColV.z * fractal->foldColor.difs0000.z + ColV.w * fractal->foldColor.difs0000.w;
	}
	return z;
}