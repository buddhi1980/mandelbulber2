/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * amazing surf Mod4 based on Mandelbulber3D. Formula proposed by Kali, with features added by
 * DarkBeam
 * This formula has a c.x c.y SWAP
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSAmazingIfs::cFractalTransfDIFSAmazingIfs() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Amazing IFS";
	internalName = "transf_difs_amazing_ifs";
	internalID = fractal::transfDIFSAmazingIfs;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSAmazingIfs::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;

	z += fractal->transformCommon.offsetA000;

	CVector4 oldZ = z;
	z.x = fabs(z.x + fractal->transformCommon.additionConstant0555.x)
				- fabs(z.x - fractal->transformCommon.additionConstant0555.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant0555.y)
				- fabs(z.y - fractal->transformCommon.additionConstant0555.y) - z.y;
	if (fractal->transformCommon.functionEnabledJFalse)
		z.z = fabs(z.z + fractal->transformCommon.additionConstant0555.z)
				- fabs(z.z - fractal->transformCommon.additionConstant0555.z) - z.z;
	CVector4 zCol = z;

	double rr = z.Dot(z);
	double rrCol = rr;
	double RR = fractal->transformCommon.minR2p25;

	if (rr < RR)
	{
		RR = 1.0 / RR;
		z *= RR;
		aux.DE *= RR;
	}
	else if (rr < 1.0)
	{
		RR =  1.0 / rr;
		z *= RR;
		aux.DE *= RR;
	}

	z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	double colorDist = aux.dist;

	if (!fractal->analyticDE.enabledFalse) aux.DE0 = z.Length() / aux.DE;
	else aux.DE0 = min(aux.dist, z.Length() / aux.DE);
	aux.dist = aux.DE0;

	// aux.color
	if (fractal->foldColor.auxColorEnabled)
	{
		if (fractal->foldColor.auxColorEnabledFalse)
		{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->foldColor.difs0000.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->foldColor.difs0000.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->foldColor.difs0000.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);
		if (rrCol > fractal->transformCommon.minR2p25)
			colorAdd +=
				fractal->foldColor.difs0000.w * (rrCol - fractal->transformCommon.minR2p25) / 100.0;

			//colorAdd += fractal->foldColor.difs0000.x * fabs(z.x * z.y);
			//colorAdd += fractal->foldColor.difs0000.y * max(z.x, z.y);
		}
		colorAdd += fractal->foldColor.difs1;
		if (fractal->foldColor.auxColorEnabledA)
		{
			if (colorDist != aux.dist) aux.color += colorAdd;
		}
		else
			aux.color += colorAdd;
	}
}
