/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfSphericalFoldPnorm using p-norm
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalFoldPnorm::cFractalTransfSphericalFoldPnorm() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Fold Pnorm";
	internalName = "transf_spherical_fold_pnorm";
	internalID = fractal::transfSphericalFoldPnorm;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalFoldPnorm::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 lpN = fabs(z);
	lpN.x = pow(lpN.x, fractal->transformCommon.scale2);
	lpN.y = pow(lpN.y, fractal->transformCommon.scale2);
	lpN.z = pow(lpN.z, fractal->transformCommon.scale2);
	double pNorm = lpN.x + lpN.y + lpN.z;
	if (fractal->transformCommon.scaleA1 != 1.0)
		pNorm = pow(pNorm, fractal->transformCommon.scaleA1);
	double rr = pNorm;
	double useScale = 1.0;
	useScale = fractal->transformCommon.scale1 - aux.actualScaleA;
	if (fractal->transformCommon.functionEnabledKFalse) // update actualScaleA
		aux.actualScaleA = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) + 1.0);
	pNorm = useScale / pNorm;

	z += fractal->transformCommon.offset000;
	double minR = fractal->transformCommon.minR0;
	double temp = 0.0;
	if (rr < minR)
	{
		temp = useScale / minR;
		z *= temp;
		aux.DE *= temp;
	}
	else if (rr < useScale)
	{

		z *= pNorm;
		aux.DE *= pNorm;
	}
	z -= fractal->transformCommon.offset000;

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;


	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux.color += fractal->foldColor.difs0000.x * temp;
		aux.color += fractal->foldColor.difs0000.y * pNorm;
	}

}
