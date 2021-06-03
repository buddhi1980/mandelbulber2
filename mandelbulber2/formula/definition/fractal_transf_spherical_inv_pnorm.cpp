/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfSphericalInvPnorm  using p-norm
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalInvPnorm::cFractalTransfSphericalInvPnorm() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Invert p-norm";
	internalName = "transf_spherical_inv_pnorm";
	internalID = fractal::transfSphericalInvPnorm;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalInvPnorm::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 lpN = fabs(z);
	double pr = fractal->transformCommon.scale2;
	lpN.x = pow(lpN.x, pr);
	lpN.y = pow(lpN.y, pr);
	lpN.z = pow(lpN.z, pr);
	double pNorm = pow((lpN.x + lpN.y + lpN.z), 1.0 / pr);

	pNorm = pow(pNorm, fractal->transformCommon.scaleA2);
	pNorm = max(pNorm, fractal->transformCommon.offset0);

	double useScale = fractal->transformCommon.scale1 - aux.actualScaleA;
	if (fractal->transformCommon.functionEnabledKFalse) // update actualScaleA
		aux.actualScaleA = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) + 1.0);
	pNorm = useScale / pNorm;
	z *= pNorm;
	aux.DE *= fabs(pNorm);

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
