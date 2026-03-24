/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Pseudo Kleinian Mod7, Knighty - Theli-at's Pseudo Kleinian (Scale 1 JuliaBox + Something
 * @reference https://github.com/Syntopia/Fragmentarium/blob/master/
 * Fragmentarium-Source/Examples/Knighty%20Collection/PseudoKleinian.frag
 */

#include "all_fractal_definitions.h"

cFractalPseudoKleinianMod7::cFractalPseudoKleinianMod7() : cAbstractFractal()
{
	nameInComboBox = "Pseudo Kleinian - Mod 7";
	internalName = "pseudo_kleinian_mod7";
	internalID = fractal::pseudoKleinianMod7;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalPseudoKleinianMod7::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double oldZz = z.z;



	z =
		fabs(z + fractal->transformCommon.offset111) - fabs(z - fractal->transformCommon.offset111) - z;

	CVector4 signs = z;
	signs.x = sign(z.x);
	signs.y = sign(z.y);
	signs.z = sign(z.z);
	signs.w = sign(z.w);

	z = fabs(z);
	CVector4 tt = z - fractal->mandelbox.offset;

	double trr = tt.Dot(tt);
	double tp = min(max(1.0 / trr, 1.0), 1.0 / fractal->transformCommon.offsetR0);

	z += fractal->transformCommon.offsetA000;

	z *= tp;
	aux.DE *= tp;
	z *= signs;

	if (fractal->transformCommon.functionEnabledCFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
		z.z = -z.z;

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	aux.pseudoKleinianDE = fractal->transformCommon.offsetA1; // for pkDE function

	//	aux->dist = min(length(z.xy), fabs (z.z - fractal->transformCommon.offsetA0) )  / aux->DE-
	//.001;

	if (fractal->analyticDE.enabled && aux.i >= fractal->analyticDE.startIterationsA
			&& aux.i < fractal->analyticDE.stopIterationsA) // customDE
	{
		double rxy = sqrt(z.x * z.x - fractal->transformCommon.offsetD0 + z.y * z.y - fractal->transformCommon.offsetD0)
				- fractal->transformCommon.offsetC0;
				//length(z.xy - fractal->transformCommon.offsetD0) - fractal->transformCommon.offsetC0;

		if (fractal->transformCommon.functionEnabledBFalse && aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			aux.DE0 = min(rxy, fabs(z.z - fractal->transformCommon.offsetA0)) / aux.DE
								 - fractal->transformCommon.offsetB0;
			aux.dist = min(aux.dist, aux.DE0);
		}
		else // pk
		{
			double tp2 = 0.0;
			if (!fractal->transformCommon.functionEnabledEFalse)
			{
				tp = (rxy * z.z - fractal->transformCommon.offsetA0);
				tp2 = fabs(tp);
			}
			else
			{
				tp2 = (rxy * z.z - fractal->transformCommon.offsetA0);
				tp = fabs(tp2);
			}
			if (fractal->transformCommon.functionEnabledDFalse)
			{
				if (aux.i >= fractal->transformCommon.startIterationsD
						&& aux.i < fractal->transformCommon.stopIterationsD)
					tp2 = tp;
			}
			aux.DE0 = max(rxy - fractal->transformCommon.offsetA1, tp2 / z.Length()) / aux.DE
								 - fractal->transformCommon.offsetB0;
			aux.dist = min(aux.dist, aux.DE0);
		}
	}



	// color
	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
		&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colorAdd = 0.0;
	//	colorAdd += fractal->foldColor.difs0000.x * k;
		colorAdd += fractal->foldColor.difs0000.y * fabs(z.z);
		colorAdd += fractal->foldColor.difs0000.z * fabs(z.z - oldZz);
/*
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			double Size = 2.0 * cSize.x * fractal->transformCommon.constantMultiplier111.x;
			double bb = ((z.x + Size) / Size) + fractal->transformCommon.additionConstantP000.x;
			bb = fabs(bb - round(bb)) * fractal->transformCommon.constantMultiplierC111.x;
			double dd = ((aux.const_c.x + Size) / Size) + fractal->transformCommon.additionConstantP000.x;
			dd = fabs(dd - round(dd)) * fractal->transformCommon.constantMultiplierC111.x;

			Size = 2.0 * cSize.y * fractal->transformCommon.constantMultiplier111.y;
			double cc = ((z.y + Size) / Size) + fractal->transformCommon.additionConstantP000.y;
			cc = fabs(cc - round(cc)) * fractal->transformCommon.constantMultiplierC111.y;
			double ee = ((aux.const_c.y + Size) / Size) + fractal->transformCommon.additionConstantP000.y;
			ee = fabs(ee - round(ee)) * fractal->transformCommon.constantMultiplierC111.y;

			if (!fractal->transformCommon.functionEnabledOFalse)
			{
				bb = bb + cc;
				dd = dd + ee;
			}
			else
			{
				bb = bb * bb + cc * cc;
				dd = dd * dd + ee * ee;
			}

			if (fractal->transformCommon.functionEnabledAFalse)
			{	Size = 2.0 * cSize.z * fractal->transformCommon.constantMultiplier111.z;
				double aa = ((z.z + Size) / Size) + fractal->transformCommon.additionConstantP000.z;
				aa = fabs(aa - round(aa)) * fractal->transformCommon.constantMultiplierC111.z;
				bb = bb + aa;
				double ff = ((aux.const_c.z + Size) / Size) + fractal->transformCommon.additionConstantP000.z;
				ff = fabs(ff - round(ff)) * fractal->transformCommon.constantMultiplierC111.z;
				dd = dd + ff;
			}
			bb = dd * (1.0 - fractal->foldColor.difs1) + bb * fractal->foldColor.difs1; // mix

			colorAdd += fractal->foldColor.difs0000.w * bb;
		}
*/
		if (!fractal->foldColor.auxColorEnabledBFalse)
		{
			aux.color += colorAdd;
		}
		else
		{
			if ((fractal->foldColor.int0 + aux.i) % fractal->foldColor.int2 == 0)
				aux.color += colorAdd;
		}
	}
}
