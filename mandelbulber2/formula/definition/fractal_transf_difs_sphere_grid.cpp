/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSSphereGrid
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSSphereGrid::cFractalTransfDIFSSphereGrid() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Sphere Grid";
	internalName = "transf_difs_sphere_grid";
	internalID = fractal::transfDIFSSphereGrid;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSSphereGrid::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;
	double ang, cosA, sinB, temp;

	if (fractal->transformCommon.functionEnabledFalse)
	{
		zc += fractal->transformCommon.offset000;
		zc *= fractal->transformCommon.scale1;
		aux.DE *= fabs(fractal->transformCommon.scale1);
		zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
		if (fractal->transformCommon.functionEnabledDFalse)
		{
			ang = sqrt(zc.x * zc.x + zc.y * zc.y) * fractal->transformCommon.scaleA0
					+ zc.z * fractal->transformCommon.scaleB0
					+ fractal->transformCommon.angleDegA;
			cosA = cos(ang);
			sinB = sin(ang);
			temp = zc.x;
			zc.x = zc.x * cosA - zc.y * sinB;
			zc.y = temp * sinB + zc.y * cosA;
		}
	}

	zc.x = fabs(zc.x);
	double psi = M_PI / fractal->transformCommon.int8Z;
	psi = fabs(fmod(atan2(zc.x, zc.y) + psi, 2.0 * psi) - psi);
	double len = sqrt(zc.y * zc.y + zc.x * zc.x);
	zc.y = cos(psi) * len;
	zc.x = sin(psi) * len;

	if (fractal->transformCommon.rotation2EnabledFalse)
	{
		zc = fractal->transformCommon.rotationMatrix2.RotateVector(zc);
	}

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		ang = sqrt(zc.x * zc.x + zc.y * zc.y) * fractal->transformCommon.scaleC0;
		cosA = cos(ang);
		sinB = sin(ang);
		temp = zc.x;
		zc.x = zc.x * cosA - zc.y * sinB;
		zc.y = temp * sinB + zc.y * cosA;
	}

	double T1 = sqrt(zc.y * zc.y + zc.z * zc.z) - fractal->transformCommon.offsetR1;
	if (!fractal->transformCommon.functionEnabledJFalse)
		T1 = sqrt(T1 * T1 + zc.x * zc.x) - fractal->transformCommon.offsetp01;
	else
		T1 = max(fabs(T1), fabs(zc.x)) - fractal->transformCommon.offsetp01;

	double T2 = 1000.0;
	if (fractal->transformCommon.functionEnabledMFalse)
	{
		T2 = sqrt(zc.y * zc.y + zc.x * zc.x) - fractal->transformCommon.offsetR1;
		if (!fractal->transformCommon.functionEnabledNFalse)
			T2 = sqrt(T2 * T2 + zc.z * zc.z) - fractal->transformCommon.offsetAp01;
		else
			T2 = max(fabs(T2), fabs(zc.z)) - fractal->transformCommon.offsetAp01;
	}

	double T3 = 1000.0;
	if (fractal->transformCommon.functionEnabledOFalse)
	{
		double z2 = fractal->transformCommon.offset05;
		double z1 = fabs(zc.z) - z2;
		T3 = sqrt(zc.y * zc.y + zc.x * zc.x)
				 - sqrt(
					 fractal->transformCommon.offsetR1 * fractal->transformCommon.offsetR1 - z2 * z2);

		if (!fractal->transformCommon.functionEnabledPFalse)
			T3 = sqrt(T3 * T3 + z1 * z1) - fractal->transformCommon.offsetBp01;
		else
			T3 = max(fabs(T3), fabs(z1)) - fractal->transformCommon.offsetBp01;
	}

	double torD = min(T1, T2);
	torD = min(torD, T3);

	if (!fractal->analyticDE.enabledFalse)
		aux.dist = torD / (aux.DE + fractal->analyticDE.offset0);
	else
		aux.dist = min(aux.dist, torD / (aux.DE + fractal->analyticDE.offset0));

	if (fractal->foldColor.auxColorEnabled)
	{
		double colorAdd = 0.0f;
		if (T1 == torD) colorAdd += fractal->foldColor.difs0000.x;
		if (T2 == torD) colorAdd += fractal->foldColor.difs0000.y;
		if (T3 == torD) colorAdd += fractal->foldColor.difs0000.z;
		aux.color = colorAdd;
	}

	if (fractal->transformCommon.functionEnabledYFalse) z = zc;
}
