/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSPrismV2Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSPrismV2::cFractalTransfDIFSPrismV2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Prism V2";
	internalName = "transf_difs_prism_v2";
	internalID = fractal::transfDIFSPrismV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSPrismV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.functionEnabledxFalse) z.x = -fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = -fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = -fabs(z.z);

	CVector4 zc = z;

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse) swap(zc.x, zc.z);

	double absZ = fabs(zc.z);
	double tp;
	double len = fractal->transformCommon.offset1;
	double face = fractal->transformCommon.offset05;

	if (fractal->transformCommon.functionEnabledMFalse)
	{
		tp = absZ;
		if (fractal->transformCommon.functionEnabledCxFalse) tp *= tp;
		zc.y += tp * fractal->transformCommon.scale0;
	}

	if (fractal->transformCommon.functionEnabledNFalse)
	{
		tp = absZ;
		if (fractal->transformCommon.functionEnabledCyFalse) tp *= tp;
		len += tp * fractal->transformCommon.scaleA0;
	}

	if (fractal->transformCommon.functionEnabledOFalse)
	{
		tp = absZ;
		if (fractal->transformCommon.functionEnabledCzFalse) tp *= tp;
		face += tp * fractal->transformCommon.scaleB0;
	}

	double priD = max(fabs(zc.x) - len, max(fabs(zc.y) * SQRT_3_4 + zc.z * 0.5, -zc.z) - face);

	aux.dist = min(aux.dist, priD / (aux.DE + 1.0));
}
