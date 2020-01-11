/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MsltoeToroidal
 * @reference http://www.fractalforums.com/theory/toroidal-coordinates/msg9428/
 */

#include "all_fractal_definitions.h"

cFractalMsltoeToroidal::cFractalMsltoeToroidal() : cAbstractFractal()
{
	nameInComboBox = "Msltoe - Toroidal Bulb";
	internalName = "msltoe_toroidal";
	internalID = fractal::msltoeToroidal;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMsltoeToroidal::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1) // pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.DE *= z.Length() / aux.r;
	}

	// Toroidal bulb
	double r1 = fractal->transformCommon.minR05; // default 0.5
	double theta = atan2(z.y, z.x);
	double x1 = r1 * cos(theta);
	double y1 = r1 * sin(theta);
	aux.r = (z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1) + z.z * z.z; //+ 1e-061
	double phi = asin(z.z / sqrt(aux.r));
	double rp = pow(aux.r, fractal->transformCommon.pwr4); // default 4.0

	phi *= fractal->transformCommon.pwr8; // default 8
	theta *= fractal->bulb.power;					// default 9 gives 8 symmetry

	// convert back to cartesian coordinates
	double r1RpCosPhi = r1 + rp * cos(phi);
	z.x = r1RpCosPhi * cos(theta);
	z.y = r1RpCosPhi * sin(theta);
	z.z = -rp * sin(phi);

	// DEcalc
	if (!fractal->analyticDE.enabledFalse)
	{
		aux.DE = rp * aux.DE * (fractal->transformCommon.pwr4 + 1.0) + 1.0;
	}
	else
	{
		aux.DE = rp * aux.DE * (fractal->transformCommon.pwr4 + fractal->analyticDE.offset2)
							 * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
	}

	if (fractal->transformCommon.functionEnabledAxFalse) // spherical offset
	{
		double lengthTempZ = -z.Length();
		// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;
	}
	// then add Cpixel constant vector
}
