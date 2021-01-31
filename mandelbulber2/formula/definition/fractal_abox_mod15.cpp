/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * amazing surf Mod4 based on Mandelbulber3D. Formula proposed by Kali, with features added by
 * DarkBeam
 * This formula has a c.x c.y SWAP
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalAboxMod15::cFractalAboxMod15() : cAbstractFractal()
{
	nameInComboBox = "Abox - Mod 15";
	internalName = "abox_mod15";
	internalID = fractal::aboxMod15;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalAboxMod15::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;

	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (aux.i >= fractal->transformCommon.startIterations
				&& aux.i < fractal->transformCommon.stopIterations)
		{
			if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
			if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
			if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);
		}

		if (aux.i >= fractal->transformCommon.startIterationsT
				&& aux.i < fractal->transformCommon.stopIterationsT)
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: z = CVector4(z.x, z.y, z.z, z.w); break;
				case multi_OrderOfXYZ_xzy: z = CVector4(z.x, z.z, z.y, z.w); break;
				case multi_OrderOfXYZ_yxz: z = CVector4(z.y, z.x, z.z, z.w); break;
				case multi_OrderOfXYZ_yzx: z = CVector4(z.y, z.z, z.x, z.w); break;
				case multi_OrderOfXYZ_zxy: z = CVector4(z.z, z.x, z.y, z.w); break;
				case multi_OrderOfXYZ_zyx: z = CVector4(z.z, z.y, z.x, z.w); break;
			}
			if (fractal->transformCommon.functionEnabledxFalse) z.x = -z.x;
			if (fractal->transformCommon.functionEnabledyFalse) z.y = -z.y;
			if (fractal->transformCommon.functionEnabledzFalse) z.z = -z.z;
		}
	}

	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		z += fractal->transformCommon.offset000;
		double rr = z.Dot(z);
		z *= fractal->transformCommon.scaleG1 / rr;
		aux.DE *= (fractal->transformCommon.scaleG1 / rr);
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleA1;
		aux.DE *= fractal->transformCommon.scaleA1;
	}


	CVector4 oldZ = z;
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabled)
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
					- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
	}
	CVector4 zCol = z;


	// offset1
	if (aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
			z += fractal->transformCommon.offsetA000;

	// spherical fold
	double rrCol = 0.0;
	double m = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);
		rrCol = rr;
		if (rr < fractal->transformCommon.invert0) m = fractal->transformCommon.inv0;
		else if (rr < fractal->transformCommon.invert1) m = 1.0 / rr;
		else m = fractal->transformCommon.inv1;
		z *= m;
		aux.DE *= m;
	}

	// scale
	if (aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		double useScale = 1.0;

		useScale = (aux.actualScaleA + fractal->transformCommon.scale015); //  / dividend;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + fractal->analyticDE.offset0;
		if (fractal->transformCommon.functionEnabledKFalse)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			aux.actualScaleA = -vary;
		}
	}

	if (fractal->transformCommon.rotation2EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
		z += CVector4(c.y, c.x, c.z, c.w) * fractal->transformCommon.constantMultiplier111;

	z += fractal->transformCommon.additionConstantA000;

	if (aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
				z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	if (fractal->transformCommon.functionEnabledNFalse
			&& aux.i >= fractal->transformCommon.startIterationsN
			&& aux.i < fractal->transformCommon.stopIterationsN)
	{
		double foldX = fractal->transformCommon.offset1;
		double foldY = fractal->transformCommon.offsetA1;

		double t;
		z.x = fabs(z.x);
		z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAFalse)
		{
			t = z.x;
			z.x = z.y;
			z.y = t;
		}
		t = z.x;
		z.x = z.x + z.y - fractal->transformCommon.offset0;
		z.y = t - z.y - fractal->transformCommon.offsetA0;
		if (fractal->transformCommon.functionEnabledBxFalse
				&& aux.i >= fractal->transformCommon.startIterationsO
				&& aux.i < fractal->transformCommon.stopIterationsO)
					z.x = -fabs(z.x);
		if (fractal->transformCommon.functionEnabledBx
				&& aux.i >= fractal->transformCommon.startIterationsP
				&& aux.i < fractal->transformCommon.stopIterationsP)
					z.y = -fabs(z.y);

		t = z.x;
		z.x = z.x + z.y;
		z.y = t - z.y;
		z.x *= 0.5;
		z.y *= 0.5;
		if (fractal->transformCommon.functionEnabledAx
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD)
					z.x = foldX - fabs(z.x + foldX);
		if (fractal->transformCommon.functionEnabledAxFalse
				&& aux.i >= fractal->transformCommon.startIterationsRV
				&& aux.i < fractal->transformCommon.stopIterationsRV)
					z.y = foldY - fabs(z.y + foldY);
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->foldColor.difs0000.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->foldColor.difs0000.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->foldColor.difs0000.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);
		if (rrCol > fractal->transformCommon.invert0)
			colorAdd +=
				fractal->foldColor.difs0000.w * (rrCol - fractal->transformCommon.invert0) / 100.0;
		aux.color += colorAdd;
	}
}
