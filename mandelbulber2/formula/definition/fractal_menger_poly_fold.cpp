/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * menger poly fold
 * @reference
 * https://fractalforums.org/fragmentarium/17/polyfoldsym-pre-transform/2684
 */

#include "all_fractal_definitions.h"

cFractalMengerPolyFold::cFractalMengerPolyFold() : cAbstractFractal()
{
	nameInComboBox = "Menger - Poly Fold";
	internalName = "menger_poly_fold";
	internalID = fractal::mengerPolyFold;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalMengerPolyFold::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		// pre abs
		if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);

		if (fractal->transformCommon.functionEnabledCx)
		{
			if (fractal->transformCommon.functionEnabledAxFalse && z.y < 0.0) z.x = -z.x;
			int poly = fractal->transformCommon.int8X;
			double psi = fabs(fmod(atan(z.y / z.x) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
			double len = sqrt(z.x * z.x + z.y * z.y);
			z.x = cos(psi) * len;
			z.y = sin(psi) * len;
		}

		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			if (fractal->transformCommon.functionEnabledAyFalse && z.z < 0.0) z.y = -z.y;
			int poly = fractal->transformCommon.int8Y;
			double psi = fabs(fmod(atan(z.z / z.y) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
			double len = sqrt(z.y * z.y + z.z * z.z);
			z.y = cos(psi) * len;
			z.z = sin(psi) * len;
		}

		if (fractal->transformCommon.functionEnabledCzFalse)
		{
			if (fractal->transformCommon.functionEnabledAzFalse && z.x < 0.0) z.z = -z.z;
			int poly = fractal->transformCommon.int8Z;
			double psi = fabs(fmod(atan(z.x / z.z) + M_PI / poly, M_PI / (0.5 * poly)) - M_PI / poly);
			double len = sqrt(z.z * z.z + z.x * z.x);
			z.z = cos(psi) * len;
			z.x = sin(psi) * len;
		}

		z += fractal->transformCommon.additionConstant000;

		// rotation
		if (fractal->transformCommon.rotation2EnabledFalse)
		{
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}
	}

	// Menger Sponge
	z = fabs(z);
	if (z.x - z.y < 0.0) swap(z.y, z.x);
	if (z.x - z.z < 0.0) swap(z.z, z.x);
	if (z.y - z.z < 0.0) swap(z.z, z.y);
	z *= fractal->transformCommon.scale3;
	z.x -= 2.0 * fractal->transformCommon.constantMultiplierA111.x;
	z.y -= 2.0 * fractal->transformCommon.constantMultiplierA111.y;
	if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplierA111.z;
	aux.DE *= fabs(fractal->transformCommon.scale3 * fractal->transformCommon.scaleA1);

	z += fractal->transformCommon.additionConstantA000;

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}
