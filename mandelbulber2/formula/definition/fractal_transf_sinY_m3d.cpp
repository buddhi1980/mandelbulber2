/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * _sinY code from M3D,
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfSinYM3d::cFractalTransfSinYM3d() : cAbstractFractal()
{
	nameInComboBox = "T>SinY M3D";
	internalName = "transf_sinY_m3d";
	internalID = fractal::transfSinYM3d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSinYM3d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAx)
		z.x = sin((z.x - fractal->transformCommon.offset000.x) * fractal->transformCommon.constantMultiplierA111.x)
				* fractal->transformCommon.constantMultiplierB111.x + fractal->transformCommon.offsetA000.x;
	if (fractal->transformCommon.functionEnabledAyFalse)
		z.y = sin((z.y - fractal->transformCommon.offset000.y) * fractal->transformCommon.constantMultiplierA111.y)
				* fractal->transformCommon.constantMultiplierB111.y + fractal->transformCommon.offsetA000.y;
	if (fractal->transformCommon.functionEnabledAzFalse)
		z.z = sin((z.z - fractal->transformCommon.offset000.z) * fractal->transformCommon.constantMultiplierA111.z)
				* fractal->transformCommon.constantMultiplierB111.z + fractal->transformCommon.offsetA000.z;

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
