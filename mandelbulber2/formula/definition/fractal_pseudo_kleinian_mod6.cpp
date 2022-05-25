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
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	double k = 0.0;
	double Dk =1.0;

	for (int h = 0; h < fractal->transformCommon.int16; h++)
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
/*		if (fractal->transformCommon.functionEnabledFFalse
				&& h >= fractal->transformCommon.startIterationsF
				&& h < fractal->transformCommon.stopIterationsF)
		{
			z = fabs(z + fractal->transformCommon.offsetA000)
					- fabs(z - fractal->transformCommon.offsetA000) - z;
		}*/
		if (fractal->transformCommon.addCpixelEnabledFalse
				&& h >= fractal->transformCommon.startIterationsF
				&& h < fractal->transformCommon.stopIterationsF) // symmetrical addCpixel
		{
			CVector4 tempFAB = c;
			if (fractal->transformCommon.functionEnabledx) tempFAB.x = fabs(tempFAB.x);
			if (fractal->transformCommon.functionEnabledy) tempFAB.y = fabs(tempFAB.y);
			if (fractal->transformCommon.functionEnabledz) tempFAB.z = fabs(tempFAB.z);

			tempFAB *= fractal->transformCommon.offsetF000;
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

	// thingy2 is iterated then z goes into thingy and into boxmod	(which is iterated), then the results from
	// thingy and boxmod are used for the return function
		// thingy
		CVector4 q = z;
		q -= fractal->transformCommon.offsetA000;
		aux.pseudoKleinianDE = Dk;

	//q.y = min(q.y, 1.4f - q.y);
		double temp = q.x * q.x + q.y * q.y;
		double rxy = sqrt(temp);
		if (!fractal->transformCommon.functionEnabledAyFalse) temp = q.Length();
		else temp  = q.Dot(q);

		if (fractal->transformCommon.functionEnabledAx)
		{
			double d1 = (fabs(rxy * q.z) - fractal->transformCommon.offsetD0)
							/ (temp + fractal->transformCommon.offset02);
			aux.DE0 = d1; // q.Length() q.Dot(q)
		}

		if (fractal->transformCommon.functionEnabledAzFalse)
		{
			double d3 = fabs(fractal->transformCommon.scale05 * fabs(q.z - fractal->transformCommon.offsetA0));

			if (!fractal->transformCommon.functionEnabledAwFalse) aux.DE0 = d3;
			else aux.DE0 = min(aux.DE0, d3);
		}



		if (fractal->transformCommon.functionEnabledCFalse)
			aux.DE0 = max(rxy - fractal->transformCommon.offsetA1, aux.DE0);

		aux.DE0 = aux.DE0 / aux.pseudoKleinianDE - fractal->analyticDE.offset0;

/*



	CVector4 zz = z * z;
	double d1 = sqrt(min(min( zz.x + zz.y, zz.y + zz.z), zz.z + zz.x));
	if (fractal->transformCommon.functionEnabledKFalse) d1 = sqrt(zz.x + zz.y);
	d1 -= fractal->transformCommon.offsetR0;

	double d2 = fabs(z.z);
	aux.DE0 = d2;
	if( d1 < d2) aux.DE0 = d1;

	aux.DE0 = 0.5 * (aux.DE0 - fractal->transformCommon.offset0) / aux.DE;*/

		if (fractal->transformCommon.functionEnabledFFalse)
		{
			// KaliBoxMod
			CVector4 p = z;
			double m;
			double r2 = 0.0;

			// REAL Dd = 1.0f;
			for (int n = 0; n < fractal->transformCommon.int32 && r2 < 100.0f; n++)
			{
				p = fractal->transformCommon.additionConstant0555 - fabs(p);
				r2 = p.x * p.x + p.y * p.y + p.z * p.z;
				if (r2 < (fractal->transformCommon.minR2p25 )) // sqrt can be optimized out
				{
					m = fractal->transformCommon.scale015 / (fractal->transformCommon.minR2p25 ); // sqrt can be optimized out
				}

				else if (r2 < 1.0f)
				{
					m = fractal->transformCommon.scale015 / r2;
				}
				else
				{
					m = fractal->transformCommon.scale015;
				}

				p = p * m + fractal->transformCommon.offsetF000;

				aux.DE = aux.DE * fabs(m);
				//	if (i < ColorIterationsk) orbitTrap = min(orbitTrap, fabs(vec4(p,rr)));
			}
			double r = sqrt(r2);
			double d2 = r / p.Length();

			aux.DE0 = fabs(min(.5 * d2, 0.5 * aux.DE0) / aux.pseudoKleinianDE - fractal->analyticDE.offset0);

		}




	if (!fractal->transformCommon.functionEnabledDFalse)
		aux.DE0 = min(aux.dist, aux.DE0);

	aux.dist = aux.DE0;

	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		colorAdd += fractal->foldColor.difs0000.x * fabs(z.x);
		colorAdd += fractal->foldColor.difs0000.y * fabs(z.y);
		colorAdd += fractal->foldColor.difs0000.z * fabs(z.z);
		colorAdd += fractal->foldColor.difs0000.w * k;

		aux.color += colorAdd;
	}
}
