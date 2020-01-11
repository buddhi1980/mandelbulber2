/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsCylinderV2Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSCylinderV2::cFractalTransfDIFSCylinderV2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Cylinder V2";
	internalName = "transf_difs_cylinder_v2";
	internalID = fractal::transfDIFSCylinderV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSCylinderV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.offset000;
	CVector4 zc = z;

	double temp;
	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse) swap(zc.x, zc.z);

	double cylR = zc.x * zc.x;
	double absH = fabs(zc.z);
	double lengthCyl = zc.z;

	cylR = sqrt(cylR + zc.y * zc.y);
	double cylH = absH - fractal->transformCommon.offsetA1;

	// no absz
	if (fractal->transformCommon.functionEnabledMFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		absH = lengthCyl;
	}

	// abs sqrd
	if (fractal->transformCommon.functionEnabledTFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		absH *= absH;
	}

	double cylRm = cylR - fractal->transformCommon.radius1;
	cylRm += fractal->transformCommon.scale0 * absH;

	// tops
	if (fractal->transformCommon.functionEnabledNFalse
			&& aux.i >= fractal->transformCommon.startIterationsN
			&& aux.i < fractal->transformCommon.stopIterationsN)
	{
		temp = cylR;
	}
	else
	{
		temp = cylRm;
	}
	temp = max(temp, 0.0);
	double cylHm = max(cylH, 0.0);
	double cylD = sqrt(temp * temp + cylHm * cylHm);

	// rings
	if (fractal->transformCommon.functionEnabledOFalse
			&& aux.i >= fractal->transformCommon.startIterationsO
			&& aux.i < fractal->transformCommon.stopIterationsO)
	{
		cylD = sqrt(cylRm * cylRm + cylH * cylH);
	}
	cylD = min(max(cylRm, cylH) - fractal->transformCommon.offsetR0, 0.0) + cylD;

	aux.dist = min(aux.dist, cylD / (aux.DE + 1.0));
}
