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

cFractalTransfBoxTilingV2::cFractalTransfBoxTilingV2() : cAbstractFractal()
{
	nameInComboBox = "T>Box Tiling V2";
	internalName = "transf_box_tiling_v2";
	internalID = fractal::transfBoxTilingV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBoxTilingV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 size = fractal->transformCommon.offset222;
	CVector4 t = fractal->transformCommon.offset111;

	if (!fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledx && size.x != 0.0)
		{
			z.x -= round(z.x / size.x) * size.x;
			z.x = clamp(fabs(z.x), -t.x, t.x);
		}
		if (fractal->transformCommon.functionEnabledyFalse && size.y != 0.0)
		{
			z.y -= round(z.y / size.y) * size.y;
			z.y = clamp(fabs(z.y), -t.y, t.y);
		}
		if (fractal->transformCommon.functionEnabledzFalse && size.z != 0.0)
		{
			z.z -= round(z.z / size.z) * size.z;
			z.z = clamp(fabs(z.z), -t.z, t.z);
		}
	}
	else
	{
		CVector4 repeatPos = fractal->transformCommon.offsetA111;
		CVector4 repeatNeg = fractal->transformCommon.offsetB111;

		if (fractal->transformCommon.functionEnabledx && z.x < (repeatPos.x + 0.5) * size.x
				&& z.x > (repeatNeg.x + 0.5) * -size.x && size.x != 0.0)
		{
			z.x -= round(z.x / size.x) * size.x;
			z.x = clamp(fabs(z.x), -t.x, t.x);
		}
		if (fractal->transformCommon.functionEnabledyFalse && z.y < (repeatPos.y + 0.5) * size.y
				&& z.y > (repeatNeg.y + 0.5) * -size.y && size.y != 0.0)
		{
			z.y -= round(z.y / size.y) * size.y;
			z.y = clamp(fabs(z.y), -t.y, t.y);
		}
		if (fractal->transformCommon.functionEnabledzFalse && z.z < (repeatPos.z + 0.5) * size.z
				&& z.z > (repeatNeg.z + 0.5) * -size.z && size.z != 0.0)
		{
			z.z -= round(z.z / size.z) * size.z;
			z.z = clamp(fabs(z.z), -t.z, t.z);
		}
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
