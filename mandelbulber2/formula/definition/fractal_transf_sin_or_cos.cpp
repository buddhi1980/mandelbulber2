/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * sin or cos z
 */

#include "all_fractal_definitions.h"

cFractalTransfSinOrCos::cFractalTransfSinOrCos() : cAbstractFractal()
{
	nameInComboBox = "T>Sin Or Cos";
	internalName = "transf_sin_or_cos";
	internalID = fractal::transfSinOrCos;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSinOrCos::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 trigZ = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 scaleZ = z * fractal->transformCommon.constantMultiplierC111;

	if (fractal->transformCommon.functionEnabledAx)
	{
		if (!fractal->transformCommon.functionEnabledAxFalse)
			trigZ.x = sin(scaleZ.x);
		else
			trigZ.x = cos(scaleZ.x); // scale =0, cos = 1
	}
	if (fractal->transformCommon.functionEnabledAy)
	{
		if (!fractal->transformCommon.functionEnabledAyFalse)
			trigZ.y = sin(scaleZ.y);
		else
			trigZ.y = cos(scaleZ.y);
	}
	if (fractal->transformCommon.functionEnabledAz)
	{
		if (!fractal->transformCommon.functionEnabledAzFalse)
			trigZ.z = sin(scaleZ.z);
		else
			trigZ.z = cos(scaleZ.z);
	}

	z = trigZ * fractal->transformCommon.scale;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		z.x = z.x * fractal->transformCommon.scale / (fabs(oldZ.x) + 1.0);
		z.y = z.y * fractal->transformCommon.scale / (fabs(oldZ.y) + 1.0);
		z.z = z.z * fractal->transformCommon.scale / (fabs(oldZ.z) + 1.0);
		// aux.DE = aux.DE * z.Length() / oldZ.Length();
	}
	//   if z == oldZ    z = oldZ * fractal->transformCommon.scale;
	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
}
