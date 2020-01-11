/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * scale vary multi- based on DarkBeam's maths
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfScaleVaryMulti::cFractalTransfScaleVaryMulti() : cAbstractFractal()
{
	nameInComboBox = "T>Scale Vary Multi";
	internalName = "transf_scale_vary_multi";
	internalID = fractal::transfScaleVaryMulti;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfScaleVaryMulti::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			aux.actualScaleA = fabs(aux.actualScaleA);
		}
		double base = fractal->transformCommon.scaleMain2;
		double vary = (aux.actualScaleA - fractal->transformCommon.offset1);

		if (fractal->transformCommon.functionEnabled)
		{
			aux.actualScaleA = base + fractal->transformCommon.scaleVary0 * vary;
		}
		else if (fractal->transformCommon.functionEnabledCxFalse)
		{
			base = aux.actualScaleA;
			aux.actualScaleA = base + fractal->transformCommon.scale0 * vary;
		}

		else if (fractal->transformCommon.functionEnabledCyFalse)
		{
			double base2 = base
										 + fractal->transformCommon.scaleVary0
												 * (aux.actualScaleA - fractal->transformCommon.offset1);

			aux.actualScaleA =
				base2
				+ fractal->transformCommon.scale0 * (aux.actualScaleA - fractal->transformCommon.offset1);
		}

		else if (fractal->transformCommon.functionEnabledCzFalse)
		{

			base = aux.actualScaleA;
			double base2 =
				base
				+ fractal->transformCommon.scale0 * (aux.actualScaleA - fractal->transformCommon.offset1);

			aux.actualScaleA = base2
												 + fractal->transformCommon.scaleVary0
														 * (aux.actualScaleA - fractal->transformCommon.offset1);
		}
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
		aux.DE = aux.DE * fabs(fractal->transformCommon.scaleMain2) + 1.0;
	}
	else
	{
		if (fractal->transformCommon.functionEnabledBzFalse)
		{
			aux.actualScaleA = fractal->transformCommon.scaleMain2;
		}
		z *= aux.actualScaleA;
		aux.DE = aux.DE * fabs(aux.actualScaleA) + 1.0;
	}
}
