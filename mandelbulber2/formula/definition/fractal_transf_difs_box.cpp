/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsBoxIteration  fragmentarium code, mdifs by knighty (jan 2012)
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSBox::cFractalTransfDIFSBox() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Box";
	internalName = "transf_difs_box";
	internalID = fractal::transfDIFSBox;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSBox::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 q = fabs(z) - fractal->transformCommon.additionConstant111;
	CVector4 zc = q;
	zc.x = max(zc.x, 0.0);
	zc.y = max(zc.y, 0.0);
	zc.z = max(zc.z, 0.0);
	double zcd = zc.Length();
	double colDist = aux.dist;
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
			{
				double t = z.x * z.y;
				if ((t > 0.0 && z.z > 0.0) || (t < 0.0 && z.z < 0.0)) aux.color += fractal->foldColor.difs0;
			}
		}
	}
}
