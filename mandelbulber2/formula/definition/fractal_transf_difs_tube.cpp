/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * greek ifs based on Mandelbulber3D.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSTube::cFractalTransfDIFSTube() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Tube";
	internalName = "transf_difs_tube";
	internalID = fractal::transfDIFSTube;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSTube::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z *= fractal->transformCommon.scale1;
	aux.DE *= fractal->transformCommon.scale1;
	z += fractal->transformCommon.offsetA000;

	CVector4 zc = z;

	// swap axis
	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
				&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double temp = zc.x;
		zc.x = zc.y;
		zc.y = temp;
	}

	// swap axis
	if (fractal->transformCommon.functionEnabledMFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		double temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
				&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		if (fractal->transformCommon.functionEnabledAxFalse)
		{
			if (!fractal->transformCommon.functionEnabledBxFalse)
				zc.x = fractal->transformCommon.offset000.x - fabs(zc.x);
			else
				zc.x = sign(z.x) * (fractal->transformCommon.offset000.x - fabs(zc.x));
		}

		if (fractal->transformCommon.functionEnabledAyFalse)
		{
			if (!fractal->transformCommon.functionEnabledByFalse)
				zc.y = fractal->transformCommon.offset000.y - fabs(zc.y);
			else
				zc.y = sign(z.y) * (fractal->transformCommon.offset000.y - fabs(zc.y));
		}

		if (fractal->transformCommon.functionEnabledAzFalse)
		{
			if (!fractal->transformCommon.functionEnabledBzFalse)
				zc.z = fractal->transformCommon.offset000.z - fabs(zc.z);
			else
				zc.z = sign(z.z) * (fractal->transformCommon.offset000.z - fabs(zc.z));
		}
	}

	double t = zc.x;
	if (fractal->transformCommon.functionEnabledBx)
		t = fabs(t) - fractal->transformCommon.offsetF000.x;
	if (fractal->transformCommon.functionEnabledKFalse)
		zc.z = fabs(zc.z) - fractal->transformCommon.offsetF000.y;

/*		REAL psi = M_PI_F / fractal->transformCommon.int8Z;
		psi = fabs(fmod(atan2(t, zc.z) + psi, 2.0f * psi) - psi);
		REAL len = native_sqrt(zc.z * zc.z + t *t);
		zc.z = native_cos(psi) * len;
		t = native_sin(psi) * len;
*/

	if (fractal->transformCommon.functionEnabledCx)
		t = max(fabs(t), fabs(zc.z));
	if (fractal->transformCommon.functionEnabledDFalse)
		t = sqrt(t * t + zc.z * zc.z);
	if (fractal->transformCommon.functionEnabledEFalse)
		t = max(fabs(t) * SQRT_3_4 + zc.z * 0.5, -zc.z);
	if (fractal->transformCommon.functionEnabledFFalse)
	{
		double zA = zc.z;
		if (fractal->transformCommon.functionEnabledCy)
			zA = fabs(zA) - fractal->transformCommon.offsetB0;

		if (fractal->transformCommon.functionEnabledCyFalse)
			zA = -zA + fractal->transformCommon.offsetB0;

		double zB = zc.z;
		if (fractal->transformCommon.functionEnabledCz)
			zB = fabs(zB) - fractal->transformCommon.offsetC0;

		if (fractal->transformCommon.functionEnabledCzFalse)
			zB = -zB + fractal->transformCommon.offsetC0;

		t = max(fabs(t) * SQRT_3_4 + zA * 0.5, zB);
	}

	t -= fractal->transformCommon.offset02;

	double cylR = t;
	if (!fractal->transformCommon.functionEnabledGFalse)
	{
		cylR = fabs(cylR) - fractal->transformCommon.offsetp01;
		cylR = max(cylR, t);
	}

	double cylH = fabs(zc.y) - fractal->transformCommon.offsetA1;

	cylR = max(cylR, 0.0);
	cylH = max(cylH, 0.0);
	double cylD = sqrt(cylR * cylR + cylH * cylH);
	cylD = min(max(cylR, cylH), 0.0) + cylD;

	double colDist = aux.dist;
	aux.dist = min(aux.dist, cylD / (aux.DE + fractal->analyticDE.offset1));

	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colAdd = fractal->foldColor.difs0000.y;
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (cylD > t) colAdd = fractal->foldColor.difs0000.z;
			if (fractal->transformCommon.offsetA1
					- fractal->foldColor.difs0 < fabs(zc.y))
				colAdd = fractal->foldColor.difs0000.w;
		}
		if (colDist != aux.dist)
			aux.color = colAdd;

		if (fractal->foldColor.auxColorEnabledBFalse)
			aux.color += fractal->foldColor.difs0000.x;
	}
}
