/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Hybrid of Mandelbox and Mandelbulb power 2 with scaling of z axis
 */

#include "all_fractal_definitions.h"

cFractalBoxFoldBulbPow2::cFractalBoxFoldBulbPow2() : cAbstractFractal()
{
	nameInComboBox = "Box Fold Bulb Pow 2";
	internalName = "box_fold_bulb_pow2";
	internalID = fractal::boxFoldBulbPow2;
	DEType = deltaDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10000.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalBoxFoldBulbPow2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	(void)aux;
	if (fabs(z.x) > fractal->foldingIntPow.foldFactor)
		z.x = sign(z.x) * fractal->foldingIntPow.foldFactor * 2.0 - z.x;
	if (fabs(z.y) > fractal->foldingIntPow.foldFactor)
		z.y = sign(z.y) * fractal->foldingIntPow.foldFactor * 2.0 - z.y;
	if (fabs(z.z) > fractal->foldingIntPow.foldFactor)
		z.z = sign(z.z) * fractal->foldingIntPow.foldFactor * 2.0 - z.z;

	double fR2_2 = 1.0;
	double mR2_2 = 0.25;
	double r2_2 = z.Dot(z);
	double tglad_factor1_2 = fR2_2 / mR2_2;

	if (r2_2 < mR2_2)
	{
		z = z * tglad_factor1_2;
		aux.DE *= tglad_factor1_2;
	}
	else if (r2_2 < fR2_2)
	{
		double tglad_factor2_2 = fR2_2 / r2_2;
		z = z * tglad_factor2_2;
		aux.DE *= tglad_factor2_2;
	}

	z = z * 2.0;
	double x2 = z.x * z.x;
	double y2 = z.y * z.y;
	double z2 = z.z * z.z;
	double temp = 1.0 - z2 / (x2 + y2);
	CVector4 zTemp;
	zTemp.x = (x2 - y2) * temp;
	zTemp.y = 2.0 * z.x * z.y * temp;
	zTemp.z = -2.0 * z.z * sqrt(x2 + y2);
	zTemp.w = z.w;
	z = zTemp;
	z.z *= fractal->foldingIntPow.zFactor;
	// analyticDE controls
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = (aux.DE + 1.0) * 5.0 * aux.r * fractal->analyticDE.scale1
							 * sqrt(fractal->foldingIntPow.zFactor * fractal->foldingIntPow.zFactor + 2.0
											+ fractal->analyticDE.offset2)
						 + fractal->analyticDE.offset1;
	}
	// INFO remark: changed sequence of operation.
	// adding of C constant was before multiplying by z-factor
}
