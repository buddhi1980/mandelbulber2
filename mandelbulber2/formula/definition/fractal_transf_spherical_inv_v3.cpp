/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical invert v3
 * from M3D. Formula by Luca GN 2011, updated May 2012.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalInvV3::cFractalTransfSphericalInvV3() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Invert V3";
	internalName = "transf_spherical_inv_v3";
	internalID = fractal::transfSphericalInvV3;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalInvV3::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		z += fractal->transformCommon.offset000 + fractal->transformCommon.offsetA000;
		double rr = z.Dot(z);
		double r = sqrt(rr);
		double inv = 1.0 / rr;
		z *= fractal->transformCommon.maxR2d1 * inv;
		z -= fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;

		if (!fractal->transformCommon.functionEnabledNFalse)
			aux.DE *= inv * (fractal->transformCommon.maxR2d1 + r * aux.DE);
		else
			aux.DE *= inv * (fractal->transformCommon.maxR2d1 + r * aux.DE * fractal->transformCommon.scale0);

		z *= fractal->transformCommon.scaleA1;
		aux.DE *= fractal->transformCommon.scaleA1;
	}

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}
