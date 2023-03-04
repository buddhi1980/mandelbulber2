/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Box Tiling v3
 */

#include "all_fractal_definitions.h"

cFractalTransfBoxTilingV3::cFractalTransfBoxTilingV3() : cAbstractFractal()
{
	nameInComboBox = "T>Box Tiling V3";
	internalName = "transf_box_tiling_v3";
	internalID = fractal::transfBoxTilingV3;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBoxTilingV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 size = fractal->transformCommon.offset222;
	z -= fractal->transformCommon.additionConstantA000;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (!fractal->transformCommon.functionEnabledAxFalse)
		{
			if (fractal->transformCommon.functionEnabledCxFalse) z.x = fabs(z.x);
			z.x -= fractal->transformCommon.offset000.x;
			z.x -= round(z.x / size.x) * size.x;
		}
		else
		{
			if (z.x < (fractal->transformCommon.offsetA111.x + 0.5) * size.x
					&& z.x > (fractal->transformCommon.offsetB111.x + 0.5) * -size.x)
			{

				if (fractal->transformCommon.functionEnabledCxFalse) z.x = fabs(z.x);
				z.x -= fractal->transformCommon.offset000.x;
				z.x -= round(z.x / size.x) * size.x;
			}
		}
		if (fractal->transformCommon.functionEnabledBxFalse) z.x = -z.x;

		if (fractal->surfBox.enabledX2False)
		{
			if (!fractal->surfBox.enabledX4False)
			{
				z.x = min(aux.const_c.x + fractal->transformCommon.offsetA000.x, -z.x);
				z.x = max(aux.const_c.x - fractal->transformCommon.offsetF000.x, z.x);
			}
			else
			{
				z.x = max(aux.const_c.x - fractal->transformCommon.offsetF000.x, z.x);
				z.x = min(aux.const_c.x + fractal->transformCommon.offsetA000.x, -z.x);
			}
		}
	}

	if (fractal->transformCommon.functionEnabledyFalse)
	{
		if (!fractal->transformCommon.functionEnabledAyFalse)
		{
			if (fractal->transformCommon.functionEnabledCyFalse) z.y = fabs(z.y);
			z.y -= fractal->transformCommon.offset000.y;
			z.y -= round(z.y / size.y) * size.y;
		}
		else
		{
			if (z.y < (fractal->transformCommon.offsetA111.y + 0.5) * size.y
					&& z.y > (fractal->transformCommon.offsetB111.y + 0.5) * -size.y)
			{
				if (fractal->transformCommon.functionEnabledCyFalse) z.y = fabs(z.y);
				z.y -= fractal->transformCommon.offset000.y;
				z.y -= round(z.y / size.y) * size.y;
			}
		}
		if (fractal->transformCommon.functionEnabledByFalse) z.y = -z.y;

		if (fractal->surfBox.enabledY2False)
		{
			if (!fractal->surfBox.enabledY4False)
			{
				z.y = min(aux.const_c.y + fractal->transformCommon.offsetA000.y, -z.y);
				z.y = max(aux.const_c.y - fractal->transformCommon.offsetF000.y, z.y);
			}
			else
			{
				z.y = max(aux.const_c.y - fractal->transformCommon.offsetF000.y, z.y);
				z.y = min(aux.const_c.y + fractal->transformCommon.offsetA000.y, -z.y);
			}
		}
	}

	if (fractal->transformCommon.functionEnabledzFalse)
	{
		if (!fractal->transformCommon.functionEnabledAzFalse)
		{
			if (fractal->transformCommon.functionEnabledCzFalse) z.z = fabs(z.z);
			z.z -= fractal->transformCommon.offset000.z;
			z.z -= round(z.z / size.z) * size.z;
		}
		else
		{
			if (z.z < (fractal->transformCommon.offsetA111.z + 0.5) * size.z
					&& z.z > (fractal->transformCommon.offsetB111.z + 0.5) * -size.z)
			{
				if (fractal->transformCommon.functionEnabledCzFalse) z.z = fabs(z.z);
				z.z -= fractal->transformCommon.offset000.z;
				z.z -= round(z.z / size.z) * size.z;
			}
		}
		if (fractal->transformCommon.functionEnabledBzFalse) z.z = -z.z;

		if (fractal->surfBox.enabledZ2False)
		{
			if (!fractal->surfBox.enabledZ4False)
			{
				z.z = min(aux.const_c.z + fractal->transformCommon.offsetA000.z, -z.z);
				z.z = max(aux.const_c.z - fractal->transformCommon.offsetF000.z, z.z);
			}
			else
			{
				z.z = max(aux.const_c.z - fractal->transformCommon.offsetF000.z, z.z);
				z.z = min(aux.const_c.z + fractal->transformCommon.offsetA000.z, -z.z);
			}
		}
	}

	if (fractal->analyticDE.enabled)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
		aux.const_c = z;
}
