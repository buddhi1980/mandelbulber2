/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Modified Mandelbox (ABox) formula
 * This formula contains aux.color and aux.actualScaleA
 */

#include "all_fractal_definitions.h"

cFractalFoldBoxMod1::cFractalFoldBoxMod1() : cAbstractFractal()
{
	nameInComboBox = "Fold Box - Mod 1";
	internalName = "fold_box_mod1";
	internalID = fractal::foldBoxMod1;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalFoldBoxMod1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	if (aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		CVector4 tempA, tempB;
		CVector4 oldZ = z;
		if (fractal->transformCommon.functionEnabledx)
			tempA.x = fabs(z.x + fractal->transformCommon.additionConstant111.x);
		if (fractal->transformCommon.functionEnabledAx)
			tempB.x = fabs(z.x - fractal->transformCommon.additionConstantA111.x);
		z.x = tempA.x - tempB.x - (z.x * fractal->transformCommon.scale3D111.x);

		if (fractal->transformCommon.functionEnabledy)
			tempA.y = fabs(z.y + fractal->transformCommon.additionConstant111.y);
		if (fractal->transformCommon.functionEnabledAy)
			tempB.y = fabs(z.y - fractal->transformCommon.additionConstantA111.y);
		z.y = tempA.y - tempB.y - (z.y * fractal->transformCommon.scale3D111.y);

		if (fractal->transformCommon.functionEnabledz)
			tempA.z = fabs(z.z + fractal->transformCommon.additionConstant111.z);
		if (fractal->transformCommon.functionEnabledAz)
			tempB.z = fabs(z.z - fractal->transformCommon.additionConstantA111.z);
		z.z = tempA.z - tempB.z - (z.z * fractal->transformCommon.scale3D111.z);
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
	}

	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA) // box fold
	{
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
			colorAdd += fractal->mandelbox.color.factor.x;
		}
		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
			colorAdd += fractal->mandelbox.color.factor.y;
		}
		double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
		double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
			colorAdd += fractal->mandelbox.color.factor.z;
		}
	}

	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB) // spherical fold
	{
		double r2 = z.Dot(z);
		// if (r2 < 1e-21 && r2 > -1e-21)
		//	r2 = (r2 > 0) ? 1e-21 : -1e-21;
		if (r2 < fractal->mandelbox.mR2)
		{
			z *= fractal->mandelbox.mboxFactor1;
			aux.DE *= fractal->mandelbox.mboxFactor1;
			colorAdd += fractal->mandelbox.color.factorSp1;
		}
		else if (r2 < fractal->mandelbox.fR2)
		{
			double tglad_factor2 = fractal->mandelbox.fR2 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			colorAdd += fractal->mandelbox.color.factorSp2;
		}
	}

	// scale
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		double useScale = aux.actualScaleA + fractal->mandelbox.scale;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;

		// update actualScale for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
		if (fractal->transformCommon.functionEnabledMFalse)
			aux.actualScaleA = -vary;
		else
			aux.actualScaleA = aux.actualScaleA - vary;
	}
	else
	{
		z *= fractal->mandelbox.scale;
		aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;
	}
	if (fractal->mandelbox.mainRotationEnabled && aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
		z = fractal->mandelbox.mainRot.RotateVector(z);

	// color updated v2.14
	if (fractal->foldColor.auxColorEnabled)
	{
		aux.color += colorAdd;
	}
}
