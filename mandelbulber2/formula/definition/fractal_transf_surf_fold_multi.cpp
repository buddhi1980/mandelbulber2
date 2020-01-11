/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfSurfFoldMultiIteration
 * Based on amazing_surf_mod1 from Mandelbulb3D
 * This formula contains aux.color, analytic aux.DE and aux.r-dz
 */

#include "all_fractal_definitions.h"

cFractalTransfSurfFoldMulti::cFractalTransfSurfFoldMulti() : cAbstractFractal()
{
	nameInComboBox = "T>Surf Fold Multi";
	internalName = "transf_surf_fold_multi";
	internalID = fractal::transfSurfFoldMulti;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSurfFoldMulti::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAx)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	}

	if (fractal->transformCommon.functionEnabledAyFalse)
	{ // z = fold - fabs( fabs(z) - fold)
		z.x = fractal->transformCommon.additionConstant111.x
					- fabs(fabs(z.x) - fractal->transformCommon.additionConstant111.x);
		z.y = fractal->transformCommon.additionConstant111.y
					- fabs(fabs(z.y) - fractal->transformCommon.additionConstant111.y);
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
	}

	if (fractal->transformCommon.functionEnabledxFalse)
	{ // if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
		if (z.x > fractal->transformCommon.additionConstant111.x)
		{
			z.x = fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		else if (z.x < -fractal->transformCommon.additionConstant111.x)
		{
			z.x = -fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (z.y > fractal->transformCommon.additionConstant111.y)
		{
			z.y = fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		else if (z.y < -fractal->transformCommon.additionConstant111.y)
		{
			z.y = -fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
	}

	if (fractal->transformCommon.functionEnabledyFalse)
	{ // z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
		z.x = fractal->transformCommon.offset2
					- fabs(fabs(z.x + fractal->transformCommon.additionConstant111.x)
								 - fractal->transformCommon.offset2)
					- fractal->transformCommon.additionConstant111.x;
		z.y = fractal->transformCommon.offset2
					- fabs(fabs(z.y + fractal->transformCommon.additionConstant111.y)
								 - fractal->transformCommon.offset2)
					- fractal->transformCommon.additionConstant111.y;
	}
	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0; // tweak
}
