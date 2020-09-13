/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * initial 4D
 * This formula contains aux.actualScaleA
 */

#include "all_fractal_definitions.h"

cFractalTransfInitial4d::cFractalTransfInitial4d() : cAbstractFractal()
{
	nameInComboBox = "T>Initial 4D";
	internalName = "transf_initial4d";
	internalID = fractal::transfInitial4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfInitial4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		if (!fractal->transformCommon.functionEnabledFalse)
		{
			z = CVector4(z.x + z.y + z.z, -z.x - z.y + z.z, -z.x + z.y - z.z, z.x - z.y - z.z);
			z = fabs(z);
			z = fabs(z - fractal->transformCommon.offsetA0000);
		}
		else
		{
			z = CVector4(z.x, z.y, z.z, z.Length());
		}
		if (fractal->transformCommon.addCpixelEnabledFalse)
			aux.const_c = z * fractal->transformCommon.scale1111;

		if (fractal->transformCommon.functionEnabledAFalse) z = CVector4(0.0, 0.0, 0.0, 0.0);
	}
	// DE tweak
	if (!fractal->analyticDE.enabledFalse) aux.DE = aux.DE * z.Length() / aux.r;

	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
