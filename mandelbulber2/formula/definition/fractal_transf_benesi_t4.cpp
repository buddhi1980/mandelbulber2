/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * benesi T4
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */

#include "all_fractal_definitions.h"

cFractalTransfBenesiT4::cFractalTransfBenesiT4() : cAbstractFractal()
{
	nameInComboBox = "T>Benesi T4";
	internalName = "transf_benesi_t4";
	internalID = fractal::transfBenesiT4;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBenesiT4::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
	z = CVector4(
		(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

	CVector4 tempV2 = z;
	tempV2.x = (z.y * z.y + z.z * z.z);
	tempV2.y = (z.x * z.x + z.z * z.z); // switching, squared,
	tempV2.z = (z.x * z.x + z.y * z.y);
	z = (fabs(tempV2 - fractal->transformCommon.additionConstant111))
			* fractal->transformCommon.scale3D222;

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
