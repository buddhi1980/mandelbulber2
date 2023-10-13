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
	double dt = 0.0; // mmmmmmmmmmmmmmmmmm
	double t = 0.0;
	CVector4 oldZ = z;
	double col = 0.0;
	double d;
	//int mat_struct = fractal->transformCommon.int1;
	//int children = fractal->transformCommon.int3;

	double scale = fractal->transformCommon.scale3;
	CVector4 ColV = CVector4(0.0, 0.0, 0.0, 0.0);

	z *= fractal->transformCommon.scale015; // master scale
	aux.DE *= fractal->transformCommon.scale015;

//	z *= fractal->transformCommon.scale3;
//	aux.DE *= fractal->transformCommon.scale3;
//	double min_dist = 100000.0;
	for (int n = 0; n < fractal->transformCommon.int8X; n++)
	{
		z = fabs(z);
		if (fractal->transformCommon.functionEnabledPFalse
				&& n >= fractal->transformCommon.startIterationsP
				&& n < fractal->transformCommon.stopIterationsP1)
		{
			z += fractal->transformCommon.offset000;
		}
		// rotation
		if (n >= fractal->transformCommon.startIterationsR
				&& n < fractal->transformCommon.stopIterationsR)
		{
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}

		if (z.y > z.x)
		{
			t = z.x;
			z.x = z.y;
			z.y = t;
		}

		if (z.z > z.x)
		{
			t = z.x;
			z.x = z.z;
			z.z = t;
		}

		if (fractal->transformCommon.functionEnabled
				&& n >= fractal->transformCommon.startIterations
				&& n < fractal->transformCommon.stopIterations
				&& z.z > z.y)
		{
			t = z.y;
			z.y = z.z;
			z.z = t;
		}

		CVector4 p1 = z - fractal->transformCommon.offsetA111; // mmmmmmmmmmmmm
		if (fractal->transformCommon.functionEnabledAFalse
				&& n >= fractal->transformCommon.startIterationsA
				&& n < fractal->transformCommon.stopIterationsA)
		{
			p1 = z - fractal->transformCommon.offsetA000;
		}




		CVector4 p2 = z - CVector4(1.0, 0.0, 1.0, 0.0);
		if (fractal->transformCommon.functionEnabledEFalse
				&& n >= fractal->transformCommon.startIterationsE
				&& n < fractal->transformCommon.stopIterationsE)
		{
			 p2 = z - CVector4(2.0, 0.0, 0.0, 0.0);
		}


		CVector4 p3 = z - fractal->transformCommon.offset110;
		if (fractal->transformCommon.functionEnabledDFalse
				&& n >= fractal->transformCommon.startIterationsD
				&& n < fractal->transformCommon.stopIterationsD)
		{
			p3 += fractal->transformCommon.offsetF000;
		}


		CVector4 p4 = z; //- (REAL4){0.0f, 0.0f, 0.0f, 0.0f};
				if (fractal->transformCommon.functionEnabledFFalse
				&& n >= fractal->transformCommon.startIterationsF
				&& n < fractal->transformCommon.stopIterationsF)
		{
			p4 = z - CVector4(2.0, 0.0, 0.0, 0.0);
		}


		if (fractal->transformCommon.functionEnabledIFalse
					&& n >= fractal->transformCommon.startIterationsI
					&& n < fractal->transformCommon.stopIterationsI)
		{
			if (z.x <= 1.5)
			{
				//aux.dist = (z.x - 1.5) / aux.DE;
				break;
			}
		}


		double d1, d2, d3, d4;
		d1 = p1.Dot(p1);
		d2 = p2.Dot(p2);
		d3 = p3.Dot(p3);
		d4 = p4.Dot(p4);

		if (d1 < d2 && d1 < d3 && d1 < d4)
		{
			z = p1;
			ColV.x = 1.0;
		}
		else if (d2 < d1 && d2 < d3 && d2 < d4)
		{
			z = p2;
			ColV.y = 1.0;
		}
		else if (d3 < d1 && d3 < d2 && d3 < d4)
		{
			z = p3;
			ColV.z = 1.0;
		}
		else if (fractal->transformCommon.functionEnabledGFalse
				&& n >= fractal->transformCommon.startIterationsG
				&& n < fractal->transformCommon.stopIterationsG)
		{
			z = p4;
			ColV.w = 1.0;

		}
		else if (fractal->transformCommon.functionEnabledJFalse
				&& n >= fractal->transformCommon.startIterationsJ
				&& n < fractal->transformCommon.stopIterationsJ)
		{
			z = p4;
			ColV.w = 1.0;

			{
				z *= scale;
				aux.DE *= scale;
				break;
			}
		}

		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

		z *= scale;
		aux.DE *= scale;
/*	z.z = fabs(z.z - FRAC_1_3 * 1.) + FRAC_1_3 * 1.;
	z = z * scale - CVector4(1.0, 1.0, 1.0, 0.0) * (scale - 1.0);
		aux.DE = aux.DE * scale;*/





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

	if (!fractal->transformCommon.functionEnabledOFalse)
	{
		if (!fractal->transformCommon.functionEnabledSwFalse)
		{
			d = max(fabs(z.x), max(fabs(z.y), fabs(z.z))) - fractal->transformCommon.offset0;
		}
		else
		{
			d = z.Length() - fractal->transformCommon.offset0;
		}
	}
	else
	{
		double r = z.Length() - fractal->transformCommon.offsetA0;
		double m = (max(fabs(z.x), max(fabs(z.y), fabs(z.z)))) - fractal->transformCommon.offsetB0;
		d = r + (m - r) * fractal->transformCommon.scale0;
	}

	d= d * fractal->transformCommon.scaleB1 / aux.DE;



	if (fractal->transformCommon.functionEnabledYFalse)
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
