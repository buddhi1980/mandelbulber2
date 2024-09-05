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
	CVector4 zc = z;
	zc = fabs(zc) - fractal->transformCommon.additionConstant111;
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
			if (zc.y < max(zc.x, zc.z)) aux.color += fractal->foldColor.difs0000.y;
			if (zc.x < max(zc.y, zc.z)) aux.color += fractal->foldColor.difs0000.z;
			double t = z.x * z.y;
			if ((t > 0.0 && z.z > 0.0) || (t < 0.0 && z.z < 0.0)) aux.color += fractal->foldColor.difs0000.w;
		}
	}
}
