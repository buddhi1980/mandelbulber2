/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * From M3D
 * @reference http://www.fractalforums.com/3d-fractal-generation/another-shot-at-the-holy-grail/
 */

#include "all_fractal_definitions.h"

cFractalQuickDudleyMod::cFractalQuickDudleyMod() : cAbstractFractal()
{
	nameInComboBox = "Quick Dudley - Mod";
	internalName = "quick_dudley_mod";
	internalID = fractal::quickDudleyMod;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalQuickDudleyMod::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double newx = x2 * fractal->transformCommon.constantMultiplier111.x
								- fractal->transformCommon.constantMultiplier222.x * z.y * z.z;
	double newy = z2 * fractal->transformCommon.constantMultiplier111.y
								+ fractal->transformCommon.constantMultiplier222.y * z.x * z.y;
	double newz = y2 * fractal->transformCommon.constantMultiplier111.z
								+ fractal->transformCommon.constantMultiplier222.z * z.z * z.x;
	z.x = newx;
	z.y = newy;
	z.z = newz;
}
