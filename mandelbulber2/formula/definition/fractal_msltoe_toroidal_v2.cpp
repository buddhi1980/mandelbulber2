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
		double temp;
	double rp;


	//else
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		// Toroidal bulb
		double r1 = fractal->transformCommon.minR05; // default 0.5f
		double theta = atan2(z.y, z.x);
		double x1 = r1 * cos(theta);
		double y1 = r1 * sin(theta);
		double rr;
		double r;
		rr = sqrt((z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1));
		r = rr + z.z * z.z;
		r = sqrt(r);
	//	aux->DE *= aux->r / r;
		temp = r;
	rr = sqrt((z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1));
		r = rr + z.z * z.z;
		r = sqrt(r);
		aux.DE *= aux.r / r;
		temp = r;

		if (fractal->transformCommon.functionEnabledAFalse)
		{
			rr = sqrt(z.x * z.x + z.y * z.y) - r1;
			r = rr * rr + z.z * z.z;
			r = sqrt(r);
			aux.DE *= aux.r / r;
			aux.DE *= aux.r / r;
		}








	/*	if (!fractal->transformCommon.functionEnabledAFalse)
		{
			temp = sqrt((z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1));
			rr = temp * temp;
			r = rr + z.z * z.z;
			r = sqrt(r);
		}
		else
		{
			temp = sqrt(z.x * z.x + z.y * z.y) - r1;
			rr = temp * temp;
			r = rr + z.z * z.z;
			r = sqrt(r);
		}*/

//		aux.DE *= aux.r / r; // hhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
		double phi = 0.0;


		if (!fractal->transformCommon.functionEnabledFFalse)
		{
			rr = (z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1);

			if (fractal->transformCommon.functionEnabledBFalse) temp = rr;
			if (fractal->transformCommon.functionEnabledCFalse) temp = rr + z.z * z.z;
			if (fractal->transformCommon.functionEnabledDFalse) temp = rr * rr;
			if (fractal->transformCommon.functionEnabledEFalse) temp = sqrt((z.x - x1) * (z.x - x1) + (z.y - y1) * (z.y - y1));
			phi = atan2(z.z, temp);
		}
		else
		{
			rr = (z.x * z.x + z.y * z.y) - r1;

			if (fractal->transformCommon.functionEnabledBFalse) temp = rr;
			if (fractal->transformCommon.functionEnabledCFalse) temp = rr + z.z * z.z;
			if (fractal->transformCommon.functionEnabledDFalse) temp = rr * rr;
			if (fractal->transformCommon.functionEnabledEFalse) temp = sqrt(z.x * z.x + z.y * z.y) - r1;
			phi = atan2(z.z , temp);
		}

		//phi = atan2(z.z, temp); // pppppppppppppppppppppppppppppppppppppppppppppppp
		if (fractal->transformCommon.functionEnabledXFalse)
			phi = atan2(z.z, sqrt(z.x * z.x + z.y * z.y) - r1);
		if (fractal->transformCommon.functionEnabledYFalse)
			phi = asin(z.z / r);

		r = r + (aux.r - r) * fractal->transformCommon.offsetR0;
		rp = pow(r, fractal->bulb.power - 1.0);

		aux.DE = rp * aux.DE * (fractal->bulb.power + fractal->analyticDE.offset0) + 1.0;

		rp *= r;

		phi *= fractal->bulb.power; // default 8
		theta *= fractal->bulb.power; // default 9 gives 8 symmetry

		// convert back to cartesian coordinates
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
					* fractal->analyticDE.scale1;

	z.z *= fractal->transformCommon.scaleA1;


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
