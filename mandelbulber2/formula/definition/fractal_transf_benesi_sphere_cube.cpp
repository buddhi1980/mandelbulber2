/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Benesi Sphere to Cube transform
 * Warps a sphere to a cube; transform made by M.Benesi, optimized by
 * Luca.  Scavenged and edited from code optimized by Luca.
 * @reference http://www.fractalforums.com/mathematics/circle2square/
 */

#include "all_fractal_definitions.h"

cFractalTransfBenesiSphereCube::cFractalTransfBenesiSphereCube() : cAbstractFractal()
{
	nameInComboBox = "T>Benesi Sphere>Cube";
	internalName = "transf_benesi_sphere_cube";
	internalID = fractal::transfBenesiSphereCube;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBenesiSphereCube::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	CVector4 oldZ = z;
	z *= z;
	// if (z.z == 0.0) z.z = 1e-21;
	double rCyz = z.y / z.z;
	if (rCyz < 1.0)
		rCyz = 1.0 / sqrt(rCyz + 1.0);
	else
		rCyz = 1.0 / sqrt(1.0 / rCyz + 1.0);

	z.y *= rCyz;
	z.z *= rCyz;

	// if (z.x == 0.0) z.x = 1e-21;
	double rCxyz = (z.y * z.y + z.z * z.z) / z.x;

	if (rCxyz < 1.0)
		rCxyz = 1.0 / sqrt(rCxyz + 1.0);
	else
		rCxyz = 1.0 / sqrt(1.0 / rCxyz + 1.0);

	z *= rCxyz * SQRT_3_2;
	// aux.DE *= z.Length() / oldZ.Length();
	if (fractal->analyticDE.enabled)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 * z.Length() / oldZ.Length()
						 + fractal->analyticDE.offset1;
	}
}
