/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsTorusV3Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSTorusV3::cFractalTransfDIFSTorusV3() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Torus V3";
	internalName = "transf_difs_torus_v3";
	internalID = fractal::transfDIFSTorusV3;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSTorusV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.functionEnabledxFalse) z.x = -fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = -fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = -fabs(z.z);

	CVector4 zc = z;

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		swap(zc.x, zc.z);
	}

	double lenX = fractal->transformCommon.offset1;
	double lenY = fractal->transformCommon.offsetA0;
	CVector4 absZ = fabs(zc);

	if (fractal->transformCommon.functionEnabledMFalse)
		lenX += absZ.z * fractal->transformCommon.scale0;
	if (fractal->transformCommon.functionEnabledNFalse)
		lenY += absZ.z * fractal->transformCommon.scaleA0;
	if (fractal->transformCommon.functionEnabledOFalse)
		lenX += absZ.y * fractal->transformCommon.scaleB0;
	if (fractal->transformCommon.functionEnabledPFalse)
		lenY += absZ.x * fractal->transformCommon.scaleC0;

	CVector3 q = CVector3(max(absZ.y - lenY, 0.0), max(absZ.x - lenX, 0.0), zc.z);
	q *= q;

	double streD = sqrt(q.x + q.y) - fractal->transformCommon.offsetR1;

	if (!fractal->transformCommon.functionEnabledJFalse)
		streD = sqrt(streD * streD + q.z);
	else
		streD = max(fabs(streD), fabs(zc.z));

	aux.dist = min(aux.dist, (streD - fractal->transformCommon.offsetA05) / (aux.DE + 1.0));
}
