/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical fold varyVCL MBox type
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalFoldVaryVCL::cFractalTransfSphericalFoldVaryVCL() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Fold VaryVCL";
	internalName = "transf_spherical_fold_vary_vcl";
	internalID = fractal::transfSphericalFoldVaryVCL;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalFoldVaryVCL::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double para = 0.0;
	if (fractal->Cpara.enabledLinear)
	{
		para = fractal->Cpara.para00; // parameter value at iter 0
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
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{ // parabolic = paraOffset + iter *slope + (iter *iter *scale)
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope)
								+ (aux.i * aux.i * 0.001 * fractal->Cpara.parabScale);
	}
	para += paraAddP0 + fractal->transformCommon.minR2p25;

	// spherical fold
	double r2 = z.Dot(z);

	z += fractal->mandelbox.offset;

	// if (r2 < 1e-21) r2 = 1e-21;
	if (r2 < para)
	{
		z *= fractal->transformCommon.maxR2d1 / para;
		aux.DE *= fractal->transformCommon.maxR2d1 / para;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->transformCommon.maxR2d1 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
	z -= fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale);
}
