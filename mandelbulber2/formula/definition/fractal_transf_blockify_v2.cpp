/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Blockify
 * based on a block of Fragmentarium code, from Adam Nixon
 * analytic aux.DE
 */

#include "all_fractal_definitions.h"

cFractalTransfBlockifyV2::cFractalTransfBlockifyV2() : cAbstractFractal()
{
	nameInComboBox = "T>Blockify V2";
	internalName = "transf_blockify_v2";
	internalID = fractal::transfBlockifyV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfBlockifyV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double master = fractal->transformCommon.scale / 100.0;
	CVector4 bSize = fractal->transformCommon.constantMultiplier111 * master;
	if (!fractal->transformCommon.functionEnabledFalse)
	{
		if (!fractal->transformCommon.functionEnabledDFalse)
		{
			if (fractal->transformCommon.functionEnabledCx)
			{
				if (!fractal->transformCommon.functionEnabledAxFalse)
					z.x = (floor(z.x / bSize.x) + 0.5) * bSize.x;
				else z.x = floor(z.x / bSize.x + 0.5) * bSize.x;
			}
			if (fractal->transformCommon.functionEnabledCy)
			{
				if (!fractal->transformCommon.functionEnabledAyFalse)
					z.y = (floor(z.y / bSize.y) + 0.5) * bSize.y;
				else z.y = floor(z.y / bSize.y + 0.5) * bSize.y;
			}
			if (fractal->transformCommon.functionEnabledCz)
			{
				if (!fractal->transformCommon.functionEnabledAzFalse)
					 z.z = (floor(z.z / bSize.z) + 0.5) * bSize.z;
				else z.z = floor(z.z / bSize.z + 0.5) * bSize.z;
			}
		}
		else // normalize
		{
			double rNorm = z.Length();
			z /= rNorm;
			if (fractal->transformCommon.functionEnabledCx)
			{
				if (!fractal->transformCommon.functionEnabledAxFalse)
					z.x = (floor(z.x / bSize.x) + 0.5) * bSize.x;
				else z.x = floor(z.x / bSize.x + 0.5) * bSize.x;
			}
			if (fractal->transformCommon.functionEnabledCy)
			{
				if (!fractal->transformCommon.functionEnabledAyFalse)
					z.y = (floor(z.y / bSize.y) + 0.5) * bSize.y;
				else z.y = floor(z.y / bSize.y + 0.5) * bSize.y;
			}
			if (fractal->transformCommon.functionEnabledCz)
			{
				if (!fractal->transformCommon.functionEnabledAzFalse)
					 z.z = (floor(z.z / bSize.z) + 0.5) * bSize.z;
				else z.z = floor(z.z / bSize.z + 0.5) * bSize.z;
			}
			z *= rNorm;
		}
	}
	else // radial
	{
		CVector4 zz = z * z;
		double rr = zz.x + zz.y + zz.z;
		if (fractal->transformCommon.functionEnabledRFalse) rr = sqrt(rr);
		if (fractal->transformCommon.functionEnabledBxFalse) rr = zz.x + zz.y;
		if (fractal->transformCommon.functionEnabledByFalse) rr = zz.y + zz.z;
		if (fractal->transformCommon.functionEnabledBzFalse) rr = zz.z + zz.x;
		if (!fractal->transformCommon.functionEnabledEFalse)
		{
			z /= rr;
			rr = floor(rr / master) * master;
			z *= rr;
		}
		else
		{
			z *= rr;
			rr = floor(rr / master) * master;
			z /= rr;
		}
	}

	// post scale
	z *= fractal->transformCommon.scale1;
	aux.DE = aux.DE * fractal->transformCommon.scale1 * fractal->analyticDE.scale1
							 + fractal->analyticDE.offset0;
	//aux.DE = aux.DE - 0.001;
	//aux.DE *= z.Length() / oldZ.Length();
}
