/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Adds Cpixel constant to z vector, scator algebra
 * based on Manuel's math
 * @reference
 * https://luz.izt.uam.mx/drupal/en/fractals/hun
 * @author Manuel Fernandez-Guasti
 */

#include "all_fractal_definitions.h"

cFractalTransfAddCpixelScator::cFractalTransfAddCpixelScator() : cAbstractFractal()
{
	nameInComboBox = "T>Add Cpixel - Scator";
	internalName = "transf_add_cpixel_scator";
	internalID = fractal::transfAddCpixelScator;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddCpixelScator::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector4 tempC = aux.const_c;
	if (fractal->transformCommon.functionEnabledSwFalse) swap(tempC.x, tempC.z);

	CVector4 cc = tempC * tempC;
	CVector4 newC = tempC;
	double limitA = fractal->transformCommon.scale0;

	if (fractal->transformCommon.functionEnabledRFalse)
	{
		cc = fabs(tempC);
	}

	// scator algebra
	if (cc.x < limitA)
	{
		double temp = 1.0 / cc.x - 1.0;
		cc.x = temp;
	}

	if (!fractal->transformCommon.functionEnabledFalse)
	{																	// real
		newC.x += (cc.y * cc.z) / cc.x; // all pos
		newC.y *= (1.0 + cc.z / cc.x);
		newC.z *= (1.0 + cc.y / cc.x);
		newC *= fractal->transformCommon.constantMultiplier111;
		if (fractal->transformCommon.functionEnabledSwFalse) swap(newC.x, newC.z);

		if (!fractal->transformCommon.functionEnabledSFalse)
		{
			z += newC;
		}
		else
		{
			z.x += sign(z.x) * newC.x;
			z.y += sign(z.y) * newC.y;
			z.z += sign(z.z) * newC.z;
		}
	}
	else
	{																	// imaginary
		newC.x += (cc.y * cc.z) / cc.x; // pos
		newC.y *= (1.0 - cc.z / cc.x);	// pos  neg
		newC.z *= (1.0 - cc.y / cc.x);	// pos  neg
		newC *= fractal->transformCommon.constantMultiplier111;
		if (fractal->transformCommon.functionEnabledy) newC.y = fabs(newC.y);
		if (fractal->transformCommon.functionEnabledz) newC.z = fabs(newC.z);

		if (fractal->transformCommon.functionEnabledSwFalse) swap(newC.x, newC.z);

		// add Cpixel
		if (!fractal->transformCommon.functionEnabledSFalse)
		{
			z += newC;
		}
		else
		{
			z.x += sign(z.x) * newC.x;
			z.y += sign(z.y) * newC.y;
			z.z += sign(z.z) * newC.z;
		}
	}
	// DE calculations
	if (fractal->analyticDE.enabledFalse)
	{
		double vecDE = z.Length() / oldZ.Length();
		aux.DE = aux.DE * vecDE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
}
