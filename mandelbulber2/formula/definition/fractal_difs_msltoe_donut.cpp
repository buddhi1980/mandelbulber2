/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * DIFSMsltoeDonutIteration
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * low-hanging-dessert-an-escape-time-donut-fractal/msg90171/#msg90171
 */

#include "all_fractal_definitions.h"

cFractalDIFSMsltoeDonut::cFractalDIFSMsltoeDonut() : cAbstractFractal()
{
	nameInComboBox = "DIFS Msltoe Donut";
	internalName = "difs_msltoe_donut";
	internalID = fractal::dIFSMsltoeDonut;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalDIFSMsltoeDonut::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zt = z;
	double radius2 = fractal->donut.ringThickness;
	double nSect = M_PI_2x / fractal->donut.number;

	double R2 = fractal->donut.ringRadius - sqrt(z.x * z.x + z.y * z.y);
	R2 *= R2;
	double t = R2 + z.z * z.z - radius2 * radius2;
	double theta2 = nSect * round(atan2(z.y, z.x) / nSect);

	if (t > 0.03)
	{
		double c1 = cos(theta2);
		double s1 = sin(theta2);

		z.x = c1 * zt.x + s1 * z.y - fractal->donut.ringRadius;
		z.z = -s1 * zt.x + c1 * z.y; // z.y z.z swap
		z.y = zt.z;
		z *= fractal->donut.factor;
		aux.DE *= fractal->donut.factor;
	}
	else
	{
		z /= t;
	}

	t = sqrt(R2 + zt.z * zt.z) - radius2;
	aux.dist = min(aux.dist, t / (aux.DE + 1.0));

	// aux.color
	if (fractal->foldColor.auxColorEnabled) aux.color += fractal->foldColor.difs1;
}
