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

cFractalMsltoeToroidalV2::cFractalMsltoeToroidalV2() : cAbstractFractal()
{
	nameInComboBox = "Msltoe - Toroidal Bulb V2";
	internalName = "msltoe_toroidal_v2";
	internalID = fractal::msltoeToroidalV2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMsltoeToroidalV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1) // pre-scale
	{
		z *= fractal->transformCommon.scale3D111;
		aux.DE *= z.Length() / aux.r;
	}

	double rp;

	if (!fractal->transformCommon.functionEnabledAFalse)
	{	// Toroidal bulb
		double r1 = fractal->transformCommon.minR05; // default 0.5
		double theta = atan2(z.y, z.x);
		double x1 = r1 * cos(theta);
		double y1 = r1 * sin(theta);
		aux.r = (z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1) + z.z * z.z; //+ 1e-061
		double phi = asin(z.z / sqrt(aux.r));
		rp = pow(aux.r, fractal->transformCommon.pwr4); // default 4.0

		phi *= fractal->transformCommon.pwr8; // default 8
		theta *= fractal->bulb.power; // default 9 gives 8 symmetry

		// convert back to cartesian coordinates
		double r1RpCosPhi = r1 + rp * cos(phi);
		z.x = r1RpCosPhi * cos(theta);
		z.y = r1RpCosPhi * sin(theta);
		z.z = -rp * sin(phi);
		aux.DE = rp * aux.DE * (fractal->transformCommon.pwr4 + 1.0) + 1.0; //
	}
	else
	{
		double temp;

		// Toroidal bulb
		double r1 = fractal->transformCommon.minR05; // default 0.5f
		double theta = atan2(z.y, z.x);
		double x1 = r1 * cos(theta);
		double y1 = r1 * sin(theta);
		double rr = (z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1);
		double r = rr + z.z * z.z;
		r = sqrt(r);
		aux.DE *= aux.r / r;
temp = r;

		/*double rr = sqrt(z.x * z.x + z.y * z.y) - r1;
		rr = rr * rr;
		rr = (rr)+ z.z * z.z;
		rr = sqrt(rr);*/



		double phi;


		if (!fractal->transformCommon.functionEnabledEFalse)
		{
			if (fractal->transformCommon.functionEnabledBFalse) temp = rr + z.z * z.z;
			if (fractal->transformCommon.functionEnabledCFalse) temp = sqrt(rr);
			if (fractal->transformCommon.functionEnabledDFalse) temp = (rr);
			phi = atan2(z.z , temp);
		}
		else
		{
			rr = (z.x * z.x + z.y * z.y) - r1;
		//	rr = rr * rr;
		//	temp = (rr)+ z.z * z.z;
//
			temp = sqrt(rr * rr);


			if (fractal->transformCommon.functionEnabledBFalse) temp = rr;
			if (fractal->transformCommon.functionEnabledCFalse) temp = sqrt(z.x * z.x + z.y * z.y) - r1;
			if (fractal->transformCommon.functionEnabledDFalse) temp = rr * rr;
			phi = atan2(z.z , temp);
		}


		/*{
			if (!fractal->transformCommon.functionEnabledCFalse)
				phi = atan2(z.z , sqrt(temp)); // this
			else
				phi = atan2(z.z , r);
		}
		else
		{
			if (!fractal->transformCommon.functionEnabledCFalse) // phi = asin(z.z / r);asin(z.z / temp);sqrt(z.x * z.x + z.y * z.y) - r1
				phi = atan2(z.z , (temp + z.z * z.z));
			else
				phi = atan2(z.z , (temp));
		}*/

		//if (!fractal->transformCommon.functionEnabledDFalse)
		//{
		//	rp = pow(r, fractal->bulb.power - 1.0);
		//}
		//else
		//{
		//	temp = pow(aux.r, fractal->bulb.power - 1.0);
		//}

		r = r + (aux.r - r) * fractal->transformCommon.radius1;
			rp = pow(r, fractal->bulb.power - 1.0);
		//rp = pow(r, fractal->bulb.power)/pow(fractal->transformCommon.radius1, fractal->bulb.power-1.0); // default 4.0f

		phi *= fractal->bulb.power; // default 8
		theta *= fractal->bulb.power; // default 9 gives 8 symmetry
		aux.DE = (rp * aux.DE) * fractal->bulb.power + 1.0;
		// convert back to cartesian coordinates

		// rp = pow(r, fractal->bulb.power - 1.0); // pow(fractal->transformCommon.radius1, fractal->bulb.power);
		rp *= r;

		if (!fractal->transformCommon.functionEnabledSwFalse)
		{
			double r1RpCosPhi = r1 + rp * cos(phi);
			z.x = r1RpCosPhi * cos(theta);
			z.y = r1RpCosPhi * sin(theta);
		}
		else
		{
			z.x = (sign(z.x) * x1 + rp * cos(phi)) * cos(theta);
			z.y = (sign(z.y) * y1 + rp * cos(phi)) * sin(theta);


		}
		z.z = -rp * sin(phi);
	}

	aux.DE = aux.DE
					* fractal->analyticDE.scale1
					+ fractal->analyticDE.offset1;




/*	// DEcalc
	if (!fractal->analyticDE.enabledFalse)
	{
		aux.DE = rp * aux.DE * (fractal->transformCommon.pwr4 + 1.0) + 1.0;
	}
	else
	{
		aux.DE = rp * aux.DE * (fractal->transformCommon.pwr4 + fractal->analyticDE.offset2)
							 * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;
	}*/

	if (fractal->transformCommon.functionEnabledAxFalse) // spherical offset
	{
		//double lengthTempZ = -z.Length();
		// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 - fractal->transformCommon.offset / z.Length();
		//aux.DE = aux.DE * fabs(1.0 + fractal->transformCommon.offset / -z.Length());
		z *= fractal->transformCommon.scale;
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale);
	}
	// then add Cpixel constant vector
}
