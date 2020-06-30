/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * formula by TGlad, extras by sabine62
 * https://fractalforums.org/fractal-mathematics-and-new-theories/28/new-sphere-tree/3557/msg22100#msg22100
 */

#include "all_fractal_definitions.h"

cFractalSpheretree::cFractalSpheretree() : cAbstractFractal()
{
	nameInComboBox = "Spheretree";
	internalName = "spheretree";
	internalID = fractal::spheretree;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalSpheretree::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double Dd;
	if (!fractal->transformCommon.functionEnabledByFalse)
		Dd = 1.0;
	else
		Dd = aux.DE;
	CVector4 ColV = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 oldZ = z;
	double t = fractal->transformCommon.minR06;
	CVector4 t1 = CVector4(SQRT_3_4, -0.5, 0.0, 0.0);
	CVector4 t2 = CVector4(-SQRT_3_4, -0.5, 0.0, 0.0);
	CVector4 n1 = CVector4(-0.5, -SQRT_3_4, 0.0, 0.0);
	CVector4 n2 = CVector4(-0.5, SQRT_3_4, 0.0, 0.0);
	double innerScale = SQRT_3 / (1.0 + SQRT_3);
	double innerScaleB = innerScale * innerScale * 0.25;

	// for (int i = 0; i < fractal->transformCommon.int8X && dot(z, z) < 0.5; i++)
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
		double invSC = fractal->transformCommon.scaleF1 / z.Dot(z);

		if (z.z < maxH && zD.Dot(zD) > 0.5 * 0.5)
		{
			// needs a sphere inverse
			Dd *= invSC;
			z *= invSC;
			ColV.x += 1.0;
		}
		else
		{
			// stretch onto a plane at zero
			ColV.y += 1.0;
			Dd *= invSC;
			z *= invSC;
			z.z -= 1.0;
			z.z *= -1.0;
			z *= SQRT_3;
			Dd *= SQRT_3;
			z.z += 1.0;

			// and rotate it a twelfth of a revolution pi/6
			double a = M_PI / fractal->transformCommon.scale6;
			double cosA = cos(a);
			double sinA = sin(a);
			double xx = z.x * cosA + z.y * sinA;
			double yy = -z.x * sinA + z.y * cosA;
			z.x = xx;
			z.y = yy;
		}

		// now modolu the space so we move to being in just the central hexagon, inner radius 0.5
		double h = z.z * fractal->transformCommon.scaleE1;
		double x = z.Dot(-1.0 * n2) * fractal->transformCommon.scaleA2 / SQRT_3;
		double y = z.Dot(-1.0 * n1) * fractal->transformCommon.scaleA2 / SQRT_3;
		x = x - floor(x);
		y = y - floor(y);

		if (x + y > 1.0)
		{
			x = 1.0 - x;
			y = 1.0 - y;
			ColV.z += 1.0;
		}

		z = x * t1 - y * t2;
		// fold the space to be in a kite
		double l0 = z.Dot(z);
		CVector4 zt1 = z - t1;
		CVector4 zt2 = z + t2;
		double l1 = zt1.Dot(zt1);
		double l2 = zt2.Dot(zt2);

		if (l1 < l0 && l1 < l2)
		{
			z -= t1 * (2.0 * z.Dot(t1) - 1.0);
		}
		else if (l2 < l0 && l2 < l1)
		{
			z -= t2 * (2.0 * z.Dot(t2) + 1.0);
		}
		z.z = h;
		z *= fractal->transformCommon.scaleD1;
		Dd *= fractal->transformCommon.scaleD1;
		z += fractal->transformCommon.offset000;
	}
	// aux.DE
	aux.DE = Dd;
	CVector4 len = z - CVector4(0.0, 0.0, 0.5, 0.0);
	double d = (len.Length() - 0.5);
	// d = (sqrt(d + 1.0) - 1.0) * 2.0;
	ColV.w = d;
	d /= fractal->analyticDE.scale1 * 2.22 * aux.DE;

	if (!fractal->transformCommon.functionEnabledXFalse)
		aux.dist = min(aux.dist, d);
	else
		aux.dist = d;

	if (fractal->analyticDE.enabledFalse) z = oldZ;

	// aux.color
	if (fractal->foldColor.auxColorEnabled)
	{
		aux.color += ColV.x * fractal->foldColor.difs0000.x + ColV.y * fractal->foldColor.difs0000.y
								 + ColV.z * fractal->foldColor.difs0000.z + ColV.w * fractal->foldColor.difs0000.w;
	}
}
