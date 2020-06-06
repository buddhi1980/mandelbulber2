/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical invert ( v2.17)
 * from M3D. Formula by Luca GN 2011, updated May 2012.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalFoldV2::cFractalTransfSphericalFoldV2() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Fold V2";
	internalName = "transf_spherical_fold_v2";
	internalID = fractal::transfSphericalFoldV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalFoldV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.offset000;
	double m = 1.0;
	double rr = z.Dot(z);
	if (rr < fractal->transformCommon.invert0) m = fractal->transformCommon.inv0;
	else if (rr < fractal->transformCommon.invert1) m = 1.0 / rr;
	else m = fractal->transformCommon.inv1;
	z *= m;
	aux.DE *= m;
	z += fractal->transformCommon.additionConstant000
			- fractal->transformCommon.offset000;

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}
