/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * JosLeys-Kleinian V2 formula
 * @reference
 * http://www.fractalforums.com/3d-fractal-generation/an-escape-tim-algorithm-for-kleinian-group-limit-sets/msg98248/#msg98248
 * This formula contains aux.color and aux.DE
 */

#include "all_fractal_definitions.h"

cFractalJosKleinianV2::cFractalJosKleinianV2() : cAbstractFractal()
{
	nameInComboBox = "JosLeys-Kleinian V2";
	internalName = "jos_kleinian_v2";
	internalID = fractal::josKleinianV2;
	DEType = analyticDEType;
	DEFunctionType = josKleinianDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionJosKleinian;
	coloringFunction = coloringFunctionDefault;
}

void cFractalJosKleinianV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		double rr = 1.0;
		z += fractal->transformCommon.offset000;
		rr = z.Dot(z);
		z *= fractal->transformCommon.maxR2d1 / rr;
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleA1;
		// double r = sqrt(rr);
		aux.DE *= (fractal->transformCommon.maxR2d1 / rr) * fractal->analyticDE.scale1
							* fractal->transformCommon.scaleA1;
	}

	if (fractal->transformCommon.functionEnabledCyFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		CVector4 oldZ = z;
		CVector4 trigZ = CVector4(0.0, 0.0, 0.0, 0.0);
		CVector4 scaleZ = z * fractal->transformCommon.constantMultiplierC111;

		if (fractal->transformCommon.functionEnabledAx)
		{
			if (!fractal->transformCommon.functionEnabledAxFalse)
				trigZ.x = sin(scaleZ.x);
			else
				trigZ.x = cos(scaleZ.x); // scale =0, cos = 1
		}
		if (fractal->transformCommon.functionEnabledAy)
		{
			if (!fractal->transformCommon.functionEnabledAyFalse)
				trigZ.y = sin(scaleZ.y);
			else
				trigZ.y = cos(scaleZ.y);
		}
		if (fractal->transformCommon.functionEnabledAz)
		{
			if (!fractal->transformCommon.functionEnabledAzFalse)
				trigZ.z = sin(scaleZ.z);
			else
				trigZ.z = cos(scaleZ.z);
		}

		z = trigZ * fractal->transformCommon.scale;
		if (fractal->transformCommon.functionEnabledFalse)
		{
			z.x = z.x * fractal->transformCommon.scale / (fabs(oldZ.x) + 1.0);
			z.y = z.y * fractal->transformCommon.scale / (fabs(oldZ.y) + 1.0);
			z.z = z.z * fractal->transformCommon.scale / (fabs(oldZ.z) + 1.0);
			// aux.DE = aux.DE * z.Length() / oldZ.Length();
		}
	}

	if (fractal->transformCommon.functionEnabledJFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (z.z > z.x) swap(z.x, z.z);
	}

	// kleinian
	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		double a = fractal->transformCommon.foldingValue;
		double b = fractal->transformCommon.offset;
		double c = fractal->transformCommon.offsetA0;
		double f = sign(b);

		// wrap
		CVector4 box_size = fractal->transformCommon.offset111;
		CVector3 box1 = CVector3(2.0 * box_size.x, a * box_size.y, 2.0 * box_size.z);
		CVector3 box2 = CVector3(-box_size.x, -box_size.y + 1.0, -box_size.z);
		CVector3 wrapped = wrap(z.GetXYZ(), box1, box2);

		z = CVector4(wrapped.x, wrapped.y, wrapped.z, z.w);
		/*{
			z.x += box_size.x;
			z.z += box_size.z;
			z.x = z.x - 2.0 * box_size.x * floor(z.x / 2.0 * box_size.x) - box_size.x;
			z.z = z.z - 2.0 * box_size.z * floor(z.z / 2.0 * box_size.z) - box_size.z;
			z.y += box_size.y - 1.0;
			z.y = z.y - a * box_size.y * floor(z.y / a * box_size.y);
			z.y -= (box_size.y - 1.0);
		}*/

		if (z.y >= a * (0.5 + 0.2 * sin(f * M_PI * (z.x + b * 0.5) / box_size.x)))
		{
			z.x = -z.x - b;
			z.y = -z.y + a;
			z.z = -z.z - c;
		}

		double rr = z.Dot(z);

		CVector4 colorVector = CVector4(z.x, z.y, z.z, rr);
		aux.color = min(aux.color, colorVector.Length()); // For coloring

		double iR = 1.0 / rr;
		z *= -iR; // invert and mirror
		z.x = -z.x - b;
		z.y = a + z.y;
		z.z = -z.z - c;

		aux.DE *= fabs(iR);
	}

	if (fractal->transformCommon.functionEnabledEFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		z.z = sign(z.z)
					* (fractal->transformCommon.offset1 - fabs(z.z)
						 + fabs(z.z) * fractal->transformCommon.scale0);
	}
}
