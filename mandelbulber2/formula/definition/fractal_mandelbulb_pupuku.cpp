/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * mandelbulbPupuku
 * @reference polar cartesian variation be Pupukuusikko
 * https://https://fractalforums.org/share-a-fractal/22/mandelbulb-fixed/5099
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbPupuku::cFractalMandelbulbPupuku() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb Pupuku";
	internalName = "mandelbulb_pupuku";
	internalID = fractal::mandelbulbPupuku;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbPupuku::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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
	double th = (t + fractal->bulb.betaAngleOffset)
			* fractal->bulb.power * fractal->transformCommon.scaleA1;
	double ph = (atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset)
			* fractal->bulb.power * fractal->transformCommon.scaleB1;
	double rp = pow(aux.r, fractal->bulb.power - fractal->transformCommon.offset1);

	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		if (cos(th) < 0.0) ph = ph + M_PI;
	}

	aux.DE = rp * aux.DE * fabs(fractal->bulb.power) + fractal->analyticDE.offset1;
	rp *= aux.r;

	// polar to cartesian
	double zth;
	if (!fractal->transformCommon.functionEnabledDFalse)
	{
		t = cos(th);
		zth = sin(th);
	}
	else
	{
		t = sin(th);
		zth = cos(th);
	}
	if (fractal->transformCommon.functionEnabledBFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x = (1.0 + (t - 1.0) * fractal->transformCommon.scaleB0) * cos(ph);
	}
	else
	{
		z.x = t * cos(ph);
	}
	if (fractal->transformCommon.functionEnabledAFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z.y = (1.0 + (t - 1.0) * fractal->transformCommon.scaleA0) * sin(ph);
	}
	else
	{
		z.y = t * sin(ph);
	}
	z.z = zth;
	z *= rp;

	if (fractal->transformCommon.functionEnabledBzFalse) z.y = min(z.y, fractal->transformCommon.offset0 - z.y);

	z += fractal->transformCommon.offset000;

	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		z *= fractal->transformCommon.scaleC1;
		aux.DE *= fabs(fractal->transformCommon.scaleC1);
	}

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		aux.DE0 = z.Length();

		if (aux.DE0 > 1.0)
		{
			aux.DE0 = 0.5 * log(aux.DE0) * aux.DE0 / aux.DE;
		}
		else
		{
			aux.DE0 = 0.0; // 0.01 artifacts in openCL
		}

		if (!fractal->transformCommon.functionEnabledEFalse)
		{
			aux.dist = aux.DE0;
		}
		else
		{
			if (aux.i >= fractal->transformCommon.startIterationsE
				&& aux.i < fractal->transformCommon.stopIterationsE)
			{
				aux.dist = min(aux.dist, aux.DE0);
			}
			else
			{
				aux.dist = aux.DE0;
			}
		}
	}
}
