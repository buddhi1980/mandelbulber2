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

cFractalSierpinski3d::cFractalSierpinski3d() : cAbstractFractal()
{
	nameInComboBox = "Sierpinski 3D";
	internalName = "sierpinski3d";
	internalID = fractal::sierpinski3d;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalSierpinski3d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
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
