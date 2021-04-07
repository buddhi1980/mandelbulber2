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
 * rec fold from darkbeam
 */

#include "all_fractal_definitions.h"

cFractalPseudoKleinianMod5::cFractalPseudoKleinianMod5() : cAbstractFractal()
{
	nameInComboBox = "Pseudo Kleinian - Mod 5";
	internalName = "pseudo_kleinian_mod5";
	internalID = fractal::pseudoKleinianMod5;
	DEType = analyticDEType;
	DEFunctionType = pseudoKleinianDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionPseudoKleinian;
	coloringFunction = coloringFunctionDefault;
}

void cFractalPseudoKleinianMod5::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	//double colorAdd = 0.0;

	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		z += fractal->transformCommon.offset000;
		double rr = z.Dot(z);
		z *= fractal->transformCommon.scaleG1 / rr;
		aux.DE *= (fractal->transformCommon.scaleG1 / rr);
		z += fractal->transformCommon.additionConstantP000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleA1;
		aux.DE *= fractal->transformCommon.scaleA1;
	}

	// box offset
	if (aux.i >= fractal->transformCommon.startIterationsM
			&& aux.i < fractal->transformCommon.stopIterationsM)
	{
		z.x -= fractal->transformCommon.constantMultiplier000.x * sign(z.x);
		z.y -= fractal->transformCommon.constantMultiplier000.y * sign(z.y);
		z.z -= fractal->transformCommon.constantMultiplier000.z * sign(z.z);
	}

	// Pseudo kleinian
	if (fractal->transformCommon.functionEnabledCx)
		z.x = z.x - fractal->transformCommon.additionConstant222.x
			* round(z.x / fractal->transformCommon.additionConstant222.x);
	if (fractal->transformCommon.functionEnabledCy)
		z.y = z.y - fractal->transformCommon.additionConstant222.y
			* round(z.y / fractal->transformCommon.additionConstant222.y);
	if (fractal->transformCommon.functionEnabledCz)
		z.z = z.z - fractal->transformCommon.additionConstant222.z
			* round(z.z / fractal->transformCommon.additionConstant222.z);

	if (fractal->transformCommon.functionEnabledCxFalse)
		z.x = fabs(z.x + fractal->transformCommon.offset111.x)
			- fabs(z.x - fractal->transformCommon.offset111.x) - z.x;
	if (fractal->transformCommon.functionEnabledCyFalse)
		z.y = fabs(z.y + fractal->transformCommon.offset111.y)
			- fabs(z.y - fractal->transformCommon.offset111.y) - z.y;
	if (fractal->transformCommon.functionEnabledCzFalse)
		z.z = fabs(z.z + fractal->transformCommon.offset111.z)
			- fabs(z.z - fractal->transformCommon.offset111.z) - z.z;

	CVector4 lpN = fabs(z);
	double pr = fractal->transformCommon.scale2;
	lpN.x = pow(lpN.x, pr);
	lpN.y = pow(lpN.y, pr);
	lpN.z = pow(lpN.z, pr);
	double pNorm = pow((lpN.x + lpN.y + lpN.z), 1.0 / pr);

	pNorm = pow(pNorm, fractal->transformCommon.scaleA2);
	pNorm = max(pNorm, fractal->transformCommon.offset02);
	double r2 = fractal->transformCommon.scale1p1 / pNorm;
	z *= r2;
	aux.DE = aux.DE * r2;

	z += fractal->transformCommon.additionConstant000;

	if (fractal->transformCommon.functionEnabledGFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		z.x += aux.pos_neg * fractal->transformCommon.additionConstantA000.x;
		z.y += aux.pos_neg * fractal->transformCommon.additionConstantA000.y;
		z.z += aux.pos_neg * fractal->transformCommon.additionConstantA000.z;

		aux.pos_neg *= fractal->transformCommon.scaleNeg1;
	}


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
				&& aux.i >= fractal->transformCommon.startIterationsR
				&& aux.i < fractal->transformCommon.stopIterationsR)
					z.x = foldX - fabs(z.x + foldX);
		if (fractal->transformCommon.functionEnabledAxFalse
				&& aux.i >= fractal->transformCommon.startIterationsRV
				&& aux.i < fractal->transformCommon.stopIterationsRV)
					z.y = foldY - fabs(z.y + foldY);
	}


	double len = z.Length();

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		double k = max(fractal->transformCommon.minR05 / z.Dot(z), 1.0);
		z *= k;
		aux.DE *= k;
	}

	aux.DE *= 1.0 + fractal->analyticDE.tweak005;

	if (fractal->transformCommon.functionEnabledDFalse)
	{
		len = min(len, fractal->transformCommon.foldingValue - len);
	}

	// DE options

	if (fractal->transformCommon.functionEnabledBFalse)
	{
		len -= fractal->transformCommon.offsetD0;
		if (!fractal->transformCommon.functionEnabledXFalse)
		{
			aux.DE0 = len / aux.DE;
		}
		else
		{
			aux.DE0 = min(len, fractal->analyticDE.offset1)
							/ max(aux.DE, fractal->analyticDE.offset1);
		}
		aux.dist = aux.DE0;
	}

	if (fractal->transformCommon.functionEnabledJFalse)
	{
		double rxy = sqrt(z.x * z.x + z.y * z.y);
		aux.DE0 = max(rxy - fractal->analyticDE.scale1, fabs(rxy * z.z) / len) / aux.DE;
		aux.dist = aux.DE0;
	}

	aux.pseudoKleinianDE = fractal->analyticDE.scale1; // for pK DE

/*	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		colorAdd += fractal->foldColor.difs0000.x * fabs(z.x);
		colorAdd += fractal->foldColor.difs0000.y * fabs(z.y);
		colorAdd += fractal->foldColor.difs0000.z * fabs(z.z);
		colorAdd += fractal->foldColor.difs0000.w * k;

		aux.color += colorAdd;
	}*/
}
