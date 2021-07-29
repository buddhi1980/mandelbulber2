/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * amazing ifs based on Mandelbulber3D.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalDIFSAmazingIfs::cFractalDIFSAmazingIfs() : cAbstractFractal()
{
	nameInComboBox = "DIFS Amazing IFS";
	internalName = "difs_amazing_ifs";
    internalID = fractal::dIFSAmazingIfs;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalDIFSAmazingIfs::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	z += fractal->transformCommon.offsetA000;

	// polyfold
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		double psi = M_PI / fractal->transformCommon.int6;
		psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}

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

	// scale
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z *= fractal->transformCommon.scale2;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale2) + fractal->analyticDE.offset0;
	}

	// offset
	z += fractal->transformCommon.offset001;

	z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	// DE
	double colorDist = aux.dist; // for color
	aux.DE0 = z.Length() / aux.DE;
	if (!fractal->analyticDE.enabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
				aux.DE0 = min(aux.dist, aux.DE0);
	aux.dist = aux.DE0;

	// aux.color
	if (fractal->foldColor.auxColorEnabled)
	{
		double colorAdd = 0.0;
		if (fractal->foldColor.auxColorEnabledA)
			if (colorDist != aux.dist) colorAdd += fractal->foldColor.difs1;

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
		}
		aux.color += colorAdd;
	}
}
