/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * poly fold atan2
 * @reference
 * https://fractalforums.org/fragmentarium/17/polyfoldsym-pre-transform/2684
 */

#include "all_fractal_definitions.h"

cFractalTransfPolyFoldAtan2Iter::cFractalTransfPolyFoldAtan2Iter() : cAbstractFractal()
{
	nameInComboBox = "T>Poly Fold Atan2 Iter";
	internalName = "transf_poly_fold_atan2_iter";
	internalID = fractal::transfPolyFoldAtan2Iter;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfPolyFoldAtan2Iter::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	// pre abs
	if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);

	if (fractal->transformCommon.functionEnabledCx
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (fractal->transformCommon.functionEnabledAxFalse && z.y < 0.0) z.x = -z.x;
		double psi = M_PI / fractal->transformCommon.int8X;
		psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledCyFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		if (fractal->transformCommon.functionEnabledAyFalse && z.z < 0.0) z.y = -z.y;
		double psi = M_PI / fractal->transformCommon.int8Y;
		psi = fabs(fmod(atan2(z.z, z.y) + psi, 2.0 * psi) - psi);
		double len = sqrt(z.y * z.y + z.z * z.z);
		z.y = cos(psi) * len;
		z.z = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledCzFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		if (fractal->transformCommon.functionEnabledAzFalse && z.x < 0.0) z.z = -z.z;
		double psi = M_PI / fractal->transformCommon.int8Z;
		psi = fabs(fmod(atan2(z.x, z.z) + psi, 2.0 * psi) - psi);
		double len = sqrt(z.z * z.z + z.x * z.x);
		z.z = cos(psi) * len;
		z.x = sin(psi) * len;
	}

	// addition constant
	z += fractal->transformCommon.additionConstant000;

	// DE tweaks
	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}
