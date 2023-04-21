/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * MengerV3Iteration
 * Based on a fractal proposed by Buddhi, with a DE outlined by Knighty:
 * http://www.fractalforums.com/3d-fractal-generation/revenge-of-the-half-eaten-menger-sponge/
 */

#include "all_fractal_definitions.h"

cFractalMengerV6::cFractalMengerV6() : cAbstractFractal()
{
	nameInComboBox = "Menger V6";
	internalName = "menger_v6";
	internalID = fractal::mengerV6;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerV6::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double t;
	CVector4 n;

	// abs z
	if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y); // hmmmmmm
	if (fractal->transformCommon.functionEnabledAz) z.z = fabs(z.z);

	z.y  *= fractal->transformCommon.scaleA1;
	z  *= 0.5;

	for (int k = 0; k < fractal->transformCommon.int8X; k++)
	{
		z *= fractal->transformCommon.scale3;
		aux.DE *= fractal->transformCommon.scale3;
		CVector4 Offset1 = fractal->transformCommon.offset222;
		z.y = z.y - (2.0 * max(z.y, 0.0)) + Offset1.y;
		z.x = -(z.x - (2.0 * max(z.x, 0.0)) + Offset1.x);

		t = fractal->transformCommon.cosA;
		n = CVector4{t * fractal->transformCommon.sinB, fractal->transformCommon.sinA, t * fractal->transformCommon.cosB, 0.0};

		t = n.Length();
		if (t == 0.0) t = 1e-21;
		n /= t;
		t = z.Dot(n) * 2.0;
		z -= max(t, 0.0) * n;

		z.z += Offset1.z;

		t = cos(fractal->transformCommon.angle45 * M_PI_180);
		n = CVector4{t * fractal->transformCommon.sinC, sin(-fractal->transformCommon.angle45 * M_PI_180), t * fractal->transformCommon.cosC, 0.0};
		t = n.Length();
		if (t == 0.0) t = 1e-21;
		n /= t;
		t = z.Dot(n) * 2.0;
		z -= max(t, 0.0) * n;
		t = max((z.x + z.y), 0.0);
		z.y = z.y - t;
		z.x = z.x - t + fractal->transformCommon.offset2;
		z.x = z.x - (2.0 * max(z.x, 0.0)) + fractal->transformCommon.offsetA1;
		z.x = z.x - (2.0 * max(z.x, 0.0)) + fractal->transformCommon.offsetT1;

		t = max((z.x + z.y), 0.0);
		z.x -= t;
		z.y -= t;

		if (fractal->transformCommon.functionEnabledRFalse
					&& k >= fractal->transformCommon.startIterationsR
					&& k < fractal->transformCommon.stopIterationsR)
		{
			z = fractal->transformCommon.rotationMatrix2.RotateVector(z);
		}



	}

	CVector4 edgeDist = fabs(z) - CVector4{1., 1., 1., 0.0};
	edgeDist.x = max(edgeDist.x, 0.0);
	edgeDist.y = max(edgeDist.y, 0.0);
	edgeDist.z = max(edgeDist.z, 0.0);
	t = edgeDist.Length(); // + min(max(edgeDist.x, max(edgeDist.y, edgeDist.z)));

	t /= aux.DE;

	double colDist = aux.dist;
	if (!fractal->analyticDE.enabledFalse)
		aux.dist = t;
	else
		aux.dist = min(aux.dist, t);

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		double colorAdd = 0.0;
		if (colDist != aux.dist) colorAdd = fractal->foldColor.difs0000.x;
		//if (t <= e) colorAdd = fractal->foldColor.difs0000.y;

		aux.color += colorAdd;
	}
}
