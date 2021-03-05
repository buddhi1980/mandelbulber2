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

cFractalJosKleinianV3::cFractalJosKleinianV3() : cAbstractFractal()
{
	nameInComboBox = "JosLeys-Kleinian V3";
	internalName = "jos_kleinian_v3";
	internalID = fractal::josKleinianV3;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

/*CVector4 PolyfoldAbs(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);

		if (fractal->transformCommon.functionEnabledCx)
		{
			double psi = M_PI / fractal->transformCommon.int8X;
			psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
			double len = sqrt(z.x * z.x + z.y * z.y);
			z.x = cos(psi) * len;
			z.y = sin(psi) * len;
		}

		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			double psi = M_PI / fractal->transformCommon.int8Y;
			psi = fabs(fmod(atan2(z.z, z.y) + psi, 2.0 * psi) - psi);
			double len = sqrt(z.y * z.y + z.z * z.z);
			z.y = cos(psi) * len;
			z.z = sin(psi) * len;
		}

		if (fractal->transformCommon.functionEnabledCzFalse)
		{
			double psi = M_PI / fractal->transformCommon.int8Z;
			psi = fabs(fmod(atan2(z.x, z.z) + psi, 2.0 * psi) - psi);
			double len = sqrt(z.z * z.z + z.x * z.x);
			z.z = cos(psi) * len;
			z.x = sin(psi) * len;
		}
		// addition constant
		z += fractal->transformCommon.offsetF000;
	}
	return z;
}*/

void cFractalJosKleinianV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (!fractal->transformCommon.functionEnabledIFalse)
	{
		if (fractal->transformCommon.functionEnabledPFalse
				&& aux.i >= fractal->transformCommon.startIterationsP
				&& aux.i < fractal->transformCommon.stopIterationsP1)
		{
			if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
			if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
			if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);

			if (fractal->transformCommon.functionEnabledCx)
			{
				double psi = M_PI / fractal->transformCommon.int8X;
				psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
				double len = sqrt(z.x * z.x + z.y * z.y);
				z.x = cos(psi) * len;
				z.y = sin(psi) * len;
			}

			if (fractal->transformCommon.functionEnabledCyFalse)
			{
				double psi = M_PI / fractal->transformCommon.int8Y;
				psi = fabs(fmod(atan2(z.z, z.y) + psi, 2.0 * psi) - psi);
				double len = sqrt(z.y * z.y + z.z * z.z);
				z.y = cos(psi) * len;
				z.z = sin(psi) * len;
			}

			if (fractal->transformCommon.functionEnabledCzFalse)
			{
				double psi = M_PI / fractal->transformCommon.int8Z;
				psi = fabs(fmod(atan2(z.x, z.z) + psi, 2.0 * psi) - psi);
				double len = sqrt(z.z * z.z + z.x * z.x);
				z.z = cos(psi) * len;
				z.x = sin(psi) * len;
			}
			// addition constant
			z += fractal->transformCommon.offsetF000;
		}
	}

	double rr = 0.0;
	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		z += fractal->transformCommon.offset000;
		rr = z.Dot(z);
		z *= fractal->transformCommon.maxR2d1 / rr;
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleA1;
		aux.DE *= (fractal->transformCommon.maxR2d1 / rr) * fractal->analyticDE.scale1
							* fractal->transformCommon.scaleA1;
	}

	if (fractal->transformCommon.functionEnabledNFalse
			&& aux.i >= fractal->transformCommon.startIterationsO
			&& aux.i < fractal->transformCommon.stopIterationsO)
	{
		if (fractal->transformCommon.functionEnabledAwFalse)
			z.x -= round(z.x / fractal->transformCommon.offset2)
					* fractal->transformCommon.offset2;
		if (fractal->transformCommon.functionEnabledAw)
			z.y -= round(z.y / fractal->transformCommon.offsetA2)
					* fractal->transformCommon.offsetA2;
	}

	// diagonal fold
	if (fractal->transformCommon.functionEnabledCFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		if (z.y > z.x) swap(z.x, z.y);
	}

	// alternative polyfold position
	if (fractal->transformCommon.functionEnabledIFalse)
	{
		if (fractal->transformCommon.functionEnabledPFalse
				&& aux.i >= fractal->transformCommon.startIterationsP
				&& aux.i < fractal->transformCommon.stopIterationsP1)
		{
			if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
			if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
			if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);

			if (fractal->transformCommon.functionEnabledCx)
			{
				double psi = M_PI / fractal->transformCommon.int8X;
				psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
				double len = sqrt(z.x * z.x + z.y * z.y);
				z.x = cos(psi) * len;
				z.y = sin(psi) * len;
			}

			if (fractal->transformCommon.functionEnabledCyFalse)
			{
				double psi = M_PI / fractal->transformCommon.int8Y;
				psi = fabs(fmod(atan2(z.z, z.y) + psi, 2.0 * psi) - psi);
				double len = sqrt(z.y * z.y + z.z * z.z);
				z.y = cos(psi) * len;
				z.z = sin(psi) * len;
			}

			if (fractal->transformCommon.functionEnabledCzFalse)
			{
				double psi = M_PI / fractal->transformCommon.int8Z;
				psi = fabs(fmod(atan2(z.x, z.z) + psi, 2.0 * psi) - psi);
				double len = sqrt(z.z * z.z + z.x * z.x);
				z.z = cos(psi) * len;
				z.x = sin(psi) * len;
			}
			// addition constant
			z += fractal->transformCommon.offsetF000;
		}
	}

	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		// square
		if (fractal->transformCommon.functionEnabledBx) z.x = max(fabs(z.x), fabs(z.y));
		// circle
		if (fractal->transformCommon.functionEnabledOFalse) z.x = sqrt((z.x * z.x) + (z.y * z.y));
	}

	// kleinian
	if (aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		CVector4 box_size = fractal->transformCommon.offset111;
		double a = fractal->transformCommon.foldingValue;
		double b = fractal->transformCommon.offset;
		double f = sign(b);
		z.x += box_size.x;
		z.y += box_size.y;
		z.x = z.x - 2.0 * box_size.x * floor(z.x / 2.0 * box_size.x) - box_size.x;
		z.y = z.y - 2.0 * box_size.y * floor(z.y / 2.0 * box_size.y) - box_size.y;
		z.z += box_size.z - 1.0;
		z.z = z.z - a * box_size.z * floor(z.z / a * box_size.z);
		z.z -= (box_size.z - 1.0);
		if (z.z >= a * (0.5 + 0.2 * sin(f * M_PI * (z.x + b * 0.5) / box_size.x)))
		{
			z.x = -z.x - b;
			z.z = -z.z + a;
		}

		double useScale = (1.0 - aux.actualScaleA);
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) * fractal->transformCommon.scale1;
		if (fractal->transformCommon.functionEnabledKFalse) // update actualScaleA
			aux.actualScaleA = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) + 1.0);

		rr = z.Dot(z);
		double iR = 1.0 / rr;
		aux.DE *= iR;
		z *= -iR; // invert and mirror
		z.x = -z.x - b;
		z.z = a + z.z;
	}

	if (fractal->transformCommon.functionEnabledEFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		z.y = sign(z.y)
					* (fractal->transformCommon.offset1 - fabs(z.y)
						 + fabs(z.y) * fractal->transformCommon.scale0);
	}

	double Ztemp = z.z;
	if (aux.i >= fractal->transformCommon.startIterationsG)
	{
		if (fractal->transformCommon.spheresEnabled)
			Ztemp = min(z.z, fractal->transformCommon.foldingValue - z.z);
		aux.dist =
			min(Ztemp + fractal->analyticDE.offset0, fractal->analyticDE.tweak005)
			/ max(aux.DE, fractal->analyticDE.offset1);
	}

	if (fractal->transformCommon.functionEnabledTFalse
			&& aux.i >= fractal->transformCommon.startIterationsT)
				z.z = Ztemp;


	// aux.color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		double colorAdd = 0.0;
		aux.temp1000 = min(aux.temp1000, rr) * fractal->foldColor.difs0000.x;
		colorAdd += aux.temp1000; // temp
		colorAdd += fractal->foldColor.difs0000.y * max(fabs(z.x), fabs(z.y));
		colorAdd += fractal->foldColor.difs0000.z * z.z;
		colorAdd += fractal->foldColor.difs0000.w * rr;
		//colorAdd += fractal->foldColor.difs1;

		if (!fractal->transformCommon.functionEnabledJFalse)
			if (!fractal->transformCommon.functionEnabledMFalse)
				aux.color = colorAdd;
			else
				aux.color += colorAdd;
		else
			aux.color = max(aux.color, colorAdd);

	}
}
