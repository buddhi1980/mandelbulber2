/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical fold Cuboid
 * This has a box shape MinR2 condition
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalFoldCuboid::cFractalTransfSphericalFoldCuboid() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Fold Cuboid";
	internalName = "transf_spherical_fold_cuboid";
	internalID = fractal::transfSphericalFoldCuboid;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalFoldCuboid::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 temp3;
	CVector4 R2;
	double minR2 = fractal->transformCommon.minR2p25;
	CVector4 limitMinR2 = fractal->transformCommon.scaleP222;
	double m = fractal->transformCommon.scale;

	double rr = z.Dot(z);
	z += fractal->transformCommon.offset000;

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (fractal->transformCommon.functionEnabledAxFalse)
			temp3 = z * z;
		else
			temp3 = fabs(z);

		if (temp3.x < limitMinR2.x && temp3.y < limitMinR2.y && temp3.z < limitMinR2.z)
		{ // if inside cuboid
			R2.x = limitMinR2.x / temp3.x;
			R2.y = limitMinR2.y / temp3.y;
			R2.z = limitMinR2.z / temp3.z;
			double First = min(R2.x, min(R2.y, R2.z));
			minR2 = rr * First;

			if (fractal->transformCommon.functionEnabled && minR2 > fractal->transformCommon.maxR2d1)
			{ // stop overlapping potential
				minR2 = fractal->transformCommon.maxR2d1;
			}

			m *= fractal->transformCommon.maxR2d1 / minR2;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
				aux.color += fractal->mandelbox.color.factorSp1;
			}
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			m *= fractal->transformCommon.maxR2d1 / rr;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
				aux.color += fractal->mandelbox.color.factorSp2;
			}
		}
	}
	else if (rr < minR2)
	{
		m *= fractal->transformCommon.maxR2d1 / minR2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		m *= fractal->transformCommon.maxR2d1 / rr;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
	z -= fractal->transformCommon.offset000;

	// scale
	z *= m;
	aux.DE = aux.DE * fabs(m) + 1.0;
}
