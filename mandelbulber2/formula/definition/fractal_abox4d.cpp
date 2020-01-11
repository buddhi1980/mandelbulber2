/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Formula based on Mandelbox (ABox). Extended to 4 dimensions
 * This formula contains aux.color and aux.actualScale
 */

#include "all_fractal_definitions.h"

cFractalAbox4d::cFractalAbox4d() : cAbstractFractal()
{
	nameInComboBox = "Abox 4D";
	internalName = "abox4d";
	internalID = fractal::abox4d;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalAbox4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;

	aux.actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);

	// parabolic = paraOffset + iter *slope + (iter *iter *scale)
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{
		double parabScale = 0.0;
		if (fractal->Cpara.parabScale != 0.0)
			parabScale = aux.i * aux.i * 0.001 * fractal->Cpara.parabScale;
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope) + (parabScale);
		z.w += paraAddP0;
	}

	CVector4 oldZ = z;
	z.x = fabs(z.x + fractal->transformCommon.offset1111.x)
				- fabs(z.x - fractal->transformCommon.offset1111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.offset1111.y)
				- fabs(z.y - fractal->transformCommon.offset1111.y) - z.y;
	z.z = fabs(z.z + fractal->transformCommon.offset1111.z)
				- fabs(z.z - fractal->transformCommon.offset1111.z) - z.z;
	z.w = fabs(z.w + fractal->transformCommon.offset1111.w)
				- fabs(z.w - fractal->transformCommon.offset1111.w) - z.w;
	zCol = z;

	double rr = z.Dot(z);
	rrCol = rr;
	if (fractal->mandelboxVary4D.rPower != 1.0) rr = pow(rr, fractal->mandelboxVary4D.rPower);

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

	z *= aux.actualScale;
	aux.DE = aux.DE * fabs(aux.actualScale) + 1.0;

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

	if (fractal->foldColor.auxColorEnabled)
	{
		if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor4D.x;
		if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor4D.y;
		if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor4D.z;
		if (zCol.w != oldZ.w) colorAdd += fractal->mandelbox.color.factor4D.w;

		if (rrCol < fractal->transformCommon.minR2p25)
			colorAdd += fractal->mandelbox.color.factorSp1;
		else if (rrCol < fractal->transformCommon.maxR2d1)
			colorAdd += fractal->mandelbox.color.factorSp2;

		aux.color += colorAdd;
	}
}
