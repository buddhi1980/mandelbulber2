/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Menger4D MOD1   from Syntopia & DarkBeam's Menger4 code from M3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalMenger4dMod2::cFractalMenger4dMod2() : cAbstractFractal()
{
	nameInComboBox = "Menger 4D Mod2";
	internalName = "menger4d_mod2";
	internalID = fractal::menger4dMod2;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMenger4dMod2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		if (fractal->transformCommon.functionEnabledAFalse)
		{
			z = CVector4(z.x, z.y, z.z, z.Length());
			aux.DE += 0.5;
		}
		z = fabs(z - fractal->transformCommon.offsetA0000);
	}

		z = fabs(z);
		if (aux.i >= fractal->transformCommon.startIterationsC
				&& aux.i < fractal->transformCommon.stopIterationsC)
		{
			z += fractal->transformCommon.additionConstant0000; // offset
		}


		if (z.x - z.y < 0.0) swap(z.y, z.x);
		if (z.x - z.z < 0.0) swap(z.z, z.x);
		if (z.y - z.z < 0.0) swap(z.z, z.y);
		if (z.x - z.w < 0.0) swap(z.w, z.x);
		if (z.y - z.w < 0.0) swap(z.w, z.y);
		if (z.z - z.w < 0.0) swap(z.w, z.z);

		// 6 plane rotation
		if (fractal->transformCommon.functionEnabledRFalse
				&& aux.i >= fractal->transformCommon.startIterationsR
				&& aux.i < fractal->transformCommon.stopIterationsR)
		{
			CVector4 tp;
			if (fractal->transformCommon.rotation44a.x != 0)
			{
				tp = z;
				double alpha = fractal->transformCommon.rotation44a.x * M_PI_180;
				z.x = tp.x * cos(alpha) + tp.y * sin(alpha);
				z.y = tp.x * -sin(alpha) + tp.y * cos(alpha);
			}
			if (fractal->transformCommon.rotation44a.y != 0)
			{
				tp = z;
				double beta = fractal->transformCommon.rotation44a.y * M_PI_180;
				z.y = tp.y * cos(beta) + tp.z * sin(beta);
				z.z = tp.y * -sin(beta) + tp.z * cos(beta);
			}
			if (fractal->transformCommon.rotation44a.z != 0)
			{
				tp = z;
				double gamma = fractal->transformCommon.rotation44a.z * M_PI_180;
				z.x = tp.x * cos(gamma) + tp.z * sin(gamma);
				z.z = tp.x * -sin(gamma) + tp.z * cos(gamma);
			}
			if (fractal->transformCommon.rotation44b.x != 0)
			{
				tp = z;
				double delta = fractal->transformCommon.rotation44b.x * M_PI_180;
				z.x = tp.x * cos(delta) + tp.w * sin(delta);
				z.w = tp.x * -sin(delta) + tp.w * cos(delta);
			}
			if (fractal->transformCommon.rotation44b.y != 0)
			{
				tp = z;
				double epsilon = fractal->transformCommon.rotation44b.y * M_PI_180;
				z.y = tp.y * cos(epsilon) + tp.w * sin(epsilon);
				z.w = tp.y * -sin(epsilon) + tp.w * cos(epsilon);
			}
			if (fractal->transformCommon.rotation44b.z != 0)
			{
				tp = z;
				double zeta = fractal->transformCommon.rotation44b.z * M_PI_180;
				z.z = tp.z * cos(zeta) + tp.w * sin(zeta);
				z.w = tp.z * -sin(zeta) + tp.w * cos(zeta);
			}
		}

		CVector4 scaleM = fractal->transformCommon.scale3 * fractal->transformCommon.scale1111;
		CVector4 offsetM = fractal->transformCommon.additionConstant111d5;
		z.x = scaleM.x * z.x - offsetM.x;
		z.y = scaleM.y * z.y - offsetM.y;
		z.w = scaleM.w * z.w - offsetM.w;
		z.z *= fractal->transformCommon.scale1;
		double t = 0.5 * offsetM.z / scaleM.z;
		z.z -= t;
		z.z = -fabs(z.z);
		z.z += t;
		z.z *= fractal->transformCommon.scaleA3;
		aux.DE *= fractal->transformCommon.scale3;

		if (fractal->transformCommon.functionEnabledSFalse
				&& aux.i >= fractal->transformCommon.startIterationsS
				&& aux.i < fractal->transformCommon.stopIterationsS)
		{
			double rr = z.Dot(z);
			// if (r2 < 1e-21 && r2 > -1e-21) r2 = (r2 > 0) ? 1e-21 : -1e-21;

			if (rr < fractal->transformCommon.minR2p25)
			{
				z *= fractal->transformCommon.maxMinR2factor;
				aux.DE *= fractal->transformCommon.maxMinR2factor;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
				z *= tglad_factor2;
				aux.DE *= tglad_factor2;
			}
		}

		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
