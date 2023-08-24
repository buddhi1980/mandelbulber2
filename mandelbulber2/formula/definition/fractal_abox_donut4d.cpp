/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Formula based on Aexion's The-Golden-Donnut
 * https://www.deviantart.com/aexion/art/The-Golden-Donnut-210148578
 * This formula contains aux.color and aux.actualScale
 */

#include "all_fractal_definitions.h"

cFractalAboxDonut4d::cFractalAboxDonut4d() : cAbstractFractal()
{
	nameInComboBox = "Abox - Donut 4D";
	internalName = "abox_donut4d";
	internalID = fractal::aboxDonut4d;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalAboxDonut4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;

	// initial conditions 4D
	CVector4 ct = CVector4(0.0, 0.0, 0.0, 0.0);
	if (aux.i == 0)
	{
		double angle = atan2(z.x, z.y) * fractal->transformCommon.scaleA2;
		double radius = sqrt(z.x * z.x + z.y * z.y);
		double t_radius = radius - fractal->transformCommon.offset4;

		ct.x = sin(angle) * radius;
		ct.y = cos(angle) * radius;
		ct.z = z.z - fractal->transformCommon.offset0;
		ct.w = t_radius * t_radius + z.z * z.z;

		z = ct;
		z = fabs(z) - fractal->transformCommon.offsetA0000;
		aux.const_c = z;
	}

	// abox
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;

	z = fabs(z + fractal->transformCommon.offset1111)
				- fabs(z - fractal->transformCommon.offset1111) - z;

	zCol = z;

	double rr = z.Dot(z);
	rrCol = rr;
	z += fractal->transformCommon.offset0000;
	if (rr < fractal->transformCommon.minR2p25)
	{
		z *= fractal->transformCommon.maxMinR2factor;
		aux.DE *= fractal->transformCommon.maxMinR2factor;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		z *= fractal->transformCommon.maxR2d1 / rr;
		aux.DE *= fractal->transformCommon.maxR2d1 / rr;
	}
	z -= fractal->transformCommon.offset0000;

	// scale
	z *= fractal->transformCommon.scale2;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale2) + 1.0;

	z += fractal->transformCommon.additionConstant0000;

	z += aux.const_c * fractal->transformCommon.scale1111;

	// aux.color
	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsN
			&& aux.i < fractal->transformCommon.stopIterationsN)
	{
		if (zCol.x != oldZ.x) colorAdd += fractal->foldColor.difs0000.x;
		if (zCol.y != oldZ.y) colorAdd += fractal->foldColor.difs0000.y;
		if (zCol.z != oldZ.z) colorAdd += fractal->foldColor.difs0000.z;
		if (zCol.w != oldZ.w) colorAdd += fractal->foldColor.difs0000.w;

		if (rrCol < fractal->transformCommon.minR2p25)
			colorAdd += fractal->mandelbox.color.factorSp1;
		else if (rrCol < fractal->transformCommon.maxR2d1)
			colorAdd += fractal->foldColor.difs1;
		aux.color += colorAdd;
	}

	// DE tweak
	if (fractal->analyticDE.enabled)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
