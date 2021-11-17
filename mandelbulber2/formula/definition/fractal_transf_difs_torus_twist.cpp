/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsTorusV3Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSTorusTwist::cFractalTransfDIFSTorusTwist() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Torus Twist";
	internalName = "transf_difs_torus_twist";
	internalID = fractal::transfDIFSTorusTwist;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSTorusTwist::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		// pre abs
		if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);

		if (fractal->transformCommon.functionEnabledCx)
		{
			double psi = M_PI / fractal->transformCommon.int8X;
			psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
			double len = sqrt(z.x * z.x + z.y * z.y);
			z.x = cos(psi) * len;
			z.y = sin(psi) * len;
		}

		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			double psi = M_PI / fractal->transformCommon.int8Y;
			psi = fabs(fmod(atan2(z.z, z.y) + psi, 2.0 * psi) - psi);
			double len = sqrt(z.y * z.y + z.z * z.z);
			z.y = cos(psi) * len;
			z.z = sin(psi) * len;
		}

		if (fractal->transformCommon.functionEnabledCzFalse)
		{
			double psi = M_PI / fractal->transformCommon.int8Z;
			psi = fabs(fmod(atan2(z.x, z.z) + psi, 2.0 * psi) - psi);
			double len = sqrt(z.z * z.z + z.x * z.x);
			z.z = cos(psi) * len;
			z.x = sin(psi) * len;
		}

		// addition constant
		z += fractal->transformCommon.additionConstant000;

		// rotation
		if (fractal->transformCommon.rotationEnabledFalse)
		{
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}
	}
	double temp;
	CVector4 zc = z;
	double ang = atan2(zc.y, zc.x);
	double spiral = 0.0;
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		double Voff = fractal->transformCommon.offset02;
		temp = zc.z - 2.0 * Voff * ang / M_PI_2x + Voff;
		zc.z = temp - 2.0 * Voff * floor(temp / (2.0 * Voff)) - Voff;
		spiral = z.z * fractal->transformCommon.scaleC0;
	}
	temp = zc.y;
	zc.y = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.radius1
			+ spiral;

	ang = atan2(temp, zc.x) * fractal->transformCommon.int6 * 0.25;
	double cosA = cos(ang);
	double sinB = sin(ang);
	temp = zc.z;
	zc.z = zc.y * cosA + zc.z * sinB;
	zc.y = temp * cosA + zc.y * -sinB;

	CVector4 d = fabs(zc);
	double lenY = fractal->transformCommon.offset01;
	double lenZ = fractal->transformCommon.offsetp1;

	if (fractal->transformCommon.functionEnabledMFalse) // y face
		lenY += d.z * fractal->transformCommon.scale0;
	if (fractal->transformCommon.functionEnabledNFalse) // z face
		lenZ += d.z * fractal->transformCommon.scale3D000.x;
	if (fractal->transformCommon.functionEnabledOFalse) // y axis
		lenY += d.x * fractal->transformCommon.scale3D000.y;
	if (fractal->transformCommon.functionEnabledKFalse) // z axis
		lenZ += d.y * fractal->transformCommon.scale3D000.z;

	d.x = 0.0;
	d.y = max(d.y - lenY, 0.0);
	d.z = max(d.z - lenZ, 0.0);
	aux.DE0 = d.Length() / (aux.DE + fractal->analyticDE.offset0) - fractal->transformCommon.offset0005;

	// clip
	if (fractal->transformCommon.functionEnabledEFalse)
	{
		aux.DE0 = max(fractal->transformCommon.offsetE0 - aux.const_c.z, aux.DE0);
	}

	double colDist = aux.dist;
	if (!fractal->analyticDE.enabledFalse)
		aux.dist = aux.DE0;
	else
		aux.dist = min(aux.dist, aux.DE0);

	if (fractal->transformCommon.functionEnabledZcFalse
			&& aux.i >= fractal->transformCommon.startIterationsZc
			&& aux.i < fractal->transformCommon.stopIterationsZc)
				z = zc;

	// aux.color
	if (aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addColor = 0.0;
		if (aux.dist == colDist) addColor += fractal->foldColor.difs0000.x;
		if (aux.dist != colDist) addColor += fractal->foldColor.difs0000.y;
		aux.color += addColor;
	}
}
