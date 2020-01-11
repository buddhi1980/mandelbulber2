/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * ABoxModKaliV2, a formula from Mandelbulb3D
 * @reference http://www.fractalforums.com/new-theories-and-research/aboxmodkali-the-2d-version/
 */

#include "all_fractal_definitions.h"

cFractalAboxModKaliV2::cFractalAboxModKaliV2() : cAbstractFractal()
{
	nameInComboBox = "Abox - Mod Kali-V2";
	internalName = "abox_mod_kali_v2";
	internalID = fractal::aboxModKaliV2;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionIFS;
}

void cFractalAboxModKaliV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;

	// fold
	CVector4 oldZ = z;
	z.x = fractal->transformCommon.additionConstant0555.x - fabs(z.x);
	z.y = fractal->transformCommon.additionConstant0555.y - fabs(z.y);
	if (fractal->transformCommon.functionEnabledM)
	{
		z.z = fractal->transformCommon.additionConstant0555.z - fabs(z.z);
	}
	CVector4 postZ = z;

	// spherical fold & scaling
	double rr = z.Dot(z);
	// z += fractal->transformCommon.additionConstant000;
	double m = fractal->transformCommon.scale015;
	if (rr < fractal->transformCommon.minR2p25)
	{
		m *= fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
		colorAdd += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		m *= fractal->transformCommon.maxR2d1 / rr;
		colorAdd += fractal->mandelbox.color.factorSp2;
	}
	// z -= fractal->transformCommon.additionConstant000;
	z = z * m;
	aux.DE = aux.DE * fabs(m) + 1.0;

	z += fractal->transformCommon.additionConstant000;

	// rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	z += fractal->transformCommon.additionConstant000;

	// add cpixel symmetrical
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
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

	// sign options
	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x *= sign(oldZ.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y *= sign(oldZ.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z *= sign(oldZ.z);
	}

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	// aux.color controls
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		postZ -= oldZ;
		if (postZ.x < 1.0) aux.color += fractal->mandelbox.color.factor.x;
		if (postZ.y < 1.0) aux.color += fractal->mandelbox.color.factor.y;
		if (postZ.z < 1.0) aux.color += fractal->mandelbox.color.factor.z;
		aux.color += colorAdd;
	}
}
