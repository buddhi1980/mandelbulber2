/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsBoxV3Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSBoxV3::cFractalTransfDIFSBoxV3() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Box V3";
	internalName = "transf_difs_box_v3";
	internalID = fractal::transfDIFSBoxV3;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSBoxV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.functionEnabledCxFalse) z.x = -fabs(z.x);
	if (fractal->transformCommon.functionEnabledCyFalse) z.y = -fabs(z.y);
	if (fractal->transformCommon.functionEnabledCzFalse) z.z = -fabs(z.z);

	CVector4 zc = z;
	CVector4 boxSize = fractal->transformCommon.additionConstant111;

	// curvy shape
	if (fractal->transformCommon.functionEnabledTFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		double Zxx = zc.x * zc.x;
		double Zzz = zc.z * zc.z;
		boxSize.x += Zzz * fractal->transformCommon.constantMultiplier000.x;
		boxSize.y += Zzz * fractal->transformCommon.constantMultiplier000.y;
		boxSize.z += Zxx * fractal->transformCommon.constantMultiplier000.z;
	}

	// uneven box
	if (fractal->transformCommon.functionEnabledMFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		CVector4 subZ = CVector4(z.y, z.z, z.x, z.w) * fractal->transformCommon.scale3D000;
		boxSize -= subZ;
	}

	if (fractal->transformCommon.functionEnabledNFalse
			&& aux.i >= fractal->transformCommon.startIterationsN
			&& aux.i < fractal->transformCommon.stopIterationsN)
	{
		double k = fractal->transformCommon.angle0;

		if (fractal->transformCommon.functionEnabledAxFalse)
			k *= aux.i + fractal->transformCommon.offset1;

		double swap;
		if (!fractal->transformCommon.functionEnabledOFalse)
			swap = zc.x;
		else
			swap = zc.z;

		if (fractal->transformCommon.functionEnabledAzFalse) swap = fabs(swap);

		double c = cos(k * zc.y);
		double s = sin(k * zc.y);
		if (!fractal->transformCommon.functionEnabledOFalse)
			zc.x = c * swap + -s * zc.y;
		else
			zc.z = c * swap + -s * zc.y;
		zc.y = s * swap + c * zc.y;
	}

	CVector4 q = fabs(zc) - boxSize;
	zc = q;
	zc.x = max(zc.x, 0.0);
	zc.y = max(zc.y, 0.0);
	zc.z = max(zc.z, 0.0);
	double zcd = zc.Length();

	double colDist = aux.dist;
	if (!fractal->transformCommon.functionEnabledEFalse)
		aux.dist = min(aux.dist, zcd / (aux.DE + 1.0));
	else
		aux.dist = min(aux.dist, zcd / (aux.DE + 1.0) - fractal->transformCommon.offsetB0);

	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		if (colDist != aux.dist) aux.color += fractal->foldColor.difs0000.x;
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (fractal->foldColor.difs0000.y != 0.0f && zc.x == fabs(q.x))
				aux.color += fractal->foldColor.difs0000.y;
			if (fractal->foldColor.difs0000.z != 0.0f && zc.y == fabs(q.y))
				aux.color += fractal->foldColor.difs0000.z;
			if (fractal->foldColor.difs0000.w != 0.0f && zc.z == fabs(q.z))
				aux.color += fractal->foldColor.difs0000.w;
/*			if (zc.y < max(zc.x, zc.z))
				aux.color += fractal->foldColor.difs0000.y;
			if (zc.x < max(zc.y, zc.z))
				aux.color += fractal->foldColor.difs0000.z;
			if (zc.z < max(zc.y, zc.x))
				aux.color += fractal->foldColor.difs0000.w;*/

			if (fractal->foldColor.difs0 != 0.0)
			{
				double t = z.x * z.y;
				if ((t > 0.0 && z.z > 0.0) || (t < 0.0 && z.z < 0.0)) aux.color += fractal->foldColor.difs0;
			}
		}
	}
}
