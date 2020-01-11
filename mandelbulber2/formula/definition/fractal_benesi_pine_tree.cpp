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
	CVector4 c = aux.const_c * fractal->transformCommon.constantMultiplier100;
	CVector4 zz = z * z;
	aux.r = sqrt(zz.x + zz.y + zz.z); // needed when alternating pwr2s
	aux.DE = aux.r * aux.DE * 2.0 + 1.0;

	double t = 1.0;
	double temp = zz.y + zz.z;
	if (temp > 0.0) t = 2.0 * z.x / sqrt(temp);
	temp = z.z;
	z.x = (zz.x - zz.y - zz.z);
	z.y = (2.0 * t * z.y * temp);
	z.z = (t * (zz.y - zz.z));
	// c.yz swap
	z.x += c.x;
	z.z += c.y;
	z.y += c.z;

	if (fractal->transformCommon.angle0 != 0)
	{
		double tempY = z.y;
		double beta = fractal->transformCommon.angle0 * M_PI_180;
		z.y = z.y * cos(beta) + z.z * sin(beta);
		z.z = tempY * -sin(beta) + z.z * cos(beta);
	}
}
