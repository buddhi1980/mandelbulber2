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

cFractalSierpinski3dV2::cFractalSierpinski3dV2() : cAbstractFractal()
{
	nameInComboBox = "Sierpinski 3D V2";
	internalName = "sierpinski3d_v2";
	internalID = fractal::sierpinski3dV2;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalSierpinski3dV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledAFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		z.x += fractal->transformCommon.offset000.x;
		z.y += SQRT_1_3 + fractal->transformCommon.offset000.y;
		z.z += SQRT_3_4d2 / 2.0 + fractal->transformCommon.offset000.z - 0.0123;



		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

		double temp = z.x;
		z.x = z.z;
		z.z = temp;
		temp = SQRT_1_2 * (z.y + z.x);
		z.y = SQRT_1_2 * (z.y - z.x);
		z.x = temp;


		/*double an = (60.) * M_PI / 180;

		double cosa = cos(an);
		double sina = sin(an);

		temp = cosa * z.y + z.z * sina;
		z.y = cosa * z.y - z.z * sina;
		z.z = temp;*/
	}



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

	z *= fractal->transformCommon.scaleA2;
	z -= fractal->transformCommon.offset111; // neg offset



	// Reversed full tetra-fold;
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);

		z *= fractal->transformCommon.scale1;
		aux.DE *= fractal->transformCommon.scale1;
		z -= fractal->transformCommon.offsetF000;
	}

	if (fractal->transformCommon.functionEnabledNFalse)
	{
		double rr = z.Dot(z);
		if (rr < 1.0)
		{
			double m = 1.0;
			z += fractal->mandelbox.offset;
			if (rr < fractal->transformCommon.scale025)
			m = fractal->transformCommon.scale025;
			else m = rr;
			m = 1.0 / m;
			z *= m;
			aux.DE *= m;
			z -= fractal->mandelbox.offset;
		}
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
