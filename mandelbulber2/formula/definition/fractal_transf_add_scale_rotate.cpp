/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Adds c constant to z vector
 */

#include "all_fractal_definitions.h"

cFractalTransfAddScaleRotate::cFractalTransfAddScaleRotate() : cAbstractFractal()
{
	nameInComboBox = "T>Add Scale Rotate";
	internalName = "transf_add_scale_rotate";
	internalID = fractal::transfAddScaleRotate;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddScaleRotate::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	z += fractal->transformCommon.additionConstant000;

	z *= fractal->transformCommon.scale1;

	if (!fractal->analyticDE.enabledFalse)
		aux.DE *= fabs(fractal->transformCommon.scale1);
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scale1) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset1;

	z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	z += fractal->transformCommon.additionConstantA000;
}
