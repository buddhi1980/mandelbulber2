/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSOctahedronIteration https://www.shadertoy.com/view/sd2yWW
 * and https://iquilezles.org/articles/distfunctions/
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSOctahedron::cFractalTransfDIFSOctahedron() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Octahedron";
	internalName = "transf_difs_octahedron";
	internalID = fractal::transfDIFSOctahedron;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSOctahedron::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 q = z;
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
	double m = (z.x + z.y + z.z - fractal->transformCommon.offset1) * FRAC_1_3;

	q = z - CVector4(m, m, m, 0.0);
	CVector4 k = z;
	k.x = min(q.x, 0.0);
	k.y = min(q.y, 0.0);
	k.z = min(q.z, 0.0);
	t = (k.x + k.y + k.z) * 0.5;
	if (fractal->transformCommon.functionEnabledBFalse) q = fabs(z);
	q = q + CVector4(t, t, t, 0.0); // - k * 1.5;

	t  = fractal->transformCommon.offset1;
	CVector4 p = q;
	q.x = clamp(q.x, 0.0, t);
	q.y = clamp(q.y, 0.0, t);
	q.z = clamp(q.z, 0.0, t);
	CVector4 o = q;
	q = z - q;
	t = q.Dot(q);
	t = t / sqrt(t);
	double zcd = t * sign(m) - fractal->transformCommon.offset0005;

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	double colDist = aux.dist;
	aux.dist = min(aux.dist, zcd / aux.DE);
	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		if (colDist != aux.dist) aux.color += fractal->foldColor.difs0000.x;
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			t = oldZ.x * oldZ.y;
			if ((t > 0.0 && oldZ.z > 0.0) || (t < 0.0 && oldZ.z < 0.0)) aux.color += fractal->foldColor.difs0000.y;
			if (t > 0.0) aux.color += fractal->foldColor.difs0000.z;
			if (fractal->foldColor.difs0000.w != 0.0)
			{
				p -= o;
				if (p.Dot(p) > 0.0) aux.color += fractal->foldColor.difs0000.w;
			}
		}
	}
}
