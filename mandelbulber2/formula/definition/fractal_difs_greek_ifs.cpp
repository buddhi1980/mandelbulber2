/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * greek ifs based on Mandelbulber3D.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalDIFSGreekIfs::cFractalDIFSGreekIfs() : cAbstractFractal()
{
	nameInComboBox = "DIFS GreekIFS";
	internalName = "difs_greek_ifs";
	internalID = fractal::dIFSGreekIfs;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalDIFSGreekIfs::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
				&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		if (fractal->transformCommon.functionEnabledAxFalse)
		{
			if (!fractal->transformCommon.functionEnabledBxFalse)
				z.x = fractal->transformCommon.offset000.x - fabs(z.x);
			else
				z.x = sign(z.x) * (fractal->transformCommon.offset000.x - fabs(z.x));
		}

		if (fractal->transformCommon.functionEnabledAyFalse)
		{
			if (!fractal->transformCommon.functionEnabledByFalse)
				z.y = fractal->transformCommon.offset000.y - fabs(z.y);
			else
				z.y = sign(z.y) * (fractal->transformCommon.offset000.y - fabs(z.y));
		}

		if (fractal->transformCommon.functionEnabledAzFalse)
		{
			if (!fractal->transformCommon.functionEnabledBzFalse)
				z.z = fractal->transformCommon.offset000.z - fabs(z.z);
			else
				z.z = sign(z.z) * (fractal->transformCommon.offset000.z - fabs(z.z));
		}
	}

	z *= fractal->transformCommon.scale2;
	aux.DE *= fractal->transformCommon.scale2;
	z += fractal->transformCommon.offsetA000;

	CVector4 zc = z;

	if (zc.x > 0.0 ) zc.y += fractal->transformCommon.offsetA0;

	if (!fractal->transformCommon.functionEnabledMFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
		zc.y = zc.y + sign(zc.x) * fractal->transformCommon.scale05
				+ fractal->transformCommon.offset0;

	if (fractal->transformCommon.functionEnabledNFalse
			&& aux.i >= fractal->transformCommon.startIterationsN
				&& aux.i < fractal->transformCommon.stopIterationsN)
		zc.x = zc.x + sign(zc.y) * fractal->transformCommon.offsetB05
				+ fractal->transformCommon.offsetF0;

	if (!fractal->transformCommon.functionEnabledOFalse)
	{
		if (!fractal->transformCommon.functionEnabledFalse)
			zc.x = max(fabs(zc.x) + fractal->transformCommon.offset05,
				fabs(zc.y) + fractal->transformCommon.offsetA05);
		else
			zc.y = max(fabs(zc.x) + fractal->transformCommon.offset05,
				fabs(zc.y) + fractal->transformCommon.offsetA05);
	}

	if (fractal->transformCommon.functionEnabledCFalse // spiral
			&& aux.i >= fractal->transformCommon.startIterationsC
				&& aux.i < fractal->transformCommon.stopIterationsC)
		zc.x = sqrt((zc.x * zc.x) + (zc.y * zc.y));

	double t = zc.x - round(zc.x);
	if (fractal->transformCommon.functionEnabledBx)
		t = fabs(t) - fractal->transformCommon.offsetF000.x;
	if (fractal->transformCommon.functionEnabledKFalse)
		zc.z = fabs(zc.z) - fractal->transformCommon.offsetF000.y;

	if (fractal->transformCommon.functionEnabledCx)
		t = max(t, fabs(zc.z));
	if (fractal->transformCommon.functionEnabledDFalse)
		t = sqrt(t * t + zc.z * zc.z);
	if (fractal->transformCommon.functionEnabledEFalse)
		t = max(fabs(t) * SQRT_3_4 + zc.z * 0.5, -zc.z);
	if (fractal->transformCommon.functionEnabledFFalse)
	{
		double zA = zc.z;
		if (fractal->transformCommon.functionEnabledCy)
			zA = fabs(zA) - fractal->transformCommon.offsetB0;

		if (fractal->transformCommon.functionEnabledCyFalse)
			zA = -zA + fractal->transformCommon.offsetB0;

		double zB = zc.z;
		if (fractal->transformCommon.functionEnabledCz)
			zB = fabs(zB) - fractal->transformCommon.offsetC0;

		if (fractal->transformCommon.functionEnabledCzFalse)
			zB = -zB + fractal->transformCommon.offsetC0;

		t = max(fabs(t) * SQRT_3_4 + zA * 0.5, zB);
	}

	t -= fractal->transformCommon.offset02;

	double xyR = t;
	if (fractal->transformCommon.functionEnabledGFalse) // hollow
	{
		t = fabs(t) - fractal->transformCommon.offsetp01;
		t = max(t, xyR);
	}

	// plane
//		double colorAdd = 0.0;
//	double g = 0.0;
	if (fractal->transformCommon.functionEnabledRFalse)
	{
		double g = fabs(zc.z - fractal->transformCommon.offsetR0) - fractal->transformCommon.offsetE0;
		t = min(g, t);
		//			if (t == g) colorAdd = 2.0;
	}



	if (fractal->transformCommon.functionEnabledSFalse) // clip
	{
		CVector4 f = fabs(aux.const_c) - fractal->transformCommon.offset333;
		double e = max(max(f.x, f.y), f.z);
		t = max(e, t);
	}

	double colDist = aux.dist;
	aux.dist = min(aux.dist, t / (aux.DE + fractal->analyticDE.offset1));

	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colorAdd = 0.0;
		if (colDist != aux.dist) colorAdd += fractal->foldColor.difs0000.x;

		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (zc.z > fractal->transformCommon.offsetF000.z)
				colorAdd += fractal->foldColor.difs0000.y;
			colorAdd += fabs(zc.z) * fractal->foldColor.difs0000.z;
			colorAdd += zc.z * fractal->foldColor.difs0000.w;
			if (xyR < t) colorAdd += fractal->foldColor.difs0;


		}
		if (fractal->foldColor.auxColorEnabled)
			aux.color += colorAdd;
		else
			aux.color = max(colorAdd, aux.color); // hmmmm test when para neg




	}

}
