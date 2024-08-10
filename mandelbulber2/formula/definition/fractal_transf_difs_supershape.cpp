/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * superformula Johan Gielis
 * https://bsapubs.onlinelibrary.wiley.com/doi/10.3732/ajb.90.3.333
 * http://paulbourke.net/geometry/supershape/
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSSupershape::cFractalTransfDIFSSupershape() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Supershape";
	internalName = "transf_difs_supershape";
	internalID = fractal::transfDIFSSupershape;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSSupershape::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsP
				&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			z.x = sign(z.x)
						* (fractal->transformCommon.offset000.x - fabs(z.x));
		}
		if (fractal->transformCommon.functionEnabledByFalse)
		{
			z.y = sign(z.y)
						* (fractal->transformCommon.offset000.y - fabs(z.y));
		}
		if (fractal->transformCommon.functionEnabledBzFalse)
		{
			z.z = sign(z.z)
						* (fractal->transformCommon.offset000.z - fabs(z.z));
		}
	}
	z *= fractal->transformCommon.scale1;
	aux.DE *= fabs(fractal->transformCommon.scale1);
	double r1 = sqrt(z.x * z.x + z.y * z.y);
	//double r2 = sqrt(z.x * z.x + z.y * z.y); // <<<<<<<<<<<<<<<<<<<<
	double phi;
	double tho = asin(z.z / r1);
	if (!fractal->transformCommon.functionEnabledAFalse)
		phi = atan2(z.x, z.y);
	else
		phi = atan2(z.y, z.x);

	double t1 = fabs(cos(fractal->transformCommon.constantMultiplierA111.x * phi)
					 * fractal->transformCommon.constantMultiplierA111.y);
	if (fractal->transformCommon.functionEnabledXFalse)
				 t1 = pow(t1, fractal->transformCommon.constantMultiplierB111.x);

	double t2 = fabs(sin(fractal->transformCommon.constantMultiplierA111.x * phi)
					 * fractal->transformCommon.constantMultiplierA111.z);
	if (fractal->transformCommon.functionEnabledYFalse)
				 t2 = pow(t2, fractal->transformCommon.constantMultiplierB111.y);

	if (!fractal->transformCommon.functionEnabledEFalse) r1 = (t1 + t2);
	else r1 = pow(t1 + t2, fractal->transformCommon.constantMultiplierB111.z);

	if (!fractal->transformCommon.functionEnabledFFalse)
		r1 = 1.0f / r1;

	r1 = r1 * fractal->transformCommon.radius1;

	if (!fractal->transformCommon.functionEnabledGFalse)
		r1 = fabs(r1 - aux.r * fractal->transformCommon.scaleA1);
	else
		r1 = fabs(r1 - sqrt(z.x * z.x + z.y * z.y) * fractal->transformCommon.scaleA1);


	//if (fractal->transformCommon.functionEnabledBFalse)
	//	aux.DE0 = r;

	z.x = r1 * cos(phi);
	z.y = r1 * sin(phi);

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		double cth = cos(tho);

			z.x = cth * cos(phi);
			z.y = cth * sin(phi);
			z.z = sin(tho);
			z *= r1;
	}

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;


	// Custom DE
	CVector4 zc = z;
	zc = fractal->transformCommon.rotationMatrix2.RotateVector(zc);

	if (aux.i >= fractal->transformCommon.startIterationsD
		&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		zc = (zc - fractal->transformCommon.offsetA000);
	}


	double T1;
	zc *= fractal->transformCommon.scaleB1;
	aux.DE *= fabs(fractal->transformCommon.scaleB1);
	zc.z = fabs((zc.z + fractal->transformCommon.offsetD0) * fractal->transformCommon.scaleD1)
				 - fractal->transformCommon.offsetF0;

	if (!fractal->transformCommon.functionEnabledIFalse)
	{
		if (!fractal->transformCommon.functionEnabledJFalse)
			T1 =zc.Length() - (fractal->transformCommon.offset0);
		else
			T1 = sqrt(zc.x * zc.x + zc.y * zc.y) - (fractal->transformCommon.offset0 - fabs(zc.z));
	}
	else
	{
		zc = fabs(zc);
		if (!fractal->transformCommon.functionEnabledJFalse)
			T1 = max(max(zc.x, zc.y), zc.z) - (fractal->transformCommon.offset0);
		else
			T1 = (max(zc.x, zc.y)) - (fractal->transformCommon.offset0 - fabs(zc.z));
	}
	double colDist = aux.dist;
	T1 = T1 / (aux.DE + fractal->transformCommon.offset1);
	aux.dist = min(T1, aux.dist);

	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		if (colDist != aux.dist) aux.color += fractal->foldColor.difs0000.x;
	}

	if (fractal->transformCommon.functionEnabledZcFalse) z = zc;

}
