/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * box fold 4D Tglad
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfBoxFold4dTglad::cFractalTransfBoxFold4dTglad() : cAbstractFractal()
{
	nameInComboBox = "T>Box Fold 4D Tglad";
	internalName = "transf_box_fold4d_tglad";
	internalID = fractal::transfBoxFold4dTglad;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBoxFold4dTglad::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 limit = fractal->transformCommon.offset1111;

	if (!fractal->transformCommon.functionEnabledCyFalse)
	{
		z.x = fabs(z.x + limit.x) - fabs(z.x - limit.x) - z.x;
		z.y = fabs(z.y + limit.y) - fabs(z.y - limit.y) - z.y;
		z.z = fabs(z.z + limit.z) - fabs(z.z - limit.z) - z.z;
		z.w = fabs(z.w + limit.w) - fabs(z.w - limit.w) - z.w;
	}
	else
	{
		if (fractal->transformCommon.functionEnabledAx)
		{
			if (aux.i > fractal->transformCommon.startIterationsA)
			{
				limit.x *= (1.0
										 - 1.0
												 / (1.0
														+ (aux.i - fractal->transformCommon.startIterationsA)
																/ fractal->transformCommon.offset0000.x))
									 * fractal->transformCommon.scale1111.x;
			}
			z.x = fabs(z.x + limit.x) - fabs(z.x - limit.x) - z.x;
		}
		if (fractal->transformCommon.functionEnabledAy)
		{
			if (aux.i > fractal->transformCommon.startIterationsB)
			{
				limit.y *= (1.0
										 - 1.0
												 / (1.0
														+ (aux.i - fractal->transformCommon.startIterationsB)
																/ fractal->transformCommon.offset0000.y))
									 * fractal->transformCommon.scale1111.y;
			}
			z.y = fabs(z.y + limit.y) - fabs(z.y - limit.y) - z.y;
		}
		if (fractal->transformCommon.functionEnabledAz)
		{
			if (aux.i > fractal->transformCommon.startIterationsB)
			{
				limit.z *= (1.0
										 - 1.0
												 / (1.0
														+ (aux.i - fractal->transformCommon.startIterationsC)
																/ fractal->transformCommon.offset0000.z))
									 * fractal->transformCommon.scale1111.z;
			}
			z.z = fabs(z.z + limit.z) - fabs(z.z - limit.z) - z.z;
		}
		if (fractal->transformCommon.functionEnabledAw)
		{
			if (aux.i > fractal->transformCommon.startIterationsB)
			{
				limit.w *= (1.0
										 - 1.0
												 / (1.0
														+ (aux.i - fractal->transformCommon.startIterationsD)
																/ fractal->transformCommon.offset0000.w))
									 * fractal->transformCommon.scale1111.w;
			}
			z.w = fabs(z.w + limit.w) - fabs(z.w - limit.w) - z.w;
		}
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (!fractal->transformCommon.functionEnabledCxFalse)
		{
			if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x;
			if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y;
			if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z;
			if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w;
		}
		else
		{
			if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x * (fabs(z.x) - limit.x);
			if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y * (fabs(z.y) - limit.y);
			if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z * (fabs(z.z) - limit.z);
			if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w * (fabs(z.w) - limit.w);
		}
	}
}
