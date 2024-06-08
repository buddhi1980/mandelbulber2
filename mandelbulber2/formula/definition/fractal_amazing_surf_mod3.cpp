/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * asurf mod3  beta
 * amazing surf from Mandelbulber3D. Formula proposed by Kali, with features added by DarkBeam
 *
 * Note for the original version apply a c.x c.y SWAP
 *
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalAmazingSurfMod3::cFractalAmazingSurfMod3() : cAbstractFractal()
{
	nameInComboBox = "Amazing Surf - Mod 3";
	internalName = "amazing_surf_mod3";
	internalID = fractal::amazingSurfMod3;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalAmazingSurfMod3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;
	CVector4 c = aux.const_c;

	oldZ = z;

	bool functionEnabledN[3] = {fractal->transformCommon.functionEnabledAx,
		fractal->transformCommon.functionEnabledAyFalse,
		fractal->transformCommon.functionEnabledAzFalse};
	int startIterationN[3] = {fractal->transformCommon.startIterationsA,
		fractal->transformCommon.startIterationsB, fractal->transformCommon.startIterationsC};
	int stopIterationN[3] = {fractal->transformCommon.stopIterationsA,
		fractal->transformCommon.stopIterationsB, fractal->transformCommon.stopIterationsC};
	enumMulti_orderOf3Folds foldN[3] = {fractal->aSurf3Folds.orderOf3Folds1,
		fractal->aSurf3Folds.orderOf3Folds2, fractal->aSurf3Folds.orderOf3Folds3};

	for (int f = 0; f < 3; f++)
	{
		if (functionEnabledN[f] && aux.i >= startIterationN[f] && aux.i < stopIterationN[f])
		{
			switch (foldN[f])
			{
				case multi_orderOf3Folds_type1: // tglad fold
				default:
					if (fractal->transformCommon.functionEnabledCx)
					{
						z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
									- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
						z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
									- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
						zCol = z;
					}
					break;
				case multi_orderOf3Folds_type2:
					if (fractal->transformCommon.functionEnabledDFalse)
					{
						// tglad fold
						CVector4 foldMod = fractal->transformCommon.offsetA222;
						CVector4 fold = fractal->transformCommon.offset111;
						CVector4 sg = z;
						sg.x = sign(z.x);
						sg.y = sign(z.y);

						CVector4 folder = z;
						CVector4 Tglad = z;

						Tglad.x = fabs(z.x + fold.x) - fabs(z.x - fold.x) - z.x;
						Tglad.y = fabs(z.y + fold.y) - fabs(z.y - fold.y) - z.y;

						folder.x = sg.x * (z.x - Tglad.x);
						folder.y = sg.y * (z.y - Tglad.y);
						folder.x = fabs(folder.x);
						folder.y = fabs(folder.y);
						folder.x = min(folder.x, foldMod.x);
						folder.y = min(folder.y, foldMod.y);
						z.x -= sg.x * folder.x;
						z.y -= sg.y * folder.y;
						zCol = z;
					}
					break;
				case multi_orderOf3Folds_type3:
					if (fractal->transformCommon.functionEnabledEFalse)
					{
						// z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold), darkbeam's
						z.x = fractal->transformCommon.offset222.x
									- fabs(fabs(z.x + fractal->transformCommon.offsetA111.x)
												 - fractal->transformCommon.offset222.x)
									- fractal->transformCommon.offsetA111.x;
						z.y = fractal->transformCommon.offset222.y
									- fabs(fabs(z.y + fractal->transformCommon.offsetA111.y)
												 - fractal->transformCommon.offset222.y)
									- fractal->transformCommon.offsetA111.y;
						zCol = z;
					}
					break;
			}
		}
	}

	// enable z axis
	if (fractal->transformCommon.functionEnabledAxFalse
			&& aux.i >= fractal->transformCommon.startIterationsZ
			&& aux.i < fractal->transformCommon.stopIterationsZ)
	{
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		zCol.z = z.z;
	}

	// swizzle, swap xy alternating
	if (fractal->transformCommon.functionEnabledSwFalse
			&& aux.i >= fractal->transformCommon.startIterationsI
			&& aux.i < fractal->transformCommon.stopIterationsI)
	{
		z = CVector4(z.y, z.x, z.z, z.w);
	}
	// change y sign if iter is even
	if (fractal->transformCommon.functionEnabledJFalse
			&& aux.i >= fractal->transformCommon.startIterationsJ
			&& aux.i < fractal->transformCommon.stopIterationsJ)
	{
		if (aux.i % 2 < 1) z.y = -z.y;
	}

	// change z sign if iter is even
	if (fractal->transformCommon.functionEnabledKFalse
			&& aux.i >= fractal->transformCommon.startIterationsK
			&& aux.i < fractal->transformCommon.stopIterationsK)
	{
		if (aux.i % 2 < 1) z.z = -z.z;
	}

	// offset
	if (fractal->transformCommon.functionEnabledBzFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			CVector4 temp = fractal->transformCommon.additionConstant000;
			CVector4 temp2 = temp * temp;

			z.x -= ((temp.x * temp2.x) / ((z.x * z.x) + (temp2.x)) - 2.0 * temp.x)
						 * fractal->transformCommon.scale1;
			z.y -= ((temp.y * temp2.y) / ((z.y * z.y) + (temp2.y)) - 2.0 * temp.y)
						 * fractal->transformCommon.scale1;
			z.z -= ((temp.z * temp2.z) / ((z.z * z.z) + (temp2.z)) - 2.0 * temp.z)
						 * fractal->transformCommon.scale1;
		}
		if (fractal->transformCommon.functionEnabledByFalse)
		{
			CVector4 temp = fractal->transformCommon.additionConstant000;
			CVector4 temp2 = temp * temp;

			z.x -= ((temp2.x) / ((z.x * z.x) + (temp2.x)) - 2.0 * temp.x)
						 * fractal->transformCommon.scale1; // * sign(z.x);
			z.y -= ((temp2.y) / ((z.y * z.y) + (temp2.y)) - 2.0 * temp.y)
						 * fractal->transformCommon.scale1; // * sign(z.y);
			z.z -= ((temp2.z) / ((z.z * z.z) + (temp2.z)) - 2.0 * temp.z)
						 * fractal->transformCommon.scale1; // * sign(z.z);
		}
	}
	else if (aux.i >= fractal->transformCommon.startIterationsO
					 && aux.i < fractal->transformCommon.stopIterationsO)
		z += fractal->transformCommon.additionConstant000;

	// standard functions
	if (fractal->transformCommon.functionEnabledAy)
	{
		double rr = z.Dot(z);
		rrCol = rr;
		if (fractal->transformCommon.functionEnabledFalse
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD) // force cylinder fold
			rr -= z.z * z.z * fractal->transformCommon.scaleB1;		 // fold weight

		// Mandelbox Spherical fold
		if (aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
		{
			z += fractal->mandelbox.offset;

			// if (r2 < 1e-21) r2 = 1e-21;
			if (rr < fractal->transformCommon.minR0)
			{
				// double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR0;
				z *= fractal->transformCommon.maxMinR0factor;
				aux.DE *= fractal->transformCommon.maxMinR0factor;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
				z *= tglad_factor2;
				aux.DE *= tglad_factor2;
			}
			z -= fractal->mandelbox.offset;
		}

		// scale
		double useScale = 1.0;
		if (aux.i >= fractal->transformCommon.startIterationsS
				&& aux.i < fractal->transformCommon.stopIterationsS)
		{
			useScale = aux.actualScaleA + fractal->mandelbox.scale;

			z *= useScale;

			if (!fractal->analyticDE.enabledFalse)
				aux.DE = aux.DE * fabs(useScale) + 1.0;
			else
				aux.DE = aux.DE * fabs(useScale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;

			if (fractal->transformCommon.functionEnabledFFalse
					&& aux.i >= fractal->transformCommon.startIterationsY
					&& aux.i < fractal->transformCommon.stopIterationsY)
			{
				// update actualScaleA for next iteration
				double vary = fractal->transformCommon.scaleVary0
											* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
				if (fractal->transformCommon.functionEnabledMFalse)
					aux.actualScaleA = -vary;
				else
					aux.actualScaleA = aux.actualScaleA - vary;
			}
		}
	}

	if (fractal->mandelbox.mainRotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->mandelbox.color.factor.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->mandelbox.color.factor.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->mandelbox.color.factor.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);

		if (rrCol < fractal->transformCommon.maxR2d1)
		{
			colorAdd += fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
										+ fractal->mandelbox.color.factorSp2
												* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
		}
		aux.color += colorAdd;
	}
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
	{
		CVector4 tempC = aux.const_c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
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
			switch (fractal->mandelbulbMulti.orderOfXYZ)
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
		if (fractal->transformCommon.rotationEnabled
				&& aux.i >= fractal->transformCommon.startIterationsG
				&& aux.i < fractal->transformCommon.stopIterationsG)
		{
			tempC = fractal->transformCommon.rotationMatrix.RotateVector(tempC);
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}

	// invert c
	if (fractal->transformCommon.functionEnabledCxFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			aux.c *= fractal->transformCommon.scale3D111;
			double rSqrL = aux.c.Dot(aux.c);
			rSqrL = 1.0 / rSqrL;
			aux.c *= rSqrL;
			z += aux.c;
		}
		else
		{
			c *= fractal->transformCommon.scale3D111;
			double rSqrL = c.Dot(c);
			rSqrL = 1.0 / rSqrL;
			z += c * rSqrL;
		}
	}
}
