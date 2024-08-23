/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * JosLeys-Kleinian formula
 * @reference
 * http://www.fractalforums.com/3d-fractal-generation/an-escape-tim-algorithm-for-kleinian-group-limit-sets/msg98248/#msg98248
 * This formula contains aux.color and aux.DE
 */

#include "all_fractal_definitions.h"

cFractalJosKleinian::cFractalJosKleinian() : cAbstractFractal()
{
	nameInComboBox = "JosLeys-Kleinian";
	internalName = "jos_kleinian";
	internalID = fractal::josKleinian;
	DEType = analyticDEType;
	DEFunctionType = josKleinianDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionJosKleinian;
	coloringFunction = coloringFunctionDefault;
}

void cFractalJosKleinian::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double oldZy = z.y;
	double oldDE = aux.DE;

	// sphere inversion slot#1 iter == 0
	if (fractal->transformCommon.sphereInversionEnabledFalse)
	{
		if (aux.i < 1)
		{
			double rr = 1.0;
			z += fractal->transformCommon.offset000;
			rr = z.Dot(z);
			z *= fractal->transformCommon.maxR2d1 / rr;
			z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
			aux.DE *= (fractal->transformCommon.maxR2d1 / rr) * fractal->analyticDE.scale1;
		}
	}

	CVector4 box_size = fractal->transformCommon.offset111;
	// kleinian
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		double a = fractal->transformCommon.foldingValue;
		double b = fractal->transformCommon.offset;
		double f = sign(b);

		CVector3 box1 = CVector3(2.0 * box_size.x, a * box_size.y, 2.0 * box_size.z);
		CVector3 box2 = CVector3(-box_size.x, -box_size.y + 1.0, -box_size.z);
		CVector3 wrapped = wrap(z.GetXYZ(), box1, box2);

		z = CVector4(wrapped.x, wrapped.y, wrapped.z, z.w);

		// If above the separation line, rotate by 180deg about (-b/2, a/2)
		if (z.y >= a * (0.5 + 0.2 * sin(f * M_PI * (z.x + b * 0.5) / box_size.x)))
			z = CVector4(-b, a, 0., z.w) - z; // z.xy = vec2(-b, a) - z.xy;

		double rr = z.Dot(z);

		if (fractal->foldColor.auxColorEnabled)
		{
			CVector4 colorVector = CVector4(z.x, z.y, z.z, rr);
			aux.color = min(aux.color, colorVector.Length()); // For coloring
		}

		double iR = 1.0 / rr;
		z *= -iR;
		z.x = -b - z.x;
		z.y = a + z.y;

		aux.DE *= fabs(iR);
	}
	// color
	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
		&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colorAdd = 0.0;
		colorAdd += fractal->foldColor.difs0000.x * fabs(oldDE / aux.DE);
		colorAdd += fractal->foldColor.difs0000.y * fabs(z.y);
		colorAdd += fractal->foldColor.difs0000.z * fabs(z.y - oldZy);

		if (fractal->foldColor.auxColorEnabledAFalse
				&& aux.i >= fractal->transformCommon.startIterationsT
					&& aux.i < fractal->transformCommon.stopIterationsT)
		{
			double Size = box_size.x * fractal->transformCommon.scale3D222.x;
			double dd = ((aux.const_c.x + Size) / Size) + fractal->transformCommon.additionConstantP000.x;
			dd = fabs(dd - round(dd)) * fractal->transformCommon.constantMultiplierC111.x;

			Size = box_size.z * fractal->transformCommon.scale3D222.z;
			double ee = ((aux.const_c.z + Size) / Size) + fractal->transformCommon.additionConstantP000.z;
			ee = fabs(ee - round(ee)) * fractal->transformCommon.constantMultiplierC111.z;

			double bb  = dd + ee;
			dd = dd * dd + ee * ee;

			if (fractal->transformCommon.functionEnabledAFalse)
			{
				Size = box_size.y * fractal->transformCommon.scale3D222.y;
				double ff = ((aux.const_c.y + Size) / Size) + fractal->transformCommon.additionConstantP000.y;
				ff = fabs(ff - round(ff)) * fractal->transformCommon.constantMultiplierC111.y;
				dd = dd + ff;
			}
			dd = bb * (1.0 - fractal->foldColor.difs1) + dd * fractal->foldColor.difs1; // mix

			colorAdd += fractal->foldColor.difs0000.w * dd;
		}

		aux.color += colorAdd;
	}
}
