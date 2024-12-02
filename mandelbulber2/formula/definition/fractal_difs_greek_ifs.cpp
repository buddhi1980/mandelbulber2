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
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			z.x = sign(z.x)
						* (fractal->transformCommon.offset000.x - fabs(z.x));
		}
		if (fractal->transformCommon.functionEnabledByFalse)
		{
			z.y = sign(z.y)
						* (fractal->transformCommon.offset000.y - fabs(z.y));
		}
		if (fractal->transformCommon.functionEnabledBzFalse)
		{
			z.z = sign(z.z)
						* (fractal->transformCommon.offset000.z - fabs(z.z));
		}
	}

	z *= fractal->transformCommon.scale2;
	aux.DE *= fractal->transformCommon.scale2;
	z += fractal->transformCommon.offsetA000;

	// z = fabs(z);
	CVector4 zc = z;
	if (!fractal->transformCommon.functionEnabledMFalse
			&& aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
		zc.y = zc.y + sign(zc.x) * fractal->transformCommon.scale05 + fractal->transformCommon.offset0;


	if (fractal->transformCommon.functionEnabledNFalse
			&& aux.i >= fractal->transformCommon.startIterationsN
				&& aux.i < fractal->transformCommon.stopIterationsN)
		zc.x = zc.x + sign(zc.y) * fractal->transformCommon.offsetB05 + fractal->transformCommon.offsetF0;

	if (!fractal->transformCommon.functionEnabledOFalse)
		zc.x = max(fabs(zc.x) + fractal->transformCommon.offset05,
			fabs(zc.y) + fractal->transformCommon.offsetA05);
	else
		zc.y = max(fabs(zc.x) + fractal->transformCommon.offset05,
			fabs(zc.y) + fractal->transformCommon.offsetA05);



	if (fractal->transformCommon.functionEnabledCFalse)
		zc.x = sqrt((zc.x * zc.x) + (zc.y * zc.y)); // circ

	double t = zc.x - round(zc.x);

	t = fabs(t) - fractal->transformCommon.offsetB0;

	if (!fractal->transformCommon.functionEnabledDFalse)
	{
		if (!fractal->transformCommon.functionEnabledEFalse)
			t = max(t, fabs(zc.z)); // sq
		else
		{
			double zz = (zc.z);
			if (fractal->transformCommon.functionEnabledFFalse) zz = fabs(zz);
			zz = (zz) - fractal->transformCommon.offsetC0;
			t = max(fabs(t) * SQRT_3_4 + zz * 0.5, -zz);

		}
	}
	else
		t = sqrt(t * t + zc.z * zc.z); // circ


	t -= fractal->transformCommon.offset02;


	if (fractal->transformCommon.functionEnabledGFalse) // hollow
	{
		double xyR = t;
		xyR = fabs(xyR) - fractal->transformCommon.offsetp01;
		t = max(t, xyR);
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
	//		if (t == f.x) colorAdd += fractal->foldColor.difs0000.y;
	//		if (t == f.y) colorAdd += fractal->foldColor.difs0000.z;
	//		if (t == t) aux.color += fractal->foldColor.difs0000.w;
			//if (xyR <= -fractal->transformCommon.offsetp01)
			//	colorAdd += fractal->foldColor.difs0000.z;
		//	if (fractal->transformCommon.offsetA1 - fractal->foldColor.difs0 < fabs(zc.z))
			//	colorAdd += fractal->foldColor.difs0000.w;
		}
		if (fractal->foldColor.auxColorEnabled)
			aux.color = colorAdd;
		else
			aux.color += colorAdd;




	}

}
