/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsCylinderIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
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

	CVector4 o = z - CVector4(m, m, m, 0.0);

	CVector4 k = z;
	k.x = min(o.x, 0.0);
	k.y = min(o.y, 0.0);
	k.z = min(o.z, 0.0);

	t = (k.x + k.y + k.z) * 0.5;
	o = o + CVector4(t, t, t, 0.0); // - k * fractal->transformCommon.scale015;

	if (!fractal->transformCommon.functionEnabledAFalse)
		t  = fractal->transformCommon.offset1;
	else
		t = fractal->transformCommon.offsetA1;

	o.x = clamp(o.x, 0.0, t);
	o.y = clamp(o.y, 0.0, t);
	o.z = clamp(o.z, 0.0, t);

	CVector4 v = z - o;
	t = v.Dot(v);
	double v2Rsqrt = t / sqrt(t);
	double zcd = v2Rsqrt * sign(m) - fractal->transformCommon.offset0;

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	double colDist = aux.dist;
	aux.dist = min(aux.dist, zcd / aux.DE);

	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		if (colDist != aux.dist) aux.color += fractal->foldColor.difs0000.x;
	}
}
