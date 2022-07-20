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

cFractalTransfDIFSHelixV2::cFractalTransfDIFSHelixV2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Helix V2";
	internalName = "transf_difs_helix_v2";
	internalID = fractal::transfDIFSHelixV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSHelixV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp;
	CVector4 zc;
	if (!fractal->transformCommon.functionEnabledAuxCFalse) zc = z;
	else zc = aux.const_c;


	zc *= fractal->transformCommon.scale1;
	aux.DE *= fractal->transformCommon.scale1;
	// torus
	double ang = atan2(zc.y, zc.x) / M_PI_2x;
	zc.y = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.radius1;

	// vert helix
	if (fractal->transformCommon.functionEnabledAx)
	{
		double Voff = fractal->transformCommon.offsetA2;
		temp = zc.z - Voff * ang  * fractal->transformCommon.int1 + Voff * 0.5;
		zc.z = temp - Voff * floor(temp / (Voff)) - Voff * 0.5;
	}
	// stretch around helix
	if (fractal->transformCommon.functionEnabledAy)
	{
		if (!fractal->transformCommon.functionEnabledAyFalse)
			zc.x = fractal->transformCommon.offset1;
		else
		{
			double off = fractal->transformCommon.offset1;
			double stretch = fractal->transformCommon.scaleA2 * ang + off;
			zc.x = (stretch) - 2.0 * floor((stretch) * 0.5) - 1.0;
		}
	}
	// twist
	if (fractal->transformCommon.functionEnabledAz)
	{
		double a = zc.y;
		double b = zc.z;
		if (fractal->transformCommon.functionEnabledSwFalse)
		{
			a = zc.z;
			b = zc.y;
		}
		ang *= M_PI * fractal->transformCommon.int2;
		double cosA = cos(ang);
		double sinB = sin(ang);
		zc.y = b * cosA + a * sinB;
		zc.z = a * cosA + b * -sinB;
	}

	// menger sponge
	if (fractal->transformCommon.functionEnabledAwFalse)
	{
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
				col += fractal->foldColor.difs0000.z;
			}
			if (n >= fractal->foldColor.startIterationsA
						&& n < fractal->foldColor.stopIterationsA)
			{
				aux.color += col;
			}

			temp = fractal->transformCommon.scale3 - 1.0;
			double bz = temp * fractal->transformCommon.offsetA111.z;
			zc = fractal->transformCommon.scale3 * zc
					- temp * fractal->transformCommon.offsetA111;
			aux.DE = fractal->transformCommon.scale3 * (aux.DE + 1.0);
			if (zc.z < -0.5 * bz) zc.z += bz;
		}
	}
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}


	aux.color += fractal->foldColor.difs0000.w * (zc.y * zc.y);

	CVector4 d = fabs(zc);
	d.x = max(d.x - fractal->transformCommon.offsetA1, 0.0);
	d.y = max(d.y - fractal->transformCommon.offset01, 0.0);
	d.z = max(d.z - fractal->transformCommon.offsetp1, 0.0);
	double rDE = d.Length();

	rDE = rDE / (aux.DE + fractal->analyticDE.offset0) - fractal->transformCommon.offset0;

	if (fractal->transformCommon.functionEnabledJFalse) // z clip
	{
		rDE = max(
			fabs(aux.const_c.z - fractal->transformCommon.offsetE0) - fractal->transformCommon.offset2,
			rDE);
	}
	aux.dist = min(aux.dist, rDE);

	if (fractal->transformCommon.functionEnabledZcFalse
			&& aux.i >= fractal->transformCommon.startIterationsZc
			&& aux.i < fractal->transformCommon.stopIterationsZc)
		z = zc;
}
