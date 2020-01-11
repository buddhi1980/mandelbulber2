/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Quaternion3DE - Quaternion fractal with extended controls
 * @reference http://www.fractalforums.com/3d-fractal-generation
 * /true-3d-mandlebrot-type-fractal/
 */

#include "all_fractal_definitions.h"

cFractalQuaternion3d::cFractalQuaternion3d() : cAbstractFractal()
{
	nameInComboBox = "Quaternion3D";
	internalName = "quaternion3d";
	internalID = fractal::quaternion3d;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalQuaternion3d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r;
	z = CVector4(z.x * z.x - z.y * z.y - z.z * z.z, z.x * z.y, z.x * z.z, z.w);

	double tempL = z.Length();
	z *= fractal->transformCommon.constantMultiplier122;
	// if (tempL < 1e-21) tempL = 1e-21;
	CVector4 tempAvgScale = CVector4(z.x, z.y / 2.0, z.z / 2.0, z.w);
	double avgScale = tempAvgScale.Length() / tempL;
	double tempAux = aux.DE * avgScale;
	aux.DE = aux.DE + (tempAux - aux.DE) * fractal->transformCommon.scaleA1;

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	z += fractal->transformCommon.additionConstant000;
}
