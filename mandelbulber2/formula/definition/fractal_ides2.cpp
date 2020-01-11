/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * From M3D. A formula made by Trafassel, the original Ide's Formula thread
 * @reference http://www.fractalforums.com/videos/formula-21-%28julia-set-interpretation%29/
 */

#include "all_fractal_definitions.h"

cFractalIdes2::cFractalIdes2() : cAbstractFractal()
{
	nameInComboBox = "Ides 2";
	internalName = "ides2";
	internalID = fractal::ides2;
	DEType = deltaDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalIdes2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	CVector4 z2 = z * z;
	CVector4 newZ;
	newZ.x = fractal->transformCommon.constantMultiplier121.x * z2.x
					 - fractal->transformCommon.additionConstant0555.x * (z2.y + z2.z);
	newZ.y = fractal->transformCommon.constantMultiplier121.y * z.x * z.y * z.z;
	newZ.z = fractal->transformCommon.constantMultiplier121.z * z2.z
					 - fractal->transformCommon.additionConstant0555.z * (z2.x + z2.y);
	newZ.w = z.w;
	z = newZ + z;
}
