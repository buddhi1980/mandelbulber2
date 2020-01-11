/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * scale vary V2.12.- based on DarkBeam's maths
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfScaleVaryV212::cFractalTransfScaleVaryV212() : cAbstractFractal()
{
	nameInComboBox = "T>Scale Vary V2.12";
	internalName = "transf_scale_vary_v212";
	internalID = fractal::transfScaleVaryV212;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfScaleVaryV212::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			aux.actualScaleA = fabs(aux.actualScaleA);
		}
		aux.actualScaleA =
			fractal->transformCommon.scaleMain2 + fractal->transformCommon.scaleVary0 * aux.actualScaleA;
		double temp = aux.actualScaleA;
		if (fractal->transformCommon.functionEnabledByFalse) // limits
		{
			if (aux.actualScaleA < fractal->transformCommon.offset0)
				temp = fractal->transformCommon.offset0;
			if (aux.actualScaleA > fractal->transformCommon.offset4)
				temp = fractal->transformCommon.offset4;
		}

		z *= temp;
		aux.DE = aux.DE * fabs(temp) + 1.0;
	}

	else if (aux.i < fractal->transformCommon.startIterations)
	{
		z *= fractal->transformCommon.scaleMain2;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scaleMain2 + 1.0);
	}
	else
	{
		if (fractal->transformCommon.functionEnabledBzFalse)
		{
			aux.actualScaleA = fractal->transformCommon.scaleMain2;
		}
		z *= aux.actualScaleA;
		aux.DE = aux.DE * fabs(aux.actualScaleA + 1.0);
	}
}
