/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Formula based on Aexion's The-Golden-Donnut
 * https://www.deviantart.com/aexion/art/The-Golden-Donnut-210148578
 * This formula contains aux.color and aux.actualScale
 */

#include "all_fractal_definitions.h"

cFractalAboxDonut4dV2::cFractalAboxDonut4dV2() : cAbstractFractal()
{
	nameInComboBox = "Abox - Donut 4D V2";
	internalName = "abox_donut4d_v2";
	internalID = fractal::aboxDonut4dV2;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalAboxDonut4dV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double t;

	// initial conditions 4D
	CVector4 ct = CVector4(0.0, 0.0, 0.0, 0.0);
	//
	if (aux.i == 0)
	{
		double angle = atan2(z.x, z.y) * fractal->transformCommon.scaleA2;
		double radius = sqrt(z.x * z.x + z.y * z.y);
		double t_radius = radius - fractal->transformCommon.offset4;

		if (!fractal->transformCommon.functionEnabledAFalse)
		{
			t = radius;
		}
		else
		{
			t = radius * radius;
			t = (t + (radius - t) * fractal->transformCommon.scaleA1);
		}
		ct.x = sin(angle) * t;

		if (!fractal->transformCommon.functionEnabledBFalse)
		{
			t = radius;
		}
		else
		{
			t = radius * radius;
			t = (t + (radius - t) * fractal->transformCommon.scaleB1);
		}
		ct.y = cos(angle) * t;

		ct.z = z.z - fractal->transformCommon.offset0;

		if (!fractal->transformCommon.functionEnabledDFalse)
		{
			t = t_radius * t_radius;
		}
		else
		{
			t = t_radius;
			t = (t + (t_radius * t_radius - t) * fractal->transformCommon.scaleD1);
		}
		ct.w = t + z.z * z.z;

		if (fractal->transformCommon.functionEnabledEFalse)
		{
			t = z.Length();
			ct.w = (ct.w + (t - ct.w) * fractal->transformCommon.scaleE1);
		}

		z = ct;
		CVector4 sgns = CVector4(sign(z.x), sign(z.y), sign(z.z), sign(z.w));


		z = fabs(z) - fractal->transformCommon.offsetA0000;
				if (fractal->transformCommon.functionEnabledFFalse) z = sgns * z;
		aux.const_c = z;
	}

	// abox
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;

	z.x = fabs(z.x + fractal->transformCommon.offset1111.x)
				- fabs(z.x - fractal->transformCommon.offset1111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.offset1111.y)
				- fabs(z.y - fractal->transformCommon.offset1111.y) - z.y;
	if (fractal->transformCommon.functionEnabled)
	{
		z.z = fabs(z.z + fractal->transformCommon.offset1111.z)
					- fabs(z.z - fractal->transformCommon.offset1111.z) - z.z;
	}
	z.w = fabs(z.w + fractal->transformCommon.offset1111.w)
				- fabs(z.w - fractal->transformCommon.offset1111.w) - z.w;
	zCol = z;

if (fractal->transformCommon.functionEnabledFFalse) z.w *= fractal->transformCommon.scaleNeg1; // mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

	double rr = z.Dot(z);
	rrCol = rr;
	z += fractal->transformCommon.offset0000;
	if (rr < fractal->transformCommon.minR2p25)
	{
		z *= fractal->transformCommon.maxMinR2factor;
		aux.DE *= fractal->transformCommon.maxMinR2factor;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		z *= fractal->transformCommon.maxR2d1 / rr;
		aux.DE *= fractal->transformCommon.maxR2d1 / rr;
	}
	z -= fractal->transformCommon.offset0000;

	// scale
	double useScale = 1.0;
	useScale = aux.actualScaleA + fractal->transformCommon.scale2;
	z *= useScale;
	aux.DE = aux.DE * fabs(useScale) + 1.0;
	// scale vary
	if (fractal->transformCommon.functionEnabledKFalse
			&& aux.i >= fractal->transformCommon.startIterationsK
			&& aux.i < fractal->transformCommon.stopIterationsK)
	{
		// update actualScaleA for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
		aux.actualScaleA -= vary;
	}

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
	z += fractal->transformCommon.additionConstant0000;

	z += aux.const_c * fractal->transformCommon.scale1111;

	// aux.color
	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsN
			&& aux.i < fractal->transformCommon.stopIterationsN)
	{
		if (zCol.x != oldZ.x) colorAdd += fractal->foldColor.difs0000.x;
		if (zCol.y != oldZ.y) colorAdd += fractal->foldColor.difs0000.y;
		if (zCol.z != oldZ.z) colorAdd += fractal->foldColor.difs0000.z;
		if (zCol.w != oldZ.w) colorAdd += fractal->foldColor.difs0000.w;

		if (rrCol < fractal->transformCommon.minR2p25)
			colorAdd += fractal->mandelbox.color.factorSp1;
		else if (rrCol < fractal->transformCommon.maxR2d1)
			colorAdd += fractal->foldColor.difs1;

		if (!fractal->transformCommon.functionEnabledJFalse)
			if (!fractal->transformCommon.functionEnabledM)
				aux.color = colorAdd;
			else
				aux.color += colorAdd;
		else
			aux.color = max(aux.color, colorAdd);
	}

	// DE tweak
	if (fractal->analyticDE.enabled)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
}
