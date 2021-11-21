/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * poly xy fold
 * @reference
 * https://fractalforums.org/fragmentarium/17/polyfoldsym-pre-transform/2684
 */

#include "all_fractal_definitions.h"

cFractalTransfPolyXYFoldV1::cFractalTransfPolyXYFoldV1() : cAbstractFractal()
{
	nameInComboBox = "T>Poly XY Fold V1";
	internalName = "transf_poly_xy_fold_v1";
	internalID = fractal::transfPolyXYFoldV1;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfPolyXYFoldV1::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);
		z += fractal->transformCommon.offset000;
		double psi = M_PI / fractal->transformCommon.int8X;
		psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0f * psi) - psi);
		double len = sqrt(z.y * z.y + z.x * z.x);
		double cz = cos(psi) * len;
		double sz = sin(psi) * len;
		z.x = sz + (cz - sz) * fractal->transformCommon.scaleF1;
		z.y = cz + (sz - cz) * fractal->transformCommon.scaleG1;
	}

	if (fractal->transformCommon.rotation2EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z += fractal->transformCommon.additionConstant000;
	}

	// DE tweaks
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}
