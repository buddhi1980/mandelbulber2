/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsTorusV2Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSTorusV2::cFractalTransfDIFSTorusV2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Torus V2";
	internalName = "transf_difs_torus_v2";
	internalID = fractal::transfDIFSTorusV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSTorusV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.functionEnabledxFalse) z.x = -fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = -fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = -fabs(z.z);

	CVector4 zc = z;
	double torD;
	double lenX = 0.0;
	double lenY = 0.0;

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		swap(zc.x, zc.z);
	}

	CVector4 absZ = fabs(zc);
	if (fractal->transformCommon.functionEnabledMFalse)
		lenX = absZ.z * fractal->transformCommon.scale0;
	if (fractal->transformCommon.functionEnabledNFalse)
		lenY = absZ.z * fractal->transformCommon.scaleA0;

	CVector3 q = CVector3(max(absZ.y - lenY, 0.0), max(absZ.x - lenX, 0.0), zc.z);
	q *= q;

	torD = sqrt(q.y + q.x) - fractal->transformCommon.offsetT1;

	if (!fractal->transformCommon.functionEnabledJFalse)
		torD = sqrt(torD * torD + q.z);
	else
		torD = max(fabs(torD), fabs(zc.z));

	aux.dist = min(aux.dist, (torD - fractal->transformCommon.offset05) / (aux.DE + 1.0));
	aux.DE0 = (torD - fractal->transformCommon.offset05); // temp testing
}
