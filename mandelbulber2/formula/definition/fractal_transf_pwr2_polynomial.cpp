/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * z = (Az^2 + Bz + C ) / D; ((VectA +ScaleA * z) * fn(z) + scaleB * z + VectC ) * ScaleD
 */

#include "all_fractal_definitions.h"

cFractalTransfPwr2Polynomial::cFractalTransfPwr2Polynomial() : cAbstractFractal()
{
	nameInComboBox = "T>Power2 Polynomial";
	internalName = "transf_pwr2_polynomial";
	internalID = fractal::transfPwr2Polynomial;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfPwr2Polynomial::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 partA = z;
	if (fractal->transformCommon.functionEnabledFalse) // fabs
		partA = fabs(z);
	if (fractal->transformCommon.functionEnabledxFalse) // pwr3 or z * fabs(z^2)
		partA *= z;
	partA = partA * fractal->transformCommon.scale2 + fractal->transformCommon.constantMultiplier111;

	CVector4 fnZ1 = z;
	if (fractal->transformCommon.functionEnabledBxFalse) // cos(z*Pi)
	{
		double scalePi = M_PI * fractal->transformCommon.scaleC1;
		fnZ1.x = cos(z.x * scalePi);
		fnZ1.y = cos(z.y * scalePi);
		fnZ1.z = cos(z.z * scalePi);
	}
	if (fractal->transformCommon.functionEnabledyFalse) // pi rotation
		fnZ1 = fnZ1.RotateAroundVectorByAngle(fractal->transformCommon.constantMultiplier111.GetXYZ(),
			M_PI * fractal->transformCommon.scale0);				// * cPI ;
	if (fractal->transformCommon.functionEnabledzFalse) // box offset
	{
		fnZ1.x = fnZ1.x + sign(fnZ1.x) * fractal->transformCommon.additionConstant000.x;
		fnZ1.y = fnZ1.y + sign(fnZ1.y) * fractal->transformCommon.additionConstant000.y;
		fnZ1.z = fnZ1.z + sign(fnZ1.z) * fractal->transformCommon.additionConstant000.z;
	}

	if (fractal->transformCommon.functionEnabledAxFalse) // fabs fnZ1
		fnZ1 = fabs(fnZ1);

	CVector4 partB = z;
	if (fractal->transformCommon.functionEnabledAzFalse) partB = fabs(z);
	partB *= fractal->transformCommon.scale4;

	CVector4 constantMult = CVector4(fractal->transformCommon.constantMultiplierB111.x,
		fractal->transformCommon.constantMultiplierB111.y,
		fractal->transformCommon.constantMultiplierB111.z, 0.0);

	z = constantMult + partB - partA * fnZ1;
	z *= fractal->transformCommon.scale025;

	aux.DE = aux.DE * 4.0 * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
