/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Reciprocal3  based on DarkBeam's code from M3D,
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfReciprocal3::cFractalTransfReciprocal3() : cAbstractFractal()
{
	nameInComboBox = "T>Reciprocal3";
	internalName = "transf_reciprocal3";
	internalID = fractal::transfReciprocal3;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfReciprocal3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 tempZ = z;

	if (fractal->transformCommon.functionEnabledx)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.x = (1.0 / fractal->transformCommon.offset111.x)
								- 1.0 / (fabs(z.x) + fractal->transformCommon.offset111.x);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.x = (fractal->transformCommon.offsetA111.x)
								- 1.0 / (fabs(z.x) + fractal->transformCommon.offset111.x);

		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.x = (1.0 / fractal->transformCommon.offset111.x)
								+ (1.0 / fractal->transformCommon.offsetA111.x)
								- 1.0 / (fabs(z.x * M1) + fractal->transformCommon.offset111.x)
								- 1.0 / ((z.x * z.x * M2) + fractal->transformCommon.offsetA111.x);
		}
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.x = fractal->transformCommon.offsetB111.x
								- 1.0 / (fabs(z.x * M1) + fractal->transformCommon.offset111.x)
								- 1.0 / ((z.x * z.x * M2) + fractal->transformCommon.offsetA111.x);
		}

		tempZ.x += fabs(z.x) * fractal->transformCommon.offset000.x; // function slope
		z.x = sign(z.x) * tempZ.x;
	}

	if (fractal->transformCommon.functionEnabledy)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.y = (1.0 / fractal->transformCommon.offset111.y)
								- 1.0 / (fabs(z.y) + fractal->transformCommon.offset111.y);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.y = (fractal->transformCommon.offsetA111.y)
								- 1.0 / (fabs(z.y) + fractal->transformCommon.offset111.y);

		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.y = (1.0 / fractal->transformCommon.offset111.y)
								+ (1.0 / fractal->transformCommon.offsetA111.y)
								- 1.0 / (fabs(z.y * M1) + fractal->transformCommon.offset111.y)
								- 1.0 / ((z.y * z.y * M2) + fractal->transformCommon.offsetA111.y);
		}

		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.y = fractal->transformCommon.offsetB111.y
								- 1.0 / (fabs(z.y * M1) + fractal->transformCommon.offset111.y)
								- 1.0 / ((z.y * z.y * M2) + fractal->transformCommon.offsetA111.y);
		}
		tempZ.y += fabs(z.y) * fractal->transformCommon.offset000.y;
		z.y = sign(z.y) * tempZ.y;
	}

	if (fractal->transformCommon.functionEnabledz)
	{
		if (fractal->transformCommon.functionEnabledAx)
			tempZ.z = (1.0 / fractal->transformCommon.offset111.z)
								- 1.0 / (fabs(z.z) + fractal->transformCommon.offset111.z);

		if (fractal->transformCommon.functionEnabledAxFalse)
			tempZ.z = (fractal->transformCommon.offsetA111.z)
								- 1.0 / (fabs(z.z) + fractal->transformCommon.offset111.z);

		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.z = (1.0 / fractal->transformCommon.offset111.z)
								+ (1.0 / fractal->transformCommon.offsetA111.z)
								- 1.0 / (fabs(z.z * M1) + fractal->transformCommon.offset111.z)
								- 1.0 / ((z.z * z.z * M2) + fractal->transformCommon.offsetA111.z);
		}
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			double M1 = fractal->transformCommon.scale1;
			double M2 = fractal->transformCommon.scaleA1;
			tempZ.z = fractal->transformCommon.offsetB111.z
								- 1.0 / (fabs(z.z * M1) + fractal->transformCommon.offset111.z)
								- 1.0 / ((z.z * z.z * M2) + fractal->transformCommon.offsetA111.z);
		}

		tempZ.z += fabs(z.z) * fractal->transformCommon.offset000.z;
		z.z = sign(z.z) * tempZ.z;
	}
	aux.DE *= fractal->analyticDE.scale1; // DE tweak
}
