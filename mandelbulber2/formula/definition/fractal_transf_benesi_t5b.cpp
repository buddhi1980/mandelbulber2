/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * benesi T5
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */

#include "all_fractal_definitions.h"

cFractalTransfBenesiT5b::cFractalTransfBenesiT5b() : cAbstractFractal()
{
	nameInComboBox = "T>Benesi T5b";
	internalName = "transf_benesi_t5b";
	internalID = fractal::transfBenesiT5b;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBenesiT5b::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector4(
		(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

	// if (z.x > -1e-21 && z.x < 1e-21)
	// z.x = (z.x > 0) ? 1e-21 : -1e-21;
	// if (z.y > -1e-21 && z.y < 1e-21)
	// z.y = (z.y > 0) ? 1e-21 : -1e-21;
	// if (z.z > -1e-21 && z.z < 1e-21)
	// z.z = (z.z > 0) ? 1e-21 : -1e-21;
	CVector4 tempV2 = z;
	tempV2.x =
		fabs(pow(pow(z.y, fractal->transformCommon.int8X) + pow(z.z, fractal->transformCommon.int8X),
			fractal->transformCommon.power025.x));
	tempV2.y =
		fabs(pow(pow(z.x, fractal->transformCommon.int8Y) + pow(z.z, fractal->transformCommon.int8Y),
			fractal->transformCommon.power025.y));
	tempV2.z =
		fabs(pow(pow(z.x, fractal->transformCommon.int8Z) + pow(z.y, fractal->transformCommon.int8Z),
			fractal->transformCommon.power025.z));
	z = (fabs(tempV2 - fractal->transformCommon.offsetC111)) * fractal->transformCommon.scale3Dd222;

	double avgScale = z.Length() / tempV2.Length();
	if (fractal->analyticDE.enabled)
	{
		aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}

	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	tempXZ = (z.y + z.x) * SQRT_1_2;
	z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
		z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
}
