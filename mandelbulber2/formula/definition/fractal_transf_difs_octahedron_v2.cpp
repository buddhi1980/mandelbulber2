/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSOctahedronV2Iteration https://www.shadertoy.com/view/sd2yWW
 * and https://iquilezles.org/articles/distfunctions/
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSOctahedronV2::cFractalTransfDIFSOctahedronV2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Octahedron V2";
	internalName = "transf_difs_octahedron_v2";
	internalID = fractal::transfDIFSOctahedronV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSOctahedronV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 q = z;
	double addCol = 0.0f;
	if (fractal->transformCommon.functionEnabledFFalse)
	{
		CVector4 h = fractal->transformCommon.offsetF000;
		q.x = clamp(q.x, -h.x, h.x);
		q.y = clamp(q.y, -h.y, h.y);
		q.z = clamp(q.z, -h.z, h.z);
		z = z - q;
	}

	CVector4 oldZ = z;
	double t = 0.0;
	z *= fractal->transformCommon.scaleA1;
	aux.DE *= fractal->transformCommon.scaleA1;

	z = fabs(z);
	double m = 0.0;
	if (!fractal->transformCommon.functionEnabledAFalse)
	{
		m = (z.x + z.y + z.z - fractal->transformCommon.offset1) * FRAC_1_3;
	}
	else
	{
		z -= fractal->transformCommon.offset000;
		m = (z.x + z.y + z.z - fractal->transformCommon.offset1) * FRAC_1_3;
		m = m * m * fractal->transformCommon.scale1;
	}

	q = z - CVector4(m, m, m, 0.0);
	CVector4 k = z;
	k.x = min(q.x, 0.0);
	k.y = min(q.y, 0.0);
	k.z = min(q.z, 0.0);
	t = (k.x + k.y + k.z) * 0.5;
	if (fractal->transformCommon.functionEnabledBFalse) q = fabs(z);
	q = q + CVector4(t, t, t, 0.0); // - k * fractal->transformCommon.scale015;

	if (!fractal->transformCommon.functionEnabledAFalse)
		t  = fractal->transformCommon.offset1;
	else
		t = fractal->transformCommon.offsetA1;
	CVector4 p = q;
	q.x = clamp(q.x, 0.0, t);
	q.y = clamp(q.y, 0.0, t);
	q.z = clamp(q.z, 0.0, t);
	CVector4 o = q;
	q = z - q;
	t = q.Dot(q);
	t = t / sqrt(t);
	double zcd = t * sign(m) - fractal->transformCommon.offset0005;
	addCol = fractal->foldColor.difs0000.x; // octhed color

	// box
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		CVector4 zc = aux.const_c;
		zc = fabs(zc) - fractal->transformCommon.additionConstant0555;
		zc.x = max(zc.x, 0.0);
		zc.y = max(zc.y, 0.0);
		zc.z = max(zc.z, 0.0);
		double zcb = zc.Length() + min(max(max(zc.x, zc.y), zc.z), 0.0);
		if (zcb < zcd)
		{
			zcd = zcb;
			addCol = fractal->transformCommon.offset4; // box color
		}
	}

	// sphere
	if (fractal->transformCommon.functionEnabledEFalse)
	{
		double zcs = aux.const_c.Length() - fractal->transformCommon.scale08;
		if (zcs < zcd)
		{
			zcd = zcs;
			addCol = fractal->transformCommon.offset2; // sphere color
		}
	}

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	aux.dist = min(aux.dist, zcd / aux.DE);
	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{

		/*if (fractal->foldColor.auxColorEnabledAFalse)
		{
			t = oldZ.x * oldZ.y;
			if ((t > 0.0 && oldZ.z > 0.0) || (t < 0.0 && oldZ.z < 0.0)) aux.color += fractal->foldColor.difs0000.y;
			if (t > 0.0) aux.color += fractal->foldColor.difs0000.z;
			if (fractal->foldColor.difs0000.w != 0.0)
			{
				p -= o;
				if (p.Dot(p) > 0.0) aux.color += fractal->foldColor.difs0000.w;
			}
		}*/
		aux.color += addCol;
	}
}
