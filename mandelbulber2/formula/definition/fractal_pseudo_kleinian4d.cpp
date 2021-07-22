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

cFractalPseudoKleinian4d::cFractalPseudoKleinian4d() : cAbstractFractal()
{
	nameInComboBox = "Pseudo Kleinian - 4D";
	internalName = "pseudo_kleinian4d";
	internalID = fractal::pseudoKleinian4d;
	DEType = analyticDEType;
	// DEFunctionType = pseudoKleinianDEFunction;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	// DEAnalyticFunction = analyticFunctionPseudoKleinian;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalPseudoKleinian4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;

	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		z += fractal->transformCommon.offset0000;
		double rr = z.Dot(z);
		z *= fractal->transformCommon.scaleG1 / rr;
		aux.DE *= (fractal->transformCommon.scaleG1 / rr);
		z += fractal->transformCommon.offsetA0000 - fractal->transformCommon.offset0000;
		z *= fractal->transformCommon.scaleA1;
		aux.DE *= fractal->transformCommon.scaleA1;
	}

	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		if (fractal->transformCommon.functionEnabledAFalse)
		{
			z = CVector4(z.x, z.y, z.z, z.Length());
			aux.DE += 0.5;
		}
		if (fractal->transformCommon.functionEnabledBFalse)
		{
			z = CVector4(z.x + z.y + z.z, -z.x - z.y + z.z, -z.x + z.y - z.z, z.x - z.y - z.z);
			aux.DE *= z.Length() / aux.r;
		}
	}

	// box offset
	if (aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		z.x -= fractal->transformCommon.scale0000.x * sign(z.x);
		z.y -= fractal->transformCommon.scale0000.y * sign(z.y);
		z.z -= fractal->transformCommon.scale0000.z * sign(z.z);
		z.w -= fractal->transformCommon.scale0000.w * sign(z.w);
	}

	double k = 0.0;
	// Pseudo kleinian
	CVector4 cSize = fractal->transformCommon.offset1111;
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterations50)
	{
		z = fabs(z + cSize) - fabs(z - cSize) - z;
		k = max(fractal->transformCommon.scale1 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
		aux.pseudoKleinianDE = fractal->analyticDE.scale1;
	}

	if (fractal->transformCommon.functionEnabledGFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		z.x += aux.pos_neg * fractal->transformCommon.additionConstant0000.x;
		z.y += aux.pos_neg * fractal->transformCommon.additionConstant0000.y;
		z.z += aux.pos_neg * fractal->transformCommon.additionConstant0000.z;
		z.w += aux.pos_neg * fractal->transformCommon.additionConstant0000.w;
		aux.pos_neg *= fractal->transformCommon.scaleNeg1;
	}

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		z = fabs(z + fractal->transformCommon.offsetB1111)
				- fabs(z - fractal->transformCommon.offsetB1111) - z;
	}

	if (fractal->transformCommon.rotation2EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	CVector4 zz = z * z;
	double d1 = 0.0;
	if (!fractal->transformCommon.functionEnabledKFalse) d1 = sqrt(zz.x + zz.y + zz.w);
	else d1 = sqrt(min(min(min(zz.x + zz.y, zz.y + zz.z), zz.z + zz.w), zz.w + zz.x));
	d1 -= fractal->transformCommon.offsetR0;

	double d2 = fabs(z.z);
	aux.DE0 = d2;
	if( d1 < d2) aux.DE0 = d1;

	aux.DE0 = 0.5 * (aux.DE0 - fractal->transformCommon.offset0) / aux.DE;

	if (fractal->transformCommon.functionEnabledDFalse)
		aux.DE0 = min(aux.dist, aux.DE0);

	aux.dist = aux.DE0;

	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (aux.i >= fractal->foldColor.startIterationsA
				&& aux.i < fractal->foldColor.stopIterationsA)
		{
			colorAdd += fractal->foldColor.difs0000.x * fabs(z.x);
			colorAdd += fractal->foldColor.difs0000.y * fabs(z.y);
			colorAdd += fractal->foldColor.difs0000.z * fabs(z.z);
			//colorAdd += fractal->foldColor.difs0000.w * k;
			colorAdd += fractal->foldColor.difs0000.w * fabs(z.w);

		}
		aux.color += colorAdd;
	}
}
