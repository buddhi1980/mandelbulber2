/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * box wrap 4d
 */

#include "all_fractal_definitions.h"

cFractalTransfBoxWrap4d::cFractalTransfBoxWrap4d() : cAbstractFractal()
{
	nameInComboBox = "T>Box Wrap 4D";
	internalName = "transf_box_wrap4d";
	internalID = fractal::transfBoxWrap4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBoxWrap4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 box_size = fractal->transformCommon.offset1111;
	CVector4 wrap_mode = z;
	CVector4 oldZ = z;

	if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);
	if (fractal->transformCommon.functionEnabledwFalse) z.w = fabs(z.w);

	if (fractal->transformCommon.functionEnabledx)
	{
		z.x += box_size.x;
		if (!fractal->transformCommon.functionEnabledFalse)
			wrap_mode.x = fmod(z.x, 2.0 * box_size.x);
		else
			wrap_mode.x = z.x - 2.0 * box_size.x * floor(z.x / 2.0 * box_size.x);
		z.x = wrap_mode.x - box_size.x;
	}
	if (fractal->transformCommon.functionEnabledByFalse)
	{
		z.y += box_size.y;
		if (!fractal->transformCommon.functionEnabledFalse)
			wrap_mode.y = fmod(z.y, 2.0 * box_size.y);
		else
			wrap_mode.y = z.y - 2.0 * box_size.y * floor(z.y / 2.0 * box_size.y);
		z.y = wrap_mode.y - box_size.y;
	}
	if (fractal->transformCommon.functionEnabledBzFalse)
	{
		z.z += box_size.z;
		if (!fractal->transformCommon.functionEnabledFalse)
			wrap_mode.z = fmod(z.z, 2.0 * box_size.z);
		else
			wrap_mode.z = z.z - 2.0 * box_size.z * floor(z.z / 2.0 * box_size.z);
		z.z = wrap_mode.z - box_size.z;
	}

	if (fractal->transformCommon.functionEnabledCxFalse)
	{
		z.w += box_size.w;
		if (!fractal->transformCommon.functionEnabledFalse)
			wrap_mode.w = fmod(z.w, 2.0 * box_size.w);
		else
			wrap_mode.w = z.w - 2.0 * box_size.w * floor(z.w / 2.0 * box_size.w);
		z.w = wrap_mode.w - box_size.w;
	}
	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		z.x = z.x * fractal->transformCommon.scale1 / (fabs(oldZ.x) + 1.0);
		z.y = z.y * fractal->transformCommon.scale1 / (fabs(oldZ.y) + 1.0);
		z.z = z.z * fractal->transformCommon.scale1 / (fabs(oldZ.z) + 1.0);
		z.w = z.w * fractal->transformCommon.scale1 / (fabs(oldZ.w) + 1.0);
	}
	if (fractal->transformCommon.functionEnabledAxFalse) z.x *= sign(oldZ.x);
	if (fractal->transformCommon.functionEnabledAyFalse) z.y *= sign(oldZ.y);
	if (fractal->transformCommon.functionEnabledAzFalse) z.z *= sign(oldZ.z);
	if (fractal->transformCommon.functionEnabledAwFalse) z.w *= sign(oldZ.w);

	aux.DE *= fractal->analyticDE.scale1;
}
