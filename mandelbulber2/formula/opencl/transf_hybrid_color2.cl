/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2024 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Hybrid Color Trial2
 *
 *
 * bailout may need to be adjusted with some formulas

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_transf_hybrid_color2.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 TransfHybridColor2Iteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	if (aux->i >= fractal->transformCommon.startIterationsT
			&& aux->i < fractal->transformCommon.stopIterationsT)
	{
		REAL componentMaster = 0.0f;
		REAL orbitPoints = 0.0f;
		REAL distL = 0.0f;
		REAL newR = 0.0f;
		REAL lastVec = 0.0f;
		REAL colTiling = 0.0f;

		// summation of r
		if (fractal->transformCommon.functionEnabledMFalse)
		{
			newR = aux->r * fractal->transformCommon.scaleD1;
		}

		// max distance travelled
		if (fractal->transformCommon.functionEnabledSFalse)
		{
			REAL4 oldPt = aux->old_z;
			REAL4 newPt = z;
			REAL4 diffZ = oldPt - newPt;
			distL = length(diffZ) * fractal->transformCommon.scaleC1;
		}

		// last two z lengths
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			if (aux->i < fractal->transformCommon.stopIterationsM) // remove
			{
				REAL4 oldPt = aux->old_z;
				REAL lastZ = length(oldPt); // aux->old_r;
				REAL newZ = aux->r;
				if (fractal->transformCommon.functionEnabledBwFalse) lastVec = newZ / lastZ;
				if (fractal->transformCommon.functionEnabledByFalse) lastVec = lastZ / newZ;
				if (fractal->transformCommon.functionEnabledBzFalse) lastVec = fabs(lastZ - newZ);
				lastVec *= fractal->transformCommon.scaleB1;
			}
		}

		// orbitTrap points
		if (fractal->transformCommon.functionEnabledBxFalse)
		{
			REAL4 PtOne = z - fractal->transformCommon.offset000;
			REAL distOne = length(PtOne); // * weight
			orbitPoints = distOne;
			if (fractal->transformCommon.functionEnabledAzFalse)
			{
				REAL4 PtTwo = z - fractal->transformCommon.offsetA000;
				REAL distTwo = length(PtTwo);
				orbitPoints = min(orbitPoints, distTwo);
			}
			if (fractal->transformCommon.functionEnabledAxFalse)
			{
				REAL4 PtThree = z - fractal->transformCommon.offsetF000;
				REAL distThree = length(PtThree);
				orbitPoints = min(orbitPoints, distThree);
			}
			if (fractal->transformCommon.functionEnabledAyFalse)
			{
				REAL4 PtFour = z - fractal->transformCommon.additionConstantA000;
				REAL distFour = length(PtFour);
				orbitPoints = min(orbitPoints, distFour);
			}
			orbitPoints *= fractal->transformCommon.scaleA1;
		}

		if (fractal->transformCommon.functionEnabledTFalse)
		{
			// REAL4 tV = 0.0f;
			//	if (fractal->foldColor.auxColorEnabledA) tV = z;
			//	else tV = aux->const_c;
			//	REAL4 tV = aux->const_c * (1.0f - fractal->foldColor.difs0000.w) + z *
			// fractal->foldColor.difs0000.w; // x * (1-a) +y * a

			REAL Size = 2.0f * fractal->transformCommon.scale3D111.x;
			REAL bb = ((z.x + Size) / Size) + fractal->transformCommon.additionConstantP000.x;
			bb = fabs(bb - round(bb)) * fractal->transformCommon.constantMultiplierC111.x;
			REAL dd = ((aux->const_c.x + Size) / Size) + fractal->transformCommon.additionConstantP000.x;
			dd = fabs(dd - round(dd)) * fractal->transformCommon.constantMultiplierC111.x;

			Size = 2.0f * fractal->transformCommon.scale3D111.y;
			REAL cc = ((z.y + Size) / Size) + fractal->transformCommon.additionConstantP000.y;
			cc = fabs(cc - round(cc)) * fractal->transformCommon.constantMultiplierC111.y;
			REAL ee = ((aux->const_c.y + Size) / Size) + fractal->transformCommon.additionConstantP000.y;
			ee = fabs(ee - round(ee)) * fractal->transformCommon.constantMultiplierC111.y;

			bb = bb + cc;
			dd = dd + ee;

			if (fractal->transformCommon.functionEnabledAFalse)
			{
				Size = 2.0f * fractal->transformCommon.scale3D111.z;
				REAL aa = ((z.z + Size) / Size) + fractal->transformCommon.additionConstantP000.z;
				aa = fabs(aa - round(aa)) * fractal->transformCommon.constantMultiplierC111.z;
				bb = bb + aa;
				REAL ff =
					((aux->const_c.z + Size) / Size) + fractal->transformCommon.additionConstantP000.z;
				ff = fabs(ff - round(ff)) * fractal->transformCommon.constantMultiplierC111.z;
				dd = dd + ff;
			}
			bb = dd * (1.0f - fractal->foldColor.difs0000.w) + bb * fractal->foldColor.difs0000.w; // mix

			colTiling = fractal->foldColor.difs1 * bb;
		}

		// build  componentMaster
		componentMaster = (distL + orbitPoints + lastVec + newR + colTiling);
		componentMaster *= fractal->transformCommon.scale;

		if (!fractal->transformCommon.functionEnabledFalse)
		{
			aux->colorHybrid += componentMaster;
		}
		else
		{
			aux->temp1000 = min(aux->temp1000, componentMaster);
			aux->colorHybrid = aux->temp1000;
		}
	}
	aux->old_z = z;
	return z;
}