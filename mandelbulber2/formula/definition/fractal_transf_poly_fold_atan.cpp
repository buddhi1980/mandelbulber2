/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * poly fold atan
 * @reference
 * https://fractalforums.org/fragmentarium/17/polyfoldsym-pre-transform/2684
 */

#include "all_fractal_definitions.h"

cFractalTransfPolyFoldAtan::cFractalTransfPolyFoldAtan() : cAbstractFractal()
{
	nameInComboBox = "T>Poly Fold Atan";
	internalName = "transf_poly_fold_atan";
	internalID = fractal::transfPolyFoldAtan;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfPolyFoldAtan::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
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

	// addition constant
	z += fractal->transformCommon.additionConstant000;

	// rotation
	if (fractal->transformCommon.rotation2EnabledFalse)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	// DE tweaks
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
