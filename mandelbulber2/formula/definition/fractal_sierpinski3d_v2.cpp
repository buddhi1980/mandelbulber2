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
	if (fractal->transformCommon.functionEnabledTFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		z += fractal->transformCommon.offset000;
		z = fractal->transformCommon.rotationMatrix2.RotateVector(z);
	}
	if (fractal->transformCommon.functionEnabledPFalse)
	{
		// abs z
		if (fractal->transformCommon.functionEnabledAx
				&& aux.i >= fractal->transformCommon.startIterationsC
				&& aux.i < fractal->transformCommon.stopIterationsC1)
			z.x = fabs(z.x + fractal->transformCommon.offsetA000.x);

		if (fractal->transformCommon.functionEnabledAy
				&& aux.i >= fractal->transformCommon.startIterationsD
				&& aux.i < fractal->transformCommon.stopIterationsD1)
			z.y = fabs(z.y + fractal->transformCommon.offsetA000.y);

		if (fractal->transformCommon.functionEnabledAz
				&& aux.i >= fractal->transformCommon.startIterationsP
				&& aux.i < fractal->transformCommon.stopIterationsP1)
			z.z = fabs(z.z + fractal->transformCommon.offsetA000.z);
	}

	CVector4 va = CVector4( 0.0, 0.0, SQRT_1_3, 0.0);
	CVector4 vb = CVector4( 0.0, -2.0 * SQRT_1_3, -1.0, 0.0);
	CVector4 vc = CVector4( -1.0, SQRT_1_3, -1.0, 0.0);
	CVector4 vd = CVector4( 1.0, SQRT_1_3, -1.0, 0.0);
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		va *= fractal->transformCommon.scale1111.x;
		vb *= fractal->transformCommon.scale1111.y;
		vc *= fractal->transformCommon.scale1111.z;
		vd *= fractal->transformCommon.scale1111.w;
	}
	CVector4 tv = z - va;
	double d = tv.Dot(tv);
	CVector4 v = va;
	double td = d;

	tv = z - vb;
	d = tv.Dot(tv);
	if(d < td)
	{
		v = vb;
		td = d;
	}
	tv = z - vc;
	d = tv.Dot(tv);
	if(d < td)
	{
		v = vc;
		td = d;
	}
	tv = z - vd;
	d = tv.Dot(tv);
	if(d < td)
	{
		v = vd;
	}
	z = v + fractal->transformCommon.scale2 * (z - v);
	aux.DE *= fabs(fractal->transformCommon.scale2);

	// spherical inv
	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double m = 1.0;
		double rr = z.Dot(z);
		if (rr < fractal->transformCommon.invert0) m = fractal->transformCommon.inv0;
		else if (rr < fractal->transformCommon.invert1) m = 1.0 / rr;
		else m = fractal->transformCommon.inv1;
		z *= m;
		aux.DE *= m;
	}

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1
						 + fractal->analyticDE.offset0;
}
