/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * RiemannSphereHobold multi powers
 * @reference https://fractalforums.org/fractal-mathematics-and-new-theories/28/
 * riemandelettuce-without-trigonometry/2996/msg16097#msg16097
 */

#include "all_fractal_definitions.h"

cFractalRiemannSphereHoboldMulti::cFractalRiemannSphereHoboldMulti() : cAbstractFractal()
{
	nameInComboBox = "Riemann - Sphere Hobold Multi";
	internalName = "riemann_sphere_hobold_multi";
	internalID = fractal::riemannSphereHoboldMulti;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 2.5;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalRiemannSphereHoboldMulti::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z *= fractal->transformCommon.scale08
			 / aux.r; // normalize vector to unit length => project onto sphere

	// find X-related iso-plane: polar projection onto unit circle
	double Kx = 2.0 * z.x * (1.0 - z.y) / ((z.y - 2.0) * z.y + z.x * z.x + 1.0);
	double Ky = 1.0 - 2.0 * ((z.y - 2.0) * z.y + 1.0) / ((z.y - 2.0) * z.y + z.x * z.x + 1.0);

	if (fractal->transformCommon.functionEnabledx)
	{
		int countDXY = fractal->transformCommon.int3;
		int n;
		for (n = 0; n < countDXY; n++)
		{
			double tempX = Kx;
			double tempY = Ky;
			Kx = -2.0 * tempX * tempY;
			Ky = -(tempY * tempY - tempX * tempX);
		}
	}

	if (fractal->transformCommon.functionEnabledxFalse)
	{
		int countTXY = fractal->transformCommon.int3X;
		int p;
		for (p = 0; p < countTXY; p++)
		{
			double tempY = Ky;
			double tempX = Kx;
			Kx = -tempX * (3.0 - 4.0 * tempX * tempX);
			Ky = -tempY * (4.0 * tempY * tempY - 3.0);
		}
	}

	// (relevant) normal vector coordinates of doubled point plane
	double n1x = Ky - 1.0;
	double n1y = -Kx;

	n1x += fractal->transformCommon.offsetA0; // offset tweak

	// find Z-related iso-plane: polar projection onto unit circle
	double Kz = 2.0 * z.z * (1.0 - z.y) / ((z.y - 2.0) * z.y + z.z * z.z + 1.0);
	Ky = 1.0 - 2.0 * ((z.y - 2.0) * z.y + 1.0) / ((z.y - 2.0) * z.y + z.z * z.z + 1.0);

	if (fractal->transformCommon.functionEnabledy)
	{
		int countDYZ = fractal->transformCommon.int3Y;
		int n;
		for (n = 0; n < countDYZ; n++)
		{
			double tempZ = Kz;
			double tempY = Ky;
			Kz = -2.0 * tempZ * tempY;
			Ky = -(tempY * tempY - tempZ * tempZ);
		}
	}

	if (fractal->transformCommon.functionEnabledyFalse)
	{
		int countTYZ = fractal->transformCommon.int3Z;
		int p;
		for (p = 0; p < countTYZ; p++)
		{
			double tempY = Ky;
			double tempZ = Kz;
			Kz = -tempZ * (3.0 - 4.0 * tempZ * tempZ);
			Ky = -tempY * (4.0 * tempY * tempY - 3.0);
		}
	}

	// (relevant) normal vector coordinates of doubled point plane
	double n2y = -Kz;
	double n2z = Ky - 1.0;

	n2z += fractal->transformCommon.offsetB0; // offset tweak

	// internal rotation
	if (fractal->transformCommon.angle0 != 0)
	{
		double tpx = n1x;
		double tpz = n2z;
		double beta = fractal->transformCommon.angle0 * M_PI_180;
		n1x = tpx * cos(beta) + tpz * sin(beta);
		n2z = tpx * -sin(beta) + tpz * cos(beta);
	}

	// compute position of doubled point as intersection of planes and sphere
	// solved ray parameter
	double nt =
		2.0 * (n1x * n1x * n2z * n2z) / ((n1x * n1x + n1y * n1y) * n2z * n2z + n1x * n1x * n2y * n2y);

	// doubled point position
	z.y = 1.0 - nt;
	z.x = n1y * (1.0 - z.y) / n1x;
	z.z = n2y * (1.0 - z.y) / n2z;

	// raise original length to the power, then add constant
	z *= pow(aux.r, fractal->transformCommon.pwr8 / 2.0);

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	if (fractal->analyticDE.enabled)
	{
		aux.DE = fractal->analyticDE.offset1 + aux.DE * fabs(fractal->transformCommon.scale08) / aux.r;
		aux.DE *= 8.0 * fractal->analyticDE.scale1 * z.Length() / aux.r;
	}
}
