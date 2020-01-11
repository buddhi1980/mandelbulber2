/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Sponge Polynomial Hybrid modified by Mclarekin
 */

#include "all_fractal_definitions.h"

cFractalMengerPwr2Poly::cFractalMengerPwr2Poly() : cAbstractFractal()
{
	nameInComboBox = "Menger - Pwr2 Polynomial";
	internalName = "menger_pwr2_poly";
	internalID = fractal::mengerPwr2Poly;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionIFS;
}

void cFractalMengerPwr2Poly::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;

	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		CVector4 partA = z;
		if (fractal->transformCommon.functionEnabledFalse) // fabs
			partA = fabs(z);
		if (fractal->transformCommon.functionEnabledxFalse) // pwr3 or z * fabs(z^2)
			partA *= z;
		partA =
			partA * fractal->transformCommon.scale2 + fractal->transformCommon.constantMultiplier111;

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
				M_PI * fractal->transformCommon.scaleB1);

		if (fractal->transformCommon.functionEnabledzFalse) // box offset
		{
			fnZ1.x = (sign(fnZ1.x) * fractal->transformCommon.additionConstant000.x) + fnZ1.x;
			fnZ1.y = (sign(fnZ1.y) * fractal->transformCommon.additionConstant000.y) + fnZ1.y;
			fnZ1.z = (sign(fnZ1.z) * fractal->transformCommon.additionConstant000.z) + fnZ1.z;
		}

		if (fractal->transformCommon.functionEnabledAxFalse) // fabs(fnZ1)
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
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZC)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplierC111;
	}

	int count = fractal->transformCommon.int1; // Menger Sponge
	int k;
	for (k = 0; k < count; k++)
	{
		z = fabs(z);
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		z *= fractal->transformCommon.scale3;
		z.x -= 2.0 * fractal->transformCommon.constantMultiplierA111.x;
		z.y -= 2.0 * fractal->transformCommon.constantMultiplierA111.y;
		if (z.z > 1.0) z.z -= 2.0 * fractal->transformCommon.constantMultiplierA111.z;

		aux.DE *= fractal->transformCommon.scale3;

		if (fractal->transformCommon.rotationEnabled
				&& aux.i >= fractal->transformCommon.startIterationsA
				&& aux.i < fractal->transformCommon.stopIterationsA) // rotation
		{
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}
		z += fractal->transformCommon.additionConstantA000;
	}
}
