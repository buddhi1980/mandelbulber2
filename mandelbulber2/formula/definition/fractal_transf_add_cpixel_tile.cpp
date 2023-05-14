/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Based on M3D _julia sets
 * @reference A pretransform to show juliasets from formulas.
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfAddCpixelTile::cFractalTransfAddCpixelTile() : cAbstractFractal()
{
	nameInComboBox = "T>Add Cpixel Tile";
	internalName = "transf_add_cpixel_tile";
	internalID = fractal::transfAddCpixelTile;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddCpixelTile::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 cv = aux.const_c;
	if (aux.i == fractal->transformCommon.startIterations)
	{
		CVector4 p = aux.const_c;
		CVector4 rs = 1.0 / fractal->transformCommon.scale3D444;
		cv.x = round(p.x * rs.x);
		cv.y = round(p.y * rs.y);
		cv.z = round(p.z * rs.z);
		z = (p * rs - cv) * fractal->transformCommon.scale3D444;

		if (fractal->transformCommon.functionEnabledCxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledCyFalse) z.y = fabs(z.y);

		cv = cv * fractal->transformCommon.constantMultiplier111
				+ fractal->transformCommon.offset000;

		if (fractal->transformCommon.functionEnabledBFalse)
		{
			cv.x *= sign(aux.const_c.x);
			cv.y *= sign(aux.const_c.y);
			cv.z *= sign(aux.const_c.z);
		}
		aux.c = cv;
	}

	if (aux.i >= fractal->transformCommon.startIterationsA)
	{
		cv = aux.c;
		if (fractal->transformCommon.functionEnabledCFalse)
		{
			cv.x *= sign(z.x);
			cv.y *= sign(z.y);
			cv.z *= sign(z.z);
		}
		z += cv;
		aux.c = cv;
	}

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1
								+ fractal->analyticDE.offset0;

	// aux->color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux.color += fabs(cv.x * cv.y) * fractal->foldColor.difs0000.x;
		aux.color += (cv.x * cv.x + cv.y * cv.y) * fractal->foldColor.difs0000.y;
	}
}
