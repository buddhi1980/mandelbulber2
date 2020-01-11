/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * z vector - axis swap
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfZvectorAxisSwap::cFractalTransfZvectorAxisSwap() : cAbstractFractal()
{
	nameInComboBox = "T>Zvector Axis Swap";
	internalName = "transf_zvector_axis_swap";
	internalID = fractal::transfZvectorAxisSwap;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfZvectorAxisSwap::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	switch (fractal->mandelbulbMulti.orderOfXYZ)
	{
		case multi_OrderOfXYZ_xyz:
		default: z = CVector4(z.x, z.y, z.z, z.w); break;
		case multi_OrderOfXYZ_xzy: z = CVector4(z.x, z.z, z.y, z.w); break;
		case multi_OrderOfXYZ_yxz: z = CVector4(z.y, z.x, z.z, z.w); break;
		case multi_OrderOfXYZ_yzx: z = CVector4(z.y, z.z, z.x, z.w); break;
		case multi_OrderOfXYZ_zxy: z = CVector4(z.z, z.x, z.y, z.w); break;
		case multi_OrderOfXYZ_zyx: z = CVector4(z.z, z.y, z.x, z.w); break;
	}
	if (fractal->transformCommon.functionEnabledxFalse) z.x = -z.x;
	if (fractal->transformCommon.functionEnabledyFalse) z.y = -z.y;
	if (fractal->transformCommon.functionEnabledzFalse) z.z = -z.z;

	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		double xTemp = SQRT_1_2 * (z.x - z.y);
		z.y = SQRT_1_2 * (z.y + z.x);
		z.x = xTemp;
	}
}
