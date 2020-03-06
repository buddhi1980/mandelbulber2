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

cFractalSierpinski3dV3::cFractalSierpinski3dV3() : cAbstractFractal()
{
	nameInComboBox = "Sierpinski 3D V3";
	internalName = "sierpinski3d_v3";
	internalID = fractal::sierpinski3dV3;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalSierpinski3dV3::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.rotation2EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		z.x += fractal->transformCommon.offset000.x;
		z.y += SQRT_1_3 + fractal->transformCommon.offset000.y;
		z.z += SQRT_3_4d2 / 2.0 + fractal->transformCommon.offset000.z - 0.0123;
				;

		//z.z += SQRT_3_4d2 / 2; // - 0.0123;

		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

		/*double an = (35.264389683 + 6.784268031) * M_PI_180;
		double cosa = cos(an);
		double sina = sin(an);

		if (fractal->transformCommon.functionEnabledFalse)
		{
			double zTemp = cosa * z.y + z.z * sina;
			z.y = cosa * z.y - z.z * sina;
			z.z = zTemp;
		}
		else
		{
			double yTemp = cosa * z.z + z.y * sina;
			z.z = cosa * z.z - z.y * sina;
			z.y = yTemp;
		}*/

		//z.y += fractal->transformCommon.offset111.y; // SQRT_1_3;

		//z.y +=  SQRT_1_2;

		double xTemp = SQRT_1_2 * (z.y + z.x);
		z.y = SQRT_1_2 * (z.y - z.x);
		z.x = xTemp;
	}

	if (z.x - z.y < 0.0) swap(z.y, z.x);
	if (z.x - z.z < 0.0) swap(z.z, z.x);
	if (z.y - z.z < 0.0) swap(z.z, z.y);

	z *= fractal->transformCommon.scaleA2;
	z -= fractal->transformCommon.offset100; // neg offset

	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}


	// Reversed full tetra-fold;
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 temp = z;
		if (z.x + z.y < 0.0)
		{
			temp.x = -z.y;
			z.y = -z.x;
			z.x = temp.x;
		}
		if (z.x + z.z < 0.0)
		{
			temp.x = -z.z;
			z.z = -z.x;
			z.x = temp.x;
		}
		if (z.y + z.z < 0.0)
		{
			temp.y = -z.z;
			z.z = -z.y;
			z.y = temp.y;
		}
	}

	if (!fractal->analyticDE.enabledFalse)
		aux.DE *= fabs(fractal->transformCommon.scaleA2);
	else
		aux.DE = aux.DE * fabs(fractal->transformCommon.scaleA2) * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset0;
}
