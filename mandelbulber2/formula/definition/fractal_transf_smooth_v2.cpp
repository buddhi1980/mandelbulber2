/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * smoothing based on Menger Smooth
 * http://www.fractalforums.com/fragmentarium/help-t22583/
 */

#include "all_fractal_definitions.h"

cFractalTransfSmoothV2::cFractalTransfSmoothV2() : cAbstractFractal()
{
	nameInComboBox = "T>SmoothV2";
	internalName = "transf_smooth_v2";
	internalID = fractal::transfSmoothV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSmoothV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double OffsetS = fractal->transformCommon.offset0005;

	CVector4 ZZ = z * z;
	CVector4 sc = fabs(z) * fractal->transformCommon.offset000;
	CVector4 tc = fabs(z) * fractal->transformCommon.offsetF000;

	// the closer to origin the greater the effect of smooth
	z = CVector4(sqrt(ZZ.x + OffsetS + sc.x), sqrt(ZZ.y + OffsetS) + sc.y, sqrt(ZZ.z + OffsetS + sc.z), z.w);

	double t;
	if (fractal->transformCommon.functionEnabledx)
	{
		t = z.x - z.y;
		t = 0.5 * (t - sqrt(t * t + OffsetS + tc.x));
		z.x = z.x - t;
		z.y = z.y + t;
	}
	if (fractal->transformCommon.functionEnabledy)
	{
		t = z.y - z.z;
		t = 0.5 * (t - sqrt(t * t + OffsetS + tc.y));
		z.y = z.y - t;
		z.z = z.z + t;
	}
	if (fractal->transformCommon.functionEnabledz)
	{
		t = z.z - z.x;
		t = 0.5 * (t - sqrt(t * t + OffsetS + tc.z));
		z.z = z.z - t;
		z.x = z.x + t;
	}

	z += fractal->transformCommon.offsetA000;
}
