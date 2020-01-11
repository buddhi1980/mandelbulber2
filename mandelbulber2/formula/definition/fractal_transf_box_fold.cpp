/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Box Fold
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfBoxFold::cFractalTransfBoxFold() : cAbstractFractal()
{
	nameInComboBox = "T>Box Fold";
	internalName = "transf_box_fold";
	internalID = fractal::transfBoxFold;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBoxFold::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	double colorAdd = 0.0;
	if (fabs(z.x) > fractal->mandelbox.foldingLimit)
	{
		z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
	}
	if (fabs(z.y) > fractal->mandelbox.foldingLimit)
	{
		z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
	}
	double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
	double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
	if (fabs(z.z) > zLimit)
	{
		z.z = sign(z.z) * zValue - z.z;
	}
	// aux.colo mode 1
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
	}

	// mode 2
	if (fractal->transformCommon.functionEnabledCxFalse)
	{
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			colorAdd += fractal->mandelbox.color.factor.x;
		}
		else
		{
			colorAdd +=
				fractal->mandelbox.color.factor.x
				* (1.0 - (fractal->mandelbox.foldingLimit - fabs(z.x)) / fractal->mandelbox.foldingLimit);
		}

		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			colorAdd += fractal->mandelbox.color.factor.y;
		}
		else
		{
			colorAdd +=
				fractal->mandelbox.color.factor.y
				* (1.0 - (fractal->mandelbox.foldingLimit - fabs(z.y)) / fractal->mandelbox.foldingLimit);
		}

		if (fabs(z.z) > fractal->mandelbox.foldingLimit)
		{
			colorAdd += fractal->mandelbox.color.factor.z;
		}
		else
		{
			colorAdd +=
				fractal->mandelbox.color.factor.z
				* (1.0 - (fractal->mandelbox.foldingLimit - fabs(z.z)) / fractal->mandelbox.foldingLimit);
		}
		aux.color += colorAdd;
	}

	// mode 3
	if (fractal->transformCommon.functionEnabledCyFalse)
	{
		double valMinusLim = fractal->mandelbox.foldingValue - fractal->mandelbox.foldingLimit;
		if (z.x != oldZ.x)
		{
			colorAdd += fractal->mandelbox.color.factor.x * (fabs(z.x) - fractal->mandelbox.foldingLimit)
									/ valMinusLim;
		}
		if (z.y != oldZ.y)
		{
			colorAdd += fractal->mandelbox.color.factor.y * (fabs(z.y) - fractal->mandelbox.foldingLimit)
									/ valMinusLim;
		}
		if (z.z != oldZ.z)
		{
			colorAdd += fractal->mandelbox.color.factor.z * (fabs(z.z) - fractal->mandelbox.foldingLimit)
									/ valMinusLim;
		}
		aux.color += colorAdd;
	}
}
