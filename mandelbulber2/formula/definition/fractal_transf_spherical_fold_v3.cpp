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

cFractalTransfSphericalFoldV3::cFractalTransfSphericalFoldV3() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Fold V3";
	internalName = "transf_spherical_fold_v3";
	internalID = fractal::transfSphericalFoldV3;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalFoldV3::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 signs = z;
	signs.x = sign(z.x);
	signs.y = sign(z.y);
	signs.z = sign(z.z);
	signs.w = sign(z.w);

	CVector4 out = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 in = out;
	if (aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		out = fractal->transformCommon.offset000;
		if (fractal->transformCommon.functionEnabledAzFalse)
			out *= signs;
	}
	if (aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
	{
		in = fractal->transformCommon.offsetF000;
		if (fractal->transformCommon.functionEnabledBzFalse)
			in *= signs;
	}
	if (fractal->transformCommon.functionEnabledAx) z -= out;


	double m = 0.0f;
	double rr = z.Dot(z);
	if (fractal->transformCommon.functionEnabledBxFalse) z -= in;


	if (rr < fractal->transformCommon.minR2p25)
	{
		m = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
		z *= m;
		aux.DE *= m;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		m = fractal->transformCommon.maxR2d1 / rr;
		z *= m;
		aux.DE *= m;
	}
	if (fractal->transformCommon.functionEnabledAy) z += out;
	if (fractal->transformCommon.functionEnabledByFalse) z += in;

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}
