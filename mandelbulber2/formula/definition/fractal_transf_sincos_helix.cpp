/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsTorusTwistIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * M3D difs code by darkbeam
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfSincosHelix::cFractalTransfSincosHelix() : cAbstractFractal()
{
	nameInComboBox = "T>SinCos Helix";
	internalName = "transf_sincos_helix";
	internalID = fractal::transfSincosHelix;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSincosHelix::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		// pre abs
		if (fractal->transformCommon.functionEnabledxFalse)
		{
			if (!fractal->transformCommon.functionEnabledBxFalse) z.x = fabs(z.x);
			else z.x = -fabs(z.x);
		}
		if (fractal->transformCommon.functionEnabledyFalse)
		{
			if (!fractal->transformCommon.functionEnabledByFalse) z.y = fabs(z.y);
			else z.y = -fabs(z.y);
		}
		if (fractal->transformCommon.functionEnabledzFalse)
		{
			if (!fractal->transformCommon.functionEnabledBzFalse) z.z = fabs(z.z);
			else z.z = -fabs(z.z);
		}

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
	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		temp = z.x;
		z.x = z.y;
		z.y = temp;
	}

	// swap axis
	if (fractal->transformCommon.functionEnabledSFalse)
	{
		temp = z.x;
		z.x = z.z;
		z.z = temp;
	}


	//double temp;
	double ang;
	if (!fractal->transformCommon.functionEnabledOFalse)
		ang = atan2(z.x, z.y) * fractal->transformCommon.scaleA1;
	else
		ang = atan2(z.y, z.x) * fractal->transformCommon.scaleA1;

	if (fractal->transformCommon.functionEnabledTFalse)
		z.y = sqrt(z.x * z.x + z.y * z.y) - fractal->transformCommon.radius1;

	// vert helix
	if (fractal->transformCommon.functionEnabledAxFalse)
	{
		double Voff = fractal->transformCommon.offsetA2;
		temp = z.z - Voff * ang * fractal->transformCommon.int1 + Voff * 0.5f;
		z.z = temp - Voff * floor(temp / (Voff)) - Voff * 0.5f;
	}
	// stretch around helix
	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		if (!fractal->transformCommon.functionEnabledAy)
			z.x = fractal->transformCommon.offsetR1;
		else
		{
			temp = fractal->transformCommon.scaleA2 * ang + fractal->transformCommon.offsetR1;
			z.x = temp - 2.0 * floor(temp * 0.5) - 1.0;
		}
	}
	if (fractal->transformCommon.functionEnabledAzFalse)
	{

		if (fractal->transformCommon.functionEnabledAwFalse)
		{
			double Phi = z.z;
			double Rho = sqrt(z.x * z.x + z.y * z.y);

			ang = Rho * fractal->transformCommon.offsetA0 + Phi * fractal->transformCommon.offsetB0
						+ fractal->transformCommon.offsetC0;
		}
		double cosA = cos(ang);
		double sinB = sin(ang);
		temp = z.x;
		z.x = z.y * cosA + z.x * sinB;
		z.y = temp * cosA - z.y * sinB;

	//	temp = z.x;
	//	z.x = z.x * cosA + z.y * sinB;
	//	z.y = z.y * cosA - temp * sinB;

	//			temp = z.x;
	//	z.x = z.x * cosA + z.y * sinB;
	//	z.y = -z.y * cosA + temp * sinB;


	}

	if (fractal->transformCommon.functionEnabledMFalse)
	{
		if ( z.z < (fractal->transformCommon.offsetA1 + 0.5) * fractal->transformCommon.offset2
			&& z.z > (fractal->transformCommon.offsetT1 + 0.5) * -fractal->transformCommon.offset2)
		{
			z.z -= round(z.z / fractal->transformCommon.offset2) * fractal->transformCommon.offset2;
			z.z = clamp(fabs(z.z), -fractal->transformCommon.offset1, fractal->transformCommon.offset1);

			// clip
			if (fractal->transformCommon.functionEnabledEFalse)
				z.z = max(aux.const_c.z - fractal->transformCommon.offsetE0, z.z);

			if (fractal->transformCommon.functionEnabledFFalse)
				z.z = min(aux.const_c.z + fractal->transformCommon.offsetF0, -z.z);

		}
	}





/*	temp = zc.y;
	zc.y = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.radius1
			+ spiral;*/





	// aux.color
	if (aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addColor = 0.0;
		//if (aux.dist == colDist) addColor += fractal->foldColor.difs0000.x;
		//= fractal->foldColor.difs0000.y;
		aux.color += addColor;
	}
}
