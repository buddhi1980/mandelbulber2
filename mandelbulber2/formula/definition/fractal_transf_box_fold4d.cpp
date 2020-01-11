/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * box fold 4D
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfBoxFold4d::cFractalTransfBoxFold4d() : cAbstractFractal()
{
	nameInComboBox = "T>Box Fold 4D";
	internalName = "transf_box_fold4d";
	internalID = fractal::transfBoxFold4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBoxFold4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	if (z.x > fractal->mandelbox.foldingLimit)
	{
		z.x = fractal->mandelbox.foldingValue - z.x;
	}
	else if (z.x < -fractal->mandelbox.foldingLimit)
	{
		z.x = -fractal->mandelbox.foldingValue - z.x;
	}
	if (z.y > fractal->mandelbox.foldingLimit)
	{
		z.y = fractal->mandelbox.foldingValue - z.y;
	}
	else if (z.y < -fractal->mandelbox.foldingLimit)
	{
		z.y = -fractal->mandelbox.foldingValue - z.y;
	}
	if (z.z > fractal->mandelbox.foldingLimit)
	{
		z.z = fractal->mandelbox.foldingValue - z.z;
	}
	else if (z.z < -fractal->mandelbox.foldingLimit)
	{
		z.z = -fractal->mandelbox.foldingValue - z.z;
	}
	if (z.w > fractal->mandelbox.foldingLimit)
	{
		z.w = fractal->mandelbox.foldingValue - z.w;
	}
	else if (z.w < -fractal->mandelbox.foldingLimit)
	{
		z.w = -fractal->mandelbox.foldingValue - z.w;
	}
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x;
		if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y;
		if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z;
		if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w;
	}
}
