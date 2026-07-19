/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Chebyshev Bulb — Mandelbulb with Chebyshev polynomial iteration.
 * Replaces z^power with T_degree(z/|z|) * |z| where T_n is the Chebyshev
 * polynomial of degree n. T_n(x) = cos(n * arccos(x)) for |x| <= 1.
 * Creates sharp, polygonal-like fractal structures.
 * @reference https://www.fractalforums.com/
 */

#include "all_fractal_definitions.h"

cFractalChebyshevBulb::cFractalChebyshevBulb() : cAbstractFractal()
{
	nameInComboBox = "Chebyshev Bulb";
	internalName = "chebyshev_bulb";
	internalID = fractal::chebyshevBulb;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

// Compute Chebyshev polynomial T_n(x) using the identity T_n(x) = cos(n * arccos(x))
// For |x| > 1, use T_n(x) = cosh(n * acosh(x))
static double chebyshev_t(double x, double n)
{
	if (x > 1.0)
	{
		// T_n(x) = cosh(n * acosh(x)) for x > 1
		return cosh(n * acosh(x));
	}
	else if (x < -1.0)
	{
		// T_n(x) = cosh(n * acosh(-x)) * sign for x < -1
		return (fmod(n, 2.0) == 0) ? cosh(n * acosh(-x)) : -cosh(n * acosh(-x));
	}
	else
	{
		// T_n(x) = cos(n * arccos(x)) for |x| <= 1
		return cos(n * acos(x));
	}
}

// Derivative: dT_n(x)/dx = n * U_{n-1}(x) where U is the Chebyshev polynomial of the second kind
// Or more directly: dT_n(x)/dx = n * sin(n * arccos(x)) / sqrt(1 - x^2)
static double chebyshev_t_derivative(double x, double n)
{
	if (x > 1.0)
	{
		// d/dx cosh(n * acosh(x)) = n * sinh(n * acosh(x)) / sqrt(x^2 - 1)
		double sqrt_val = sqrt(x * x - 1.0);
		if (sqrt_val < 1e-15) return n;
		return n * sinh(n * acosh(x)) / sqrt_val;
	}
	else if (x < -1.0)
	{
		// Similar for x < -1
		double sqrt_val = sqrt(x * x - 1.0);
		if (sqrt_val < 1e-15) return n;
		double sign = (fmod(n, 2.0) == 0) ? 1.0 : -1.0;
		return sign * n * sinh(n * acosh(-x)) / sqrt_val;
	}
	else
	{
		// d/dx cos(n * arccos(x)) = n * sin(n * arccos(x)) / sqrt(1 - x^2)
		double sqrt_val = sqrt(1.0 - x * x);
		if (sqrt_val < 1e-15) return n * sin(n * acos(x)) / 1e-15;
		return n * sin(n * acos(x)) / sqrt_val;
	}
}

void cFractalChebyshevBulb::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// Chebyshev Bulb: apply T_degree to each coordinate
	// z_new = (T_degree(x/r * r), T_degree(y/r * r), T_degree(z/r * r))
	//       = (sign(x) * T_degree(x/|x|) * |x|, ...)
	
	double degree = fractal->bulb.degree;
	if (degree < 2.0) degree = 2.0; // Minimum degree 2 for fractal behavior
	
	double r = aux.r;
	if (r < 1e-10)
	{
		z += fractal->transformCommon.offsetA000;
		z += aux.const_c * fractal->transformCommon.constantMultiplierA111;
		return;
	}
	
	// Compute the Chebyshev transform for each coordinate
	// T_n(x) = cos(n * arccos(x)) for |x| <= 1
	// For the fractal, we apply it to each coordinate normalized by r, then scaled back
	
	double x = z.x, y = z.y, zz = z.z;
	
	// Apply Chebyshev polynomial to each coordinate
	// For the Mandelbulb-style fractal, we use: T_degree(x/r) * r
	// This creates the characteristic "bulb" effect
	
	double tx = chebyshev_t(x / r, degree) * r;
	double ty = chebyshev_t(y / r, degree) * r;
	double tz = chebyshev_t(zz / r, degree) * r;
	
	// DE computation: chain rule for each coordinate
	// d(T_n(x/r) * r)/dx = T'_n(x/r) * (1/r) * r + T_n(x/r) * 0
	//                     = T'_n(x/r)
	// Total DE = sum of |T'_n(coord_i / r)| for i = x, y, z
	double dx = chebyshev_t_derivative(x / r, degree);
	double dy = chebyshev_t_derivative(y / r, degree);
	double dz = chebyshev_t_derivative(zz / r, degree);
	
	double totalDE = fabs(dx) + fabs(dy) + fabs(dz);
	
	// Set new coordinates
	z.x = tx;
	z.y = ty;
	z.z = tz;
	
	// Apply DE
	aux.DE *= totalDE;
	
	// Add constant
	z += fractal->transformCommon.offsetA000;
	z += aux.const_c * fractal->transformCommon.constantMultiplierA111;
	
	// Z.z scaling
	z.z *= fractal->transformCommon.scaleA1;
	
	// Analytic DE
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
	
	// Optional DE0 distance estimator
	if (fractal->transformCommon.functionEnabledCFalse)
	{
		double zLen = z.Length();
		if (zLen > 1.0)
			aux.DE0 = 0.5 * log(zLen) * zLen / aux.DE;
		else
			aux.DE0 = 0.01; // 0.0 artifacts in openCL
		
		if (aux.i >= fractal->transformCommon.startIterationsO
				&& aux.i < fractal->transformCommon.stopIterationsO)
			aux.dist = min(aux.dist, aux.DE0);
		else
			aux.dist = aux.DE0;
	}
}
