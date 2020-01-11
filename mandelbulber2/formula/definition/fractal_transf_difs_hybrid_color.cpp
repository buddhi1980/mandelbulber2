/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Hybrid DIFS aux.Color
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSHybridColor::cFractalTransfDIFSHybridColor() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Hybrid Color";
	internalName = "transf_difs_hybrid_color";
	internalID = fractal::transfDIFSHybridColor;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSHybridColor::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		double colorAdd = 0.0;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			colorAdd += fractal->foldColor.difs0000.x * fabs(z.x * z.y);
			colorAdd += fractal->foldColor.difs0000.y * max(z.x, z.y);
			colorAdd += fractal->foldColor.difs0000.z * max(z.x * z.x, z.y * z.y);
			colorAdd += fractal->foldColor.difs0000.w * z.z;

			if (fractal->transformCommon.functionEnabledFalse)
				colorAdd = fractal->transformCommon.scale1 * round(colorAdd);
		}
		colorAdd += fractal->foldColor.difs1; // 0 + 1 = 1

		if (fractal->foldColor.auxColorEnabledA)
		{
			if (aux.dist != aux.colorHybrid) aux.color += colorAdd;
		}
		else
		{
			aux.color += colorAdd; // color based only on iter (no aux.dist check)
		}

		// update for next iter
		aux.colorHybrid = aux.dist;
	}
	// else
	// aux.color = 0.0;

	// NOT RELATED TO COLOR  an aux.dist tweak test
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{

		aux.dist = aux.dist * fractal->analyticDE.scale1;
	}
}
