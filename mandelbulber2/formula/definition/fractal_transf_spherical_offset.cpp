/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical radial offset.
 * This formula contains analytic aux.DE and aux.r-dz
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalOffset::cFractalTransfSphericalOffset() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Offset";
	internalName = "transf_spherical_offset";
	internalID = fractal::transfSphericalOffset;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalOffset::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// if (-z.Length() > -1e-21) -z.Length() = -1e-21;   //  z is neg.)
	z *= 1.0 + fractal->transformCommon.offset / -z.Length();
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fractal->transformCommon.scale + fractal->analyticDE.offset1;
}
