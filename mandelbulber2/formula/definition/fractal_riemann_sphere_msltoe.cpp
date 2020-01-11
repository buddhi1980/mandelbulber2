/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * RiemannSphereMsltoe
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/
 * riemann-fractals/msg33500/#msg33500
 */

#include "all_fractal_definitions.h"

cFractalRiemannSphereMsltoe::cFractalRiemannSphereMsltoe() : cAbstractFractal()
{
	nameInComboBox = "Riemann - Sphere Msltoe";
	internalName = "riemann_sphere_msltoe";
	internalID = fractal::riemannSphereMsltoe;
	DEType = deltaDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 1.5;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalRiemannSphereMsltoe::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	double r = aux.r;
	// if (r < 1e-21) r = 1e-21;
	z *= fractal->transformCommon.scale / r;

	double q = 1.0 / (1.0 - z.z);
	double s = z.x * q;
	double t = z.y * q;

	double p = 1.0 + s * s + t * t;

	s = fabs(sin(M_PI * s + fractal->transformCommon.offsetA0));
	t = fabs(sin(M_PI * t + fractal->transformCommon.offsetB0));

	r *= r;
	// if (r < 1e-21)
	//	r = 1e-21;

	if (p > 36) p = 36; // problem with pow()
	r = -0.25 + pow(r, p);

	z.x = 2.0 * s;
	z.y = 2.0 * t;
	z.z = -1.0 + s * s + t * t;
	z *= r / (1.0 + s * s + t * t);

	z += fractal->transformCommon.additionConstant000;
}
