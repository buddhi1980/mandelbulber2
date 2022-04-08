/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Low res mode transform from Mandelbulb3D.
 * @reference
 * https://fractalforums.org/code-snippets-fragments/74/low-resolution-effect/4687
 */

#include "all_fractal_definitions.h"

cFractalTransfLowResMode::cFractalTransfLowResMode() : cAbstractFractal()
{
	nameInComboBox = "T>Low Res Mode";
	internalName = "transf_low_res_mode";
	internalID = fractal::transfLowResMode;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfLowResMode::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	//if (fractal->transformCommon.scale8 > 0.0)
	{
		z *= fractal->transformCommon.scale8;
		CVector4 zi = z;
		if (fractal->transformCommon.functionEnabledx) zi.x = round(zi.x);
		if (fractal->transformCommon.functionEnabledy) zi.y = round(zi.y);
		if (fractal->transformCommon.functionEnabledz) zi.z = round(zi.z);
		if (fractal->transformCommon.functionEnabledwFalse) zi.w = round(z.w);
		CVector4 zf = z - zi;

		z = (fractal->transformCommon.pwr8 * zf * zf * zf * zf * zf + zi) / fractal->transformCommon.scale8;

		if (fractal->analyticDE.enabledFalse)
		{
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}
