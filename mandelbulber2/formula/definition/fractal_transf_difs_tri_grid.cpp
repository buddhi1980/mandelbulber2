/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * based on M3D tritgridIFS
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSTriGrid::cFractalTransfDIFSTriGrid() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Tri Grid";
	internalName = "transf_difs_tri_grid";
	internalID = fractal::transfDIFSTriGrid;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSTriGrid::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.rotationEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR1)
				z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	CVector4 zc = z;
	CVector4 off = fractal->transformCommon.offset111;
	double a;
	if (!fractal->transformCommon.functionEnabledAFalse)
		a = fabs(zc.x - off.x * floor(zc.x / off.x  + 0.5));
	else a = 1000.0f;

	zc.x = zc.x * 0.5;
	zc.y = zc.y * SQRT_3 * 0.5;
	double b = zc.x + zc.y;
	b = fabs(b - off.y * floor(b / off.y  + 0.5));
	double c = zc.x - zc.y;
	c = fabs(c - off.z * floor(c / off.z  + 0.5));
	double tp = min(min(a, b), c) - fractal->transformCommon.offset0;

	// plane
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		double d = fabs(aux.const_c.z) + fractal->transformCommon.offsetA0;
		tp = min(tp, d);
		if (tp == d) aux.color = fractal->foldColor.difs1;
	}

	// clip plane
	if (fractal->transformCommon.functionEnabledCFalse)
	{
		double e = fractal->transformCommon.offset4;
		CVector4 f = fabs(aux.const_c);
		f -= CVector4(e, e, e, 0.0);
		e = max(f.x, max(f.y, f.z));
		tp = max(tp, e);
	}

	if (!fractal->transformCommon.functionEnabledJFalse)
		aux.DE0 = sqrt((tp * tp) + (zc.z * zc.z));
	else
		aux.DE0 = max(fabs(tp), fabs(zc.z));

	aux.dist = min(aux.dist, (aux.DE0 - fractal->transformCommon.offset0005)
					/ (aux.DE + fractal->analyticDE.offset1));
}
