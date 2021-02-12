/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * aexion octopus. Based on Aex_Octo from Mandelbulber3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalAexionOctopusMod::cFractalAexionOctopusMod() : cAbstractFractal()
{
	nameInComboBox = "Aexion Octopus Mod";
	internalName = "aexion_octopus_mod";
	internalID = fractal::aexionOctopusMod;
	DEType = deltaDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelAlreadyHas;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalAexionOctopusMod::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	CVector4 tempN;
	tempN.x = z.x * z.z * fractal->transformCommon.scale3D111.x;
	tempN.y = (z.x * z.x - z.z * z.z) * fractal->transformCommon.scale3D111.y;
	tempN.z = z.y;
	tempN.w = z.w;

	if (fractal->transformCommon.functionEnabledAx) tempN.x = fabs(tempN.x);
	if (fractal->transformCommon.functionEnabledAy) tempN.y = fabs(tempN.y);
	if (fractal->transformCommon.functionEnabledAz) tempN.z = fabs(tempN.z);

	z = tempN;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempFAB = c;
		if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
		if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
		if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

		tempFAB *= fractal->transformCommon.constantMultiplier000;

		z.x += sign(z.x) * tempFAB.x;
		z.y += sign(z.y) * tempFAB.y;
		z.z += sign(z.z) * tempFAB.z;
	}

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE =
			aux.DE * fractal->analyticDE.scale1 * 2.0 * z.Length() / aux.r + fractal->analyticDE.offset1;
	}
}
