/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical radial offset Curvilinear.
 * This formula contains analytic aux.DE and aux.r-dz
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalOffsetVCL::cFractalTransfSphericalOffsetVCL() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical OffsetVCL";
	internalName = "transf_spherical_offset_vcl";
	internalID = fractal::transfSphericalOffsetVCL;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalOffsetVCL::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double para = fractal->Cpara.para00;
	double paraAdd = 0.0;
	double paraAddP0 = 0.0;
	// curvilinear mode
	if (fractal->transformCommon.functionEnabled)
	{
		if (fractal->Cpara.enabledLinear)
		{
			para = fractal->Cpara.para00; // parameter value at iter 0
			double temp0 = para;
			double tempA = fractal->Cpara.paraA0;
			double tempB = fractal->Cpara.paraB0;
			double tempC = fractal->Cpara.paraC0;
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
				}
				para += paraAdd;
			}
		}
	}
	// Parabolic
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope)
								+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
		para += paraAddP0;
	}
	// para offset
	para += fractal->transformCommon.offset0;

	double div = 0.0;
	// dot mode
	if (fractal->transformCommon.functionEnabledFalse)
	{
		div = z.Dot(z);
	}
	else
	{
		div = z.Length();
	}

	// using the parameter
	z *= 1.0 + para / -div;

	// post scale
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fractal->transformCommon.scale + 1.0;

	// DE tweak
	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
