/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * 4D boxbulb, Formula based on Mandelbox (ABox). Extended to 4 dimensions and with variable scale
 * parameter.
 * This formula contains aux.color and aux.actualScale
 */

#include "all_fractal_definitions.h"

cFractalTesting4d::cFractalTesting4d() : cAbstractFractal()
{
	nameInComboBox = "Testing4d";
	internalName = "testing4d";
	internalID = fractal::testing4d;
	DEType = analyticDEType;
	DEFunctionType = logarithmicDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLogarithmic;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTesting4d::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;
	CVector4 zCol = z;
	CVector4 oldZ = z;

	// parabolic.w = paraOffset + iter *slope + (iter *iter *scale)
	double paraAddP0 = 0.0;
	if (fractal->Cpara.enabledParabFalse)
	{
		double parabScale = 0.0;
		if (fractal->Cpara.parabScale != 0.0)
			parabScale = aux.i * aux.i * 0.001 * fractal->Cpara.parabScale;
		paraAddP0 = fractal->Cpara.parabOffset0 + (aux.i * fractal->Cpara.parabSlope) + (parabScale);
		z.w += paraAddP0;
	}

	// sinusoidal w
	double sinAdd = 0.0;
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		sinAdd = sin((aux.i + fractal->transformCommon.offset0) / fractal->transformCommon.scaleA1)
						 * fractal->transformCommon.scaleC1;
		z.w += sinAdd;
	}

	/*	CVector4 temp = fractal->transformCommon.offset0000;
		CVector4 temp2 = temp * temp;

		z.x += ((8.0 * temp.x * temp2.x) / ((z.x * z.x) + (4.0 * temp2.x)) - 2.0 * temp.x) * sign(z.x) *
		fractal->transformCommon.scale1;
		z.y += ((8.0 * temp.y * temp2.y) / ((z.y * z.y) + (4.0 * temp2.y)) - 2.0 * temp.y) * sign(z.y) *
		fractal->transformCommon.scale1;
		z.z += ((8.0 * temp.z * temp2.z) / ((z.z * z.z) + (4.0 * temp2.z)) - 2.0 * temp.z) * sign(z.z) *
		fractal->transformCommon.scale1;
		z.w += ((8.0 * temp.w * temp2.w) / ((z.w * z.w) + (4.0 * temp2.w)) - 2.0 * temp.w) * sign(z.w) *
		fractal->transformCommon.scale1*/

	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		oldZ = z;
		z.x = fabs(z.x + fractal->transformCommon.offset1111.x)
					- fabs(z.x - fractal->transformCommon.offset1111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.offset1111.y)
					- fabs(z.y - fractal->transformCommon.offset1111.y) - z.y;
		z.z = fabs(z.z + fractal->transformCommon.offset1111.z)
					- fabs(z.z - fractal->transformCommon.offset1111.z) - z.z;
		z.w = fabs(z.w + fractal->transformCommon.offset1111.w)
					- fabs(z.w - fractal->transformCommon.offset1111.w) - z.w;
		zCol = z;
	}
	CVector4 temp = fractal->transformCommon.offset0000;
	CVector4 temp2 = temp * temp;
	// if (z.w < 1e-016) z.w = 1e-016;
	if (z.w < 1e-21 && z.w > -1e-21) z.w = (z.w > 0) ? 1e-21 : -1e-21;
	z.x += ((8.0 * temp.x * temp2.x) / ((z.x * z.x) + (4.0 * temp2.x)) - 2.0 * temp.x) * sign(z.x)
				 * fractal->transformCommon.scale1;
	z.y += ((8.0 * temp.y * temp2.y) / ((z.y * z.y) + (4.0 * temp2.y)) - 2.0 * temp.y) * sign(z.y)
				 * fractal->transformCommon.scale1;
	z.z += ((8.0 * temp.z * temp2.z) / ((z.z * z.z) + (4.0 * temp2.z)) - 2.0 * temp.z) * sign(z.z)
				 * fractal->transformCommon.scale1;
	z.w += ((8.0 * temp.w * temp2.w) / ((z.w * z.w) + (4.0 * temp2.w)) - 2.0 * temp.w) * sign(z.w)
				 * fractal->transformCommon.scale1;

	// r power
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);
		rrCol = rr;
		if (fractal->mandelboxVary4D.rPower != 1.0) rr = pow(rr, fractal->mandelboxVary4D.rPower);

		// spherical fold
		// z += fractal->transformCommon.offset0000;
		if (rr < fractal->transformCommon.minR2p25)
		{
			z *= fractal->transformCommon.maxMinR2factor;
			aux.DE *= fractal->transformCommon.maxMinR2factor;
			// colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rr);
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			z *= fractal->transformCommon.maxR2d1 / rr;
			aux.DE *= fractal->transformCommon.maxR2d1 / rr;
			// colorAdd += fractal->mandelbox.color.factorSp2 * (rr - fractal->transformCommon.minR2p25);
		}
		/*else
			colorAdd += fractal->mandelbox.color.factorSp2
							* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);*/

		// z -= fractal->transformCommon.offset0000;
	}

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{

		useScale = aux.actualScaleA + fractal->transformCommon.scale;

		z *= useScale;
		aux.DE = aux.DE * fabs(useScale);

		if (aux.i >= fractal->transformCommon.startIterationsX
				&& aux.i < fractal->transformCommon.stopIterationsX)
		{
			// update actualScale for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
			if (fractal->transformCommon.functionEnabledMFalse)
				aux.actualScaleA = -vary;
			else
				aux.actualScaleA = aux.actualScaleA - vary;
		}
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

	/*CVector4 temp = fractal->transformCommon.offset0000;
	CVector4 temp2 = temp * temp;

	z.x += ((8.0 * temp.x * temp2.x) / ((z.x * z.x) + (4.0 * temp2.x)) - 2.0 * temp.x) * sign(z.x)
				 * fractal->transformCommon.scale1;
	z.y += ((8.0 * temp.y * temp2.y) / ((z.y * z.y) + (4.0 * temp2.y)) - 2.0 * temp.y) * sign(z.y)
				 * fractal->transformCommon.scale1;
	z.z += ((8.0 * temp.z * temp2.z) / ((z.z * z.z) + (4.0 * temp2.z)) - 2.0 * temp.z) * sign(z.z)
				 * fractal->transformCommon.scale1;
	z.w += ((8.0 * temp.w * temp2.w) / ((z.w * z.w) + (4.0 * temp2.w)) - 2.0 * temp.w) * sign(z.w)
				 * fractal->transformCommon.scale1;*/

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		aux.r = z.Length();
		aux.DE = aux.r * aux.DE * 16.0 * fractal->analyticDE.scale1
							 * sqrt(fractal->foldingIntPow.zFactor * fractal->foldingIntPow.zFactor + 2.0
											+ fractal->analyticDE.offset2)
							 / SQRT_3
						 + fractal->analyticDE.offset1;

		z = z * 2.0;
		double x2 = z.x * z.x;
		double y2 = z.y * z.y;
		double z2 = z.z * z.z;
		double temp = 1.0 - z2 / (x2 + y2);
		CVector4 zTemp;
		zTemp.x = (x2 - y2) * temp;
		zTemp.y = 2.0 * z.x * z.y * temp;
		zTemp.z = -2.0 * z.z * sqrt(x2 + y2);
		zTemp.w = z.w;
		z = zTemp;
		z.z *= fractal->foldingIntPow.zFactor;
	}

	// color updated v2.13 & mode2 v2.14
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd +=
					fractal->mandelbox.color.factor.x * (fabs(zCol.x) - fractal->mandelbox.color.factor4D.x);
			if (zCol.y != oldZ.y)
				colorAdd +=
					fractal->mandelbox.color.factor.y * (fabs(zCol.y) - fractal->mandelbox.color.factor4D.y);
			if (zCol.z != oldZ.z)
				colorAdd +=
					fractal->mandelbox.color.factor.z * (fabs(zCol.z) - fractal->mandelbox.color.factor4D.z);
			if (zCol.w != oldZ.w)
				colorAdd +=
					fractal->mandelbox.color.factor.z * (fabs(zCol.w) - fractal->mandelbox.color.factor4D.w);
			if (rrCol < fractal->transformCommon.maxR2d1)
			{
				if (rrCol < fractal->transformCommon.minR2p25)
					colorAdd +=
						fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
						+ fractal->mandelbox.color.factorSp2
								* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
				else
					colorAdd +=
						fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			}
		}
		else
		{
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor4D.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor4D.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor4D.z;
			if (zCol.w != oldZ.w) colorAdd += fractal->mandelbox.color.factor4D.w;
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1;
			else if (rrCol < fractal->transformCommon.maxR2d1)
				colorAdd += fractal->mandelbox.color.factorSp2;
		}
		aux.color += colorAdd;
	}
}
