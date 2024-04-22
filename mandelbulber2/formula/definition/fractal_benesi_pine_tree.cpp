/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * benesiFastPwr2PineTree 3D
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */

#include "all_fractal_definitions.h"

cFractalBenesiPineTree::cFractalBenesiPineTree() : cAbstractFractal()
{
	nameInComboBox = "Benesi - Pine Tree";
	internalName = "benesi_pine_tree";
	internalID = fractal::benesiPineTree;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelAlreadyHas;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalBenesiPineTree::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.r * aux.DE * 2.0 + 1.0;

	CVector4 temp = z;

	z *= z;
	double t = z.y + z.z;
	z.x -= t;

	if (t > 0.0)
	{
		temp.x = 2.0 * temp.x / sqrt(t);
		z.z = temp.x * (z.y - z.z);
		z.y = 2.0 * temp.x * temp.y * temp.z;
	}
	else
	{
		z.z = z.y - z.z;
		z.y = 2.0 * temp.y * temp.z;
	}

	z.x += aux.const_c.x * fractal->transformCommon.constantMultiplier100.x;
		// c.yz swap
	z.z += aux.const_c.y * fractal->transformCommon.constantMultiplier100.y;
	z.y += aux.const_c.z * fractal->transformCommon.constantMultiplier100.z;

	if (fractal->transformCommon.angle0 != 0)
	{
		temp.y = z.y;
		t = fractal->transformCommon.angle0 * M_PI_180;
		temp.x = sin(t);
		temp.z = cos(t);
		z.y = z.y * temp.z + z.z * temp.x;
		z.z = temp.y * -temp.x + z.z * temp.z;
	}
}
