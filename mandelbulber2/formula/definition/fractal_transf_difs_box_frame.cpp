/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSBosFrameIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSBoxFrame::cFractalTransfDIFSBoxFrame() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Box Frame";
	internalName = "transf_difs_box_frame";
	internalID = fractal::transfDIFSBoxFrame;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSBoxFrame::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// tranform z
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z.y = fabs(z.y);
		double psi = M_PI / fractal->transformCommon.int8X;
		psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledCyFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.z = fabs(z.z);
		double psi = M_PI / fractal->transformCommon.int8Y;
		psi = fabs(fmod(atan2(z.z, z.y) + psi, 2.0 * psi) - psi);
		double len = sqrt(z.y * z.y + z.z * z.z);
		z.y = cos(psi) * len;
		z.z = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledCzFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z.x = fabs(z.x);
		double psi = M_PI / fractal->transformCommon.int8Z;
		psi = fabs(fmod(atan2(z.x, z.z) + psi, 2.0 * psi) - psi);
		double len = sqrt(z.z * z.z + z.x * z.x);
		z.z = cos(psi) * len;
		z.x = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledDFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		z = fabs(z - fractal->transformCommon.offset000);
	}

	if (fractal->transformCommon.functionEnabledEFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		z = fabs(z + fractal->transformCommon.offsetA000)
			- fabs(z - fractal->transformCommon.offsetA000) - z;
	}
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		z *= fractal->transformCommon.scale1;
		aux.DE *= fabs(fractal->transformCommon.scale1);
	}

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	CVector4 zc = z;
	zc = fractal->transformCommon.rotationMatrix2.RotateVector(zc);
	zc = fabs(zc) - fractal->transformCommon.offsetC111;
	CVector4 q = CVector4(fractal->transformCommon.offsetp01,
						fractal->transformCommon.offsetAp01,
						fractal->transformCommon.offsetBp01, 0.0);

	if (!fractal->transformCommon.functionEnabledSwFalse)
		q = fabs(zc + q) - q;
	else
		q = fabs(zc) - q;

	CVector4 len = zc;
	len.x = min(max(zc.x, max(q.y, q.z)), 0.0);
	len.y = min(max(q.x, max(zc.y, q.z)), 0.0);
	len.z = min(max(q.x, max(q.y, zc.z)), 0.0);

	CVector4 mz = zc;
	mz.x = max(zc.x, 0.0);
	mz.y = max(zc.y, 0.0);
	mz.z = max(zc.z, 0.0);

	CVector4 mq = q;
	mq.x = max(q.x, 0.0);
	mq.y = max(q.y, 0.0);
	mq.z = max(q.z, 0.0);

	CVector4 tv = (CVector4(mz.x, mq.y, mq.z, 0.0));
	len.x += tv.Length();
	tv = (CVector4(mq.x, mz.y, mq.z, 0.0));
	len.y += tv.Length();
	tv = (CVector4(mq.x, mq.y, mz.z, 0.0));
	len.z += tv.Length();

	double D = min(min(len.x, len.y), len.z) / (aux.DE + fractal->analyticDE.offset0);
	double colDist = aux.dist;
	if (aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
		aux.dist = min(aux.dist, D);
	else
		aux.dist = D;

	if (fractal->foldColor.auxColorEnabledFalse && colDist != aux.dist
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		aux.color = fractal->foldColor.difs0000.x + aux.i * fractal->foldColor.difs0;

		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (q.y < max(q.x, q.z)) aux.color += fractal->foldColor.difs0000.y;
			if (q.x < max(q.y, q.z)) aux.color += fractal->foldColor.difs0000.z;
			if (q.z < max(q.y, q.x)) aux.color += fractal->foldColor.difs0000.w;
		}
	}

	if (fractal->transformCommon.functionEnabledZcFalse
			&& aux.i >= fractal->transformCommon.startIterationsZc
			&& aux.i < fractal->transformCommon.stopIterationsZc)
				z = zc;
}
