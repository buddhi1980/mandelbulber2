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
	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
				- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
				- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	if (fractal->transformCommon.functionEnabled)
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
				- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
	CVector4 zCol = z;


	if (aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
			z += fractal->transformCommon.offsetA000;



	double rrCol = 0.0;
	// spherical fold
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{

		double rr = z.Dot(z);
		rrCol = rr;
		double MinRR = fractal->transformCommon.minR2p25;
		double dividend = rr < MinRR ? MinRR : min(rr, 1.0);
		dividend = 1.0 / dividend;
		z *= dividend;
		aux.DE *= dividend;
	}

	// scale
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

	if (fractal->transformCommon.rotation2EnabledFalse)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->transformCommon.addCpixelEnabledFalse)
		z += CVector4(c.y, c.x, c.z, c.w) * fractal->transformCommon.constantMultiplier111;



	z += fractal->transformCommon.additionConstantA000;



	if (aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
				z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (aux.i >= fractal->transformCommon.startIterations
				&& aux.i < fractal->transformCommon.stopIterations)
		{

			//z += fractal->transformCommon.additionConstant000;

			if (fractal->transformCommon.functionEnabledx) z.x = fabs(z.x);
			if (fractal->transformCommon.functionEnabledy) z.y = fabs(z.y);
			if (fractal->transformCommon.functionEnabledz) z.z = fabs(z.z);
		}

		if (aux.i >= fractal->transformCommon.startIterationsT
				&& aux.i < fractal->transformCommon.stopIterationsT)
		{
			//z += fractal->transformCommon.offsetA000;
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
		if (rrCol > fractal->transformCommon.minR2p25)
			colorAdd +=
				fractal->foldColor.difs0000.w * (rrCol - fractal->transformCommon.minR2p25) / 100.0;
		aux.color += colorAdd;
	}
}
