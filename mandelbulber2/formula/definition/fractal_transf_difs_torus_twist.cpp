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

	CVector4 zc = z;
	double temp;
	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}

	double ang = atan2(zc.y, zc.x);
	double factor = 0.0;
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		double Voff = fractal->transformCommon.scaleA2;
		temp = zc.z - 2.0 * Voff * ang / M_PI_2x + Voff;
		zc.z = temp - 2.0 * Voff * floor(temp / (2.0 * Voff)) - Voff;
		factor = (z.z - fractal->transformCommon.offset0) * fractal->transformCommon.scaleG1;
	}
	temp = zc.y;
	zc.y = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.radius1
			+ factor;

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

	if (!fractal->transformCommon.functionEnabledBFalse)
	{
		d.x = 0.0;
	}
	else
	{
		temp = fractal->transformCommon.int2 * 2.0 * ang + fractal->transformCommon.offsetR0;
		d.x = temp - floor(temp) - fractal->transformCommon.offsetA1;
		d.x = max(d.x, 0.0);
	}
	d.y = max(d.y - lenY, 0.0);
	d.z = max(d.z - lenZ, 0.0);
	aux.DE0 = d.Length() / (aux.DE + fractal->analyticDE.offset0) - fractal->transformCommon.offset0005;

	// clip
	double e = fractal->transformCommon.offset2;
	if (fractal->transformCommon.functionEnabledEFalse)
	{
		aux.const_c.z -= fractal->transformCommon.offsetD0;
		CVector4 f = fabs(aux.const_c) - CVector4(e, e, e, 0.0);
		if (!fractal->transformCommon.functionEnabledIFalse) e = f.z;
		else e = max(f.x, max(f.y, f.z)); // box
		aux.DE0 = max(e, aux.DE0);
	}

	double addColor = aux.dist;
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
		if (aux.dist == addColor) addColor += fractal->foldColor.difs0000.x;
		if (aux.dist != addColor) addColor += fractal->foldColor.difs0000.y;
		aux.color += addColor;
	}
}
