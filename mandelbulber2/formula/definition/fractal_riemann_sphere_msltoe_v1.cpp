/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * RiemannSphereMsltoe Version1
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * revisiting-the-riemann-sphere-%28again%29/
 */

#include "all_fractal_definitions.h"

cFractalRiemannSphereMsltoeV1::cFractalRiemannSphereMsltoeV1() : cAbstractFractal()
{
	nameInComboBox = "Riemann - Sphere Msltoe V1";
	internalName = "riemann_sphere_msltoe_v1";
	internalID = fractal::riemannSphereMsltoeV1;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalRiemannSphereMsltoeV1::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double r = aux.r;
	// if (r < 1e-21) r = 1e-21;
	z *= fractal->transformCommon.scale / r;
	double q = 1.0 / (1.0 - z.z);
	CVector4 t3;
	t3.x = z.x * q;
	t3.y = z.y * q;

	t3.z = (r - 1.5) * (1.0 + t3.x * t3.x + t3.y * t3.y);

	if (fractal->transformCommon.rotationEnabled)
		t3 = fractal->transformCommon.rotationMatrix.RotateVector(t3);

	t3.x = t3.x - floor(t3.x + 0.5);
	t3.y = t3.y - floor(t3.y + 0.5);
	t3.w = z.w;

	z = t3 * fractal->transformCommon.constantMultiplier441;

	z += fractal->transformCommon.additionConstant000;

	if (fractal->analyticDE.enabled)
	{
		aux.DE *= 4.0 * fractal->transformCommon.scale * z.Length() / r;
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
}
