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
	if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);
	z += fractal->transformCommon.offsetA000;

	CVector4 zc = z;

	if (fractal->transformCommon.functionEnabledFalse)
	{
		zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
		zc += fractal->transformCommon.offset000;
		zc *= fractal->transformCommon.scale1;
		aux.DE *= fractal->transformCommon.scale1;

		if (fractal->transformCommon.functionEnabledDFalse)
		{
			zc.x = fabs(zc.x);
			double psi = M_PI / fractal->transformCommon.int8Z;
			psi = fabs(fmod(atan2(zc.x, zc.y) + psi, 2.0 * psi) - psi);
			double len = sqrt(zc.y * zc.y + zc.x * zc.x);
			zc.y = cos(psi) * len;
			zc.x = sin(psi) * len;
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
			- fractal->transformCommon.offset1);

	if (!fractal->analyticDE.enabledFalse)
		aux.dist = min(aux.dist, aux.DE0 / (aux.DE + fractal->analyticDE.offset1));
	else aux.dist = aux.DE0 / (aux.DE + fractal->analyticDE.offset1);


	if (fractal->foldColor.auxColorEnabled)
	{
		double colorAdd = 0.0f;
		if (aux.dist == aux.DE0) colorAdd += fractal->foldColor.difs0000.x;
		if (!fractal->foldColor.auxColorEnabledFalse) aux.color = colorAdd;
		else  aux.color += colorAdd;
	}
	if (fractal->transformCommon.functionEnabledYFalse) z = zc;
}
