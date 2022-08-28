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

cFractalMandelbulbSinCosV4::cFractalMandelbulbSinCosV4() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb Sin Cos V4";
	internalName = "mandelbulb_sin_cos_v4";
	internalID = fractal::mandelbulbSinCosV4;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbSinCosV4::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp;
	if (fractal->transformCommon.functionEnabledPFalse
				&& aux.i >= fractal->transformCommon.startIterationsP
				&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		if (fractal->transformCommon.functionEnabledxFalse)
		{
			z.x = sign(z.x)
						* (fractal->transformCommon.offset000.x - fabs(z.x));
		}
		if (fractal->transformCommon.functionEnabledyFalse)
		{
			z.y = sign(z.y)
						* (fractal->transformCommon.offset000.y - fabs(z.y));
		}
		if (fractal->transformCommon.functionEnabledzFalse)
		{
			z.z = sign(z.z)
						* (fractal->transformCommon.offset000.z - fabs(z.z));
		}
	}

	// cartesian to polar
	double th;
	if (!fractal->transformCommon.functionEnabledMFalse)
		th = z.z / aux.r;
	else
		th = z.y / aux.r;

	double ph;
	if (!fractal->transformCommon.functionEnabledNFalse)
		ph = atan2(z.y, z.x);
	else
		ph = atan2(z.z, z.x);

	if (!fractal->transformCommon.functionEnabledBFalse)
	{
		if (!fractal->transformCommon.functionEnabledAFalse) th = asin(th);
		else th = acos(th);
	}
	else
	{
		temp = acos(th);
		th = temp + (asin(th) - temp) * fractal->transformCommon.scale1;
	}

	th = (th + fractal->bulb.betaAngleOffset) * (fractal->bulb.power + fractal->transformCommon.offset0);
	ph = (ph+ fractal->bulb.alphaAngleOffset) * (fractal->bulb.power + fractal->transformCommon.offsetA0);

	double rp = pow(aux.r, (fractal->bulb.power - 1.0f) * fractal->transformCommon.scaleA1);
	aux.DE = aux.DE * rp * fractal->bulb.power + 1.0f;
	rp *= aux.r * fractal->transformCommon.scaleB1;

	// polar to cartesian
	if (!fractal->transformCommon.functionEnabledSwFalse)
	{
		temp = sin(th);
		z.x = temp * cos(ph);
		z.y = temp * sin(ph);
		z.z = cos(th);
	}
	else
	{
		temp = cos(th);
		z.x = temp * cos(ph);
		z.y = temp * sin(ph);
		z.z = sin(th);
	}
	z *= rp;


//	if (fractal->transformCommon.functionEnabledFFalse
//			&& aux.i >= fractal->transformCommon.startIterationsF
//			&& aux.i < fractal->transformCommon.stopIterationsF)
//	{
		switch (fractal->mandelbulbMulti.orderOfXYZ)
		{
			case multi_OrderOfXYZ_xyz:
			default: z = CVector4(z.x, z.y, z.z, z.w); break;
			case multi_OrderOfXYZ_xzy: z = CVector4(z.x, z.z, z.y, z.w); break;
			case multi_OrderOfXYZ_yxz: z = CVector4(z.y, z.x, z.z, z.w); break;
			case multi_OrderOfXYZ_yzx: z = CVector4(z.y, z.z, z.x, z.w); break;
			case multi_OrderOfXYZ_zxy: z = CVector4(z.z, z.x, z.y, z.w); break;
			case multi_OrderOfXYZ_zyx: z = CVector4(z.z, z.y, z.x, z.w); break;
		}
//	}

	if (fractal->transformCommon.functionEnabledGFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		z.z *= fractal->transformCommon.scaleG1;
	}

	z += fractal->transformCommon.offsetA000;
	z += aux.const_c * fractal->transformCommon.constantMultiplier111;

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
			aux.DE0 = 0.0; // 0.01 artifacts in openCL

		if (aux.i >= fractal->transformCommon.startIterationsO
					&& aux.i < fractal->transformCommon.stopIterationsO)
			aux.dist = min(aux.dist, aux.DE0);
		else
			aux.dist = aux.DE0;
	}
}
