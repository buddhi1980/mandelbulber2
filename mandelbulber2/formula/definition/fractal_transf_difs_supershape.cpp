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
	if (aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z += fractal->transformCommon.offsetA000;
	}

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
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

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	double r1;
	double phi;
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
	z.z *= fractal->transformCommon.scaleB1;
	aux.DE *= fabs(fractal->transformCommon.scaleB1); // mmmmm

	if (!fractal->transformCommon.functionEnabledGFalse)
		r1 = fabs(r1 - z.Length() * fractal->transformCommon.scaleA1);
	else
		r1 = fabs(r1 - sqrt(z.x * z.x + z.y * z.y) * fractal->transformCommon.scaleA1);

	z.x = r1 * cos(phi);
	z.y = r1 * sin(phi);

	z *= fractal->transformCommon.scale1; // mmmmmm
	aux.DE *= fabs(fractal->transformCommon.scale1);

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	// DE
	CVector4 zc = z;
	double T1;

	if (aux.i >= fractal->transformCommon.startIterationsD
		&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		zc.z = (zc.z - fractal->transformCommon.offsetD0);
	}

	zc.z = fabs(zc.z) - fractal->transformCommon.offsetF0;
	if (!fractal->transformCommon.functionEnabledMFalse) zc.z = max(zc.z, 0.0);

	double th = (zc.x * zc.x + zc.y * zc.y);
	if (fractal->transformCommon.functionEnabledFalse)
	{
		th = th - fractal->transformCommon.offsetAp01;
		th = max(th, 0.0);
	}

	if (!fractal->transformCommon.functionEnabledIFalse)
	{
		if (!fractal->transformCommon.functionEnabledKFalse)
		{
			T1 = sqrt(th + zc.z * zc.z) - fractal->transformCommon.offsetp05;
			if (fractal->transformCommon.functionEnabledJFalse)
				T1 = T1 + zc.z * fractal->transformCommon.scaleD1;
		}
		else
		{
			T1 = sqrt(th) + (zc.z * fractal->transformCommon.scaleD1) - fractal->transformCommon.offsetp05;
		}
	}
	else
	{
		zc = fabs(zc);
		if (!fractal->transformCommon.functionEnabledKFalse)
		{
			if (!fractal->transformCommon.functionEnabledNFalse)
				T1 = max(th, zc.z) - fractal->transformCommon.offsetp05;
			else
				T1 = max(max(zc.x, zc.y), zc.z) - fractal->transformCommon.offsetp05;

			if (fractal->transformCommon.functionEnabledJFalse)
				T1 = T1 + (zc.z * fractal->transformCommon.scaleD1);
		}
		else
		{
			T1 = max(th, zc.z) + (zc.z * fractal->transformCommon.scaleD1) - fractal->transformCommon.offsetp05;
		}
	}

	double colDist = aux.dist;
	T1 = T1 / (aux.DE + fractal->transformCommon.offset1);
	aux.dist = min(T1, aux.dist);

	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addCol = 0.0;
		if (colDist != aux.dist) addCol += fractal->foldColor.difs0000.x;
		zc = fabs(zc);
		addCol += zc.x * zc.y * fractal->foldColor.difs0000.y;
		addCol += max(zc.x, zc.y) * fractal->foldColor.difs0000.z;
		aux.color += addCol;
	}

	if (fractal->transformCommon.functionEnabledZcFalse) z = zc;
}
