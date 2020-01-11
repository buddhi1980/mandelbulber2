/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * max sin - cos
 */

#include "all_fractal_definitions.h"

cFractalTransfSinAndCosMax::cFractalTransfSinAndCosMax() : cAbstractFractal()
{
	nameInComboBox = "T>Sin And Cos - Max";
	internalName = "transf_sin_and_cos_max";
	internalID = fractal::transfSinAndCosMax;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSinAndCosMax::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 maxZ = z;
	CVector4 sinZ = z;
	CVector4 cosZ = z;
	CVector4 sinCosZ = z;

	if (fractal->transformCommon.functionEnabledAx)
	{
		sinZ.x = sin(oldZ.x * M_PI_2x / fractal->transformCommon.constantMultiplierA111.x)
						 * fractal->transformCommon.scaleA1; // freq
		cosZ.x = cos(oldZ.x * M_PI_2x / fractal->transformCommon.constantMultiplierB111.x)
						 * fractal->transformCommon.scaleB1;
		sinCosZ.x = sinZ.x * cosZ.x * fractal->transformCommon.scaleC1;
		maxZ.x = max(max(sinZ.x, cosZ.x), sinCosZ.x);

		if (fractal->transformCommon.functionEnabledFalse) maxZ.x /= (fabs(oldZ.x) + 1.0);
	}

	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		sinZ.y = sin(oldZ.y * M_PI_2x / fractal->transformCommon.constantMultiplierA111.y)
						 * fractal->transformCommon.scaleA1; // freq
		cosZ.y = cos(oldZ.y * M_PI_2x / fractal->transformCommon.constantMultiplierB111.y)
						 * fractal->transformCommon.scaleB1;
		sinCosZ.y = sinZ.y * cosZ.y * fractal->transformCommon.scaleC1;
		maxZ.y = max(max(sinZ.y, cosZ.y), sinCosZ.y);

		if (fractal->transformCommon.functionEnabledFalse) maxZ.y /= (fabs(oldZ.y) + 1.0);
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		sinZ.z = sin(oldZ.z * M_PI_2x / fractal->transformCommon.constantMultiplierA111.z)
						 * fractal->transformCommon.scaleA1; // freq
		cosZ.z = cos(oldZ.z * M_PI_2x / fractal->transformCommon.constantMultiplierB111.z)
						 * fractal->transformCommon.scaleB1;
		sinCosZ.z = sinZ.z * cosZ.z * fractal->transformCommon.scaleC1;
		maxZ.z = max(max(sinZ.z, cosZ.z), sinCosZ.z);

		if (fractal->transformCommon.functionEnabledFalse) maxZ.z /= (fabs(oldZ.z) + 1.0);
	}

	// post scale
	maxZ *= fractal->transformCommon.scale;
	aux.DE *= fabs(fractal->transformCommon.scale);

	if (fractal->transformCommon.functionEnabledMFalse)
	{
		switch (fractal->combo4.combo4)
		{
			case multi_combo4_type1:
			default: maxZ += oldZ; break;
			case multi_combo4_type2: maxZ *= oldZ; break;
			case multi_combo4_type3: maxZ += fabs(oldZ); break;
			case multi_combo4_type4: maxZ *= fabs(oldZ); break;
		}
	}

	z = maxZ;

	// analytic tweaks
	if (fractal->analyticDE.enabledFalse) // temp
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
}
