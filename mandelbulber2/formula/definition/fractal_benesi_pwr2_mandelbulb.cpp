/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * benesiPwr2 mandelbulb
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */

#include "all_fractal_definitions.h"

cFractalBenesiPwr2Mandelbulb::cFractalBenesiPwr2Mandelbulb() : cAbstractFractal()
{
	nameInComboBox = "Benesi - Pwr2 Mandelbulb";
	internalName = "benesi_pwr2_mandelbulb";
	internalID = fractal::benesiPwr2Mandelbulb;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalBenesiPwr2Mandelbulb::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	// Prism shape
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		CVector4 gap = fractal->transformCommon.constantMultiplier000;
		double t;
		double dot1;
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);

		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}
	// Benesi mag transform T1
	if (fractal->transformCommon.benesiT1Enabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		double tempL = z.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		if (fractal->transformCommon.rotationEnabled)
		{ // rotation inside T1
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.addCpixelEnabled
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{ // Benesi original pwr2
		CVector4 zz = z * z;
		double rrYZ = zz.y + zz.z;
		double lenYZ = sqrt(rrYZ);
		double temp = zz.x + rrYZ;
		aux.r = sqrt(temp); // aux.r needed when alternating pwr2s
		aux.DE = aux.DE * 2.0 * aux.r;
		CVector4 newZ = z;
		temp = zz.x - rrYZ;
		newZ.x = -sign(c.x) * temp;
		if (z.x < lenYZ) newZ.x = temp;
		rrYZ = -1.0 / lenYZ * 2.0 * fabs(z.x);
		newZ.y = rrYZ * (zz.y - zz.z);
		newZ.z = rrYZ * 2.0 * z.y * z.z;
		z = newZ + (c * fractal->transformCommon.constantMultiplierA100);
	}
	//  Benesi pine tree pwr2
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 zz = z * z;
		aux.r = sqrt(zz.x + zz.y + zz.z); // needed when alternating pwr2s
		aux.DE = aux.r * aux.DE * 2.0 + 1.0;

		double t = 1.0;
		double temp = zz.y + zz.z;
		if (temp > 0.0) t = 2.0 * z.x / sqrt(temp);
		temp = z.z;
		z.x = (zz.x - zz.y - zz.z);
		z.y = (2.0 * t * z.y * temp);
		z.z = (t * (zz.y - zz.z));

		// swap c.yz then add cPixel
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c * fractal->transformCommon.constantMultiplier100;
			tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w);
			aux.c = tempC;
		}
		else
		{
			tempC *= fractal->transformCommon.constantMultiplier100;
			tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w);
		}
		z += tempC;
	}

	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		z = CVector4(z.x * cos(z.y * fractal->transformCommon.scale1),
					z.x * sin(z.y * fractal->transformCommon.scale1), z.z * fractal->transformCommon.scaleC1,
					z.w)
				* fractal->transformCommon.scaleA1;
		aux.DE *= fabs(fractal->transformCommon.scaleA1);
	}

	if (fractal->transformCommon.juliaMode && aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
		z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.rotation2EnabledFalse)
		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
