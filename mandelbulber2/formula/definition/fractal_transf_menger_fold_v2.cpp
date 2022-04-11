/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Transform Menger Fold
 * Menger Sponge formula created by Knighty
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/kaleidoscopic-(escape-time-ifs)/
 * analytic aux.DE
 */

#include "all_fractal_definitions.h"

cFractalTransfMengerFoldV2::cFractalTransfMengerFoldV2() : cAbstractFractal()
{
	nameInComboBox = "T>Menger Fold V2";
	internalName = "transf_menger_fold_v2";
	internalID = fractal::transfMengerFoldV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfMengerFoldV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// menger sponge
if (aux.i >= fractal->transformCommon.startIterationsG
		&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		z = fabs(z);
		z += fractal->transformCommon.offset000;

		if (fractal->transformCommon.rotationEnabledFalse
				&& aux.i >= fractal->transformCommon.startIterationsR
				&& aux.i < fractal->transformCommon.stopIterationsR)
		{
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}

		double t;
		if (!fractal->transformCommon.functionEnabledGFalse)
		{
			t = z.x - z.y;
			t = fractal->transformCommon.additionConstant0555.x
					* (t - fabs(t) * fractal->transformCommon.constantMultiplier111.x);
			z.x = z.x - t;
			z.y = z.y + t;
		}
		else
		{
			t = z.x;
			z.x = z.y;
			z.y = t;
		}

		t = z.x - z.z;
		t = fractal->transformCommon.additionConstant0555.y
				* (t - fabs(t) * fractal->transformCommon.constantMultiplier111.y);
		z.x = z.x - t;
		z.z = z.z + t;

		t = z.y - z.z;
		t = fractal->transformCommon.additionConstant0555.z
				* (t - fabs(t) * fractal->transformCommon.constantMultiplier111.z);
		z.y = z.y - t;
		z.z = z.z + t;

		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

		double useScale = fractal->transformCommon.scale3;
		if (fractal->transformCommon.functionEnabledXFalse
				&& aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			useScale += aux.actualScaleA;
			// update actualScale for next iteration
			double vary = fractal->transformCommon.scaleVary0
					* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
			aux.actualScaleA = -vary;
		}
		aux.DE = aux.DE * useScale + fractal->analyticDE.offset0;

		double sc1 = useScale - 1.0;
		double sc2 = sc1 / useScale;
		z.z = z.z - fractal->transformCommon.offset1105.z * sc2;
		z.z = -fabs(z.z) + fractal->transformCommon.offset1105.z * sc2;
		z.x = useScale * z.x - fractal->transformCommon.offset1105.x * sc1;
		z.y = useScale * z.y - fractal->transformCommon.offset1105.y * sc1;
		z.z = useScale * z.z;
	}
}
