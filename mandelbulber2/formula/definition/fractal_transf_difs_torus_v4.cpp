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

cFractalTransfDIFSTorusV4::cFractalTransfDIFSTorusV4() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Torus V4";
	internalName = "transf_difs_torus_v4";
	internalID = fractal::transfDIFSTorusV4;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSTorusV4::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp;
	if (aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
		z = z - fractal->transformCommon.offset000;

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->transformCommon.angleDegA != 0.0)
	{
		double s = fractal->transformCommon.sinA;
		double c = fractal->transformCommon.cosA;
		temp = z.x;
		z.x = z.x * c + z.y * -s;
		z.y = temp * s + z.y * c;

		z.x = fabs(z.x);
		temp = z.x;
		z.x = z.x * c + z.y * -s;
		z.y = temp * s + z.y * c;
	}

	// swap axis
/*	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}
	if (fractal->transformCommon.functionEnabledSFalse)
	{
		temp = zc.y;
		zc.y = zc.z;
		zc.z = temp;
	}*/

	double torD;
	double T1 = sqrt(z.y * z.y + z.x * z.x) - fractal->transformCommon.radius1;

	if (!fractal->transformCommon.functionEnabledAFalse)
		temp = -fractal->transformCommon.offset02;
	else
		temp = fractal->transformCommon.offsetp05 - fractal->transformCommon.offset02;

	if (!fractal->transformCommon.functionEnabledJFalse)
		torD = sqrt(T1 * T1 + z.z * z.z) + temp;
	else
		torD = max(fabs(T1), fabs(z.z)) + temp;

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		torD = max(fabs(torD) - fractal->transformCommon.offsetp05, 0.0);
	}
	torD = max(torD, -z.y);

	double colDist = aux.dist;
	aux.dist = min(aux.dist, torD / (aux.DE + fractal->analyticDE.offset0));

	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		if (colDist != aux.dist) aux.color += fractal->foldColor.difs0000.x;
	}
}
