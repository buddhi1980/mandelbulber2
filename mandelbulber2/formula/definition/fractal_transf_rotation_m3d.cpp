/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * compatible order of rotation for M3D
 */

#include "all_fractal_definitions.h"

cFractalTransfRotationM3d::cFractalTransfRotationM3d() : cAbstractFractal()
{
	nameInComboBox = "T>Rotation M3D";
	internalName = "transf_rotation_m3d";
	internalID = fractal::transfRotationM3d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfRotationM3d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	z = fractal->transformCommon.rotationMatrixXYZ.RotateVector(z);
}
