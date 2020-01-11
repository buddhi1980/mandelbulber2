/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical invert ( v2.17)
 * from M3D. Formula by Luca GN 2011, updated May 2012.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalInvV2::cFractalTransfSphericalInvV2() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Invert V2";
	internalName = "transf_spherical_inv_v2";
	internalID = fractal::transfSphericalInvV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalInvV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double rr = 0.0;
	// unconditional mode
	if (fractal->transformCommon.functionEnabledCz)
	{
		if (aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD1)
		{
			z += fractal->transformCommon.offset000;
			rr = z.Dot(z);
			z *= fractal->transformCommon.maxR2d1 / rr;
			z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
			// double r = sqrt(rr);
			aux.DE = (fractal->transformCommon.maxR2d1) / rr;
		}
	}

	// conditional modes
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		rr = z.Dot(z);
		double mode = rr;
		z += fractal->mandelbox.offset;

		if (rr < fractal->mandelbox.foldingSphericalFixed)
		{
			mode = 0.0;
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
		z -= fractal->mandelbox.offset;
	}

	// other modes
	if (fractal->transformCommon.functionEnabledCyFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		rr = z.Dot(z);
		double mode = rr;
		if (rr < fractal->transformCommon.scaleE1) // < maxRR
		{
			double lengthAB = fractal->transformCommon.scaleE1 - fractal->transformCommon.offsetC0;

			if (fractal->transformCommon.functionEnabledyFalse) // Mode 3a, linear addition 0.0 at Max,
			{
				if (rr < fractal->transformCommon.offsetC0)
					mode += rr * (fractal->transformCommon.offset0 / fractal->transformCommon.offsetC0);
				else
					mode +=
						(fractal->transformCommon.scaleE1 - rr) * fractal->transformCommon.offset0 / lengthAB;
			}

			if (fractal->transformCommon.functionEnabledzFalse) // Mode 3b
			{

				if (rr > fractal->transformCommon.offsetC0)
					mode += fractal->transformCommon.offsetB0 * (fractal->transformCommon.scaleE1 - rr);
				else
					mode += fractal->transformCommon.offsetA0 * (fractal->transformCommon.offsetC0 - rr)
									+ fractal->transformCommon.offsetB0 * lengthAB;
			}

			if (fractal->transformCommon.functionEnabledwFalse) // Mode 3c, basic parabolic curve
			{

				double halfLen = fractal->transformCommon.scaleE1 / 2.0;
				double slope = 2.0 / fractal->transformCommon.scaleE1;
				double factor = slope / fractal->transformCommon.scaleE1;
				double parab = 0.0;

				if (rr < halfLen)
				{
					parab = rr * rr * factor * fractal->transformCommon.scaleG1;
					mode += rr * slope * fractal->transformCommon.scaleF1 - parab;
				}
				else
				{
					double temp = fractal->transformCommon.scaleE1 - rr;
					parab = temp * temp * factor * fractal->transformCommon.scaleG1;
					mode += temp * slope * fractal->transformCommon.scaleF1 - parab;
				}
			}

			/*if (fractal->transformCommon.functionEnabledwFalse) // Mode 3d
			{
				mode = rr + fractal->transformCommon.offset0 * (fractal->mandelbox.foldingSphericalFixed -
			rr);
				if (rr < fractal->transformCommon.minR0)
					mode -= rr * (fractal->transformCommon.offset0
									* (fractal->mandelbox.foldingSphericalFixed - fractal->transformCommon.minR0))
							/ fractal->transformCommon.minR0;
			}*/
			mode = 1.0 / mode;
			z *= mode;
			aux.DE *= fabs(mode);
		}
	}

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}
