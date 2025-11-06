/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Non-trig mandelbulber pow2 version based on pow2 code from Davis Makin
 * ref: https://www.facebook.com/david.makin.7
 */

#include "all_fractal_definitions.h"

cFractalMandelbulbPow2V4::cFractalMandelbulbPow2V4() : cAbstractFractal()
{
	nameInComboBox = "Mandelbulb - Pow2 V4";
	internalName = "mandelbulb_pow2_v4";
	internalID = fractal::mandelbulbPow2V4;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandelbulbPow2V4::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	CVector4 p = aux.const_c;
	double r = z.Length();
	double cosPhi;
	double sinPhi;
	double cosTheta1;
	double cosTheta2;
	double cosTheta;
	double sinTheta;

	// Undefined case in which x=y=0 (or x=y=z=0)
	if (z.x == 0. && z.y == 0.)
	{
		// Assign evaluated phi values
		cosPhi = 1.;
		sinPhi = 0.;

		// Assign evaluated theta values if z=0
		if (p.z == 0.)
		{
			cosTheta = 1.;
			sinTheta = 0.;
		}
		// Assign evaluated theta values if z!=0
		else
		{
			cosTheta = -1.;
			sinTheta = 0.;
		}
	}
	// Remaining defined cases
	else
	{
		CVector4 v = z / r;
		CVector4 w = CVector4(z.x, z.y, 0.0, 0.0);
		double t = w.Length();

		w = w / t;
		cosPhi = w.x * w.x - w.y * w.y;
		sinPhi = w.y * w.x + w.y * w.x;
		cosTheta1 = v.x * w.x + v.y * w.y;
		cosTheta2 = v.x * w.x + v.y * w.y;
		cosTheta = cosTheta1 * cosTheta2 - v.z * v.z;
		sinTheta = v.z * cosTheta2 + v.z * cosTheta1;
	}

	CVector4 f = z;
	CVector4 g = z;

	aux.DE = aux.DE * fractal->transformCommon.scale2 * aux.r + fractal->analyticDE.offset1;
	// function pairs
	//  1. f_A, g_A - aka spherical representation 3D
	if (fractal->transformCommon.functionEnabledAFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		//f = r * r * CVector4(-sinPhi * cosTheta, cosPhi * cosTheta, 0, 0.0);
		//g = CVector4(-sinTheta / (cosPhi * cosTheta), 0, 1, 0.0);

		z = r * r * CVector4(cosPhi * cosTheta, sinPhi * cosTheta, sinTheta, 0.0);
	}

	// 2. f_A, g_A - aka sphercical representation 3D (inverted 3rd coordinate in g_A)
	if (fractal->transformCommon.functionEnabledBFalse
			&& aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
	//	f = r * r * CVector4(-sinPhi * cosTheta, cosPhi * cosTheta, 0, 0.0);
	//	g = CVector4(sinTheta / (cosPhi * cosTheta), 0, 1, 0.0);

		z = r * r * CVector4(cosPhi * cosTheta, sinPhi * cosTheta, -sinTheta, 0.0);
	}

	// 3. f_B, g_B - aka space filling tunnels
	if (fractal->transformCommon.functionEnabledCFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{

	//	f = CVector4(1, 1, 1, 0.0);
	//	g = CVector4(cos(z.x), cos(z.y), cos(z.z), 0.0);

	//	z = cross(f, g);
		g = CVector4(cos(z.x), cos(z.y), cos(z.z), 0.0);
			z.x = g.z - g.y;
			z.y = g.x - g.z;
			z.z = g.y - g.x ;


	}

	// 4. f_C, g_C - aka swirl
	if (fractal->transformCommon.functionEnabledDFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		f = CVector4(z.x * z.x, z.y * z.y, z.z * z.z, 0.0);
		g = CVector4(cos(z.x), cos(z.y), cos(z.z), 0.0);

		//z = cross(f, g);

		z.x = f.y * g.z - f.z * g.y;
		z.y = f.z * g.x - f.x * g.z;
		z.z = f.x * g.y - f.y * g.x ;


	}

	// 5. f_D, g_D - aka force Mandelbrot
	if (fractal->transformCommon.functionEnabledEFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		f = r * r * CVector4(-sinPhi, cosPhi, 0, 0.0);
		g = CVector4(0, 0, 1, 0.0);

		z = r * r * CVector4(cosPhi, sinPhi, 0, 0.0);
	}

	// 6. f_E, g_E - aka other forms
	if (fractal->transformCommon.functionEnabledFFalse
			&& aux.i >= fractal->transformCommon.startIterationsF
			&& aux.i < fractal->transformCommon.stopIterationsF)
	{
		f = CVector4(z.x * z.x * z.y, z.y * z.y, z.z * z.z, 0.0);
		g = CVector4(1, cos(z.z), 1, 0.0);


		//z = cross(f, g);
		z.x = f.y * g.z - f.z * g.y;
		z.y = f.z * g.x - f.x * g.z;
		z.z = f.x * g.y - f.y * g.x;
	}

	// 7. User defined (here without exception handling as this becomes changed by the user)
	//
	// Does not include exception handling as this is starting point for user input and
	// we do not incorporate exception handling for user input yet.
	if (fractal->transformCommon.functionEnabledGFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		f = r * r * CVector4(-sinPhi, cosPhi, 0, 0.0);
		g = CVector4(-sinTheta / cosPhi, 0, cosTheta, 0.0);

		//z = cross(f, g);
		z.x = f.y * g.z - f.z * g.y;
		z.y = f.z * g.x - f.x * g.z;
		z.z = f.x * g.y - f.y * g.x;;
	}
	if (fractal->transformCommon.functionEnabledHFalse
			&& aux.i >=fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
	{
		z = r * r *CVector4(sinPhi*cosTheta, cosPhi*cosTheta, sinTheta, 0.0);
	}
	// Apply scheme

	//  scheme 1 = +origPt, scheme 3 = + juliaC
	//	z = cross(f, g);
	//	z = r * r *CVector4(sinPhi*cosTheta, cosPhi*cosTheta, sinTheta, 0.0);
	z.z *= fractal->transformCommon.scale1;
	z *= fractal->transformCommon.scaleA1;
	aux.DE *= fabs(fractal->transformCommon.scaleA1);

	z += fractal->transformCommon.offset000; // plus  julia
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
		z += aux.const_c;

	f = z; // ??????


}
