/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Testing
 *
 * https://www.shadertoy.com/view/3ddSDs
 * Based upon: https://www.shadertoy.com/view/XdlSD4
 */

#include "all_fractal_definitions.h"

cFractalTesting::cFractalTesting() : cAbstractFractal()
{
	nameInComboBox = "Testing";
	internalName = "testing";
	internalID = fractal::testing;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTesting::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledM)
	{

		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabled)
		{
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		}

		double rr = z.Dot(z);
		if (rr < fractal->transformCommon.minR2p25)
		{
			z *= fractal->transformCommon.maxMinR2factor;
			aux.DE *= fractal->transformCommon.maxMinR2factor;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			z *= fractal->transformCommon.maxR2d1 / rr;
			aux.DE *= fractal->transformCommon.maxR2d1 / rr;
		}

		// scale
		double useScale = 1.0;
		useScale = aux.actualScaleA + fractal->transformCommon.scale2;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;

		if (fractal->transformCommon.functionEnabledKFalse
				&& aux.i >= fractal->transformCommon.startIterationsK
				&& aux.i < fractal->transformCommon.stopIterationsK)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			aux.actualScaleA -= vary;
		}


		//z *= fractal->transformCommon.scale2;
		//aux.DE = aux.DE * fabs(fractal->transformCommon.scale2) + 1.0;
	}

	// menger
	if (fractal->transformCommon.functionEnabledNFalse)
	{
		z = fabs(z + fractal->transformCommon.offsetA000);

		if (z.x - z.y < 0.0) swap(z.x, z.y);
		if (z.x - z.z < 0.0) swap(z.x, z.z);
		if (z.y - z.z < 0.0) swap(z.y, z.z);

		z *= fractal->transformCommon.scale3; // 3

		z.x -= 2.0;
		z.y -= 2.0;
		if (z.z > 1.0) z.z -= 2.0;

		aux.DE *= fractal->transformCommon.scale3; // 3
	}

	// bulb
	if (fractal->transformCommon.functionEnabledOFalse)
	{
		// if (aux.r < 1e-21) aux.r = 1e-21;
		const double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
		const double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
		double rp = pow(aux.r, fractal->bulb.power - 1.0);
		const double th = th0 * fractal->bulb.power;
		const double ph = ph0 * fractal->bulb.power;
		const double cth = cos(th);
		aux.DE = (rp * aux.DE) * fractal->bulb.power + 1.0;
		rp *= aux.r;
		z.x = cth * cos(ph) * rp;
		z.y = cth * sin(ph) * rp;
		z.z = sin(th) * rp;
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	z += fractal->transformCommon.offset000;

	z += aux.c * fractal->transformCommon.constantMultiplier111;



	// THE FOLLOWING CAN BE A TRANSFORM

	CVector4 zc = z;
	// cylinder
	double cylinder;
	double cylR = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.radius1;
	double cylH = fabs(zc.z) - fractal->transformCommon.offsetA1;
	cylR = max(cylR, 0.0);
	cylH = max(cylH, 0.0);
	double cylD = sqrt(cylR * cylR + cylH * cylH);
	cylinder = min(max(cylR, cylH), 0.0) + cylD;

	// box
	CVector4 boxSize = fractal->transformCommon.offset111;
	zc = fabs(zc) - boxSize;
	zc.x = max(zc.x, 0.0);
	zc.y = max(zc.y, 0.0);
	zc.z = max(zc.z, 0.0);
	double box = zc.Length();
	zc = z;

	// ellipsoid
	CVector4 rads4 = fractal->transformCommon.offsetA111;
	CVector3 rads3 = CVector3(rads4.x, rads4.y, rads4.z);
	CVector3 rV = CVector3(zc.x, zc.y, zc.z);
	rV /= rads3;
	CVector3 rrV = rV;
	rrV /= rads3;
	double rd = rV.Length();
	double rrd = rrV.Length();
	double ellipsoid = rd * (rd - 1.0) / rrd;

	// sphere
	double sphere = zc.Length() - fractal->transformCommon.offset3;

	// torus
	double torus = sqrt(z.x * z.x + z.z * z.z) - fractal->transformCommon.offset4;
	torus = sqrt(torus * torus + z.y * z.y) - fractal->transformCommon.offset1;

	if (fractal->transformCommon.functionEnabledxFalse) torus = cylinder;
	if (fractal->transformCommon.functionEnabledyFalse) sphere = box;
	if (fractal->transformCommon.functionEnabledzFalse) sphere = ellipsoid;

	// THE FOLLOWING CAN BE A TRANSFORM
	int count = fractal->transformCommon.int3;
	int tempC = fractal->transformCommon.int3X;
	double r;

	if (!fractal->transformCommon.functionEnabledSwFalse)
	{
		r = (aux.i < count) ? torus : sphere;
	}
	else
	{
		r = (aux.i < count) ? sphere : torus;
	}

	aux.DE = aux.DE + fractal->analyticDE.offset0;

	double dd = r;
	if (fractal->transformCommon.functionEnabledAx)
	{
		dd = dd / aux.DE; // same as an uncondtional aux.dist
	}
	if (fractal->transformCommon.functionEnabledBFalse)
	{
		double rxy = sqrt(z.x * z.x + z.y * z.y);
		double m = max(rxy - fractal->transformCommon.offsetT1, fabs(rxy * z.z) / dd);
		dd = m / aux.DE;
	}
	if (fractal->transformCommon.functionEnabledCFalse)
	{
		dd = 0.5 * dd * log(dd) / aux.DE; // = using linear and increasining detail level
	}

	if (aux.i < tempC || dd < aux.colorHybrid)
	{
		aux.colorHybrid = dd;
	}

	aux.dist = aux.colorHybrid;
}
