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

cFractalTransfDIFSHexprismV2::cFractalTransfDIFSHexprismV2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Hexprism V2";
	internalName = "transf_difs_hexprism_v2";
	internalID = fractal::transfDIFSHexprismV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSHexprismV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.offset000;

	double lenX = fractal->transformCommon.offset1;
	double lenY = fractal->transformCommon.offsetA1;
	double H = z.z;
	CVector4 zc = fabs(z);

	// swap axis
	if (fractal->transformCommon.functionEnabledSFalse)
	{
		double temp = zc.x;
		zc.x = zc.y;
		zc.y = temp;
	}

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		double temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}

	if (fractal->transformCommon.rotation2EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
	}

	CVector4 k = CVector4(-SQRT_3_4, 0.5, SQRT_1_3, 0.0);

	double tp = 2.0 * min(k.x * zc.x + k.y * zc.y, 0.0);
	zc.x -= tp * k.x;
	zc.y -= tp * k.y;

	double dx = zc.x - clamp(zc.x, -k.z * lenX, k.z * lenX);
	double dy = zc.y - lenX;

	tp = sqrt(dx * dx + dy * dy);
	dx = tp * sign(zc.y - lenX);
	dy =  zc.z - lenY;

	if (fractal->transformCommon.functionEnabledMFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		zc.z = H;
	}

	// abs sqrd
	if (fractal->transformCommon.functionEnabledTFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		zc.z *= zc.z;
	}

	dx += fractal->transformCommon.scale0 * zc.z;




	if (fractal->transformCommon.functionEnabledDFalse)
		dx = fabs(dx) - fractal->transformCommon.offset0;

	double maxdx = max(dx, 0.0);
	double maxdy = max(dy, 0.0);

	tp = sqrt(maxdx * maxdx + maxdy * maxdy);
	aux.DE0 = min(max(dx, dy), 0.0) + tp;
	aux.dist = min(aux.dist, aux.DE0 / (aux.DE + 1.0));
}
