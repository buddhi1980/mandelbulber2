/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSPrismIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSPrism::cFractalTransfDIFSPrism() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Prism";
	internalName = "transf_difs_prism";
	internalID = fractal::transfDIFSPrism;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSPrism::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;
	double t = 0.0;

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse) swap(zc.x, zc.z);

	double priL = fabs(zc.x) - fractal->transformCommon.offset1;
	double priX = max(fabs(zc.y) * SQRT_3_4 + zc.z * 0.5f, -zc.z) - fractal->transformCommon.offset05;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		t = fabs(priX) - fractal->transformCommon.offsetA0;
		priX = max(priX,(t));
	}

	double priD = max(priL, priX);

	double colDist = aux.dist;
	aux.dist = min(aux.dist, priD / (aux.DE + 1.0));

	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		if (colDist != aux.dist) aux.color += fractal->foldColor.difs0000.x;

		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (fractal->transformCommon.offset1 <= fabs(zc.x))
				aux.color += fractal->foldColor.difs0000.y;
			if (priX == t + fractal->transformCommon.offsetA0)
				aux.color += fractal->foldColor.difs0000.z;

		//	if (priX == t + fractal->transformCommon.offsetA0 && (fractal->transformCommon.offset1 - fractal->foldColor.difs0 <=fabs(zc.x)))
		//			aux.color += fractal->foldColor.difs0000.w;
		}
	}
}
