/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * XenodreambuieV2
 * @reference
 *https://fractalforums.org/share-a-fractal/22/new-mandelbulb-variant/5083
 */

#include "all_fractal_definitions.h"

cFractalXenodreambuieV3::cFractalXenodreambuieV3() : cAbstractFractal()
{
	nameInComboBox = "Xenodreambuie V3";
	internalName = "xenodreambuie_v3";
	internalID = fractal::xenodreambuieV3;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalXenodreambuieV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double t; //temp
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP)
	{
		if (fractal->transformCommon.functionEnabledCxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledCyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledCzFalse) z.z = fabs(z.z);
		z += fractal->transformCommon.offsetA000;

		if (fractal->transformCommon.functionEnabledTFalse)
		{
			aux.r = z.Length();
		}
	}

	if (!fractal->transformCommon.functionEnabledSwFalse) t = asin(z.z / aux.r);
	else t = acos(z.z / aux.r);
	t = (t + fractal->bulb.betaAngleOffset);
	double th = t * fractal->bulb.power * fractal->transformCommon.scaleA1;


	double ph = (atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset)
			* fractal->bulb.power * fractal->transformCommon.scaleB1;

	double rp = pow(aux.r, fractal->bulb.power - fractal->transformCommon.offset1);
	aux.DE = rp * aux.DE * fabs(fractal->bulb.power) + 1.0;
	rp *= aux.r;

	// polar to cartesian
	if (!fractal->transformCommon.functionEnabledDFalse)
	{
		if (aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			if (cos(th) < 0.0) ph = ph + M_PI;
		}
		double cth = cos(th);
		z.x = cth * cos(ph) * rp;
		z.y = cth * sin(ph) * rp;
		z.z = sin(th) * rp;




		/*if (fractal->transformCommon.functionEnabledBFalse
				&& aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			//z.x = cos(ph) * rp; // temp
			z.x = (cth + (1.0 - cth) * fractal->transformCommon.scaleB0) * cos(ph) * rp;
		}
		else
		{
			z.x = cth * cos(ph) * rp;
		}
		if (fractal->transformCommon.functionEnabledAFalse
				&& aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA)
		{
			z.y = (cth + (1.0f - cth) * fractal->transformCommon.scaleA0) * sin(ph) * rp;
		}
		else
		{
			z.y = cth * sin(ph) * rp;
		}
		z.z = sin(th) * rp;*/
	}
	else
	{

		if (aux.i >= fractal->transformCommon.startIterationsY
				&& aux.i < fractal->transformCommon.stopIterationsY)
		{
			if (fabs(t) > 0.5f * M_PI) t = sign(t) * M_PI - t;
		}
		th = t * fractal->bulb.power * fractal->transformCommon.scaleA1;

		double sth = sin(th);
		z.x = sth * cos(ph) * rp;
		z.y = sth * sin(ph) * rp;
		z.z = cos(th) * rp;
	}

	if (fractal->transformCommon.functionEnabledBzFalse) z.y = min(z.y, fractal->transformCommon.offset0 - z.y);

	z += fractal->transformCommon.offset000;

	z *= fractal->transformCommon.scaleC1;
	aux.DE *= fabs(fractal->transformCommon.scaleC1);

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		aux.DE0 = z.Length();
		if (!fractal->transformCommon.functionEnabledBxFalse)
		{
			if (aux.DE0 > 1.0)
				aux.DE0 = 0.5 * log(aux.DE0) * aux.DE0 / (aux.DE);
			else
				aux.DE0 = 0.0; // 0.01 artifacts in openCL
		}
		else // temp
		{
			if (aux.DE0 > 1.0)
				aux.DE0 = 1.0 / aux.DE0 / (aux.DE);
			else
				aux.DE0 = 0.0; // 0.01 artifacts in openCL
		}


		/*if (aux.i >= fractal->transformCommon.startIterationsC
					&& aux.i < fractal->transformCommon.stopIterationsC)
			aux.dist = min(aux.dist, aux.DE0);
		else
			aux.dist = aux.DE0;*/

		if (!fractal->transformCommon.functionEnabledByFalse)
		{
			aux.dist = aux.DE0;
		}
		else
		{	if (aux.i >= fractal->transformCommon.startIterationsC
				&& aux.i < fractal->transformCommon.stopIterationsC)
					aux.dist = min(aux.dist, aux.DE0);
			else aux.dist = aux.DE0;
		}





	}
}
