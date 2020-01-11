/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Inverse cylindrical coordinates, very easy transform
 * Formula by Luca GN 2011
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfInvCylindrical::cFractalTransfInvCylindrical() : cAbstractFractal()
{
	nameInComboBox = "T>Inverse Cylindrical";
	internalName = "transf_inv_cylindrical";
	internalID = fractal::transfInvCylindrical;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfInvCylindrical::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double newZx = z.x;
	double newZy = z.y;

	if (fractal->transformCommon.functionEnabledFalse) newZx = z.x * cos(z.y);
	if (fractal->transformCommon.functionEnabledxFalse) newZy = z.x * sin(z.y);

	z = CVector4(z.x * cos(newZy * fractal->transformCommon.scale1),
				newZx * sin(z.y * fractal->transformCommon.scale1), z.z * fractal->transformCommon.scaleC1,
				z.w)
			* fractal->transformCommon.scaleA1;

	aux.DE = aux.DE * fabs(fractal->transformCommon.scaleA1) * fractal->transformCommon.scaleB1
					 + fractal->transformCommon.offset1;
}
