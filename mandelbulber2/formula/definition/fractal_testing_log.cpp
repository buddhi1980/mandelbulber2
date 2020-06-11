/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 * formula by pupukuusikko
 * http://www.fractalforums.com/the-3d-mandelbulb/a-new-3d-mandelbrot-variant-mandelcup/
 */

#include "all_fractal_definitions.h"

cFractalTestingLog::cFractalTestingLog() : cAbstractFractal()
{
	nameInComboBox = "Testing Log";
	internalName = "testing_log";
	internalID = fractal::testingLog;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingLog::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	double colorAdd = 0.0;

	CVector4 ColV = CVector4(0.0, 0.0, 0.0, 0.0);
	//double tp = fractal->transformCommon.offset1;
	double t = fractal->transformCommon.minR06;
	CVector4 t1 = CVector4(SQRT_3_4, -0.5, 0.0, 0.0);
	CVector4 t2 = CVector4(-SQRT_3_4, -0.5, 0.0, 0.0);

	CVector4 n1 = CVector4(-0.5, -SQRT_3_4, 0.0, 0.0);
	CVector4 n2 = CVector4(-0.5, SQRT_3_4, 0.0, 0.0);
	double scale2 = fractal->transformCommon.scale2;
	// adjusting this can help with the stepping scheme, but doesn't affect geometry.

	double innerScale = SQRT_3 / (1.0 + SQRT_3);
	double innerScaleB = innerScale * innerScale * 0.25;

	//for (int i = 0; i < fractal->transformCommon.int8X && dot(z, z) < 0.5; i++)
	for (int n = 0; n < fractal->transformCommon.int8X; n++)
	{
		if (!fractal->transformCommon.functionEnabledBxFalse)
		{
			CVector4 zB = z - CVector4(0.0, 0.0, innerScale * 0.5, 0.0);
			if (zB.Dot(zB) < innerScaleB) break; // definitely inside
		}

		double maxH = 0.4 * fractal->transformCommon.scaleG1;
		if (n == 0) maxH = -100;

		CVector4 zC = z - CVector4(0.0, 0.0, t, 0.0);
		if (z.z > maxH && zC.Dot(zC) > t * t) break; // definitely outside

		CVector4 zD = z - CVector4(0.0, 0.0, 0.5, 0.0);
		double invSC = 1.0 / z.Dot(z) * fractal->transformCommon.scaleF1;

		if (z.z < maxH && zD.Dot(zD) > 0.5 * 0.5)
		{
			// needs a sphere inverse
			aux.DE *= invSC;
			z *= invSC;
			ColV.x += 1.0;
		}
		else
		{
			// stretch onto a plane at zero
			ColV.y += 1.0;;
			aux.DE *= invSC;
			z *= invSC;
			z.z -= 1.0;
			z.z *= -1.0;
			z *= SQRT_3;
			aux.DE *= SQRT_3;
			z.z += 1.0;

		// and rotate it a twelfth of a revolution
			double a = M_PI / double(fractal->transformCommon.int6);
			double cosA = cos(a);
			double sinA = sin(a);
			double xx = z.x * cosA + z.y * sinA;
			double yy = -z.x * sinA + z.y * cosA;
			z.x = xx;
			z.y = yy;
		}

		// now modolu the space so we move to being in just the central hexagon, inner radius 0.5
		double h = z.z;
		double x = z.Dot(-1.0 * n2) * fractal->transformCommon.scaleA2 / SQRT_3;
		double y = z.Dot(-1.0 * n1) * fractal->transformCommon.scaleA2 / SQRT_3;
		x = x - floor(x);
		y = y - floor(y);

		if (x + y > 1.0)
		{
			x = 1.0 - x;
			y = 1.0 - y;
			colorAdd += 1.0;
		}

		z = x * t1 - y * t2;
		// fold the space to be in a kite
		double l0 = z.Dot(z);
		double l1 = (z - t1).Dot(z - t1);
		double l2 = (z + t2).Dot(z + t2);

		if (l1 < l0 && l1 < l2)
		{
			z -= t1 * (2.0 * z.Dot(t1) - 1.0);
			ColV.z += 1.0;
		}

		else if (l2 < l0 && l2 < l1)
		{
			z -= t2 * (2.0 * z.Dot(t2) + 1.0);
			ColV.w += 1.0;
		}

		z.z = h;
		//double pp = -.2;
		//if ( i % 2 == 0) pp = 0.0;
		//z += fractal->transformCommon.offset000 + pp;

		}
	// aux.DE =  scale2;
	CVector4 len = z - CVector4(0.0, 0.0, 0.4, 0.0);
	double d = (len.Length() - 0.4); // the 0.4 is slightly more averaging than 0.5
	if (fractal->analyticDE.enabledFalse) d = (sqrt(d + 1.0) - 1.0) * 2.0;
	d /= (scale2 * aux.DE);

	aux.dist = min(aux.dist, d);

	// aux.color
	if (fractal->foldColor.auxColorEnabledFalse)
	{ //double colorAdd = 0.0;
		colorAdd += colorAdd * fractal->foldColor.difs1;
		colorAdd += ColV.x * fractal->foldColor.difs0000.x;
		colorAdd += ColV.y * fractal->foldColor.difs0000.y;
		colorAdd += ColV.z * fractal->foldColor.difs0000.z;
		colorAdd += ColV.w * fractal->foldColor.difs0000.w;


		aux.color += colorAdd;
	}
}
