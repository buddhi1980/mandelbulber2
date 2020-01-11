/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Hybrid Color Trial
 * bailout may need to be adjusted with some formulas
 */

#include "all_fractal_definitions.h"

cFractalTransfHybridColor::cFractalTransfHybridColor() : cAbstractFractal()
{
	nameInComboBox = "T>Hybrid Color";
	internalName = "transf_hybrid_color";
	internalID = fractal::transfHybridColor;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfHybridColor::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double linearOffset = 0.0;
	double boxTrap = 0.0;
	double sphereTrap = 0.0;
	double planeBias = 0.0;
	double twoPoints = 0.0;
	double componentMaster = 0.0;

	// max linear offset
	if (fractal->transformCommon.functionEnabledMFalse)
	{
		double temp30 = 0.0;
		CVector4 temp31 = z;
		if (fractal->transformCommon.functionEnabledM) temp31 = fabs(temp31);

		temp30 = max(max(temp31.x, temp31.y), temp31.z);
		temp30 *= fractal->transformCommon.scaleD1;
		linearOffset = temp30;
	}

	// box trap
	if (fractal->surfBox.enabledX2False)
	{
		CVector4 box = fractal->transformCommon.scale3D444;
		CVector4 temp35 = z;
		double temp39 = 0.0;
		if (fractal->transformCommon.functionEnabledCx) temp35 = fabs(temp35);

		temp35 = box - temp35;
		double temp36 = max(max(temp35.x, temp35.y), temp35.z);
		double temp37 = min(min(temp35.x, temp35.y), temp35.z);
		temp36 = temp36 + temp37 * fractal->transformCommon.offsetB0;
		temp36 *= fractal->transformCommon.scaleC1;

		if (fractal->surfBox.enabledY2False)
		{
			CVector4 temp38 = aux.c;

			if (fractal->transformCommon.functionEnabledCz) temp38 = fabs(temp38);
			temp38 = box - temp38;

			temp39 = max(max(temp38.x, temp38.y), temp38.z);
			double temp40 = min(min(temp38.x, temp38.y), temp38.z);
			temp39 = temp39 + temp40 * fractal->transformCommon.offsetA0;
			temp39 *= fractal->transformCommon.scaleE1;
		}
		boxTrap = temp36 + temp39;
	}

	// sphere trap
	if (fractal->transformCommon.functionEnabledzFalse)
	{
		double sphereRR = fractal->transformCommon.maxR2d1;
		double temp45 = z.Dot(z);
		double temp46 = sphereRR - temp45;
		double temp47 = temp46;
		double temp51 = temp46;
		if (fractal->transformCommon.functionEnabledAx) temp51 = fabs(temp51);
		temp51 *= fractal->transformCommon.scaleF1;

		if (fractal->transformCommon.functionEnabledyFalse && temp45 > sphereRR)
		{
			temp46 *= temp46 * fractal->transformCommon.scaleG1;
		}
		if (fractal->transformCommon.functionEnabledPFalse && temp45 < sphereRR)
		{
			temp47 *= temp47 * fractal->transformCommon.scaleB1;
		}
		sphereTrap = temp51 + temp47 + temp46;
	}

	// plane bias
	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		CVector4 tempP = z;
		if (fractal->transformCommon.functionEnabledEFalse)
		{
			tempP.x = tempP.x * tempP.y;
			tempP.x *= tempP.x;
		}
		else
		{
			tempP.x = fabs(tempP.x * tempP.y);
		}
		if (fractal->transformCommon.functionEnabledFFalse)
		{
			tempP.y = tempP.y * tempP.z;
			tempP.y *= tempP.y;
		}
		else
		{
			tempP.y = fabs(tempP.y * tempP.z);
		}

		if (fractal->transformCommon.functionEnabledKFalse)
		{
			tempP.z = tempP.z * tempP.x;
			tempP.z *= tempP.z;
		}
		else
		{
			tempP.z = fabs(tempP.z * tempP.x);
		}

		tempP = tempP * fractal->transformCommon.scale3D000;
		planeBias = tempP.x + tempP.y + tempP.z;
	}

	// two points
	if (fractal->transformCommon.functionEnabledBxFalse)
	{
		CVector4 PtOne = z - fractal->transformCommon.offset000;
		CVector4 PtTwo = z - fractal->transformCommon.offsetA000;
		double distOne = PtOne.Length(); // * weight
		double distTwo = PtTwo.Length();
		twoPoints = min(distOne, distTwo);
		if (fractal->transformCommon.functionEnabledAxFalse)
		{
			CVector4 PtThree = z - fractal->transformCommon.offsetF000;
			double distThree = PtThree.Length();
			twoPoints = min(twoPoints, distThree);
		}
		twoPoints *= fractal->transformCommon.scaleA1;
	}

	// build  componentMaster
	componentMaster = (linearOffset + boxTrap + sphereTrap + planeBias + twoPoints);

	aux.colorHybrid = componentMaster;
}
