/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MsltoeToroidalV3
 * @reference http://www.fractalforums.com/theory/toroidal-coordinates/msg9428/
 */

#include "all_fractal_definitions.h"

cFractalMsltoeToroidalV3::cFractalMsltoeToroidalV3() : cAbstractFractal()
{
	nameInComboBox = "Msltoe - Toroidal Bulb V3";
	internalName = "msltoe_toroidal_v3";
	internalID = fractal::msltoeToroidalV3;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMsltoeToroidalV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1) // pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.DE *= z.Length() / aux.r;
	}

	// Toroidal bulb
	double r1 = fractal->transformCommon.minR05; // default 0.5f
	double theta = atan2(z.y, z.x);
	double x1 = r1 * cos(theta);
	double y1 = r1 * sin(theta);
	double rr = (z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1);
	double r = sqrt(rr + z.z * z.z);
	double temp = r;

	double phi = 0.0;
	if (!fractal->transformCommon.functionEnabledYFalse)
	{
		phi = asin(z.z / temp);
	}
	else
	{
		if (fractal->transformCommon.functionEnabledXFalse
				&& aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			if (!fractal->transformCommon.functionEnabledBFalse)
				temp = rr;
			else
				temp = sqrt(rr);
		}
		phi = atan2(z.z, temp);
	}

	r = r + (aux.r - r) * fractal->transformCommon.offsetR0;

	double rp = pow(r, fractal->bulb.power - 1.0) / fractal->transformCommon.scaleB1;
	aux.DE = rp * aux.DE * (fractal->bulb.power + fractal->analyticDE.offset0) + 1.0;
	rp *= r;

	phi *= fractal->transformCommon.pwr8;
	theta *= fractal->bulb.power;

	// convert back to cartesian coordinates
	if (!fractal->transformCommon.functionEnabledSwFalse)
	{
		temp = r1 + rp * cos(phi);
		z.x = temp * cos(theta);
		z.y = temp * sin(theta);
	}
	else
	{
		temp = rp * cos(phi);
		z.x = (sign(z.x) * x1 + temp) * cos(theta);
		z.y = (sign(z.y) * y1 + temp) * sin(theta);
	}
	z.z = rp * sin(phi);
	z.z *= fractal->transformCommon.scaleNeg1;

	aux.DE *= fractal->analyticDE.scale1;

	if (fractal->transformCommon.functionEnabledAxFalse) // spherical offset
	{
		// double lengthTempZ = -z.Length();
		//  if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 - fractal->transformCommon.offset / z.Length();
		// aux.DE = aux.DE * fabs(1.0 + fractal->transformCommon.offset / -z.Length());
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale);
	}
	// then add Cpixel constant vector

	if (fractal->transformCommon.functionEnabledOFalse)
	{
		aux.DE0 = z.Length();
		if (aux.DE0 > 1.0)
			aux.DE0 = 0.5 * log(aux.DE0) * aux.DE0 / aux.DE;
		else
			aux.DE0 = 0.0;
		if (!fractal->transformCommon.functionEnabledCFalse)
			aux.dist = aux.DE0;
		else
			aux.dist = min(aux.dist, aux.DE0);
	}
}
