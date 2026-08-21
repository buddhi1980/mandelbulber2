/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2025 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Dual Power Mandelbox — Mandelbox with dual-power spherical folding.
 * Inside the inner sphere: power1 is applied.
 * Outside the outer sphere: power2 is applied.
 * Between the two radii: smooth interpolation between power1 and power2.
 * This creates distinct "inner" and "outer" fractal regimes in the same formula.
 * @reference Inspired by Mandelbox (Tom Lowe, 2010) and Mandelbulb spherical power folding
 */

#include "all_fractal_definitions.h"
#include "src/fractal.h"

cFractalDualPowerMandelbox::cFractalDualPowerMandelbox() : cAbstractFractal()
{
	nameInComboBox = "Mandelbox - Dual Power";
	internalName = "dual_power_mandelbox";
	internalID = fractal::dualPowerMandelbox;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalDualPowerMandelbox::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// ---- Step 1: Box fold (standard Mandelbox) ----
	if (fractal->mandelbox.rotationsEnabled)
	{
		CVector4 zRot;
		// cast vector to array pointer for address taking of components in opencl
		double *zRotP = reinterpret_cast<double *>(&zRot);
		const double *colP = reinterpret_cast<const double *>(&fractal->mandelbox.color.factor);
		for (int dim = 0; dim < 3; dim++)
		{
			// handle each dimension x, y and z sequentially in pointer var dim
			double *rotDim = (dim == 0) ? &zRotP[0] : ((dim == 1) ? &zRotP[1] : &zRotP[2]);
			const double *colorFactor = (dim == 0) ? &colP[0] : ((dim == 1) ? &colP[1] : &colP[2]);

			zRot = fractal->mandelbox.rot[0][dim].RotateVector(z);
			if (*rotDim > fractal->mandelbox.foldingLimit)
			{
				*rotDim = fractal->mandelbox.foldingValue - *rotDim;
				z = fractal->mandelbox.rotinv[0][dim].RotateVector(zRot);
				aux.color += *colorFactor;
			}
			else
			{
				zRot = fractal->mandelbox.rot[1][dim].RotateVector(z);
				if (*rotDim < -fractal->mandelbox.foldingLimit)
				{
					*rotDim = -fractal->mandelbox.foldingValue - *rotDim;
					z = fractal->mandelbox.rotinv[1][dim].RotateVector(zRot);
					aux.color += *colorFactor;
				}
			}
		}
	}
	else
	{
		if (fabs(z.x) > fractal->mandelbox.foldingLimit)
		{
			z.x = sign(z.x) * fractal->mandelbox.foldingValue - z.x;
			aux.color += fractal->mandelbox.color.factor.x;
		}
		if (fabs(z.y) > fractal->mandelbox.foldingLimit)
		{
			z.y = sign(z.y) * fractal->mandelbox.foldingValue - z.y;
			aux.color += fractal->mandelbox.color.factor.y;
		}
		if (fabs(z.z) > fractal->mandelbox.foldingLimit)
		{
			z.z = sign(z.z) * fractal->mandelbox.foldingValue - z.z;
			aux.color += fractal->mandelbox.color.factor.z;
		}
	}

	// ---- Step 2: Sphere inversion (conditional, based on fractal params) ----
	const double r2 = z.Dot(z);

	z += fractal->mandelbox.offset;

	if (r2 < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		aux.color += fractal->mandelbox.color.factorSp1;
	}
	else if (r2 < fractal->mandelbox.fR2)
	{
		const double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		aux.color += fractal->mandelbox.color.factorSp2;
	}

	z -= fractal->mandelbox.offset;

	// ---- Step 3: Dual-power Mandelbulb spherical folding ----
	// This is the key innovation: two different power exponents applied
	// depending on the radial distance, with smooth interpolation in between.

	double power1 = fractal->transformCommon.pwr4;    // inner power (default: 2)
	double power2 = fractal->transformCommon.pwr8;    // outer power (default: 8)
	double innerR2 = fractal->transformCommon.minR2p25;  // inner radius squared
	double outerR2 = fractal->transformCommon.maxR2d1;   // outer radius squared

	double r = z.Length();
	if (r < 1e-21) r = 1e-21;  // prevent division by zero

	double rp = 1.0;  // radius multiplier (accumulates DE factor)
	double power = 0.0;  // current interpolated power

	if (r2 < innerR2)
	{
		// Inside inner sphere: use power1
		power = power1;
		rp = pow(r, power1 - 1.0);
	}
	else if (r2 < outerR2)
	{
		// Between inner and outer: smooth interpolation
		double t = (r2 - innerR2) / (outerR2 - innerR2);
		// Smoothstep for smoother transition
		t = t * t * (3.0 - 2.0 * t);
		power = power1 * (1.0 - t) + power2 * t;
		rp = pow(r, power - 1.0);
	}
	else
	{
		// Outside outer sphere: use power2
		power = power2;
		rp = pow(r, power2 - 1.0);
	}

	// Spherical coordinate folding
	double th = asin(z.z / r);
	double ph = atan2(z.y, z.x);

	th = (th + fractal->bulb.betaAngleOffset) * power;
	ph = (ph + fractal->bulb.alphaAngleOffset) * power;

	double cth = cos(th);
	rp *= r;  // rp = r^power

	z.x = cth * cos(ph) * rp;
	z.y = cth * sin(ph) * rp;
	z.z = sin(th) * rp;

	// ---- Step 4: Main rotation (optional) ----
	if (fractal->mandelbox.mainRotationEnabled)
	{
		z = fractal->mandelbox.mainRot.RotateVector(z);
	}

	// ---- Step 5: Scale ----
	z = z * fractal->mandelbox.scale;
	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;

	// ---- Step 6: Add Julia constant (c) ----
	z += aux.const_c;

	// ---- DE calculation ----
	// The DE for the power part: rp * aux.DE * power
	// The DE for the scale part: * fabs(scale) + 1
	// Combined:
	aux.DE = rp * aux.DE * power + 1.0;

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
}
