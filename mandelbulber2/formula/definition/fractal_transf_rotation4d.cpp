/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * rotation 4D
 */

#include "all_fractal_definitions.h"

cFractalTransfRotation4d::cFractalTransfRotation4d() : cAbstractFractal()
{
	nameInComboBox = "T>Rotation 4D";
	internalName = "transf_rotation4d";
	internalID = fractal::transfRotation4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfRotation4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	CVector4 tp;
	if (fractal->transformCommon.rotation44a.x != 0)
	{
		tp = z;
		double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
		z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
		z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
	}
	if (fractal->transformCommon.rotation44a.y != 0)
	{
		tp = z;
		double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
		z.y = tp.y * cos(beta) + tp.z * sin(beta);
		z.z = tp.y * -sin(beta) + tp.z * cos(beta);
	}
	if (fractal->transformCommon.rotation44a.z != 0)
	{
		tp = z;
		double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
		z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
		z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
	}
	if (fractal->transformCommon.rotation44b.x != 0)
	{
		tp = z;
		double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
		z.x = tp.x * cos(delta) + tp.w * sin(delta);
		z.w = tp.x * -sin(delta) + tp.w * cos(delta);
	}
	if (fractal->transformCommon.rotation44b.y != 0)
	{
		tp = z;
		double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
		z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
		z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
	}
	if (fractal->transformCommon.rotation44b.z != 0)
	{
		tp = z;
		double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
		z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
		z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
	}
}
