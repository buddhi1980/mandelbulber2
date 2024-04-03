/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Hybrid Color Trial2
 *
 *
 * bailout may need to be adjusted with some formulas
 */

#include "all_fractal_definitions.h"

cFractalTransfHybridColor2::cFractalTransfHybridColor2() : cAbstractFractal()
{
	nameInComboBox = "T>Hybrid Color2";
	internalName = "transf_hybrid_color2";
	internalID = fractal::transfHybridColor2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfHybridColor2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT)
	{
		double componentMaster = 0.0;
		double orbitPoints = 0.0;
		double distL = 0.0;
		double newR = 0.0;
		double lastVec = 0.0;
		double colTiling = 0.0;

		// summation of r
		if (fractal->transformCommon.functionEnabledMFalse)
		{
			newR = aux.r * fractal->transformCommon.scaleD1;
		}

		// max distance travelled
		if (fractal->transformCommon.functionEnabledSFalse)
		{
			CVector4 oldPt = aux.old_z;
			CVector4 newPt = z;
			CVector4 diffZ = oldPt - newPt;
			distL = diffZ.Length() * fractal->transformCommon.scaleC1;
		}

		// last two z lengths
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			if (aux.i < fractal->transformCommon.stopIterationsM) // remove
			{
				CVector4 oldPt = aux.old_z;
				double lastZ = oldPt.Length(); // aux.old_r;
				double newZ = aux.r;
				if (fractal->transformCommon.functionEnabledBwFalse) lastVec = newZ / lastZ;
				if (fractal->transformCommon.functionEnabledByFalse) lastVec = lastZ / newZ;
				if (fractal->transformCommon.functionEnabledBzFalse) lastVec = fabs(lastZ - newZ);
				lastVec *= fractal->transformCommon.scaleB1;
			}
		}

		// orbitTrap points
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			CVector4 PtOne = z - fractal->transformCommon.offset000;
			double distOne = PtOne.Length(); // * weight
			orbitPoints = distOne;
			if (fractal->transformCommon.functionEnabledAzFalse)
			{
				CVector4 PtTwo = z - fractal->transformCommon.offsetA000;
				double distTwo = PtTwo.Length();
				orbitPoints = min(orbitPoints, distTwo);
			}
			if (fractal->transformCommon.functionEnabledAxFalse)
			{
				CVector4 PtThree = z - fractal->transformCommon.offsetF000;
				double distThree = PtThree.Length();
				orbitPoints = min(orbitPoints, distThree);
			}
			if (fractal->transformCommon.functionEnabledAyFalse)
			{
				CVector4 PtFour = z - fractal->transformCommon.additionConstantA000;
				double distFour = PtFour.Length();
				orbitPoints = min(orbitPoints, distFour);
			}
			orbitPoints *= fractal->transformCommon.scaleA1;
		}


		if (fractal->transformCommon.functionEnabledTFalse)
			if (fractal->transformCommon.functionEnabledTFalse)
			{
				// REAL4 tV = 0.0f;
				//	if (fractal->foldColor.auxColorEnabledA) tV = z;
				//	else tV = aux->const_c;
				//	REAL4 tV = aux->const_c * (1.0f - fractal->foldColor.difs0000.w) + z *
				//fractal->foldColor.difs0000.w; // x * (1-a) +y * a

				double Ax = 2.0 * fractal->transformCommon.scale3D111.x;
				double Ay = 2.0 * fractal->transformCommon.scale3D111.y;

				double bb = ((z.x + Ax) / Ax) + fractal->transformCommon.additionConstantP000.x;
				bb = fabs(bb - round(bb)) * fractal->transformCommon.offsetC111.x;
				double cc = ((z.y + Ay) / Ay) + fractal->transformCommon.additionConstantP000.y;
				cc = fabs(cc - round(cc)) * fractal->transformCommon.offsetC111.y;
				bb = bb + cc;

				double dd = ((aux.const_c.x + Ax) / Ax) + fractal->transformCommon.additionConstantP000.x;
				dd = fabs(dd - round(dd)) * fractal->transformCommon.offsetC111.x;
				double ee = ((aux.const_c.y + Ay) / Ay) + fractal->transformCommon.additionConstantP000.y;
				ee = fabs(ee - round(ee)) * fractal->transformCommon.offsetC111.y;
				dd = dd + ee;

				bb = dd * (1.0 - fractal->foldColor.difs0000.w) + bb * fractal->foldColor.difs0000.w; // mix


				colTiling = fractal->foldColor.difs1 * bb;
			}








		// build  componentMaster
		componentMaster = (distL + orbitPoints + lastVec + newR + colTiling);
		componentMaster *= fractal->transformCommon.scale;

		if (!fractal->transformCommon.functionEnabledFalse)
		{
			aux.colorHybrid += componentMaster;
		}
		else
		{
			aux.temp1000 = min(aux.temp1000, componentMaster);
			aux.colorHybrid = aux.temp1000;
		}
	}
	aux.old_z = z;
}
