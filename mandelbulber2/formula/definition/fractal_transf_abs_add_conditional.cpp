/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * abs add conditional.
 */

#include "all_fractal_definitions.h"

cFractalTransfAbsAddConditional::cFractalTransfAbsAddConditional() : cAbstractFractal()
{
	nameInComboBox = "T>Abs Add Conditional";
	internalName = "transf_abs_add_conditional";
	internalID = fractal::transfAbsAddConditional;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAbsAddConditional::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledx)
	{
		z.x = sign(z.x)
					* (fractal->transformCommon.offset111.x - fabs(z.x)
						 + fabs(z.x) * fractal->transformCommon.scale3D000.x);
	}
	/*{
		double signZx = sign(z.x);
		z.x = (signZx * fractal->transformCommon.offset111.x)
			- fabs(z.x) * (signZx - fractal->transformCommon.offset000.x);
	}*/
	/*{
		double Absx = fabs(z.x);
		z.x = sign(z.x) * (fractal->transformCommon.offset111.x - Absx
			+ Absx * fractal->transformCommon.offset000.x);
	}*/

	if (fractal->transformCommon.functionEnabledy)
	{
		z.y = sign(z.y)
					* (fractal->transformCommon.offset111.y - fabs(z.y)
						 + fabs(z.y) * fractal->transformCommon.offset000.y);
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		z.z = sign(z.z)
					* (fractal->transformCommon.offset111.z - fabs(z.z)
						 + fabs(z.z) * fractal->transformCommon.offset000.z);
	}

	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}
