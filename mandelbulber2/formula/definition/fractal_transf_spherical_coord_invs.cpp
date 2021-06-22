/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Inverse spherical coords. Fast conversion without parameters.
 * Formula by Luca GN 2011, from M3D, from Wikipedia;
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalCoordInvs::cFractalTransfSphericalCoordInvs() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Coord Invs";
	internalName = "transf_spherical_coord_invs";
	internalID = fractal::transfSphericalCoordInvs;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalCoordInvs::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	CVector4 t = z;

	if (!fractal->transformCommon.functionEnabledFalse)
	{
		t.x = z.z * sin(z.x) * sin(z.y);
		t.y = z.z * cos(z.x);
		t.z = z.z * sin(z.x) * cos(z.y);
	}
	else
	{
		t.y = z.y * sin(z.x) * cos(z.z);
		t.x = z.y * sin(z.x) * sin(z.z);
		t.z = z.y * cos(z.x);
	}

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		t.x = z.x * sin(z.y) * cos(z.z);
		t.y = z.x * sin(z.y) * sin(z.z);
		t.z = z.x * cos(z.y);
	}

	z = t + fractal->transformCommon.offset000;

}
