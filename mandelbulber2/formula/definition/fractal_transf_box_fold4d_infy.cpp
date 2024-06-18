/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Box Fold 4d Inf (_FoldingInfy)
 * Continuous, seamless folding of 4D space. Boxtiling "enhanced" version.
 * from M3D. Baded on formula by Luca GN 2011,
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfBoxFold4dInfy::cFractalTransfBoxFold4dInfy() : cAbstractFractal()
{
	nameInComboBox = "T>Box Fold 4D Infy";
	internalName = "transf_box_fold4d_infy";
	internalID = fractal::transfBoxFold4dInfy;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBoxFold4dInfy::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 size = fractal->transformCommon.offset2222;
	CVector4 m = size * 4.0;

	if (!fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledx && size.x != 0.0)
		{
			z.x -= round(z.x / m.x) * m.x;
			z.x = fabs(z.x + size.x) - fabs(z.x - size.x) - z.x;
		}
		if (fractal->transformCommon.functionEnabledyFalse && size.y != 0.0)
		{
			z.y -= round(z.y / m.y) * m.y;
			z.y = fabs(z.y + size.y) - fabs(z.y - size.y) - z.y;
		}
		if (fractal->transformCommon.functionEnabledzFalse && size.z != 0.0)
		{
			z.z -= round(z.z / m.z) * m.z;
			z.z = fabs(z.z + size.z) - fabs(z.z - size.z) - z.z;
		}
		if (fractal->transformCommon.functionEnabledwFalse && size.w != 0.0)
		{
			z.w -= round(z.w / m.w) * m.w;
			z.w = fabs(z.w + size.w) - fabs(z.w - size.w) - z.w;
		}
	}
	else
	{/*
		CVector4 repeatPos = fractal->transformCommon.offsetA1111 * 2.0;
		CVector4 repeatNeg = fractal->transformCommon.offsetB1111 * 2.0;

		if (fractal->transformCommon.functionEnabledx && z.x < (repeatPos.x + 0.5f) * size.x
				&& z.x > (repeatNeg.x + 0.5f) * -size.x && size.x != 0.0f)
		{
			z.x -= round(z.x / m.x) * m.x;
			z.x = fabs(z.x + size.x) - fabs(z.x - size.x) - z.x;
		}
		if (fractal->transformCommon.functionEnabledyFalse && z.y < (repeatPos.y + 0.5) * size.y
				&& z.y > (repeatNeg.y + 0.5) * -size.y && size.y != 0.0)
		{
			z.y -= round(z.y / m.y) * m.y;
			z.y = fabs(z.y + size.y) - fabs(z.y - size.y) - z.y;
		}
		if (fractal->transformCommon.functionEnabledzFalse && z.z < (repeatPos.z + 0.5) * size.z
				&& z.z > (repeatNeg.z + 0.5) * -size.z && size.z != 0.0)
		{
			z.z -= round(z.z / size.z) * size.z;
		}
		if (fractal->transformCommon.functionEnabledwFalse && z.w < (repeatPos.w + 0.5) * size.w
				&& z.w > (repeatNeg.w + 0.5) * -size.w && size.w != 0.0)
		{
			z.w -= round(z.w / size.w) * size.w;
		}**/
	}

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			aux.DE = aux.DE * z.Length() * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
		aux.const_c = z;
}
