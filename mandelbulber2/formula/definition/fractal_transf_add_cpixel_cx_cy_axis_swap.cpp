/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Adds Cpixel constant to z vector, swapping the Cpixel vector x and y axes
 * disable swap for normal mode
 */

#include "all_fractal_definitions.h"

cFractalTransfAddCpixelCxCyAxisSwap::cFractalTransfAddCpixelCxCyAxisSwap() : cAbstractFractal()
{
	nameInComboBox = "T>Add Cpixel - CxCy Axis Swap";
	internalName = "transf_add_cpixel_cx_cy_axis_swap";
	internalID = fractal::transfAddCpixelCxCyAxisSwap;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddCpixelCxCyAxisSwap::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	CVector4 tempC = c;
	if (fractal->transformCommon.functionEnabled)
	{
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w);
			aux.c = tempC;
		}
		else
		{
			tempC = CVector4(c.y, c.x, c.z, c.w);
		}
	}
	z += tempC * fractal->transformCommon.constantMultiplier111;
}
