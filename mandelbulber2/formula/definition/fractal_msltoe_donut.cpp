/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Msltoe Donut formula
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * low-hanging-dessert-an-escape-time-donut-fractal/msg90171/#msg90171
 */

#include "all_fractal_definitions.h"

cFractalMsltoeDonut::cFractalMsltoeDonut() : cAbstractFractal()
{
	nameInComboBox = "Msltoe - Donut";
	internalName = "msltoe_donut";
	internalID = fractal::msltoeDonut;
	DEType = deltaDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 4.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDonut;
}

void cFractalMsltoeDonut::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double radius2 = fractal->donut.ringThickness;
	double nSect = M_PI_2x / fractal->donut.number;
	double fact = fractal->donut.factor;

	double R = sqrt(z.x * z.x + z.y * z.y);
	double R2 = fractal->donut.ringRadius - R;
	double t = R2 * R2 + z.z * z.z - radius2 * radius2;

	double theta = atan2(z.y, z.x);
	double theta2 = nSect * round(theta / nSect);

	if (t > 0.03)
	{
		double c1 = cos(theta2);
		double s1 = sin(theta2);

		double x1 = c1 * z.x + s1 * z.y;
		double y1 = -s1 * z.x + c1 * z.y;
		double z1 = z.z;

		x1 = x1 - fractal->donut.ringRadius;

		z.x = fact * x1;
		z.y = fact * z1;
		z.z = fact * y1;
	}
	else
	{
		z /= t;
	}
	aux.color += theta2;
}
