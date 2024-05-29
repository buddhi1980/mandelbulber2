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
	CVector4 oldZ = z;
	double RR = 1.0;
	z += fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;

	if (!fractal->transformCommon.functionEnabledyFalse) RR = z.Dot(z);
	else RR = oldZ.Dot(oldZ);

	if (!fractal->transformCommon.functionEnabledzFalse)
	{
		RR = 1.0 / RR;
		z *= RR;
		aux.DE *= RR;
	}
	else // conditional
	{
		double mde = RR;
		z += fractal->transformCommon.offset000;
		//if (RR < fractal->mandelbox.foldingSphericalFixed)
		{
			//RR = fractal->mandelbox.foldingSphericalFixed;
			if (fractal->transformCommon.functionEnabledFalse) // Mode 1 minR0
			{
				if (RR < fractal->mandelbox.foldingSphericalFixed && RR < fractal->transformCommon.minR0)
					mde = fractal->transformCommon.minR0;

			}
			if (fractal->transformCommon.functionEnabledxFalse) // Mode 2
			{
				if (RR < fractal->mandelbox.foldingSphericalFixed && RR < fractal->transformCommon.minR0)
					mde = 2.0 * fractal->transformCommon.minR0 - RR;
			}

			RR = 1.0 / mde;
			z *= RR;
			aux.DE *= fabs(RR);
			z -= fractal->transformCommon.offset000;
		}
	}
	z -= fractal->mandelbox.offset + fractal->transformCommon.additionConstant000;

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux.color += RR * fractal->transformCommon.scale0;
	}
}
