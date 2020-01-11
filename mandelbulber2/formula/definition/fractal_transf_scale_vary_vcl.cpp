/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 *  Scale varies smoothly based on iteration parameters.
 */

#include "all_fractal_definitions.h"

cFractalTransfScaleVaryVCL::cFractalTransfScaleVaryVCL() : cAbstractFractal()
{
	nameInComboBox = "T>Scale VaryVCL";
	internalName = "transf_scale_vary_vcl";
	internalID = fractal::transfScaleVaryVCL;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfScaleVaryVCL::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double para = 0.0;

	if (fractal->Cpara.enabledLinear)
	{
		para = fractal->Cpara.para0; // parameter value at iter 0
		double temp0 = para;
		double tempA = fractal->Cpara.paraA;
		double tempB = fractal->Cpara.paraB;
		double tempC = fractal->Cpara.paraC;
		double lengthAB = fractal->Cpara.iterB - fractal->Cpara.iterA;
		double lengthBC = fractal->Cpara.iterC - fractal->Cpara.iterB;
		double grade1 = (tempA - temp0) / fractal->Cpara.iterA;
		double grade2 = (tempB - tempA) / lengthAB;
		double grade3 = (tempC - tempB) / lengthBC;

		// slopes
		if (aux.i < fractal->Cpara.iterA)
		{
			para = temp0 + (aux.i * grade1);
		}
		if (aux.i < fractal->Cpara.iterB && aux.i >= fractal->Cpara.iterA)
		{
			para = tempA + (aux.i - fractal->Cpara.iterA) * grade2;
		}
		if (aux.i >= fractal->Cpara.iterB)
		{
			para = tempB + (aux.i - fractal->Cpara.iterB) * grade3;
		}

		// Curvi part on "true"
		if (fractal->Cpara.enabledCurves)
		{
			double paraAdd = 0.0;
			double paraIt;
			if (lengthAB > 2.0 * fractal->Cpara.iterA) // stop  error, todo fix.
			{
				double curve1 = (grade2 - grade1) / (4.0 * fractal->Cpara.iterA);
				double tempL = lengthAB - fractal->Cpara.iterA;
				double curve2 = (grade3 - grade2) / (4.0 * tempL);
				if (aux.i < 2 * fractal->Cpara.iterA)
				{
					paraIt = tempA - fabs(tempA - aux.i);
					paraAdd = paraIt * paraIt * curve1;
				}
				if (aux.i >= 2 * fractal->Cpara.iterA && aux.i < fractal->Cpara.iterB + tempL)
				{
					paraIt = tempB - fabs(tempB * aux.i);
					paraAdd = paraIt * paraIt * curve2;
				}
				para += paraAdd;
			}
		}
	}
	// Parabolic
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset + (aux.i * fractal->Cpara.parabSlope)
								+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
	}
	para += paraAddP0;

	z *= para; // using the parameter
	aux.DE = aux.DE * fabs(para) + 1.0;
}
