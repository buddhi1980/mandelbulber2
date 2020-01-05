/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2019 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Aexion's Quadray Sets from FractalForums
 * @reference http://www.fractalforums.com/the-3d-mandelbulb/quadray-sets/msg31458/#msg31458
 */

#include "fractal_definitions.h"

cFractalAexion::cFractalAexion() : cAbstractFractal()
{
	nameInComboBox = "Aexion";
	internalName = "aexion";
	internalID = fractal::aexion;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelAlreadyHas;
	defaultBailout = 10000.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalAexion::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i == 0)
	{
		double cx = fabs(aux.c.x + aux.c.y + aux.c.z) + fractal->aexion.cadd;
		double cy = fabs(-aux.c.x - aux.c.y + aux.c.z) + fractal->aexion.cadd;
		double cz = fabs(-aux.c.x + aux.c.y - aux.c.z) + fractal->aexion.cadd;
		double cw = fabs(aux.c.x - aux.c.y - aux.c.z) + fractal->aexion.cadd;
		aux.c.x = cx;
		aux.c.y = cy;
		aux.c.z = cz;
		aux.cw = cw;
		double tempX = fabs(z.x + z.y + z.z) + fractal->aexion.cadd;
		double tempY = fabs(-z.x - z.y + z.z) + fractal->aexion.cadd;
		double tempZ = fabs(-z.x + z.y - z.z) + fractal->aexion.cadd;
		double tempW = fabs(z.x - z.y - z.z) + fractal->aexion.cadd;
		z.x = tempX;
		z.y = tempY;
		z.z = tempZ;
		z.w = tempW;
	}
	double tempX = z.x * z.x - z.y * z.y + 2.0 * z.w * z.z + aux.c.x;
	double tempY = z.y * z.y - z.x * z.x + 2.0 * z.w * z.z + aux.c.y;
	double tempZ = z.z * z.z - z.w * z.w + 2.0 * z.x * z.y + aux.c.z;
	double tempW = z.w * z.w - z.z * z.z + 2.0 * z.x * z.y + aux.cw;
	z.x = tempX;
	z.y = tempY;
	z.z = tempZ;
	z.w = tempW;

	if (fractal->analyticDE.enabled)
	{
		// aux.DE = aux.DE * fractal->analyticDE.scale1 * 2.2 * aux.r
		//	+ (fractal->analyticDE.offset1 *2.0);
		double de1 = 1.1 * aux.r;
		double de2 = z.Length() / aux.r;
		aux.DE =
			(de1 + (de2 - de1) * fractal->analyticDE.scale1) * 2.0 * aux.DE + fractal->analyticDE.offset1;
	}
}
