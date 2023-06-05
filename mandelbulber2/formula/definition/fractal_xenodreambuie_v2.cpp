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

cFractalXenodreambuieV2::cFractalXenodreambuieV2() : cAbstractFractal()
{
	nameInComboBox = "Xenodreambuie V2";
	internalName = "xenodreambuie_v2";
	internalID = fractal::xenodreambuieV2;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalXenodreambuieV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
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

	if (aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		if (cos(th) < 0.0) ph = ph + M_PI;
	}

	aux.DE = rp * aux.DE * fabs(fractal->bulb.power) + 1.0;
	rp *= aux.r;

	// polar to cartesian
	t = cos(th);
	z.x = t * cos(ph) * rp;
	z.y = t * sin(ph) * rp;
	z.z = sin(th) * rp;

	z += fractal->transformCommon.offset000;

	z *= fractal->transformCommon.scaleC1;
	aux.DE *= fabs(fractal->transformCommon.scaleC1);

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
