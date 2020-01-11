/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * clamp 4d
 */

#include "all_fractal_definitions.h"

cFractalTransfClamp4d::cFractalTransfClamp4d() : cAbstractFractal()
{
	nameInComboBox = "T>Clamp 4D";
	internalName = "transf_clamp4d";
	internalID = fractal::transfClamp4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfClamp4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 upper = fractal->transformCommon.offset1111;
	CVector4 lower = fractal->transformCommon.offsetNeg1111;
	CVector4 scale = fractal->transformCommon.scale0000;
	CVector4 oldZ = z;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
		if (z.x < lower.x) z.x = lower.x;
		if (z.x > upper.x) z.x = upper.x;
		z.x = oldZ.x * scale.x - z.x;
	}
	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
		if (z.y < lower.y) z.y = lower.y;
		if (z.y > upper.y) z.y = upper.y;
		z.y = oldZ.y * scale.y - z.y;
	}
	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);
		if (z.z < lower.z) z.z = lower.z;
		if (z.z > upper.z) z.z = upper.z;
		z.z = oldZ.z * scale.z - z.z;
	}

	if (fractal->transformCommon.functionEnabledAwFalse)
	{
		if (fractal->transformCommon.functionEnabledwFalse) z.w = fabs(z.w);
		if (z.w < lower.w) z.w = lower.w;
		if (z.w > upper.w) z.w = upper.w;
		z.w = oldZ.w * scale.w - z.w;
	}

	aux.DE *= fractal->analyticDE.scale1;
}
