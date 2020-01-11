/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical fold XYZ Bias
 * deforms the MinR2 condition
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalFoldXYZBias::cFractalTransfSphericalFoldXYZBias() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Fold XYZ Bias";
	internalName = "transf_spherical_fold_xyz_bias";
	internalID = fractal::transfSphericalFoldXYZBias;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalFoldXYZBias::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 xyzBias;
	double minR2 = fractal->transformCommon.minR2p25;
	double MaxR2 = fractal->transformCommon.maxR2d1;
	double m = fractal->transformCommon.scale;

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		xyzBias = fabs(aux.c) * fractal->transformCommon.constantMultiplier000;
		minR2 = minR2 + (xyzBias.x + xyzBias.y + xyzBias.z);
	}

	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		CVector4 cSquared = aux.c * aux.c;
		xyzBias = cSquared * fractal->transformCommon.scale3D000;
		minR2 = minR2 + (xyzBias.x + xyzBias.y + xyzBias.z);
	}

	if (fractal->transformCommon.functionEnabled && minR2 > MaxR2)
	{
		minR2 = MaxR2; // stop overlapping potential
	}

	double rr = z.Dot(z);
	z += fractal->transformCommon.offset000;

	if (rr < minR2)
	{
		m *= fractal->transformCommon.maxR2d1 / minR2;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		m *= fractal->transformCommon.maxR2d1 / rr;
		aux.color += fractal->mandelbox.color.factorSp2;
	}
	z -= fractal->transformCommon.offset000;

	z *= m;
	aux.DE = aux.DE * fabs(m) + 1.0;
}
