/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Amazing Surface Multi
 * Based on Amazing Surf Mod 1 from Mandelbulber3D, a formula proposed by Kali,
 * with features added by DarkBeam
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalAmazingSurfMulti::cFractalAmazingSurfMulti() : cAbstractFractal()
{
	nameInComboBox = "Amazing Surf - Multi";
	internalName = "amazing_surf_multi";
	internalID = fractal::amazingSurfMulti;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalAmazingSurfMulti::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	CVector4 oldZ = z;
	bool functionEnabledN[5] = {fractal->transformCommon.functionEnabledAx,
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
	enumMulti_orderOfFolds foldN[5] = {fractal->surfFolds.orderOfFolds1,
		fractal->surfFolds.orderOfFolds2, fractal->surfFolds.orderOfFolds3,
		fractal->surfFolds.orderOfFolds4, fractal->surfFolds.orderOfFolds5};

	for (int f = 0; f < 5; f++)
	{
		if (functionEnabledN[f] && aux.i >= startIterationN[f] && aux.i < stopIterationN[f])
		{
			switch (foldN[f])
			{
				case multi_orderOfFolds_type1: // tglad fold
				default:
					z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
								- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
					z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
								- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
					if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
					if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
					break;
				case multi_orderOfFolds_type2: // z = fold - fabs( fabs(z) - fold)
					z.x = fractal->transformCommon.additionConstant111.x
								- fabs(fabs(z.x) - fractal->transformCommon.additionConstant111.x);
					z.y = fractal->transformCommon.additionConstant111.y
								- fabs(fabs(z.y) - fractal->transformCommon.additionConstant111.y);
					if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
					if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
					break;
				case multi_orderOfFolds_type3:
					z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
					z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
					break;
				case multi_orderOfFolds_type4:
					// if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
					if (fabs(z.x) > fractal->transformCommon.additionConstant111.x)
					{
						z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
						colorAdd += fractal->mandelbox.color.factor.x;
					}
					if (fabs(z.y) > fractal->transformCommon.additionConstant111.y)
					{
						z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
						colorAdd += fractal->mandelbox.color.factor.y;
					}
					break;
				case multi_orderOfFolds_type5:
					// z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
					z.x = fractal->transformCommon.offset2
								- fabs(fabs(z.x + fractal->transformCommon.additionConstant111.x)
											 - fractal->transformCommon.offset2)
								- fractal->transformCommon.additionConstant111.x;
					z.y = fractal->transformCommon.offset2
								- fabs(fabs(z.y + fractal->transformCommon.additionConstant111.y)
											 - fractal->transformCommon.offset2)
								- fractal->transformCommon.additionConstant111.y;
					if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
					if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
					break;
			}
		}
	}

	if (fractal->transformCommon.functionEnabledAxFalse)
	{ // enable z scale
		double zLimit = fractal->transformCommon.foldingLimit * fractal->transformCommon.scale0;
		double zValue = fractal->transformCommon.foldingValue * fractal->transformCommon.scale0;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
		}
	}
	z += fractal->transformCommon.additionConstant000;

	// standard functions
	if (fractal->transformCommon.functionEnabledAy)
	{
		double rr;
		rr = z.Dot(z);
		if (fractal->transformCommon.functionEnabledFalse)		// force cylinder fold
			rr -= z.z * z.z * fractal->transformCommon.scaleB1; // fold weight;

		if (fractal->transformCommon.functionEnabledAz
				&& aux.i >= fractal->transformCommon.startIterationsT
				&& aux.i < fractal->transformCommon.stopIterationsT)
		{
			// Abox Spherical fold
			z += fractal->mandelbox.offset;
			double sqrtMinR = fractal->transformCommon.sqtR;

			if (rr < sqrtMinR)
			{
				z *= fractal->transformCommon.mboxFactor1;
				aux.DE *= fractal->transformCommon.mboxFactor1;
				colorAdd += fractal->mandelbox.color.factorSp1;
			}
			else if (rr < 1.0)
			{
				z *= 1.0 / rr;
				aux.DE *= 1.0 / rr;
				colorAdd += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->mandelbox.offset;
		}

		// Mandelbox Spherical fold
		if (fractal->transformCommon.functionEnabledzFalse
				&& aux.i >= fractal->transformCommon.startIterationsM
				&& aux.i < fractal->transformCommon.stopIterationsM)
		{
			// r2 = z.Dot(z);
			z += fractal->mandelbox.offset;
			if (rr < fractal->mandelbox.mR2) // mR2 = minR^2
			{
				z *= fractal->mandelbox.mboxFactor1; // fR2/mR2
				aux.DE *= fractal->mandelbox.mboxFactor1;
				colorAdd += fractal->mandelbox.color.factorSp1;
			}
			else if (rr < fractal->mandelbox.fR2)
			{
				double tglad_factor2 = fractal->mandelbox.fR2 / rr;
				z *= tglad_factor2;
				aux.DE *= tglad_factor2;
				colorAdd += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->mandelbox.offset;
		}

		if (aux.i >= fractal->transformCommon.startIterationsS
				&& aux.i < fractal->transformCommon.stopIterationsS)
		{ // scale
			z *= fractal->mandelbox.scale * fractal->transformCommon.scale1
					 + 1.0 * (1.0 - fractal->transformCommon.scale1);
			aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
		}
	}
	if (fractal->transformCommon.addCpixelEnabledFalse)
	{
		CVector4 tempC = c;
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
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	if (fractal->mandelbox.mainRotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	aux.color += colorAdd;
}
