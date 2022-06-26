/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2019 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Mandelbulb fractal.
 * @reference http://www.fractalforums.com/3d-fractal-generation/true-3d-mandlebrot-type-fractal/
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbSinCosV3::cFractalMandelbulbSinCosV3() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb Sin Cos V3";
	internalName = "mandelbulb_sin_cos_v3";
	internalID = fractal::mandelbulbSinCosV3;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbSinCosV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	double r1 = 0.0;
	if (fractal->transformCommon.functionEnabledPFalse
				&& aux.i >= fractal->transformCommon.startIterationsP
				&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		r1 = sqrt(z.x * z.x + z.y * z.y);

		double phi;
		if (!fractal->transformCommon.functionEnabledSwFalse)
			phi = atan2(z.y, z.x);
		else
			phi = atan2(z.x, z.y);

		double t1 = fabs(cos(fractal->transformCommon.constantMultiplierA111.x * phi)
				* fractal->transformCommon.constantMultiplierA111.y);
		if (fractal->transformCommon.functionEnabledCxFalse)
			t1 = pow(t1, fractal->transformCommon.constantMultiplierB111.x);

		double t2 = fabs(sin(fractal->transformCommon.constantMultiplierA111.x * phi)
				* fractal->transformCommon.constantMultiplierA111.z);
		if (fractal->transformCommon.functionEnabledCyFalse)
			t2 = pow(t2, fractal->transformCommon.constantMultiplierB111.y);

		if (!fractal->transformCommon.functionEnabledOFalse) r1 = t1 + t2;
		else r1 = pow(t1 + t2, -fractal->transformCommon.constantMultiplierB111.z);

		if (fractal->transformCommon.functionEnabledRFalse) r1 = 1.0 / r1;

		aux.r = fabs(aux.r + r1 * fractal->transformCommon.minR0);
	}
	// cartesian to polar
	double th = z.z / aux.r;
	if (!fractal->transformCommon.functionEnabledBFalse)
	{
		if (!fractal->transformCommon.functionEnabledAFalse) th = asin(th);
		else th = acos(th);
	}
	else
	{
		th = acos(th) * (1.0 - fractal->transformCommon.scale1)
				+ asin(th) * fractal->transformCommon.scale1;
	}
	double ph;
	if (!fractal->transformCommon.functionEnabledXFalse)
		ph = atan2(z.y, z.x);
	else
		ph = atan2(z.x, z.y);

	th = (th + fractal->bulb.betaAngleOffset) * fractal->bulb.power;
	ph = (ph + fractal->bulb.alphaAngleOffset) * fractal->bulb.power;
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;
	rp *= aux.r;

	// polar to cartesian
	if (fractal->transformCommon.functionEnabledAx
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		if (!fractal->transformCommon.functionEnabledEFalse)
		{
			double cth = cos(th);
			if (!fractal->transformCommon.functionEnabledDFalse)
			{
				z.x = cth * cos(ph);
				z.y = cth * sin(ph);
				z.z = sin(th);
			}
			else
			{
				z.x = cth * cos(ph);
				z.y = sin(th);
				z.z = cth * sin(ph);
			}
		}
		else
		{
			double sth = sin(th);
			if (!fractal->transformCommon.functionEnabledDFalse)
			{
				z.x = sth * sin(ph);
				z.y = sth * cos(ph);
				z.z = cos(th);
			}
			else
			{
				z.x = sth * sin(ph);
				z.y = cos(th);
				z.z = sth * cos(ph);
			}
		}
		z *= rp;
	}

	if (fractal->transformCommon.functionEnabledGFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		if (!fractal->transformCommon.functionEnabledFFalse)
		{
			double sth = sin(th);
			z.x = z.x + (rp * sth * sin(ph) - z.x) * fractal->transformCommon.scaleC1;
			z.y = z.y + (rp * sth * cos(ph) - z.y) * fractal->transformCommon.scaleF1;
			z.z = rp * cos(th);
		}
		else
		{
			double cth = cos(th);
			z.x = z.x + (rp * cth * cos(ph) - z.x) * fractal->transformCommon.scaleC1;
			z.y = z.y + (rp * cth * sin(ph) - z.y) * fractal->transformCommon.scaleF1;
			z.z = rp * sin(th);
		}
	}

	if (fractal->transformCommon.functionEnabledJFalse
			&& aux.i >= fractal->transformCommon.startIterationsJ
			&& aux.i < fractal->transformCommon.stopIterationsJ)
	{
		double cth = cos(th);
		z.x = sin(ph);
		z.y = cos(ph);
		z.z = sin(th);
		if (fractal->transformCommon.functionEnabledKFalse) z.x *= cth;
		if (fractal->transformCommon.functionEnabledMFalse) z.y *= cth;
		if (fractal->transformCommon.functionEnabledNFalse) z.z *= cth;
		z *= rp;
	}

	z += fractal->transformCommon.offsetA000;
	z += aux.const_c * fractal->transformCommon.constantMultiplier111;
	z.z *= fractal->transformCommon.scaleA1;

/*

		if (fractal->transformCommon.functionEnabledAzFalse)
		{
			if (fractal->transformCommon.functionEnabledAxFalse)
			{
				if (fabs(z.x) > fabs(z.y)) z.y = r * sin(phi);
				else z.y = r * cos(phi);
			}
			if (fractal->transformCommon.functionEnabledAyFalse)
			{
				if (fabs(z.x) < fabs(z.y)) z.y = r * cos(phi);
				else z.y = r * sin(phi);
			}
			if (fractal->transformCommon.functionEnabledBxFalse) z.y = r * sin(phi);
			if (fractal->transformCommon.functionEnabledByFalse) z.y = r * cos(phi);
		}

		if (fractal->transformCommon.functionEnabledBzFalse)
		{
			if (fractal->transformCommon.functionEnabledAxFalse)
			{
				if (fabs(z.x) > fabs(z.y)) z.x = r * sin(phi);
				else z.x = r * cos(phi);
			}
			if (fractal->transformCommon.functionEnabledAyFalse)
			{
				if (fabs(z.x) < fabs(z.y)) z.x = r * cos(phi);
				else z.x = r * sin(phi);
			}
			if (fractal->transformCommon.functionEnabledBxFalse) z.x = r * sin(phi);
			if (fractal->transformCommon.functionEnabledByFalse) z.x = r * cos(phi);
		}

	}*/




	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		aux.DE0 = z.Length();
		if (aux.DE0 > 1.0)
			aux.DE0 = 0.5 * log(aux.DE0) * aux.DE0 / (aux.DE);
		else
			aux.DE0 = 0.01; // 0.0 artifacts in openCL

		if (aux.i >= fractal->transformCommon.startIterationsO
					&& aux.i < fractal->transformCommon.stopIterationsO)
			aux.dist = min(aux.dist, aux.DE0);
		else
			aux.dist = aux.DE0;
	}
}
