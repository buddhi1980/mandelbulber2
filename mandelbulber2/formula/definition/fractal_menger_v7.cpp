/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * formula by TGlad,
 * https://fractalforums.org/fractal-mathematics-and-new-theories/28/fractal-clusters/5107
 */

#include "all_fractal_definitions.h"

cFractalMengerV7::cFractalMengerV7() : cAbstractFractal()
{
	nameInComboBox = "Menger V7";
	internalName = "menger_v7";
	internalID = fractal::mengerV7;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerV7::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double t = 0.0;
	CVector4 oldZ = z;
	double col = 0.0;
	double d;
	int mat_struct = fractal->transformCommon.int1;
	int children = fractal->transformCommon.int3;

		double scale = fractal->transformCommon.scale3;
	CVector4 ColV = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector3 p = CVector3(z.x, z.y, z.z); // convert to vec3
	if (fractal->transformCommon.functionEnabledDFalse) aux.DE = 1.0;

	p *= fractal->transformCommon.scaleG1; // master scale
	aux.DE *= fractal->transformCommon.scaleG1;



	//REAL scale = 1.0;
	p *= scale;
	aux.DE *= scale;
	double min_dist = 100000.0;
	for (int n = 0; n < fractal->transformCommon.int16; n++)
	{
		p.x = fabs(p.x);
		p.y = fabs(p.y);
		p.z = fabs(p.z);
		if (fractal->transformCommon.functionEnabledPFalse
				&& n >= fractal->transformCommon.startIterationsP
				&& n < fractal->transformCommon.stopIterationsP1)
		{
			p.x += fractal->transformCommon.offsetA000.x;
			p.y += fractal->transformCommon.offsetA000.y;
			p.z += fractal->transformCommon.offsetA000.z;
		}

		if (p.y > p.x)
		{
			t = p.x;
			p.x = p.y;
			p.y = t;
		}
		if (p.z > p.x)
		{
			t = p.x;
			p.x = p.z;
			p.z = t;
		}


		if (fractal->transformCommon.functionEnabledAFalse)
		{ if (p.z > p.y)
			{
				t= p.y;
				p.y = p.z;
				p.z = t;
			}
		}
		CVector3 q = CVector3(1.0, 0.0, 0.0);
		if (children >= 3 || mat_struct == 3) q = CVector3(1.0, 1.0, 1.0);
		CVector3 p1 = p - q;
		CVector3 p2 = p - CVector3(1.0, 0.0, 1.0);


		if (!fractal->transformCommon.functionEnabledMFalse)
		{
			if (children < 2)
				p2 = p - CVector3(2.0, 0.0, 0.0);
		}
		else
		{
			p2 = p - CVector3(2.0, 0.0, 0.0);
		}
		CVector3 p3 = p - CVector3(1.0, 1.0, 0.0);
		CVector3 p4 = p; //- CVector3(0.0, 0.0, 0.0);
		if (mat_struct == 3)
		{
			if (p.x <= 1.5)
			{
				//f = (p.x - 1.5) / aux->DE;
				//on = true;
				break;
			}

			p4 = p - CVector3(2.0, 0.0, 0.0);

		}

		double d1, d2, d3, d4;
		d1 = p1.Dot(p1);
		d2 = p2.Dot(p2);
		d3 = p3.Dot(p3);
		d4 = p4.Dot(p4);

		if (d1 < d2 && d1 < d3 && d1 < d4)
		{
			p = p1;
			ColV.x = 1.0;
		}
		else if (d2 < d1 && d2 < d3 && d2 < d4)
		{
			p = p2;
			ColV.y = 1.0;
		}
		else if (d3 < d1 && d3 < d2 && d3 < d4)
		{
			p = p3;
			ColV.z = 1.0;
		}
		else if (mat_struct >= 2)
		{
			p = p4;
			ColV.w = 1.0;
			if (mat_struct == 2)
			{
				p *= scale;
				aux.DE *= scale;
				break;
			}
		}
		p *= scale;
		aux.DE *= scale;


		// DE tweaks
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

		if (fractal->foldColor.auxColorEnabled && n >= fractal->foldColor.startIterationsA
				&& n < fractal->foldColor.stopIterationsA)
		{
			col += ColV.x * fractal->foldColor.difs0000.x
					+ ColV.y * fractal->foldColor.difs0000.y
					+ ColV.z * fractal->foldColor.difs0000.z
					+ ColV.w * fractal->foldColor.difs0000.w;
		//	if (fractal->foldColor.difs1 > dist) col += fractal->foldColor.difs0000.w;
		}
	}

	z = CVector4(p.x, p.y, p.z, z.w);


	if (!fractal->transformCommon.functionEnabledSwFalse)
	{
		d = min(min_dist, (max(fabs(p.x), max(fabs(p.y), fabs(p.z))) - fractal->transformCommon.offset105) / aux.DE);
	}
	else
	{
		d = (z.Length() - fractal->transformCommon.offset105) / aux.DE;
	}


	if (fractal->transformCommon.functionEnabledCFalse)
	{
		double dst1 = aux.const_c.Length() - fractal->transformCommon.offsetR1;
		d = max(d, dst1);
		d = fabs(d);
	}

	if (!fractal->transformCommon.functionEnabledXFalse)
		aux.dist = min(aux.dist, d);
	else
		aux.dist = d;

	if (fractal->analyticDE.enabledFalse) z = oldZ;

	aux.color = col;
}
