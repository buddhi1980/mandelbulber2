/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * RiemannSphereMsltoe
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/
 * riemann-fractals/msg33500/#msg33500

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_riemann_sphere_msltoe.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 RiemannSphereMsltoeIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	if (fractal->transformCommon.rotationEnabled)
		z = Matrix33MulFloat4(fractal->transformCommon.rotationMatrix, z);

	REAL r = aux->r;
	// if (r < 1e-21f) r = 1e-21f;
	z *= fractal->transformCommon.scale / r;

	REAL q = 1.0f / (1.0f - z.z);
	REAL s = z.x * q;
	REAL t = z.y * q;

	REAL p = 1.0f + s * s + t * t;

	s = fabs(native_sin(M_PI_F * s + fractal->transformCommon.offsetA0));
	t = fabs(native_sin(M_PI_F * t + fractal->transformCommon.offsetB0));

	r *= r;
	// if (r < 1e-21f)
	//	r = 1e-21f;

	if (p > 36) p = 36; // problem with pow()
	r = -0.25f + pow(r, p);

	z.x = 2.0f * s;
	z.y = 2.0f * t;
	z.z = -1.0f + s * s + t * t;
	z *= r / (1.0f + s * s + t * t);

	z += fractal->transformCommon.additionConstant000;
	return z;
}