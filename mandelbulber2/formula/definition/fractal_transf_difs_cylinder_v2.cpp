/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsCylinderV2Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSCylinderV2::cFractalTransfDIFSCylinderV2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Cylinder V2";
	internalName = "transf_difs_cylinder_v2";
	internalID = fractal::transfDIFSCylinderV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSCylinderV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}
	z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR1)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	CVector4 zc = z;

	double temp;
	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}

	double cylR = zc.x * zc.x;
	double absH = fabs(zc.z);
	double lengthCyl = zc.z;
	cylR = sqrt(cylR + zc.y * zc.y);

	double cylH = absH - fractal->transformCommon.offsetA1
			+ fractal->transformCommon.offsetB0;

	// no absz
	if (fractal->transformCommon.functionEnabledMFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		absH = lengthCyl;
	}

	// abs sqrd
	if (fractal->transformCommon.functionEnabledTFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		absH *= absH;
	}

	double t = cylR - fractal->transformCommon.radius1
			+ fractal->transformCommon.offsetB0;
	double cylRm = t;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		cylRm = fabs(cylRm) - fractal->transformCommon.offset0;
		if (fractal->transformCommon.functionEnabledBFalse)
			cylRm = max(cylRm, t);
	}

	cylRm += fractal->transformCommon.scale0 * absH;
	zc.z = absH;

	// tops
	if (fractal->transformCommon.functionEnabledNFalse
			&& aux.i >= fractal->transformCommon.startIterationsN
			&& aux.i < fractal->transformCommon.stopIterationsN)
	{
		temp = cylR;
	}
	else
	{
		temp = cylRm;
	}
	temp = max(temp, 0.0);
	double cylHm = max(cylH, 0.0);
	double cylD = sqrt(temp * temp + cylHm * cylHm);

	// rings
	if (fractal->transformCommon.functionEnabledOFalse
			&& aux.i >= fractal->transformCommon.startIterationsO
			&& aux.i < fractal->transformCommon.stopIterationsO)
	{
		cylD = sqrt(cylRm * cylRm + cylH * cylH);
	}
	cylD = min(max(cylRm, cylH) - fractal->transformCommon.offsetR0, 0.0) + cylD;
	double colDist = aux.dist;
	aux.dist = min(aux.dist, cylD /(aux.DE + fractal->analyticDE.offset0)
				- fractal->transformCommon.offsetB0);

	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colAdd = fractal->foldColor.difs0000.y;
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (t < -fractal->transformCommon.offset0
					- fractal->transformCommon.offsetB0)
				colAdd = fractal->foldColor.difs0000.z;
			if (fractal->transformCommon.offsetA1
					+ fractal->transformCommon.offsetB0
					- fractal->foldColor.difs0 < fabs(zc.z))
				colAdd = fractal->foldColor.difs0000.w;
		}
		if (colDist != aux.dist)
			aux.color = colAdd + fractal->foldColor.difs0000.x;
	}

	if (fractal->transformCommon.functionEnabledZcFalse
			&& aux.i >= fractal->transformCommon.startIterationsZc
			&& aux.i < fractal->transformCommon.stopIterationsZc)
				z = zc;
}
