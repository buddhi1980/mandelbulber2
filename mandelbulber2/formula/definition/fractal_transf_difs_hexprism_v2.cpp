/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSHexprismV2::cFractalTransfDIFSHexprismV2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Hexprism V2";
	internalName = "transf_difs_hexprism_v2";
	internalID = fractal::transfDIFSHexprismV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSHexprismV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}
	z += fractal->transformCommon.offset000;

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
	double lenX = fractal->transformCommon.offset1;

	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR1)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	CVector4 zc = fabs(z);
	CVector4 k = CVector4(-SQRT_3_4, 0.5, SQRT_1_3, 0.0);
	double tp = 2.0 * min(k.x * zc.x + k.y * zc.y, 0.0);
	zc.x -= tp * k.x;
	zc.y -= tp * k.y;

	double dx = zc.x - clamp(zc.x, -k.z * lenX, k.z * lenX) + fractal->transformCommon.offsetB0;
	double dy = zc.y - lenX + fractal->transformCommon.offsetB0;

	tp = sqrt(dx * dx + dy * dy);
	dx = tp * sign(dy);

	dy =  zc.z - fractal->transformCommon.offsetA1;

	k.x = 0.0;
	k.y = 0.0;
	if (fractal->transformCommon.functionEnabledCFalse)
	{
		if (fractal->transformCommon.functionEnabledMFalse
				&& aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
					zc.z = z.z;

		// abs sqrd
		if (fractal->transformCommon.functionEnabledTFalse
				&& aux.i >= fractal->transformCommon.startIterationsT
				&& aux.i < fractal->transformCommon.stopIterationsT)
					zc.z *= zc.z;

		if (aux.i >= fractal->transformCommon.startIterationsJ
				&& aux.i < fractal->transformCommon.stopIterationsJ)
					dx += fractal->transformCommon.scaleA1 * zc.z;

		if (aux.i >= fractal->transformCommon.startIterationsN
				&& aux.i < fractal->transformCommon.stopIterationsN)
					k.x = fractal->transformCommon.offsetR0;

		if (aux.i >= fractal->transformCommon.startIterationsO
				&& aux.i < fractal->transformCommon.stopIterationsO)
					k.y = fractal->transformCommon.offsetF0;
	}
	tp = dx;
	double colIn = 0.0;
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		colIn = dx + fractal->transformCommon.offset0;
		dx = fabs(dx) - fractal->transformCommon.offset0;

		if (fractal->transformCommon.functionEnabledBFalse)
			dx = max(dx, tp);
	}

	double maxdx = max(dx, k.x);
	double maxdy = max(dy, k.y);

	tp = sqrt(maxdx * maxdx + maxdy * maxdy);
	aux.DE0 = min(max(dx, dy), 0.0) + tp;
	double colDist = aux.dist;
	aux.dist = min(aux.dist, aux.DE0 /  (aux.DE + fractal->analyticDE.offset0)
				- fractal->transformCommon.offsetB0);


	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colAdd = fractal->foldColor.difs0000.y;
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
	//		if (fractal->transformCommon.offsetA1 < zc.z) colAdd += fractal->foldColor.difs0000.y;
			if (colIn < maxdx) colAdd = fractal->foldColor.difs0000.z;

			if (fractal->transformCommon.offsetA1 - fractal->foldColor.difs0 < zc.z && dx + fractal->transformCommon.offset0 > maxdx)
				colAdd = fractal->foldColor.difs0000.w;


			//if (fractal->transformCommon.offsetA1 - fractal->foldColor.difs0 < zc.z && colIn > maxdx)
			//	colAdd = fractal->foldColor.difs0000.w;
		}
		if (colDist != aux.dist)
			aux.color = colAdd + fractal->foldColor.difs0000.x;
	}

	if (fractal->transformCommon.functionEnabledZcFalse
			&& aux.i >= fractal->transformCommon.startIterationsZc
			&& aux.i < fractal->transformCommon.stopIterationsZc)
				z = zc;
}
