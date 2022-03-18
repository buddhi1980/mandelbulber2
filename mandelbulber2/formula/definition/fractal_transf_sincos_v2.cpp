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

cFractalTransfSincosV2::cFractalTransfSincosV2() : cAbstractFractal()
{
	nameInComboBox = "T>SinCos V2";
	internalName = "transf_sincos_v2";
	internalID = fractal::transfSincosV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSincosV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledDFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR1)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	double ang = atan2(z.y, z.x) / M_PI_2x;

	if (fractal->transformCommon.functionEnabledAFalse)
	{	double Voff = fractal->transformCommon.scale2;
		double g = z.z - 2.0 * Voff * ang + Voff;
		z.z = g - 2.0 * Voff * floor(g / (2.0 * Voff)) - Voff;
	}

	z.y = sqrt(z.x * z.x + z.y * z.y) - fractal->transformCommon.radius1;
	if (fractal->transformCommon.functionEnabledM)
	{
		z.x = (fractal->transformCommon.scaleA2 * ang + 1.0)
				- 2.0 * floor((fractal->transformCommon.scaleA2 * ang + 1.0) / 2.0) - 1.0;
	}

	ang = fractal->transformCommon.int6 * M_PI_2 * ang;
	double cosA = cos(ang);
	double sinB = sin(ang);
	double temp = z.z;
	z.z = z.y * cosA + z.z * sinB;
	z.y = temp * cosA + z.y * -sinB;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		z = fractal->transformCommon.offset000 - fabs(z);
		//z += fractal->transformCommon.offset000;
	}

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}

