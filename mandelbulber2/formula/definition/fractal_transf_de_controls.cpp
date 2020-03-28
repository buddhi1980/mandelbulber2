/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDEControlsIteration
 *
 */

#include "all_fractal_definitions.h"

cFractalTransfDEControls::cFractalTransfDEControls() : cAbstractFractal()
{
	nameInComboBox = "T>DE Controls";
	internalName = "transf_de_controls";
	internalID = fractal::transfDEControls;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDEControls::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rd = 0.0;
	CVector4 tZ;
	// distance functions
	switch (fractal->combo4.combo4)
	{
		case multi_combo4_type1:
		default: rd = aux.DE0; break;
		case multi_combo4_type2:
			tZ = fabs(z);
			rd = max(tZ.x, max(tZ.y, tZ.z)); // infnorm
			break;
		case multi_combo4_type3:
			tZ = z * z;
			rd = max(sqrt(tZ.x + tZ.y), max(sqrt(tZ.y + tZ.z), sqrt(tZ.z + tZ.x)));
			break;
		case multi_combo4_type4:
			rd = z.Length(); // eucli norm
			break;
	}
	// tweaks
	rd += fractal->transformCommon.offset0;
	aux.DE += fractal->analyticDE.offset0;

	// out distance functions
	if (fractal->transformCommon.functionEnabledAy)
	{
		rd = rd / aux.DE; // same as an uncondtional aux.dist
	}
	if (fractal->transformCommon.functionEnabledBFalse)
	{
		double rxy = sqrt(z.x * z.x + z.y * z.y);
		double pkD = max(rxy -fractal->transformCommon.offsetA1, fabs(rxy * z.z) / rd);
		rd = pkD / aux.DE;
	}
	if (fractal->transformCommon.functionEnabledCFalse)
	{
		rd = 0.5 * rd * log(rd) / aux.DE;
	}

	if (fractal->transformCommon.functionEnabledMFalse)
	{
		double mixA;
		double mixB;
		double rxy;
		switch (fractal->combo3.combo3)
		{
			case multi_combo3_type1:
			default:
				mixA = rd;
				mixB = 0.5 * rd * log(rd);
				break;
			case multi_combo3_type2:
				rxy = sqrt(z.x * z.x + z.y * z.y);
				mixA = max(rxy - fractal->transformCommon.offsetA1, fabs(rxy * z.z) / rd);
				mixB = rd;
				break;
			case multi_combo3_type3:
				mixA = 0.5 * rd * log(rd);
				rxy = sqrt(z.x * z.x + z.y * z.y);
				mixB = max(rxy - fractal->transformCommon.offsetA1, fabs(rxy * z.z) / rd);
				break;
		}
		rd = (mixA + (mixB - mixA) * fractal->transformCommon.scale1) / aux.DE;
	}
	double colorDist = aux.dist;
	if (!fractal->transformCommon.functionEnabledFalse)
	{
		aux.colorHybrid = rd; // aux.colorHybrid temp
	}
	else
	{
		int tempC = fractal->transformCommon.int3X;
		if (aux.i < tempC || rd < aux.colorHybrid)
		{
			aux.colorHybrid = rd;
		}
	}
	aux.dist = aux.colorHybrid;

	// aux.color
	if (fractal->foldColor.auxColorEnabled)
	{
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			colorAdd += fractal->foldColor.difs0000.x * fabs(z.x * z.y); // fabs(zc.x * zc.y)
			colorAdd += fractal->foldColor.difs0000.y * max(z.x, z.y);	 // max(z.x, z.y);
			colorAdd += fractal->foldColor.difs0000.z * (z.x * z.x + z.y * z.y);
			// colorAdd += fractal->foldColor.difs0000.w * fabs(zc.x * zc.y);
		}
		colorAdd += fractal->foldColor.difs1;
		if (fractal->foldColor.auxColorEnabledA)
		{
			if (colorDist != aux.dist) aux.color += colorAdd;
		}
		else
			aux.color += colorAdd;
	}
}
