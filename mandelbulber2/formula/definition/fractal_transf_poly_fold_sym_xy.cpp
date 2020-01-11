/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * poly fold sym DarkBeam's version
 * @reference
 * DarkBeam (luca) http://www.fractalforums.com/mandelbulber/
 * _polyfold_sym-and-polyfoldsymifs-in-mandelbulber-2/msg98162/#msg98162
 */

#include "all_fractal_definitions.h"

cFractalTransfPolyFoldSymXY::cFractalTransfPolyFoldSymXY() : cAbstractFractal()
{
	nameInComboBox = "T>Poly Fold Sym XY";
	internalName = "transf_poly_fold_sym_xy";
	internalID = fractal::transfPolyFoldSymXY;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfPolyFoldSymXY::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	// pre abs
	if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);

	int order = fractal->transformCommon.int6;
	double div2PI = (double)order / M_PI_2x;

	bool cy = false;
	int sector;
	if (!fractal->transformCommon.functionEnabledFalse)
		sector = (int)(-div2PI * atan(z.x / z.y));
	else
		sector = (int)(-div2PI * atan2(z.x, z.y));

	if (sector & 1) cy = true; // parity   if (sector & 1) is a "bit check", true = odd
	double angle = (double)(sector / div2PI);
	// z.xy = rotate(z.xy,angle); // sin
	double tempZx = z.x;
	z.x = z.x * cos(angle) - z.y * sin(angle);
	z.y = tempZx * sin(angle) + z.y * cos(angle);
	if (cy) z.y = -z.y;
	// if ((order&1) && (sector == 0)) z.y = fabs(z.y); // more continuous?

	// addition constant
	z.x += fractal->transformCommon.offset0;
	z.y += fractal->transformCommon.offsetA0;

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}
