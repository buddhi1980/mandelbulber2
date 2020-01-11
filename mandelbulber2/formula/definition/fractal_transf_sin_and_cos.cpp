/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * sin and cos
 */

#include "all_fractal_definitions.h"

cFractalTransfSinAndCos::cFractalTransfSinAndCos() : cAbstractFractal()
{
	nameInComboBox = "T>Sin And Cos";
	internalName = "transf_sin_and_cos";
	internalID = fractal::transfSinAndCos;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSinAndCos::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// Q_UNUSED(aux);
	CVector4 oldZ = z;
	CVector4 sinZ = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 cosZ = CVector4(0.0, 0.0, 0.0, 0.0);

	CVector4 scaleZ = z * fractal->transformCommon.constantMultiplierC111;

	if (fractal->transformCommon.functionEnabledAx) sinZ.x = sin(scaleZ.x); // scale =0, sin = 0
	if (fractal->transformCommon.functionEnabledAy) sinZ.y = sin(scaleZ.y);
	if (fractal->transformCommon.functionEnabledAz) sinZ.z = sin(scaleZ.z);

	if (fractal->transformCommon.functionEnabledAxFalse) cosZ.x = cos(scaleZ.x); // scale =0, cos = 1
	if (fractal->transformCommon.functionEnabledAyFalse) cosZ.y = cos(scaleZ.y);
	if (fractal->transformCommon.functionEnabledAzFalse) cosZ.z = cos(scaleZ.z);

	switch (fractal->combo6.combo6)
	{
		case multi_combo6_type1:
		default: z = (sinZ + cosZ); break;
		case multi_combo6_type2: z = sinZ * cosZ; break;
		case multi_combo6_type3: z = oldZ + (sinZ + cosZ); break;
		case multi_combo6_type4: z = oldZ + (sinZ * cosZ); break;
		case multi_combo6_type5: z = oldZ * (sinZ + cosZ); break;
		case multi_combo6_type6: z = oldZ * (sinZ * cosZ); break;
	}

	// post scale
	if (!fractal->transformCommon.functionEnabledFalse)
		z *= fractal->transformCommon.scale;
	else
	{
		z.x = z.x * fractal->transformCommon.scale / (fabs(oldZ.x) + 1.0);
		z.y = z.y * fractal->transformCommon.scale / (fabs(oldZ.y) + 1.0);
		z.z = z.z * fractal->transformCommon.scale / (fabs(oldZ.z) + 1.0);
		// aux.DE = aux.DE * z.Length() / oldZ.Length();
	}

	if (!fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
}
