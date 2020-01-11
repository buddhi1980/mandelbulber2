/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical invert (modes updated v2.15)
 * from M3D. Formula by Luca GN 2011, updated May 2012.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalInv::cFractalTransfSphericalInv() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Invert";
	internalName = "transf_spherical_inv";
	internalID = fractal::transfSphericalInv;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalInv::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;

	if (!fractal->transformCommon.functionEnabledzFalse)
	{
		double r2Inv = 1.0 / z.Dot(z);
		z *= r2Inv;
		aux.DE *= r2Inv;
	}
	else // conditional
	{
		double rr = z.Dot(z);
		z += fractal->mandelbox.offset;
		if (rr < fractal->mandelbox.foldingSphericalFixed)
		{
			double mode = 0.0;
			if (fractal->transformCommon.functionEnabledFalse) // Mode 1 minR0
			{
				if (rr < fractal->transformCommon.minR0) mode = fractal->transformCommon.minR0;
			}
			if (fractal->transformCommon.functionEnabledxFalse) // Mode 2
			{
				if (rr < fractal->transformCommon.minR0) mode = 2.0 * fractal->transformCommon.minR0 - rr;
			}
			mode = 1.0 / mode;
			z *= mode;
			aux.DE *= fabs(mode);
		}
	}
	z -= fractal->mandelbox.offset + fractal->transformCommon.additionConstant000;
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}
