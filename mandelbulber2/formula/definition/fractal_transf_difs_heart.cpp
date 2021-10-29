/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Isosurface invented and shared by Alex Kluchikov (22 Aug 2003).
 * Based on darkbeams version @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSHeart::cFractalTransfDIFSHeart() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Heart";
	internalName = "transf_difs_heart";
	internalID = fractal::transfDIFSHeart;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSHeart::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);
		z += fractal->transformCommon.offsetA000;
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		z *= fractal->transformCommon.scale1;
		aux.DE *= fractal->transformCommon.scale1;
	}

	CVector4 zc = z;
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		zc.y = fabs(zc.y);
		double psi = M_PI / fractal->transformCommon.int8Z;
		psi = fabs(fmod(atan2(zc.y, zc.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(zc.y * zc.y + zc.x * zc.x);
		if (!fractal->transformCommon.functionEnabledEFalse)
		{
			zc.x = cos(psi) * len;
			zc.y = sin(psi) * len;
		}
		else
		{
			double cz = cos(psi) * len;
			double sz = sin(psi) * len;
			zc.x = sz + (cz - sz) * fractal->transformCommon.scaleF1;
			zc.y = cz + (sz - cz) * fractal->transformCommon.scaleG1;
		}
	}

	if (fractal->transformCommon.rotation2EnabledFalse)
	{
		zc = fractal->transformCommon.rotationMatrix2.RotateVector(zc);
	}

	double p = (zc.z + fractal->transformCommon.offset105);
	p *= p;
	p = zc.x * zc.x + fractal->transformCommon.scale025
			* zc.y * zc.y / (p * p + fractal->transformCommon.offsetp01);
	p = fabs(zc.z * fractal->transformCommon.scale1p1
			- fractal->transformCommon.scale08 * sqrt(sqrt(p)));

	aux.DE0 = (sqrt(fractal->transformCommon.scale4 * zc.y * zc.y + zc.x * zc.x + p * p)
			- fractal->transformCommon.offset1) / (aux.DE + fractal->analyticDE.offset1);

	double col = aux.dist;

	if (!fractal->analyticDE.enabledFalse) aux.dist = min(aux.dist, aux.DE0);
	else aux.dist = aux.DE0;

	if (fractal->foldColor.auxColorEnabled)
	{
		double colorAdd = 0.0;
		if (aux.dist == col) colorAdd += fractal->foldColor.difs0000.x;
		colorAdd += fractal->foldColor.difs0000.y * zc.z;
		colorAdd += fractal->foldColor.difs0000.z * fabs(zc.x);
		colorAdd += fractal->foldColor.difs0000.w * fabs(zc.y);
		if (!fractal->foldColor.auxColorEnabledFalse) aux.color = colorAdd;
		else  aux.color += colorAdd;
	}
	if (fractal->transformCommon.functionEnabledYFalse) z = zc;
}
