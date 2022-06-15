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

cFractalMandelbulbSinCosV2::cFractalMandelbulbSinCosV2() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb Sin Cos V2";
	internalName = "mandelbulb_sin_cos_v2";
	internalID = fractal::mandelbulbSinCosV2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbSinCosV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
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

	double ph = atan2(z.y, z.x);
	//if (!fractal->transformCommon.functionEnabledFFalse)

	if (aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		th = (th + fractal->bulb.betaAngleOffset);
		ph = (ph + fractal->bulb.alphaAngleOffset);
	}

		th = (th) * fractal->bulb.power;

	ph = (ph) * fractal->bulb.power;
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	aux.DE = rp * aux.DE * fractal->bulb.power + 1.0;
	rp *= aux.r;


	// polar to cartesian
	if (fractal->transformCommon.functionEnabledAx)
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
	}
	if (fractal->transformCommon.functionEnabledGFalse)
	{
		double sth = sin(th);
		double cph = cos(ph);
		//z.x = sth * sin(ph);
		//if (fractal->transformCommon.functionEnabledIFalse)

		//z.y = z.y/rp + (sth * cos(ph) - z.y/rp) * fractal->transformCommon.scaleC1;

		z.x = sth * sin(ph) * fractal->transformCommon.scaleC1;
		z.y = sth * cph * fractal->transformCommon.scaleF1;
		z.z = cph;



	}



	if (fractal->transformCommon.functionEnabledJFalse)
	{
		double sth = sin(th);
		CVector4 vsth = CVector4{cos(ph), sin(ph), cos(th), 0.0};
		if (fractal->transformCommon.functionEnabledKFalse) vsth.x *= sth;
		if (fractal->transformCommon.functionEnabledMFalse) vsth.y *= sth;
		if (fractal->transformCommon.functionEnabledNFalse) vsth.z *= sth;

		z = vsth;
	}




/*	if (fractal->transformCommon.functionEnabledJFalse)
	{
		double sth = sin(th);
		z.x = sth * cos(ph);
		z.y = sth * sin(ph);
		z.z = cos(th);
	}
	if (fractal->transformCommon.functionEnabledKFalse)
	{
		double sth = sin(th);
		z.x = sth * cos(ph);
		z.y = sin(ph);
		z.z = cos(th);
	}
	if (fractal->transformCommon.functionEnabledMFalse)
	{
		//double sth = native_sin(th);		z *= fractal->transformCommon.scaleF1;
		aux.DE *= fractal->transformCommon.scaleF1;
		z.x = cos(ph);
		z.y = sin(ph);
		z.z = cos(th);		z *= fractal->transformCommon.scaleF1;
		aux.DE *= fractal->transformCommon.scaleF1;
	}
	if (fractal->transformCommon.functionEnabledNFalse)
	{
		double sth = sin(th);
		z.x = cos(ph);
		z.y = sin(ph);
		z.z = sth * cos(th);
	}
*/
	z *= rp;


	z += fractal->transformCommon.offsetA000;
	z += aux.const_c * fractal->transformCommon.constantMultiplierA111;
	z.z *= fractal->transformCommon.scaleA1;

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
