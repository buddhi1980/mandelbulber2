/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * scale 4D
 * This formula contains aux.actualScaleA
 */

#include "all_fractal_definitions.h"

cFractalTransfScale4d::cFractalTransfScale4d() : cAbstractFractal()
{
	nameInComboBox = "T>Scale 4D";
	internalName = "transf_scale4d";
	internalID = fractal::transfScale4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfScale4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double useScale = 1.0;
	useScale = aux.actualScaleA + fractal->transformCommon.scale;

	z *= useScale;

	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(useScale) + 1.0;
	else
		aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
	{
		// update actualScaleA for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
		if (fractal->transformCommon.functionEnabledMFalse)
			aux.actualScaleA = -vary;
		else
			aux.actualScaleA = aux.actualScaleA - vary;
	}
}
