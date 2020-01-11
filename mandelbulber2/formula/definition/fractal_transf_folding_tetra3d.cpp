/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * folding tetra3D from M3D (Luca GN 2011):
 * Code taken from the forums, KIFS original thread
 * side note - if you disable the 1st half, 2nd half will be
 * done even if you disable it... (to avoid a NOP transform)
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfFoldingTetra3d::cFractalTransfFoldingTetra3d() : cAbstractFractal()
{
	nameInComboBox = "T>Folding Tetra 3D";
	internalName = "transf_folding_tetra3d";
	internalID = fractal::transfFoldingTetra3d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfFoldingTetra3d::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double x1;
	double y1;
	if (fractal->transformCommon.functionEnabledx)
	{
		if (z.x + z.y < 0.0)
		{
			x1 = -z.y;
			z.y = -z.x;
			z.x = x1;
		}
		if (z.x + z.z < 0.0)
		{
			x1 = -z.z;
			z.z = -z.x;
			z.x = x1;
		}
		if (z.y + z.z < 0.0)
		{
			y1 = -z.z;
			z.z = -z.y;
			z.y = y1;
		}
	}
	if (fractal->transformCommon.functionEnabledy)
	{
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
	}
}
