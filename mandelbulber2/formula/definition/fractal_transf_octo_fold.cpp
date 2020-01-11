/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Octo
 * analytic aux.DE
 */

#include "all_fractal_definitions.h"

cFractalTransfOctoFold::cFractalTransfOctoFold() : cAbstractFractal()
{
	nameInComboBox = "T>Octo Fold";
	internalName = "transf_octo_fold";
	internalID = fractal::transfOctoFold;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfOctoFold::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (z.x + z.y < 0.0) z = CVector4(-z.y, -z.x, z.z, z.w);

	if (z.x + z.z < 0.0) // z.xz = -z.zx;
		z = CVector4(-z.z, z.y, -z.x, z.w);

	if (z.x - z.y < 0.0) // z.xy = z.yx;
		z = CVector4(z.y, z.x, z.z, z.w);

	if (z.x - z.z < 0.0) // z.xz = z.zx;
		z = CVector4(z.z, z.y, z.x, z.w);

	z.x = fabs(z.x);
	z = z * fractal->transformCommon.scale2
			- fractal->transformCommon.offset100 * (fractal->transformCommon.scale2 - 1.0);

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE *= fractal->transformCommon.scale2;
		else
			aux.DE = aux.DE * fractal->transformCommon.scale2 * fractal->analyticDE.scale1
							 + fractal->analyticDE.offset0;
	}
}
