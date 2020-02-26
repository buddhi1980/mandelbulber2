/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Testing transform2
 *https://fractalforums.org/fractal-mathematics-and-new-theories/28/fake-3d-mandelbrot-set/1787/msg17940#msg17940
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalFoldV1::cFractalTransfSphericalFoldV1() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Fold V1";
	internalName = "transf_spherical_fold_v1";
	internalID = fractal::transfSphericalFoldV1;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalFoldV1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	CVector4 signs = z;
	signs.x = sign(z.x);
	signs.y = sign(z.y);
	signs.z = sign(z.z);
	signs.w = sign(z.w);

	z = fabs(z);
	CVector4 tt = fractal->mandelbox.offset;
	z -= tt;

	double trr = z.Dot(z);
	double tp = min(max(1.0 / trr, 1.0), 1.0 / fractal->transformCommon.minR2p25);

	z += tt;
	z *= tp;
	aux.DE *= tp;
	z *= signs;


	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
