﻿/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSHelixMenger based on fragmentarium code by knighty (jan 2012)
 * M3D difs code by darkbeam
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSHelixMenger::cFractalTransfDIFSHelixMenger() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Helix Menger";
	internalName = "transf_difs_helix_menger";
	internalID = fractal::transfDIFSHelixMenger;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSHelixMenger::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp;
	CVector4  zc = z;
	double addCol = 0.0;

	zc *= fractal->transformCommon.scale2;
	aux.DE *= fractal->transformCommon.scale2;
	// torus
	double ang = atan2(zc.y, zc.x) * M_PI_2x_INV;

	zc.y = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.scaleMain2;

	// vert helix
	if (fractal->transformCommon.functionEnabledAx)
	{
		double Voff = fractal->transformCommon.offset4;
		temp = zc.z - Voff * ang  * fractal->transformCommon.int1 + Voff * 0.5;
		zc.z = temp - Voff * floor(temp / Voff) - Voff * 0.5;
	}
	// stretch around helix
	if (fractal->transformCommon.functionEnabledAy)
	{
		if (!fractal->transformCommon.functionEnabledAyFalse)
		{
			temp = fractal->transformCommon.scale16	* ang
					+ fractal->transformCommon.offset1;
			zc.x = temp - 2.0 * floor(temp * 0.5) - 1.0;
		}
		else
			zc.x = fractal->transformCommon.offset1;
	}
	zc.x *= fractal->transformCommon.scaleG1;
	// twist
	if (fractal->transformCommon.functionEnabledAz)
	{
		ang *= M_PI * fractal->transformCommon.int2;
		double cosA = cos(ang);
		double sinB = sin(ang);
		double a;
		double b;
		if (!fractal->transformCommon.functionEnabledKFalse)
		{
			if (!fractal->transformCommon.functionEnabledSwFalse)
			{
				a = zc.y;
				b = zc.z;
			}
			else
			{
				a = zc.z;
				b = zc.y;
			}
			zc.y = b * cosA + a * sinB;
			zc.z = a * cosA - b * sinB;
		}
		else
		{
			if (!fractal->transformCommon.functionEnabledSwFalse)
			{
				a = zc.x;
				b = zc.z;
			}
			else
			{
				a = zc.z;
				b = zc.x;
			}
			zc.x = b * cosA + a * sinB;
			zc.z = a * cosA - b * sinB;
		}
		if (fractal->transformCommon.functionEnabledPFalse) zc.x = zc.z;
		if (fractal->transformCommon.functionEnabledEFalse) zc.x = zc.y;
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{
		zc = fractal->transformCommon.offset000 - fabs(zc);
	}

	// menger sponge
	int Iterations = fractal->transformCommon.int16;

	for (int n = 0; n < Iterations; n++)
	{
		zc = fabs(zc);
		zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
		double col = 0.0;
		if (zc.x < zc.y)
		{
			temp = zc.y;
			zc.y = zc.x;
			zc.x = temp;
			col += fractal->foldColor.difs0000.x;
		}
		if (zc.x < zc.z)
		{
			temp = zc.z;
			zc.z = zc.x;
			zc.x = temp;
			col += fractal->foldColor.difs0000.y;
		}
		if (zc.y < zc.z)
		{
			temp = zc.z;
			zc.z = zc.y;
			zc.y = temp;
		}
		if (n >= fractal->foldColor.startIterationsA
					&& n < fractal->foldColor.stopIterationsA)
		{
			addCol += col;
		}

		temp = fractal->transformCommon.scale3 - 1.0;
		double bz = temp * fractal->transformCommon.offsetA111.z
				+ fractal->transformCommon.offsetA0;
		zc = fractal->transformCommon.scale3 * zc
				- temp * fractal->transformCommon.offsetA111;
		aux.DE = fractal->transformCommon.scale3 * (aux.DE + fractal->transformCommon.offsetB0);
		if (zc.z < -0.5 * bz) zc.z += bz;
	}

	CVector4 d = fabs(zc);
	d.x = max(d.x - fractal->transformCommon.offsetA1, 0.0);
	d.y = max(d.y - fractal->transformCommon.offset01, 0.0);
	d.z = max(d.z - fractal->transformCommon.offsetp1, 0.0);

	double rDE;
	if (!fractal->transformCommon.functionEnabledCFalse)
	{
		if (!fractal->transformCommon.functionEnabledTFalse)
		{
			rDE = max(d.x, max(d.y, d.z));
		}
		else
		{
			rDE = d.Length();
		}
	}
	else
	{
		if (!fractal->transformCommon.functionEnabledBFalse) temp = d.x * d.x + d.y * d.y;
		else temp = d.x + d.y;
		rDE = sqrt(temp) - fractal->transformCommon.offset0;

		if (fractal->transformCommon.functionEnabledMFalse)
			rDE = max(fabs(rDE), fabs(d.z));
		if (fractal->transformCommon.functionEnabledSFalse)
			rDE = sqrt(rDE * rDE + d.z * d.z);
	}

	rDE -= fractal->transformCommon.offset0005;
	rDE = rDE / (aux.DE + fractal->analyticDE.offset0);
	if (fractal->transformCommon.functionEnabledJFalse) // z clip
	{
		rDE = max(
			fabs(aux.const_c.z - fractal->transformCommon.offsetE0) - fractal->transformCommon.offset2,
			rDE);
	}

	double colDist = aux.dist;
	aux.dist = min(aux.dist, rDE);

	if (fractal->transformCommon.functionEnabledZcFalse
			&& aux.i >= fractal->transformCommon.startIterationsZc
			&& aux.i < fractal->transformCommon.stopIterationsZc)
		z = zc;

	if (fractal->foldColor.auxColorEnabled && colDist != aux.dist
			&& aux.i >= fractal->foldColor.startIterationsB
			&& aux.i < fractal->foldColor.stopIterationsB)
	{
		addCol += aux.i * fractal->foldColor.difs0;

		// fractal->foldColor.difs0000.y


		if (!fractal->transformCommon.functionEnabledGFalse)
		{
			ang = (M_PI - 2.0 * fabs(atan(fractal->foldColor.difs1 * zc.y / zc.z))) * 4.0 * M_PI_2x_INV;
			if (fmod(ang, 2.0) < 1.0) addCol += fractal->foldColor.difs0000.z;
			else addCol += fractal->foldColor.difs0000.w;
		}
		else
		{
			addCol += fractal->foldColor.difs0000.z * (zc.z * zc.z);
			addCol += fractal->foldColor.difs0000.w * (zc.y * zc.y);
		}
		if (!fractal->foldColor.auxColorEnabledFalse) aux.color = addCol;
		else  aux.color += addCol;
	}
}
