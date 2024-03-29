/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * poly fold sym multi DarkBeam's version
 * @reference
 * DarkBeam (luca) http://www.fractalforums.com/mandelbulber/
 * _polyfold_sym-and-polyfoldsymifs-in-mandelbulber-2/msg98162/#msg98162
 */

#include "all_fractal_definitions.h"

cFractalTransfPolyFoldSymMulti::cFractalTransfPolyFoldSymMulti() : cAbstractFractal()
{
	nameInComboBox = "T>Poly Fold Sym Multi";
	internalName = "transf_poly_fold_sym_multi";
	internalID = fractal::transfPolyFoldSymMulti;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfPolyFoldSymMulti::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	// pre abs
	if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);

	int order = fractal->transformCommon.int6;
	double div2PI = (double)order * M_PI_2x_INV;
	double temp = 0.0;
	double sAng = 0.0;
	double cAng = 0.0;
	double angle = 0.0;
	int sector;
	if (fractal->transformCommon.functionEnabledCx)
	{
		bool cy = false;
		if (!fractal->transformCommon.functionEnabledAxFalse)
			sector = (int)(-div2PI * atan(z.x / z.y));
		else
			sector = (int)(-div2PI * atan2(z.x, z.y));
		if (sector & 1) cy = true;
		angle = (double)(sector / div2PI);
		temp = z.x;
		sAng = sin(angle);
		cAng = cos(angle);
		z.x = z.x * cAng - z.y * sAng;
		z.y = temp * sAng + z.y * cAng;
		if (cy == true) z.y = -z.y;
		/*if (fractal->transformCommon.functionEnabledFalse)
		{
			if ((order&1) && (sector == 0)) z.y = fabs(z.y); // more continuous?
			else if (cy == true) z.y = -z.y;
		}
		else if (cy == true) z.y = -z.y;*/
	}
	if (fractal->transformCommon.functionEnabledCyFalse)
	{
		bool cz = false;
		if (!fractal->transformCommon.functionEnabledAyFalse)
			sector = (int)(-div2PI * atan(z.y / z.z));
		else
			sector = (int)(-div2PI * atan2(z.y, z.z));
		if (sector & 1) cz = true;
		angle = (double)(sector / div2PI);
		temp = z.y;
		sAng = sin(angle);
		cAng = cos(angle);
		z.y = z.y * cAng - z.z * sAng;
		z.z = temp * sAng + z.z * cAng;
		if (cz == true) z.z = -z.z;
	}
	if (fractal->transformCommon.functionEnabledCzFalse)
	{
		bool cx = false;
		if (!fractal->transformCommon.functionEnabledAzFalse)
			sector = (int)(-div2PI * atan(z.z / z.x));
		else
			sector = (int)(-div2PI * atan2(z.z, z.x));
		if (sector & 1) cx = true;
		angle = (double)(sector / div2PI);
		temp = z.z;
		sAng = sin(angle);
		cAng = cos(angle);
		z.z = z.z * cAng - z.x * sAng;
		z.x = temp * sAng + z.x * cAng;
		if (cx == true) z.x = -z.x;
	}

	z += fractal->transformCommon.additionConstant000;

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}
