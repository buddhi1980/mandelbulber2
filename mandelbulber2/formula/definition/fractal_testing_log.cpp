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

cFractalTestingLog::cFractalTestingLog() : cAbstractFractal()
{
	nameInComboBox = "Testing Log";
	internalName = "testing_log";
	internalID = fractal::testingLog;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingLog::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
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
	if (fractal->transformCommon.functionEnabledAy
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z = fabs(z + cSize) - fabs(z - cSize) - z;
		k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k + fractal->analyticDE.tweak005;
	}
				aux.pseudoKleinianDE = fractal->analyticDE.scale1;
	z += fractal->transformCommon.additionConstant0000;//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

	if (fractal->transformCommon.functionEnabledGFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		z.x += aux.pos_neg * fractal->transformCommon.additionConstantA000.x;
		z.y += aux.pos_neg * fractal->transformCommon.additionConstantA000.y;
		z.z += aux.pos_neg * fractal->transformCommon.additionConstantA000.z;

		aux.pos_neg *= fractal->transformCommon.scaleNeg1;
	}


	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		z = fabs(z + fractal->transformCommon.offsetA000)
				- fabs(z - fractal->transformCommon.offsetA000) - z;
	}
	if (fractal->transformCommon.addCpixelEnabledFalse) // symmetrical addCpixel
	{
		CVector4 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.offsetF000;
		z.x -= sign(z.x) * tempFAB.x;
		z.y -= sign(z.y) * tempFAB.y;
		z.z -= sign(z.z) * tempFAB.z;
	}

	CVector4 zz = z * z;
	double d1 = sqrt(min(min( zz.x + zz.y, zz.y + zz.z), zz.z + zz.x));
	if (fractal->transformCommon.functionEnabledKFalse) d1 = sqrt(zz.x + zz.y);
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
		colorAdd += fractal->foldColor.difs0000.x * fabs(z.x);
		colorAdd += fractal->foldColor.difs0000.y * fabs(z.y);
		colorAdd += fractal->foldColor.difs0000.z * fabs(z.z);
		colorAdd += fractal->foldColor.difs0000.w * k;

		aux.color += colorAdd;
	}
}
