/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsBoxV2Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSBoxV2::cFractalTransfDIFSBoxV2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Box V2";
	internalName = "transf_difs_box_v2";
	internalID = fractal::transfDIFSBoxV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSBoxV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.offset000;
	CVector4 zc = z;
	CVector4 boxSize = fractal->transformCommon.additionConstant111;

	// curve
	if (fractal->transformCommon.functionEnabledTFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		double absZZ = zc.z * zc.z * fractal->transformCommon.scale0;
		boxSize.x += absZZ;
		boxSize.y += absZZ;
	}

	// pyramid
	if (fractal->transformCommon.functionEnabledMFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		double subZ = fractal->transformCommon.scaleA0 * zc.z;
		boxSize.x -= subZ;
		boxSize.y -= subZ;
	}

	zc = fabs(zc) - boxSize;
	zc.x = max(zc.x, 0.0);
	zc.y = max(zc.y, 0.0);
	zc.z = max(zc.z, 0.0);
	double zcd = zc.Length();

	double colDist = aux.dist;
	aux.dist = min(aux.dist, zcd / (aux.DE + 1.0));

	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		if (colDist != aux.dist) aux.color += fractal->foldColor.difs0000.x;
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (zc.y < max(zc.x, zc.z)) aux.color += fractal->foldColor.difs0000.y;
			if (zc.x < max(zc.y, zc.z)) aux.color += fractal->foldColor.difs0000.z;
			double t = z.x * z.y;
			if ((t > 0.0 && z.z > 0.0) || (t < 0.0 && z.z < 0.0)) aux.color += fractal->foldColor.difs0000.w;
		}
	}
}
