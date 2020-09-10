/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical fold 4D
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalFold4dV2::cFractalTransfSphericalFold4dV2() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Fold 4D V2";
	internalName = "transf_spherical_fold4d_v2";
	internalID = fractal::transfSphericalFold4dV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalFold4dV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double rrCol = 0.0;
	double rr = 0.0;
	z += fractal->transformCommon.offset0000;
	if (fractal->transformCommon.functionEnabledBxFalse)
	{
		rr = z.x * z.x + z.y * z.y;
	}
	if (fractal->transformCommon.functionEnabledByFalse)
	{
		rr = z.x * z.x + z.y * z.y + z.z * z.z;
	}
	if (fractal->transformCommon.functionEnabledBz)
	{
		rr = z.x * z.x + z.y * z.y + z.z * z.z + z.w * z.w;
	}

	rrCol = rr;
	if (rr < fractal->transformCommon.minR2p25)
	{
		z *= fractal->transformCommon.maxMinR2factor;
		aux.DE *= fractal->transformCommon.maxMinR2factor;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
	}
	z -= fractal->transformCommon.offset0000;

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (rrCol < fractal->transformCommon.maxR2d1)
		{
			if (rrCol < fractal->transformCommon.minR2p25)
				aux.color +=
					fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
					+ fractal->mandelbox.color.factorSp2
							* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
			else
				aux.color +=
					fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
		}
	}
}
