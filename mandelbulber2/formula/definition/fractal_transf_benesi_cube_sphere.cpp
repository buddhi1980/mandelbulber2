/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Benesi Cube to sphere transform
 * Warps a cube to a sphere; transform made by M.Benesi, optimized by Luca.
 * @reference http://www.fractalforums.com/mathematics/circle2square/
 */

#include "all_fractal_definitions.h"

cFractalTransfBenesiCubeSphere::cFractalTransfBenesiCubeSphere() : cAbstractFractal()
{
	nameInComboBox = "T>Benesi Cube>Sphere";
	internalName = "transf_benesi_cube_sphere";
	internalID = fractal::transfBenesiCubeSphere;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBenesiCubeSphere::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(fractal);
	// Q_UNUSED(aux);
	CVector4 oldZ = z;
	z *= z; // so all now positive

	// if (z.x == 0.0)
	//	z.x = 1e-21;
	// if (z.z == 0.0)
	//	z.z = 1e-21;

	double rCyz = z.y / z.z;

	double rCxyz = (z.y + z.z) / z.x;

	if (rCxyz == -1.0) z.z = 1.0; //+ 1e-21
	if (rCyz < 1.0)
		rCyz = sqrt(rCyz + 1.0);
	else
		rCyz = sqrt(1.0 / rCyz + 1.0);

	if (rCxyz < 1.0)
		rCxyz = sqrt(rCxyz + 1.0);
	else
		rCxyz = sqrt(1.0 / rCxyz + 1.0);

	z.y *= rCyz;
	z.z *= rCyz;

	z *= rCxyz / SQRT_3_2;
	// aux.DE *= z.Length() / oldZ.Length();
	if (fractal->analyticDE.enabled)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 * z.Length() / oldZ.Length()
						 + fractal->analyticDE.offset1;
	}
}
