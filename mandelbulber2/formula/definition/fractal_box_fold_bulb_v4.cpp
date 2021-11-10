/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2019 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Classic Mandelbulb fractal.
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */

#include "all_fractal_definitions.h"

cFractalBoxFoldBulbV4::cFractalBoxFoldBulbV4() : cAbstractFractal()
{
	nameInComboBox = "Box Fold Bulb V4";
	internalName = "box_fold_bulb_v4";
	internalID = fractal::boxFoldBulbV4;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalBoxFoldBulbV4::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	z.x = fabs(z.x + fractal->transformCommon.offset222.x)
				- fabs(z.x - fractal->transformCommon.offset222.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.offset222.y)
				- fabs(z.y - fractal->transformCommon.offset222.y) - z.y;
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
		z.z = fabs(z.z + fractal->transformCommon.offset222.z)
				- fabs(z.z - fractal->transformCommon.offset222.z) - z.z;
	CVector4 zCol = z;
	z *= fractal->transformCommon.scaleA1;
	aux.DE *= fabs(fractal->transformCommon.scaleA1);
	aux.r = z.Length();
	// if (aux.r < 1e-21) aux.r = 1e-21;
	const double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	const double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, fractal->transformCommon.scale2 - 1.0);
	const double th = th0 * fractal->transformCommon.scale2;
	const double ph = ph0 * fractal->transformCommon.scale2;
	const double cth = cos(th);
	aux.DE = (rp * aux.DE) * fractal->transformCommon.scale2;
	rp *= aux.r;
	z.x = cth * cos(ph) * rp;
	z.y = cth * sin(ph) * rp;
	z.z = sin(th) * rp;

	z.z *= fractal->transformCommon.scaleNeg1;

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		double colorAdd = 0.0;
		if (zCol.x != oldZ.x)
			aux.color += fractal->foldColor.difs0000.x
					* (fabs(zCol.x) / fractal->transformCommon.offset222.x - 1.0f);

		if (zCol.y != oldZ.y)
			aux.color += fractal->foldColor.difs0000.y
					* (fabs(zCol.y) / fractal->transformCommon.offset222.y - 1.0f);

		if (zCol.z != oldZ.z)
			aux.color += fractal->foldColor.difs0000.z
					* (fabs(zCol.z) / fractal->transformCommon.offset222.z - 1.0f);

		colorAdd += fractal->foldColor.difs0000.w * rp * 0.001;
		aux.color += colorAdd;
	}
}
