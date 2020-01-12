/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Adds Cpixel constant sin or cos
 */

#include "all_fractal_definitions.h"

cFractalTransfAddCpixelSinOrCos::cFractalTransfAddCpixelSinOrCos() : cAbstractFractal()
{
	nameInComboBox = "T>Add Cpixel - Sin Or Cos";
	internalName = "transf_add_cpixel_sin_or_cos";
	internalID = fractal::transfAddCpixelSinOrCos;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddCpixelSinOrCos::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 trigC = aux.const_c * fractal->transformCommon.constantMultiplierC111; // freq
	CVector4 trigC0 = trigC;

	if (!fractal->transformCommon.functionEnabledBxFalse)
	{
		// apply simple trig
		if (fractal->transformCommon.functionEnabledAx) trigC.x = sin(trigC0.x);
		if (fractal->transformCommon.functionEnabledAxFalse) trigC.x = cos(trigC0.x);
		if (fractal->transformCommon.functionEnabledAy) trigC.y = sin(trigC0.y);
		if (fractal->transformCommon.functionEnabledAyFalse) trigC.y = cos(trigC0.y);
		if (fractal->transformCommon.functionEnabledAz) trigC.z = sin(trigC0.z);
		if (fractal->transformCommon.functionEnabledAzFalse) trigC.z = cos(trigC0.z);
	}
	else
	{
		// mode2
		if (fractal->transformCommon.functionEnabledAx)
			trigC.x = (1.0 + sin(trigC0.x)) * sign(trigC.x) / 2.0;
		if (fractal->transformCommon.functionEnabledAxFalse)
			trigC.x = (1.0 + cos(trigC0.x)) * sign(trigC.x) / 2.0;
		if (fractal->transformCommon.functionEnabledAy)
			trigC.y = (1.0 + sin(trigC0.y)) * sign(trigC.y) / 2.0;
		if (fractal->transformCommon.functionEnabledAyFalse)
			trigC.y = (1.0 + cos(trigC0.y)) * sign(trigC.y) / 2.0;
		if (fractal->transformCommon.functionEnabledAz)
			trigC.z = (1.0 + sin(trigC0.z)) * sign(trigC.z) / 2.0;
		if (fractal->transformCommon.functionEnabledAzFalse)
			trigC.z = (1.0 + cos(trigC0.z)) * sign(trigC.z) / 2.0;
		// mode3
		if (fractal->transformCommon.functionEnabledByFalse) trigC *= trigC0;
	}
	trigC *= fractal->transformCommon.constantMultiplier111; // ampl

	// add cPixel
	if (!fractal->transformCommon.functionEnabledFalse)
	{
		z += trigC;
	}
	else
	{ // symmetical
		trigC = fabs(trigC);

		z.x += sign(z.x) * trigC.x;
		z.y += sign(z.y) * trigC.y;
		z.z += sign(z.z) * trigC.z;
	}

	// DE tweak (sometimes hepls)
	if (fractal->analyticDE.enabledFalse)
	{
		double tweakDE = z.Length() / oldZ.Length();
		aux.DE = aux.DE * tweakDE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}
