/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger Prism Shape2
 * from code by Knighty
 * http://www.fractalforums.com/fragmentarium/
 * cross-menger!-can-anyone-do-this/msg93972/#new
 */

#include "all_fractal_definitions.h"

cFractalMengerPrismShape2::cFractalMengerPrismShape2() : cAbstractFractal()
{
	nameInComboBox = "Menger - Prism Shape2";
	internalName = "menger_prism_shape2";
	internalID = fractal::mengerPrismShape2;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerPrismShape2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		z = CVector4(-z.z, z.x, z.y, z.w);
	}
	CVector4 gap = fractal->transformCommon.constantMultiplier000;

	if (fractal->transformCommon.functionEnabledx
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		double t;
		double dot1;
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			z.x = fabs(z.x);
		}
		if (fractal->transformCommon.functionEnabledCy)
		{
			z.y = fabs(z.y);
		}
		if (fractal->transformCommon.functionEnabledCz)
		{
			z.z = fabs(z.z);
		}
		double tempOff = (fractal->transformCommon.offset0 + SQRT_3_4d2);
		switch (fractal->combo.modeA)
		{
			case combo_mode0:
			default:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.x -= t * -SQRT_3 - tempOff;
				z.y = fabs(z.y - t);
				break;
			case combo_mode1:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - tempOff;
				z.x = fabs(z.x - t); // x y swap
				break;
			case combo_mode2:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.z -= t * -SQRT_3 - tempOff;
				z.y = fabs(z.y - t); // z y swap
				break;
			case combo_mode3:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - tempOff;
				z.z = fabs(z.z - t); // z y swap
				break;
			case combo_mode4:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - tempOff;
				z.z -= t * -SQRT_3 - tempOff;
				z.x = fabs(z.x - t); // x y swap and other things
				z.y = fabs(z.y - t);
				break;
			case combo_mode5:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.x -= t * -SQRT_3 - tempOff;
				z.y -= t * -SQRT_3 - tempOff;
				z.y = fabs(z.y - t); // x y swap and other things
				z.z = fabs(z.z - t);
				break;
			case combo_mode6:
				dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.y -= t * -SQRT_3 - (fractal->transformCommon.offset0);
				z.z -= t * -SQRT_3 - (fractal->transformCommon.offset0);
				z.x = fabs(z.y - t); // x y swap and other things and swizzle
				z.y = fabs(z.x - t);
				break;
			case combo_mode7:
				dot1 = (z.z * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
				t = max(0.0, dot1);
				z.x -= t * -SQRT_3 - (fractal->transformCommon.offset0);
				z.y -= t * -SQRT_3 - (fractal->transformCommon.offset0);
				z.y = fabs(z.z - t); // x y swap and other things and swizzle
				z.z = fabs(z.y - t);
				break;
		}

		if (z.y > z.z) swap(z.y, z.z);
		z.y -= fractal->transformCommon.offsetB0;

		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);

		if (z.z > z.x) swap(z.z, z.x);

		if (fractal->transformCommon.functionEnabledyFalse)
		{
			z.y = max(0.0, z.y) * fractal->transformCommon.scaleA1;
			z.z = max(0.0, z.z) * fractal->transformCommon.scaleB1;
		}
		if (fractal->transformCommon.functionEnabledzFalse)
		{
			if (z.x >= 0.0)
			{
				z.y = max(0.0, z.y) * fractal->transformCommon.scaleA1;
				z.z = max(0.0, z.z) * fractal->transformCommon.scaleB1;
			}
		}
		z *= fractal->transformCommon.scale1;
		aux.DE *= fractal->transformCommon.scale1;
		aux.DE *= fractal->transformCommon.scaleC1;
	}

	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)

	{ // CrossMengerTrick
		double dd = aux.DE;
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			z.x = fabs(z.x);
		}
		double dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scaleD1;
		double t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		if (fractal->transformCommon.functionEnabledBzFalse)
		{
			z.y = fabs(z.y) - t;
		}
		else
		{
			z.y = fabs(z.y - t);
		}
		z.x -= SQRT_3_4;
		double dy = 0.0;
		double dz = 0.0;
		if (z.y > 0.5 && z.z > 0.5)
		{
			dy = 1.5;
			dz = 1.5;
		}
		else if (z.z < z.y)
		// else if ((z.y - 1.5) * (z.y - 1.5) + z.z * z.z < z.y * z.y + (z.z - 1.5) * (z.z - 1.5))
		{
			dy = 1.5;
		}
		else
			dz = 1.5;

		z.y -= dy;
		z.z -= dz;
		z *= fractal->transformCommon.scaleA3;
		aux.DE *= fractal->transformCommon.scaleA3;
		z.y += dy;
		z.z += dz;
		z.x += SQRT_3_4;

		if (fractal->transformCommon.functionEnabledFalse)
		{
			dd *= FRAC_1_3 * fractal->transformCommon.scaleE1;
			z *= dd;
			aux.DE *= dd;
		}
		aux.DE *= fractal->transformCommon.offset1; //.DE tweak cross menger trick part
	}
	if (fractal->transformCommon.functionEnabledPFalse)
	{
		z.x = fabs(z.x + fractal->transformCommon.offset) + fractal->transformCommon.offsetC0;
	}

	// void KIFS(vec3 z)
	//{//Pure KIFS... almost correct

	if (fractal->transformCommon.functionEnabledKFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		if (fractal->transformCommon.functionEnabledCzFalse)
		{
			z.x = fabs(fractal->transformCommon.offset - z.x) + fractal->transformCommon.offsetC0;
		}

		z.y = fabs(z.y);
		z.z = fabs(z.z);

		double dot2 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scaleF1;
		double t = max(0.0, dot2);

		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);

		z.y = fabs(z.y - 0.5) + 0.5;

		z -= CVector4(0.5 * SQRT_3, 1.5, 1.5, 0.0);

		z *= fractal->transformCommon.scaleB3;
		aux.DE *= fractal->transformCommon.scaleB3;

		z += CVector4(0.5 * SQRT_3, 1.5, 1.5, 0.0);
	}

	if (fractal->transformCommon.benesiT1EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 tempZ = z;
		double tempL = tempZ.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
		z = z - fractal->transformCommon.offset200;
	}

	if (fractal->transformCommon.benesiT1MEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsTM1)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		CVector4 temp = z;
		double tempL = temp.Length();
		z = fabs(z) * fractal->transformCommon.scale3D333;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale + 1.0;

		z = (fabs(z + fractal->transformCommon.additionConstant111)
				 - fabs(z - fractal->transformCommon.additionConstant111) - z);

		tempXZ = (z.y + z.x) * SQRT_1_2;

		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
	}

	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		CVector4 tempA, tempB;

		if (fractal->transformCommon.functionEnabledAx)
			tempA.x = fabs(z.x + fractal->transformCommon.offsetF000.x);

		if (fractal->transformCommon.functionEnabledx)
			tempB.x = fabs(z.x - fractal->transformCommon.offset000.x);

		z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale3D111.x);

		if (fractal->transformCommon.functionEnabledAy)
			tempA.y = fabs(z.y + fractal->transformCommon.offsetF000.y);

		if (fractal->transformCommon.functionEnabledy)
			tempB.y = fabs(z.y - fractal->transformCommon.offset000.y);

		z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale3D111.y);

		if (fractal->transformCommon.functionEnabledAz)
			tempA.z = fabs(z.z + fractal->transformCommon.offsetF000.z);

		if (fractal->transformCommon.functionEnabledz)
			tempB.z = fabs(z.z - fractal->transformCommon.offset000.z);

		z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale3D111.z);

		z += fractal->transformCommon.offsetA000;
	}

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	// menger
	if (fractal->transformCommon.functionEnabledM
			&& aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
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

		z += fractal->transformCommon.additionConstantA000;
	}
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
