/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Sierpinski3D. made from DarkBeam's Sierpinski code from M3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalSierpinski3dV4::cFractalSierpinski3dV4() : cAbstractFractal()
{
	nameInComboBox = "Sierpinski 3D V4";
	internalName = "sierpinski3d_v4";
	internalID = fractal::sierpinski3dV4;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalSierpinski3dV4::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		double rr = 1.0;
		z += fractal->transformCommon.offset000;
		rr = z.Dot(z);
		z *= fractal->transformCommon.maxR2d1 / rr;
		aux.DE *= fractal->transformCommon.maxR2d1 / rr;
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleA1;
		aux.DE *= fractal->transformCommon.scaleA1;
	}
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		// abs z
		if (fractal->transformCommon.functionEnabledAx
				&& aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
			z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAy
				&& aux.i >= fractal->transformCommon.startIterationsY
				&& aux.i < fractal->transformCommon.stopIterationsY)
			z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAz
				&& aux.i >= fractal->transformCommon.startIterationsZ
				&& aux.i < fractal->transformCommon.stopIterationsZ)
			z.z = fabs(z.z);
	}

	// Normal full tetra-fold
	if (fractal->transformCommon.functionEnabled)
	{
		double temp = 0.0;
		if (z.x + z.y < 0.0)
		{
			temp = -z.y;
			z.y = -z.x;
			z.x = temp;
		}
		if (z.x + z.z < 0.0)
		{
			temp = -z.z;
			z.z = -z.x;
			z.x = temp;
		}
		if (z.y + z.z < 0.0)
		{
			temp = -z.z;
			z.z = -z.y;
			z.y = temp;
		}
	}

	// Reversed full tetra-fold
	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
	}

	z *= fractal->transformCommon.scaleA2;

	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z -= fractal->transformCommon.offset111; // neg offset
	}
	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (!fractal->analyticDE.enabledFalse)
		aux.DE *= fabs(fractal->transformCommon.scaleA2);
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scaleA2) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset0;
}
