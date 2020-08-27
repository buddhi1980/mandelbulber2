/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Mandeltorus by Aexion
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/mandeldonuts/
 */

#include "all_fractal_definitions.h"

cFractalMandeltorusV2::cFractalMandeltorusV2() : cAbstractFractal()
{
	nameInComboBox = "Mandeltorus V2";
	internalName = "mandeltorus_v2";
	internalID = fractal::mandeltorusV2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandeltorusV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1) // pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.DE *= z.Length() / aux.r;
	}


	const double power1 = fractal->bulb.power; // Longitude power, symmetry
	const double power2 = fractal->transformCommon.pwr8a; // Latitude power

	double off1p5 = 1.0;

	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		off1p5 = fractal->transformCommon.offset105;
	}

	const double rh = sqrt(z.x * z.x + z.y * z.y);

	const double phi = atan2(z.y, z.x) + fractal->transformCommon.offset0;
	const double phipow = phi * power1;

	const double theta = atan2(rh, z.z);

	const double px = z.x - cos(phi) * off1p5;
	const double py = z.y - sin(phi) * off1p5;
	const double rhrad = sqrt(px * px + py * py + z.z * z.z);
	double rh1 = pow(rhrad, power2);
	double rh2 = pow(rhrad, power1);

	if (!fractal->transformCommon.functionEnabledzFalse) // mode 1
	{
		const double thetapow = theta * power2;
		const double sintheta = sin(thetapow) * rh2;
		z.x = sintheta * cos(phipow);
		z.y = sintheta * sin(phipow);
		z.z = cos(thetapow) * rh1; // mode 1
	}
	else // mode 2
	{
		const double tangle = atan2(sqrt(px * px + py * py), z.z) * power2;
		const double sintheta = (1.5 + cos(tangle)) * rh2;
		z.x = sintheta * cos(phipow);
		z.y = sintheta * sin(phipow);
		z.z = sin(tangle) * rh1; // mode 2
	}

	// DEcalc
	double temp = rh2 * (power1 - fractal->analyticDE.offset2);
	if (fractal->transformCommon.functionEnabledAyFalse)
		temp = min(temp, rh1 * (power2 - fractal->analyticDE.offset2));

	if (!fractal->analyticDE.enabledFalse)
	{
		aux.DE = temp * aux.DE + 1.0;
	}
	else
	{
		aux.DE = temp * aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
}
