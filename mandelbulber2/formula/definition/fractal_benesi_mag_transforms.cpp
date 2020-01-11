/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * benesiMagTransforms
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * do-m3d-formula-have-to-be-distance-estimation-formulas/
 */

#include "all_fractal_definitions.h"

cFractalBenesiMagTransforms::cFractalBenesiMagTransforms() : cAbstractFractal()
{
	nameInComboBox = "Benesi - Mag Transforms";
	internalName = "benesi_mag_transforms";
	internalID = fractal::benesiMagTransforms;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalBenesiMagTransforms::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	// benesi T1
	if (fractal->transformCommon.benesiT1Enabled && aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		double tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
		z = CVector4(
			(tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2, z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

		double tempL = z.Length();
		z = fabs(z) * fractal->transformCommon.scale3D222;
		// if (tempL < 1e-21) tempL = 1e-21;
		double avgScale = z.Length() / tempL;
		aux.DE = aux.DE * avgScale;

		tempXZ = (z.y + z.x) * SQRT_1_2;
		z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
			z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
		z = z - fractal->transformCommon.offset200;
	}
	// rotation
	if (fractal->transformCommon.rotationEnabled)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	//  Benesi pine tree pwr2
	if (fractal->transformCommon.addCpixelEnabled
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		CVector4 zz = z * z;
		aux.r = sqrt(zz.x + zz.y + zz.z); // needed when alternating pwr2s
		aux.DE = aux.r * aux.DE * 2.0 + 1.0;

		double t = 1.0;
		double temp = zz.y + zz.z;
		if (temp > 0.0) t = 2.0 * z.x / sqrt(temp);
		temp = z.z;
		z.x = (zz.x - zz.y - zz.z);
		z.y = (2.0 * t * z.y * temp);
		z.z = (t * (zz.y - zz.z));

		// swap c.yz then add cPixel
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c * fractal->transformCommon.constantMultiplier100;
			tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w);
			aux.c = tempC;
		}
		else
		{
			tempC *= fractal->transformCommon.constantMultiplier100;
			tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w);
		}
		z += tempC;
	}

	if (fractal->transformCommon.juliaMode)
	{
		z.x += fractal->transformCommon.juliaC.x * fractal->transformCommon.constantMultiplier100.x;
		z.z += fractal->transformCommon.juliaC.y * fractal->transformCommon.constantMultiplier100.y;
		z.y += fractal->transformCommon.juliaC.z * fractal->transformCommon.constantMultiplier100.z;
	}
	// additional transform slot controls
	bool functionEnabledN[5] = {fractal->transformCommon.functionEnabledAxFalse,
		fractal->transformCommon.functionEnabledAyFalse,
		fractal->transformCommon.functionEnabledAzFalse,
		fractal->transformCommon.functionEnabledBxFalse,
		fractal->transformCommon.functionEnabledByFalse};
	int startIterationN[5] = {fractal->transformCommon.startIterationsA,
		fractal->transformCommon.startIterationsB, fractal->transformCommon.startIterationsC,
		fractal->transformCommon.startIterationsD, fractal->transformCommon.startIterationsE};
	int stopIterationN[5] = {fractal->transformCommon.stopIterationsA,
		fractal->transformCommon.stopIterationsB, fractal->transformCommon.stopIterationsC,
		fractal->transformCommon.stopIterationsD, fractal->transformCommon.stopIterationsE};

	enumMulti_orderOfTransf transfN[5] = {fractal->magTransf.orderOfTransf1,
		fractal->magTransf.orderOfTransf2, fractal->magTransf.orderOfTransf3,
		fractal->magTransf.orderOfTransf4, fractal->magTransf.orderOfTransf5};

	for (int f = 0; f < 5; f++)
	{
		if (functionEnabledN[f] && aux.i >= startIterationN[f] && aux.i < stopIterationN[f])
		{
			double tempXZ;
			double tempL;
			CVector4 temp;
			double avgScale;
			CVector4 tempV2;

			switch (transfN[f])
			{
				case multi_orderOfTransf_typeT1:
				default:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
					z = fabs(z) * fractal->transformCommon.scale3Da222;
					tempL = temp.Length();
					// if (tempL < 1e-21) tempL = 1e-21;
					avgScale = z.Length() / tempL;
					aux.DE = aux.DE * avgScale;
					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					z = z - fractal->transformCommon.offsetA200;
					break;

				case multi_orderOfTransf_typeT1Mod:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
					z = fabs(z) * fractal->transformCommon.scale3D333;
					tempL = temp.Length();
					// if (tempL < 1e-21) tempL = 1e-21;
					avgScale = z.Length() / tempL;
					aux.DE = aux.DE * avgScale;
					z = (fabs(z + fractal->transformCommon.offset111)
							 - fabs(z - fractal->transformCommon.offset111) - z);
					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					break;

				case multi_orderOfTransf_typeT2:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
					tempV2 = z;
					tempV2.x = sqrt(z.y * z.y + z.z * z.z);
					tempV2.y = sqrt(z.x * z.x + z.z * z.z); // switching, squared, sqrt
					tempV2.z = sqrt(z.x * z.x + z.y * z.y);
					z = fabs(tempV2 - fractal->transformCommon.offsetA111);
					temp = z;
					tempL = temp.Length();
					z = fabs(z) * fractal->transformCommon.scale3D444;
					// if (tempL < 1e-21) tempL = 1e-21;
					avgScale = z.Length() / tempL;
					aux.DE = aux.DE * avgScale;
					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					break;

				case multi_orderOfTransf_typeT3:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

					tempV2 = z;
					tempV2.x = (z.y + z.z);
					tempV2.y = (z.x + z.z); // switching
					tempV2.z = (z.x + z.y);
					z = (fabs(tempV2 - fractal->transformCommon.offset222))
							* fractal->transformCommon.scale3Db222;
					aux.DE *= z.Length() / tempV2.Length();

					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					break;

				case multi_orderOfTransf_typeT4:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);

					tempV2 = z;
					tempV2.x = (z.y * z.y + z.z * z.z);
					tempV2.y = (z.x * z.x + z.z * z.z); // switching, squared,
					tempV2.z = (z.x * z.x + z.y * z.y);
					z = (fabs(tempV2 - fractal->transformCommon.offsetB111))
							* fractal->transformCommon.scale3Dc222;
					aux.DE *= z.Length() / tempV2.Length();

					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					break;

				case multi_orderOfTransf_typeT5b:
					tempXZ = z.x * SQRT_2_3 - z.z * SQRT_1_3;
					z = CVector4((tempXZ - z.y) * SQRT_1_2, (tempXZ + z.y) * SQRT_1_2,
						z.x * SQRT_1_3 + z.z * SQRT_2_3, z.w);
					// if (z.x > -1e-21 && z.x < 1e-21)
					//	z.x = (z.x > 0) ? 1e-21 : -1e-21;
					// if (z.y > -1e-21 && z.y < 1e-21)
					//	z.y = (z.y > 0) ? 1e-21 : -1e-21;
					// if (z.z > -1e-21 && z.z < 1e-21)
					//	z.z = (z.z > 0) ? 1e-21 : -1e-21;
					tempV2 = z;
					tempV2.x = fabs(
						pow(pow(z.y, fractal->transformCommon.int8X) + pow(z.z, fractal->transformCommon.int8X),
							fractal->transformCommon.power025.x));
					tempV2.y = fabs(
						pow(pow(z.x, fractal->transformCommon.int8Y) + pow(z.z, fractal->transformCommon.int8Y),
							fractal->transformCommon.power025.y));
					tempV2.z = fabs(
						pow(pow(z.x, fractal->transformCommon.int8Z) + pow(z.y, fractal->transformCommon.int8Z),
							fractal->transformCommon.power025.z));
					z = (fabs(tempV2 - fractal->transformCommon.offsetC111))
							* fractal->transformCommon.scale3Dd222;
					aux.DE *= z.Length() / tempV2.Length();

					tempXZ = (z.y + z.x) * SQRT_1_2;
					z = CVector4(z.z * SQRT_1_3 + tempXZ * SQRT_2_3, (z.y - z.x) * SQRT_1_2,
						z.z * SQRT_2_3 - tempXZ * SQRT_1_3, z.w);
					break;
			}
		}
	}
	// analyticDE controls
	if (fractal->analyticDE.enabled)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
