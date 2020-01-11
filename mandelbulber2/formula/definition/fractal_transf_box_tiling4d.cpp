/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Box Tiling 4d
 */

#include "all_fractal_definitions.h"

cFractalTransfBoxTiling4d::cFractalTransfBoxTiling4d() : cAbstractFractal()
{
	nameInComboBox = "T>Box Tiling 4D";
	internalName = "transf_box_tiling4d";
	internalID = fractal::transfBoxTiling4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBoxTiling4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 size = fractal->transformCommon.offset2222;
	CVector4 oldZ = z;

	if (!fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledx && size.x != 0.0)
		{
			z.x -= round(z.x / size.x) * size.x;
		}
		if (fractal->transformCommon.functionEnabledyFalse && size.y != 0.0)
		{
			z.y -= round(z.y / size.y) * size.y;
		}
		if (fractal->transformCommon.functionEnabledzFalse && size.z != 0.0)
		{
			z.z -= round(z.z / size.z) * size.z;
		}
		if (fractal->transformCommon.functionEnabledwFalse && size.w != 0.0)
		{
			z.w -= round(z.w / size.w) * size.w;
		}
	}
	else
	{
		CVector4 repeatPos = fractal->transformCommon.offsetA1111;
		CVector4 repeatNeg = fractal->transformCommon.offsetB1111;

		if (fractal->transformCommon.functionEnabledx && z.x < (repeatPos.x + 0.5) * size.x
				&& z.x > (repeatNeg.x + 0.5) * -size.x && size.x != 0.0)
		{
			z.x -= round(z.x / size.x) * size.x;
		}
		if (fractal->transformCommon.functionEnabledyFalse && z.y < (repeatPos.y + 0.5) * size.y
				&& z.y > (repeatNeg.y + 0.5) * -size.y && size.y != 0.0)
		{
			z.y -= round(z.y / size.y) * size.y;
		}
		if (fractal->transformCommon.functionEnabledzFalse && z.z < (repeatPos.z + 0.5) * size.z
				&& z.z > (repeatNeg.z + 0.5) * -size.z && size.z != 0.0)
		{
			z.z -= round(z.z / size.z) * size.z;
		}
		if (fractal->transformCommon.functionEnabledwFalse && z.w < (repeatPos.w + 0.5) * size.w
				&& z.w > (repeatNeg.w + 0.5) * -size.w && size.w != 0.0)
		{
			z.w -= round(z.w / size.w) * size.w;
		}
	}

	if (fractal->transformCommon.functionEnabledBxFalse)
	{
		z.x = z.x * fractal->transformCommon.scale1 / (fabs(oldZ.x) + 1.0);
		z.y = z.y * fractal->transformCommon.scale1 / (fabs(oldZ.y) + 1.0);
		z.z = z.z * fractal->transformCommon.scale1 / (fabs(oldZ.z) + 1.0);
		z.w = z.w * fractal->transformCommon.scale1 / (fabs(oldZ.w) + 1.0);
	}

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			aux.DE = aux.DE * z.Length() * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}
