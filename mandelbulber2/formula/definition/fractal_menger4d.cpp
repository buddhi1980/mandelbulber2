/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * from Syntopia & DarkBeam's Menger4 code from M3D
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalMenger4d::cFractalMenger4d() : cAbstractFractal()
{
	nameInComboBox = "Menger 4D";
	internalName = "menger4d";
	internalID = fractal::menger4d;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 10.0;
	DEAnalyticFunction = analyticFunctionIFS;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMenger4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z += fractal->transformCommon.additionConstant0000; // offset
	}

	z = fabs(z);
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

	double scaleM = fractal->transformCommon.scale3;
	CVector4 offsetM = fractal->transformCommon.additionConstant111d5;
	z.x = scaleM * z.x - offsetM.x;
	z.y = scaleM * z.y - offsetM.y;
	z.w = scaleM * z.w - offsetM.w;
	z.z -= 0.5 * offsetM.z / scaleM;
	z.z = -fabs(-z.z);
	z.z += 0.5 * offsetM.z / scaleM;
	z.z *= scaleM;
	aux.DE *= scaleM;

	if (fractal->transformCommon.functionEnabledSFalse
			&& aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double r2 = z.Dot(z);
		// if (r2 < 1e-21 && r2 > -1e-21) r2 = (r2 > 0) ? 1e-21 : -1e-21;

		if (r2 < fractal->transformCommon.minR2p25)
		{
			z *= fractal->transformCommon.maxMinR2factor;
			aux.DE *= fractal->transformCommon.maxMinR2factor;
			aux.color += fractal->mandelbox.color.factorSp1;
		}
		else if (r2 < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / r2;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{
		CVector4 zA4 = (aux.i == fractal->transformCommon.intA) ? z : CVector4();
		CVector4 zB4 = (aux.i == fractal->transformCommon.intB) ? z : CVector4();

		z = (z * fractal->transformCommon.scale) + (zA4 * fractal->transformCommon.offset)
				+ (zB4 * fractal->transformCommon.offset0);
		aux.DE *= fractal->transformCommon.scale1;
	}

	aux.DE *= fractal->analyticDE.scale1;
}
