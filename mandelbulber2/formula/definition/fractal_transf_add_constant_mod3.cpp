/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * based on darkbeams _PartlyJuiliaRoff transform from Mandelbulb3D.
 */

#include "all_fractal_definitions.h"

cFractalTransfAddConstantMod3::cFractalTransfAddConstantMod3() : cAbstractFractal()
{
	nameInComboBox = "T>Add Constant Mod3";
	internalName = "transf_add_constant_mod3";
	internalID = fractal::transfAddConstantMod3;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddConstantMod3::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 signs = CVector4(1.0, 1.0, 1.0, 1.0);
	signs.x *= sign(aux.const_c.x);
	signs.y *= sign(aux.const_c.y);
	signs.z *= sign(aux.const_c.z);

	CVector4 offset = fractal->transformCommon.additionConstantA000;
	if (fractal->transformCommon.functionEnabledDFalse) offset *= signs;

	CVector4 t;
	if (!fractal->transformCommon.functionEnabledBFalse) t = aux.const_c - offset;
	else t = z - offset;

	double r;
	if (!fractal->transformCommon.functionEnabledAFalse) r = t.Length();
	else r = t.Dot(t);

	CVector4 offset1 = fractal->transformCommon.offset000;
	if (fractal->transformCommon.functionEnabledCFalse) offset1 *= signs;

	double m = (1.0 - fractal->transformCommon.radius1 / r) * fractal->transformCommon.scale1;

	if (r > fractal->transformCommon.radius1)
	{
		offset1 =  offset1 + t * m;
		z +=  offset1;
	}

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP)
	{
		if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAz) z.z = fabs(z.z);
	}
}
