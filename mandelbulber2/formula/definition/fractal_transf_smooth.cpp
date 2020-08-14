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

cFractalTransfSmooth::cFractalTransfSmooth() : cAbstractFractal()
{
	nameInComboBox = "T>Smooth";
	internalName = "transf_smooth";
	internalID = fractal::transfSmooth;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSmooth::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double OffsetS = fractal->transformCommon.offset0005;
	CVector4 Zigns = CVector4(1.0, 1.0, 1.0, 1.0);
	Zigns.x = sign(z.x);
	Zigns.y = sign(z.y);
	Zigns.z = sign(z.z);

	// the closer to origin the greater the effect of smooth
	z =
		CVector4(sqrt(z.x * z.x + OffsetS), sqrt(z.y * z.y + OffsetS), sqrt(z.z * z.z + OffsetS), z.w);

	if (fractal->transformCommon.functionEnabledzFalse) z *= Zigns;

	double t;
	if (fractal->transformCommon.functionEnabledx)
	{
		t = z.x - z.y;
		t = 0.5 * (t - sqrt(t * t + OffsetS));
		z.x = z.x - t;
		z.y = z.y + t;
	}
	if (fractal->transformCommon.functionEnabledy)
	{
		t = z.x - z.z;
		t = 0.5 * (t - sqrt(t * t + OffsetS));
		z.x = z.x - t;
		z.z = z.z + t;
	}
	if (fractal->transformCommon.functionEnabledz)
	{
		t = z.y - z.z;
		t = 0.5 * (t - sqrt(t * t + OffsetS));
		z.y = z.y - t;
		z.z = z.z + t;
	}

	z += fractal->transformCommon.offsetA000;
}
