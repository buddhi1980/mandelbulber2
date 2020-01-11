/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Box Fold XYZ, set different folding parameters for each axis
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfBoxFoldXYZ::cFractalTransfBoxFoldXYZ() : cAbstractFractal()
{
	nameInComboBox = "T>Box Fold XYZ";
	internalName = "transf_box_fold_xyz";
	internalID = fractal::transfBoxFoldXYZ;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBoxFoldXYZ::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 Value4 = fractal->transformCommon.additionConstant222;
	if (fractal->transformCommon.functionEnabledFalse)
		Value4 = 2.0 * fractal->transformCommon.additionConstant111;

	if (z.x > fractal->transformCommon.additionConstant111.x)
	{
		z.x = Value4.x - z.x;
	}
	else if (z.x < -fractal->transformCommon.additionConstant111.x)
	{
		z.x = -Value4.x - z.x;
	}
	if (z.y > fractal->transformCommon.additionConstant111.y)
	{
		z.y = Value4.y - z.y;
	}
	else if (z.y < -fractal->transformCommon.additionConstant111.y)
	{
		z.y = -Value4.y - z.y;
	}
	if (z.z > fractal->transformCommon.additionConstant111.z)
	{
		z.z = Value4.z - z.z;
	}
	else if (z.z < -fractal->transformCommon.additionConstant111.z)
	{
		z.z = -Value4.z - z.z;
	}
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor.z;
	}
}
