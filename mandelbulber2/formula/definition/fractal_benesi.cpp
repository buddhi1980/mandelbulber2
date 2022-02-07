/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Formula invented by Matt Benesi
 * @reference http://www.fractalforums.com/index.php?action=profile;u=1170
 */

#include "all_fractal_definitions.h"

cFractalBenesi::cFractalBenesi() : cAbstractFractal()
{
	nameInComboBox = "Benesi - Pwr2";
	internalName = "benesi";
	internalID = fractal::benesi;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalBenesi::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	aux.DE = aux.DE * 2.0 * aux.r;
	CVector4 zz = z * z;
	double r1 = zz.y + zz.z;
	CVector4 t = z;
	double mul;
	if (!fractal->transformCommon.functionEnabledEFalse)
		mul = aux.const_c.x;
	else
		mul = 0.1;

	if (mul < 0.0 || z.x < sqrt(r1))
		t.x = zz.x - r1 + fractal->transformCommon.offset000.x;
	else
		t.x = -zz.x + r1 - fractal->transformCommon.offset000.x;
	r1 = -pow(r1, -0.5) * 2.0 * fabs(z.x);
	t.y = r1 * (zz.y - zz.z) + fractal->transformCommon.offset000.y;
	t.z = r1 * 2.0 * z.y * z.z + fractal->transformCommon.offset000.z + 1e-016;
	z = t;
}
