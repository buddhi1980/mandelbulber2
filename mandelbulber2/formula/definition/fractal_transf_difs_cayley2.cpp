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
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSCayley2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		double xTemp = SQRT_1_2 * (z.x - z.y);
		z.y = SQRT_1_2 * (z.y + z.x);
		z.x = xTemp;
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

	double zcd;
	CVector4 boxSize = fractal->transformCommon.additionConstant111;
	CVector4 zcv = fabs(zc) - boxSize;
	zcv.x = max(zcv.x, 0.0);
	zcv.y = max(zcv.y, 0.0);
	zcv.z = max(zcv.z, 0.0);
	zcd = zcv.Length();


	zc = fabs(zc) - boxSize;
	double dx = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.radius1;
	double dy = fabs(zc.z) - fractal->transformCommon.offset01;

	double bx = max(dx, 0.0);
	double by = max(dy, 0.0);
	double ll = sqrt(bx * bx + by * by);
	double mm = max(dx, dy);
	double zcf = min(mm, 0.0) + ll;


	//zcd = mix(zcd, zcf, fractal->transformCommon.scaleD1);
	zcd = zcd + (zcf - zcd) * fractal->transformCommon.scaleD1;

	zcd -= fractal->transformCommon.offsetA0;
	aux.dist = min(aux.dist, zcd / (aux.DE + fractal->analyticDE.offset1));

	if (fractal->transformCommon.functionEnabledZcFalse
			&& aux.i >= fractal->transformCommon.startIterationsZc
			&& aux.i < fractal->transformCommon.stopIterationsZc)
		z = zc;
}
