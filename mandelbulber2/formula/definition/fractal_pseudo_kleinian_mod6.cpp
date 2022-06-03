/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Pseudo Kleinian, Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */

#include "all_fractal_definitions.h"

cFractalPseudoKleinianMod6::cFractalPseudoKleinianMod6() : cAbstractFractal()
{
	nameInComboBox = "Pseudo Kleinian - Mod 6";
	internalName = "pseudo_kleinian_mod6";
	internalID = fractal::pseudoKleinianMod6;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalPseudoKleinianMod6::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double k = 0.0;
	double Dk = 1.0;

	for (int h = 0; h < fractal->transformCommon.int32; h++)
	{
		// sphere inversion
		if (fractal->transformCommon.sphereInversionEnabledFalse
				&& h >= fractal->transformCommon.startIterationsX
				&& h < fractal->transformCommon.stopIterations1)
		{
			z += fractal->transformCommon.offset000;
			double rr = z.Dot(z);
			z *= fractal->transformCommon.scaleG1 / rr;
			Dk *= (fractal->transformCommon.scaleG1 / rr);
			z += fractal->transformCommon.additionConstantP000 - fractal->transformCommon.offset000;
			z *= fractal->transformCommon.scaleA1;
			Dk *= fractal->transformCommon.scaleA1;
		}

		// box offset
		if (h >= fractal->transformCommon.startIterationsM
				&& h < fractal->transformCommon.stopIterationsM)
		{
			z.x -= fractal->transformCommon.constantMultiplier000.x * sign(z.x);
			z.y -= fractal->transformCommon.constantMultiplier000.y * sign(z.y);
			z.z -= fractal->transformCommon.constantMultiplier000.z * sign(z.z);
		}

		// Pseudo kleinian
		if (h >= fractal->transformCommon.startIterationsC
				&& h < fractal->transformCommon.stopIterationsC)
		{
			z = fabs(z + fractal->transformCommon.additionConstant0777)
					- fabs(z - fractal->transformCommon.additionConstant0777) - z;
			k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
			z *= k;
			Dk *= k;
		}

		z += fractal->transformCommon.additionConstant000;

		if (fractal->transformCommon.functionEnabledGFalse
					&& h >= fractal->transformCommon.startIterationsG
					&& h < fractal->transformCommon.stopIterationsG)
		{
			z.x += aux.pos_neg * fractal->transformCommon.additionConstantA000.x;
			z.y += aux.pos_neg * fractal->transformCommon.additionConstantA000.y;
			z.z += aux.pos_neg * fractal->transformCommon.additionConstantA000.z;

			aux.pos_neg *= fractal->transformCommon.scaleNeg1;
		}

		if (fractal->transformCommon.addCpixelEnabledFalse // symmetrical addCpixel
				&& h >= fractal->transformCommon.startIterationsT
				&& h < fractal->transformCommon.stopIterationsT1)
		{
			CVector4 tempFAB = aux.const_c;
			if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
			if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
			if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

			tempFAB *= fractal->transformCommon.scale3D000;
			z.x -= sign(z.x) * tempFAB.x;
			z.y -= sign(z.y) * tempFAB.y;
			z.z -= sign(z.z) * tempFAB.z;
		}

		if (fractal->transformCommon.rotationEnabledFalse
				&& h >= fractal->transformCommon.startIterationsR
				&& h < fractal->transformCommon.stopIterationsR1)
		{
			z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		}

		if (fractal->transformCommon.functionEnabledxFalse) z.x = -z.x;
		if (fractal->transformCommon.functionEnabledyFalse) z.y = -z.y;
	}

	CVector4 q = z;

	double temp = q.x * q.x + q.y * q.y;
	double rxy = sqrt(temp);

	if (fractal->transformCommon.functionEnabledAx)
	{
		double d1 = temp + q.z * q.z;
		if (!fractal->transformCommon.functionEnabledAyFalse) d1 = sqrt(d1);

		 d1 = (fabs(rxy * q.z) - fractal->transformCommon.offsetD0)
						/ (d1 + fractal->transformCommon.offset02);
		aux.DE0 = d1;
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		double d3 = fabs(fractal->transformCommon.scale05 * fabs(q.z - fractal->transformCommon.offsetA0));

		if (!fractal->transformCommon.functionEnabledAwFalse) aux.DE0 = d3;
		else aux.DE0 = min(aux.DE0, d3);
	}

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		q.x = q.x * q.x * fractal->transformCommon.scaleC1;
		q.y	= q.y * q.y * fractal->transformCommon.scaleD1;
		if (fractal->transformCommon.functionEnabledNFalse)
		{
			q.x *= q.x;
			q.y *= q.y;
		}
		rxy = sqrt(q.x + q.y);
		aux.DE0 = max(rxy - fractal->transformCommon.offsetA1, aux.DE0);
	}
	aux.DE0 = aux.DE0 / Dk;
	k = aux.DE0;

		// KaliBoxMod
	if (fractal->transformCommon.functionEnabledFFalse)
	{
		CVector4 p = z;
		double m;
		temp = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
		double r2 = 0.0;
		double Dd = 1.0;
		for (int n = 0; n < fractal->transformCommon.int16 && r2 < 100.0f; n++)
		{
			m = 1.0;
			p = fractal->transformCommon.additionConstant0555 - fabs(p);
			r2 = p.Dot(p);
			if (r2 < fractal->transformCommon.minR2p25) m *= temp;
			else if (r2 < fractal->transformCommon.maxR2d1) m *= fractal->transformCommon.maxR2d1 / r2;

			m *= fractal->transformCommon.scale015;
			p = p * m + fractal->transformCommon.offsetF000;
			Dd = Dd * m + fractal->analyticDE.offset1;
		}
		double r = sqrt(r2);
		if (!fractal->transformCommon.functionEnabledTFalse) Dd = r / fabs(Dd);
		else Dd = 0.5 * r * log(r) / fabs(Dd);

		if (!fractal->transformCommon.functionEnabledIFalse) aux.DE0 =
				min(fractal->transformCommon.scale025 * Dd, fractal->transformCommon.scaleB1 * aux.DE0);
		else aux.DE0 = max(fractal->transformCommon.offset0005 * Dd, aux.DE0);
	}

	if (fractal->transformCommon.functionEnabledDFalse)
		aux.DE0 = min(aux.dist, aux.DE0);
	if (fractal->analyticDE.enabledFalse) aux.DE = Dk;
	aux.dist = aux.DE0;

	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		double colorAdd = 0.0;
		if (aux.DE0 != k) colorAdd = fractal->foldColor.difs0000.w;
		else
		{
			colorAdd += fractal->foldColor.difs0000.x * rxy;
			colorAdd += fractal->foldColor.difs0000.y * k;
			colorAdd += fractal->foldColor.difs0000.z * fabs(q.z);
		}

		aux.color += colorAdd;
	}
}
