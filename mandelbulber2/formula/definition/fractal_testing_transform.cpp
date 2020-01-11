/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Testing
 * Based on a fractal proposed by Buddhi, with a DE outlined by Knighty:
 * http://www.fractalforums.com/3d-fractal-generation/revenge-of-the-half-eaten-menger-sponge/
 */

#include "all_fractal_definitions.h"

cFractalTestingTransform::cFractalTestingTransform() : cAbstractFractal()
{
	nameInComboBox = "Testing Transform";
	internalName = "testing_transform";
	internalID = fractal::testingTransform;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingTransform::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double Pid6 = M_PI / 6.0;
	CVector4 VPi6 = CVector4(cos(Pid6), -sin(Pid6), 0.0, 0.0);
	double beta = 72.0 * M_PI / 360.0; // ''''''''''''''''''''''''''''''''''''''
	double tc = tan(beta);
	CVector4 fp1 = CVector4(0.5, 0.0, sqrt(3.0 * tc * tc - 1.0) * 0.25, 0.0);
	// CVector4 fl1 = normalize(CVector4(1.0, 0.0, -0.5 * sqrt(3.0 * tc * tc - 1.0), 0.0));
	CVector4 fl1 = (CVector4(1.0, 0.0, -0.5 * sqrt(3.0 * tc * tc - 1.0), 0.0));
	fl1 = fl1 / fl1.Length();
	tc = cos(beta);

	double scl = fractal->transformCommon.scale4 * tc * tc;

	double rr = z.Dot(z);

	// Sierpinski triangle symmetry + fold about xy plane
	if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
	if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
	if (fractal->transformCommon.functionEnabledAz) z.z = fabs(z.z);
	//	z.y = fabs(z.y);
	//	z.z = fabs(z.z);

	double t = 2.0 * min(0.0, z.Dot(VPi6));
	z -= t * VPi6;

	z.y = fabs(z.y);

	// Koch curve fold

	z -= fp1;

	t = 2.0 * min(0.0, z.Dot(fl1));
	z -= t * fl1;

	z += fp1;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	// scale

	z.x -= 1.0;
	// z *= rot;
	z *= scl;
	z.x += 1.0;
	aux.DE *= scl;

	rr = z.Dot(z);

	aux.dist = (sqrt(rr) - 3.0) / aux.DE;
}
