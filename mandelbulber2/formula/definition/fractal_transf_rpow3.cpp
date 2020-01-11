/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Rpow3 from M3D.
 * Does a power of 3 on the current length of the  vector.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfRpow3::cFractalTransfRpow3() : cAbstractFractal()
{
	nameInComboBox = "T>Rpow3";
	internalName = "transf_rpow3";
	internalID = fractal::transfRpow3;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfRpow3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double sqrRout = z.Dot(z) * fractal->transformCommon.scale;

	z *= sqrRout;
	aux.DE = aux.DE * fabs(sqrRout) + fractal->analyticDE.offset1;
}
