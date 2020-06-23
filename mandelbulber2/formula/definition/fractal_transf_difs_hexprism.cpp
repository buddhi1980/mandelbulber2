/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSHexprism::cFractalTransfDIFSHexprism() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Hexprism";
	internalName = "transf_difs_hexprism";
	internalID = fractal::transfDIFSHexprism;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSHexprism::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double lenX = fractal->transformCommon.offset1;
	double lenY = fractal->transformCommon.offsetA1;
	CVector4 zc = fabs(z);

	CVector4 k = CVector4(-SQRT_3_4, 0.5, SQRT_1_3, 0.0);

	double tp = 2.0 * min(k.x * zc.x + k.y * zc.y, 0.0);
	zc.x -= tp * k.x;
	zc.y -= tp * k.y;

	double dx = zc.x - clamp(zc.x, -k.z * lenX, k.z * lenX);
	double dy = zc.y - lenX;

	tp = sqrt(dx * dx + dy * dy);
	dx = tp * sign(zc.y - lenX);
	dy =  zc.z - lenY;

	double maxdx = max(dx, 0.0);
	double maxdy = max(dy, 0.0);

	tp = sqrt(maxdx * maxdx + maxdy * maxdy);
	aux.DE0 = min(max(dx, dy), 0.0) + tp;
	aux.dist = min(aux.dist, aux.DE0 / (aux.DE + 1.0));
}
