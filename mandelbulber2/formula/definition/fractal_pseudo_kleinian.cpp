/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Pseudo Kleinian Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */

#include "all_fractal_definitions.h"

cFractalPseudoKleinian::cFractalPseudoKleinian() : cAbstractFractal()
{
	nameInComboBox = "Pseudo Kleinian";
	internalName = "pseudo_kleinian";
	internalID = fractal::pseudoKleinian;
	DEType = analyticDEType;
	DEFunctionType = pseudoKleinianDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionPseudoKleinian;
	coloringFunction = coloringFunctionDefault;
}

void cFractalPseudoKleinian::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// sphere inversion slot#1 iter == 0 added v2.17
	if (fractal->transformCommon.sphereInversionEnabledFalse)
	{
		if (aux.i < 1)
		{
			double rr = 1.0;
			z += fractal->transformCommon.offset000;
			rr = z.Dot(z);
			z *= fractal->transformCommon.maxR2d1 / rr;
			z += fractal->transformCommon.additionConstantA000 - fractal->transformCommon.offset000;
			// double r = sqrt(rr);
			aux.DE = aux.DE * (fractal->transformCommon.maxR2d1 / rr) + fractal->analyticDE.offset0;
		}
	}

	CVector4 gap = fractal->transformCommon.constantMultiplier000;
	double t;
	double dot1;
	// prism shape
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		z.y = fabs(z.y);
		z.z = fabs(z.z);
		dot1 = (z.x * -SQRT_3_4 + z.y * 0.5) * fractal->transformCommon.scale;
		t = max(0.0, dot1);
		z.x -= t * -SQRT_3;
		z.y = fabs(z.y - t);

		if (z.y > z.z) swap(z.y, z.z);
		z -= gap * CVector4(SQRT_3_4, 1.5, 1.5, 0.0);
		// z was pos, now some points neg (ie neg shift)
		if (z.z > z.x) swap(z.z, z.x);
		if (z.x > 0.0)
		{
			z.y = max(0.0, z.y);
			z.z = max(0.0, z.z);
		}
	}

	// box fold abs() tglad fold added v2.17
	if (fractal->transformCommon.functionEnabledByFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabledBy)
		{
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		}
	}

	// box fold
	if (fractal->transformCommon.functionEnabledBxFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		double zLimit = fractal->mandelbox.foldingLimit * fractal->transformCommon.scale1;
		double zValue = fractal->mandelbox.foldingValue * fractal->transformCommon.scale1;
		if (fabs(z.z) > zLimit)
		{
			z.z = sign(z.z) * zValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}
	// PseudoKleinian
	CVector4 cSize = fractal->transformCommon.additionConstant0777;
	CVector4 tempZ = z; //  correct c++ version.
	if (z.x > cSize.x) tempZ.x = cSize.x;
	if (z.x < -cSize.x) tempZ.x = -cSize.x;
	if (z.y > cSize.y) tempZ.y = cSize.y;
	if (z.y < -cSize.y) tempZ.y = -cSize.y;
	if (z.z > cSize.z) tempZ.z = cSize.z;
	if (z.z < -cSize.z) tempZ.z = -cSize.z;

	z = tempZ * 2.0 - z;
	double k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
	z *= k;
	aux.DE *= k + fractal->analyticDE.tweak005;
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	// offset
	z += fractal->transformCommon.additionConstant000;

	aux.pseudoKleinianDE = fractal->analyticDE.scale1;
}
