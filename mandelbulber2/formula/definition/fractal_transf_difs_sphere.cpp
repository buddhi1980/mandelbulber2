/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsSphereIteration  fragmentarium code, mdifs by knighty (jan 2012)
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSSphere::cFractalTransfDIFSSphere() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Sphere";
	internalName = "transf_difs_sphere";
	internalID = fractal::transfDIFSSphere;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSSphere::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;
	double vecLen;
	if (!fractal->transformCommon.functionEnabled4dFalse)
	{
		CVector3 vec = CVector3(zc.x, zc.y, zc.z);
		vecLen = vec.Length();
	}
	else
		vecLen = zc.Length();

	double spD = vecLen - fractal->transformCommon.offsetR1;
	aux.dist = min(aux.dist, spD / (aux.DE + 1.0));
	aux.DE0 = spD; // temp testing
}
