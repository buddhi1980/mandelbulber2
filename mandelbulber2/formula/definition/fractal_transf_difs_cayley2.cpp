/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * transfCayley2V1 based on Cayley2IFS
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSCayley2::cFractalTransfDIFSCayley2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS_Cayley2";
	internalName = "transf_difs_cayley2";
	internalID = fractal::transfDIFSCayley2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSCayley2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp;
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		temp = SQRT_1_2 * (z.x - z.y);
		z.y = SQRT_1_2 * (z.y + z.x);
		z.x = temp;
	}

	if (fractal->transformCommon.functionEnabledM)
	{
		if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}

	z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR1)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	z *= fractal->transformCommon.scale1;
	aux.DE *= fractal->transformCommon.scale1;

	CVector4 zc = z;
	double mx = zc.x * zc.x;
	double my = zc.y * zc.y;
	double m = fractal->transformCommon.scaleA2 * mx * my + mx * mx + my * my;
	double n = m + fractal->transformCommon.scale4 * zc.x * zc.y + 1.0;
	zc.y = 2.0 * (my - mx) / n;

	if (!fractal->transformCommon.functionEnabledBFalse)
	{
		zc.x = fractal->transformCommon.scale2 * (m - 1.0) / n;
	}
	else
	{
		zc.x = fractal->transformCommon.scale2 * (m - 1.0);
	}

	zc.z *= fractal->transformCommon.scaleA1;

	CVector4 zdv = fabs(zc) - fractal->transformCommon.additionConstant000;
	double zd = min(min(zdv.x, zdv.y), zdv.z);

	zc = fabs(zc) - fractal->transformCommon.offset110;

	CVector4 zcv = zc;
	zcv.x = max(zcv.x, 0.0);
	zcv.y = max(zcv.y, 0.0);
	zcv.z = max(zcv.z, 0.0);
	double zcd = zcv.Length();

	double dxy = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.radius1;
	double dz = fabs(zc.z) - fractal->transformCommon.offset01;
	double bxy = max(dxy - fractal->transformCommon.offsetA000.x,
					-fractal->transformCommon.offsetA000.y);
	double bz = max(dz - fractal->transformCommon.offsetA000.z, 0.0);
	double mm = max(dxy, dz);
	double ll = sqrt(bxy * bxy + bz * bz);
	double zcf = min(mm, 0.0) + ll;

	if (fractal->transformCommon.functionEnabledEFalse)
		zcd = max(zd, zcd) - fractal->transformCommon.offsetC0;
	if (fractal->transformCommon.functionEnabledFFalse)
		zcf = max(zd, zcf) - fractal->transformCommon.offsetD0;

	zcd = zcd + (zcf - zcd) * fractal->transformCommon.scaleD1;

	zcd -= fractal->transformCommon.offsetA0;

	double colorDist = aux.dist;

	aux.dist = min(aux.dist, zcd / (aux.DE + fractal->analyticDE.offset1));

	if (fractal->transformCommon.functionEnabledTFalse)
	{
		CVector4 c = aux.const_c;
		double dst = 1.0;

		if (!fractal->transformCommon.functionEnabledSFalse)
		{
			dst = c.Length() - fractal->transformCommon.offset4; // sphere
		}
		else
		{
			dst = max(fabs(c.x) - fractal->transformCommon.scale3D444.x,
					fabs(c.y) - fractal->transformCommon.scale3D444.y); // sqr
		}

		//dst = clamp(dst, 0.0, 100.0);

		dst = max(fabs(c.z) - fractal->transformCommon.scale3D444.z, dst);

		aux.dist = max(aux.dist, dst);
	}

	if (fractal->transformCommon.functionEnabledZcFalse
			&& aux.i >= fractal->transformCommon.startIterationsZc
			&& aux.i < fractal->transformCommon.stopIterationsZc)
		z = zc;

	// aux.color
	if (fractal->foldColor.auxColorEnabledAFalse
			&& aux.i >= fractal->foldColor.startIterationsA
					&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colorAdd = 0.0;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			colorAdd += fractal->foldColor.difs0000.x * fabs(z.x * z.y);
			colorAdd += fractal->foldColor.difs0000.y * max(z.x, z.y);
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
