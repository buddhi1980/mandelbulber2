/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MsltoeJuliaBulb Eiffie. Refer post by Eiffie    Reply #69 on: January 27, 2015
 * @reference http://www.fractalforums.com/theory/choosing-the-squaring-formula-by-location/60/
 */

#include "all_fractal_definitions.h"

cFractalMsltoeSym3Mod6::cFractalMsltoeSym3Mod6() : cAbstractFractal()
{
	nameInComboBox = "Msltoe - Julia Bulb Mod6";
	internalName = "msltoe_sym3_mod6";
	internalID = fractal::msltoeSym3Mod6;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMsltoeSym3Mod6::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	if (fractal->transformCommon.functionEnabled
			&& aux.i >= fractal->transformCommon.startIterationsJ
			&& aux.i < fractal->transformCommon.stopIterationsJ)
	{
		aux.DE = aux.DE * 2.0 * aux.r;

		double psi = fractal->transformCommon.int8Y;
		if (fractal->transformCommon.functionEnabledBFalse
				&& aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			psi += fractal->transformCommon.int1;
		}
		psi = M_PI / psi;
		psi = fabs(fmod(atan2(z.x, z.y) + M_PI + psi, 2.0 * psi) - psi);
		double len = sqrt(z.y * z.y + z.x * z.x);
		z.y = cos(psi) * len;
		z.x = sin(psi) * len;

		CVector4 z2 = z * z;
		double rr = z2.z + z2.y + z2.x;
		double m = 1.0 - z2.x / rr;
		CVector4 temp;
		temp.z = (z2.z - z2.y) * m * fractal->transformCommon.scaleB1;
		temp.y = 2.0 * z.z * z.y * m * fractal->transformCommon.scale; // scaling y;
		temp.x = 2.0 * z.x * sqrt(z2.z + z2.y);
		temp.w = z.w;
		z = temp - fractal->transformCommon.offset001;

		if (fractal->transformCommon.addCpixelEnabledFalse
				&& aux.i >= fractal->transformCommon.startIterationsC
				&& aux.i < fractal->transformCommon.stopIterationsC)
		{
			CVector4 tempFAB = c;
			if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
			if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
			if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

			tempFAB *= fractal->transformCommon.constantMultiplier000;
			z.x += sign(z.x) * tempFAB.x;
			z.y += sign(z.y) * tempFAB.y;
			z.z += sign(z.z) * tempFAB.z;
		}
		double lengthTempZ = -z.Length();
		// if (lengthTempZ > -1e-21) lengthTempZ = -1e-21;   //  z is neg.)
		z *= 1.0 + fractal->transformCommon.offset / lengthTempZ;
		z *= fractal->transformCommon.scale1;
		aux.DE *= fabs(fractal->transformCommon.scale1);
	}

	if (fractal->transformCommon.functionEnabledEFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		CVector4 zz = z * z;
		double leng = sqrt(zz.x + zz.y + zz.z); // needed when alternating pwr2s
		aux.DE = leng * aux.DE * 2.0 + 1.0;

		double t = 1.0;
		double temp = zz.y + zz.x;
		if (temp > 0.0) t = 2.0 * z.z / sqrt(temp);
		temp = z.x;
		z.z = (zz.z - zz.y - zz.x);
		z.y = (2.0 * t * z.y * temp);
		z.x = (t * (zz.y - zz.x));
		// c.yz swap
		z.z -= aux.const_c.z * fractal->transformCommon.scaleE1;
		//z.x -= c.y;
		//z.y -= c.x;
	}

	if (fractal->transformCommon.functionEnabledFalse // quaternion fold
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.DE * 2.0 * aux.r;
		z = CVector4(z.x * z.z, z.y * z.z, z.z * z.z - z.y * z.y - z.x * z.x, 0.0);
		if (!fractal->transformCommon.functionEnabledTFalse)
		{
			z *= CVector4(2.0, 2.0, 1.0, 1.0);
			//z.z -= aux.const_c.z * fractal->transformCommon.scaleF1;
			//z.z -= fractal->transformCommon.offset0;
		}
		else
		{
			double tempL = z.Length();
			z *= CVector4(2.0, 2.0, 1.0, 1.0);
			double avgScale = z.Length() / tempL;
			aux.DE *= avgScale;
		}
		z.z -= aux.const_c.z * fractal->transformCommon.scaleF1;
		z.z -= fractal->transformCommon.offset0;
	}
	if (fractal->transformCommon.angle0 != 0
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double tempY = z.y;
		double beta = fractal->transformCommon.angle0 * M_PI_180;
		z.y = z.y * cos(beta) + z.x * sin(beta);
		z.x = tempY * -sin(beta) + z.x * cos(beta);
	}
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset0;

	// aux->color // colDist != aux.dist ||
	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colAdd = fractal->foldColor.difs0000.w
				+ aux.i * fractal->foldColor.difs0;

		// last two z lengths
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			double lastVec = 0.0;
			CVector4 oldPt = aux.old_z;
			double lastZ = oldPt.Length(); // aux.old_r;
			double newZ = z.Length();
			if (fractal->transformCommon.functionEnabledBwFalse) lastVec = newZ / lastZ;
			if (fractal->transformCommon.functionEnabledByFalse) lastVec = lastZ / newZ;
			if (fractal->transformCommon.functionEnabledBzFalse) lastVec = fabs(lastZ - newZ);
			lastVec *= fractal->foldColor.difs1;
			colAdd += lastVec;

			aux.old_z = z; // update for next iter
		}

		colAdd += fractal->foldColor.difs0000.z * fabs(z.x * z.y);

		if (!fractal->foldColor.auxColorEnabledBFalse) aux.color = colAdd;
		else aux.color += colAdd;
	}
}
