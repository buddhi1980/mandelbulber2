/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsHelixIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * M3D difs code by darkbeam
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSHelix::cFractalTransfDIFSHelix() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Helix";
	internalName = "transf_difs_helix";
	internalID = fractal::transfDIFSHelix;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSHelix::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp;
	CVector4 zc;
	if (!fractal->transformCommon.functionEnabledAuxCFalse) zc = z;
	else zc = aux.const_c;

	zc *= fractal->transformCommon.scale3D111;

	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR1)
	{
		zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
	}

	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		double ang = sqrt(zc.x * zc.x + zc.y * zc.y) * fractal->transformCommon.scaleA0
				+ zc.z * fractal->transformCommon.scaleB0;
		double cosA = cos(ang);
		double sinB = sin(ang);
		temp = zc.x;
		zc.x = (zc.x * cosA - zc.y * sinB) * fractal->transformCommon.scaleC1;
		zc.y = (temp * sinB + zc.y * cosA) * fractal->transformCommon.scaleD1;

		zc += fractal->transformCommon.offsetA000;
	}

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) zc.x = fabs(zc.x);
		if (fractal->transformCommon.functionEnabledAyFalse) zc.y = fabs(zc.y);
		if (fractal->transformCommon.functionEnabledAzFalse) zc.z = fabs(zc.z);
	}
	zc += fractal->transformCommon.offset000;

	double cylR = zc.x * zc.x;
	double absH = fabs(zc.z);
	double lengthCyl = zc.z;

	cylR = sqrt(cylR + zc.y * zc.y * fractal->transformCommon.scale1);
	double cylH = absH - fractal->transformCommon.offsetA1;
	double cylRm = cylR - fractal->transformCommon.offset01;
	if (fractal->transformCommon.functionEnabledFalse)
		cylRm = fabs(cylRm ) - fractal->transformCommon.offset0;

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

	cylRm += fractal->transformCommon.scale0 * absH;
	zc.z = absH;

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
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
	cylD /= (aux.DE + 1.0);
	double colDist = aux.dist;
	aux.dist = min(aux.dist, cylD);

	if (fractal->transformCommon.functionEnabledZcFalse
			&& aux.i >= fractal->transformCommon.startIterationsZc
			&& aux.i < fractal->transformCommon.stopIterationsZc)
				z = zc;

	// aux.color
	if (aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addColor = 0.0;
		if (aux.dist == colDist) addColor += fractal->foldColor.difs0000.x;
		if (aux.dist != colDist) addColor += fractal->foldColor.difs0000.y;
		aux.color += addColor;
	}
}
