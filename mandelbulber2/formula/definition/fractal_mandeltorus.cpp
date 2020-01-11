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

cFractalMandeltorus::cFractalMandeltorus() : cAbstractFractal()
{
	nameInComboBox = "Mandeltorus";
	internalName = "mandeltorus";
	internalID = fractal::mandeltorus;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandeltorus::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1) // pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.DE *= z.Length() / aux.r;
	}

	const double power1 = fractal->transformCommon.pwr8;	// Longitude power, symmetry
	const double power2 = fractal->transformCommon.pwr8a; // Latitude power

	const double rh = sqrt(z.x * z.x + z.z * z.z);
	double rh1 = 0.0;
	double rh2 = 0.0;
	const double phi = atan2(z.z, z.x);
	const double phipow = phi * power1;

	const double theta = atan2(rh, z.y);

	if (!fractal->transformCommon.functionEnabledzFalse) // mode 1
	{
		const double thetapow = theta * power2; // mode1

		const double px = z.x - cos(phi) * 1.5;
		const double pz = z.z - sin(phi) * 1.5;

		const double rhrad = sqrt(px * px + pz * pz + z.y * z.y);

		rh1 = pow(rhrad, power2);
		rh2 = pow(rhrad, power1);

		const double sintheta = sin(thetapow) * rh2; // mode1

		z.x = sintheta * cos(phipow);
		z.z = sintheta * sin(phipow);
		z.y = cos(thetapow) * rh1; // mode 1
	}
	else // mode 2
	{
		const double px = z.x - cos(phi) * 1.5;
		const double pz = z.z - sin(phi) * 1.5;

		const double rhrad = sqrt(px * px + pz * pz + z.y * z.y);

		const double tangle = atan2(sqrt(px * px + pz * pz), z.y) * power2; // mode2

		rh1 = pow(rhrad, power2);
		rh2 = pow(rhrad, power1);

		const double sintheta = (1.5 + cos(tangle)) * rh2; // mode2
		z.x = sintheta * cos(phipow);
		z.z = sintheta * sin(phipow);
		z.y = sin(tangle) * rh1; // mode 2
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
