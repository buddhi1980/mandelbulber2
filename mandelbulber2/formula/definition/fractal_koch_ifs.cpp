/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * KochIfs
 * Based on Knighty's Kaleidoscopic IFS 3D Fractals, described here:
 * http://www.fractalforums.com/3d-fractal-generation/kaleidoscopic-%28escape-time-ifs%29/
 */

#include "all_fractal_definitions.h"

cFractalKochIfs::cFractalKochIfs() : cAbstractFractal()
{
	nameInComboBox = "Koch IFS";
	internalName = "koch_ifs";
	internalID = fractal::kochIfs;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalKochIfs::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp = 0.0;
	double colAdd = 0.0;

	if (fractal->transformCommon.functionEnabledAx
		&& aux.i >= fractal->transformCommon.startIterationsCx
		&& aux.i < fractal->transformCommon.stopIterationsCx)
			z.x = fabs(z.x);

	if (fractal->transformCommon.functionEnabledAy
		&& aux.i >= fractal->transformCommon.startIterationsCy
		&& aux.i < fractal->transformCommon.stopIterationsCy)
			z.y = fabs(z.y);

	if (fractal->transformCommon.functionEnabledAzFalse
		&& aux.i >= fractal->transformCommon.startIterationsCz
		&& aux.i < fractal->transformCommon.stopIterationsCz)
			z.z = fabs(z.z);

	if (fractal->transformCommon.functionEnabledCx
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD)
	{
		if (z.y > z.x)
		{
			temp = z.x;
			z.x = z.y;
			z.y = temp;
			colAdd += fractal->foldColor.difs0000.x;
		}
	}

	if (fractal->transformCommon.functionEnabledCFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		z = z - fractal->transformCommon.offsetA000;
	}

	// folds
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterations
			&& aux.i < fractal->transformCommon.stopIterations)
	{
		// diagonal2
		if (fractal->transformCommon.functionEnabledCxFalse)
			if (z.x > z.y) swap(z.x, z.y);
		// polyfold
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			z.x = fabs(z.x);
			double psi = M_PI / fractal->transformCommon.int8X;
			psi = fabs(fmod(atan2(z.y, z.x) + psi, 2.0 * psi) - psi);
			double len = sqrt(z.x * z.x + z.y * z.y);
			z.x = cos(psi) * len;
			z.y = sin(psi) * len;
		}
		// abs offsets
		if (fractal->transformCommon.functionEnabledMFalse)
		{
			double xOffset = fractal->transformCommon.offsetC0;
			if (z.x < xOffset) z.x = fabs(z.x - xOffset) + xOffset;
		}
		if (fractal->transformCommon.functionEnabledDFalse)
		{
			double yOffset = fractal->transformCommon.offsetD0;
			if (z.y < yOffset) z.y = fabs(z.y - yOffset) + yOffset;
		}
	}

	if (fractal->transformCommon.functionEnabledGFalse
			&& aux.i >= fractal->transformCommon.startIterationsG
			&& aux.i < fractal->transformCommon.stopIterationsG)
	{
		z += fractal->mandelbox.offset;
		double rr = z.Dot(z);

		if (rr < fractal->transformCommon.minR0)
		{
			z *= fractal->transformCommon.maxMinR0factor;
			aux.DE *= fractal->transformCommon.maxMinR0factor;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
		}
		z -= fractal->mandelbox.offset;
	}

	double YOff = FRAC_1_3 * fractal->transformCommon.scale1;
	z.y = YOff - fabs(z.y - YOff);

	double third = FRAC_1_3;
	if (fractal->transformCommon.functionEnabledNFalse)
		third = -FRAC_1_3;

	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z.x += third;
		if (z.z > z.x)
		{
			temp = z.x;
			z.x = z.z;
			z.z = temp;
			colAdd += fractal->foldColor.difs0000.y;
		}
		z.x -= third;
	}

	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		z.x -= third;
		if (z.z > z.x)
		{
			temp = z.x;
			z.x = z.z;
			z.z = temp;
			colAdd += fractal->foldColor.difs0000.z;
		}
		z.x += third;
	}

	if (aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP)
		z = z - fractal->transformCommon.offset100;

	CVector4 Offset = fractal->transformCommon.additionConstantNeg100;
	z -= Offset;

	double useScale = fractal->transformCommon.scale2;
	if (fractal->transformCommon.functionEnabledKFalse)
	{
		useScale += aux.actualScaleA;
		// update actualScale for next iteration
		double vary = fractal->transformCommon.scaleVary0
								* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
		aux.actualScaleA = -vary;
	}
	aux.DE = aux.DE * fabs(useScale) + fractal->analyticDE.offset0;
	z *= useScale;

	if (fractal->transformCommon.functionEnabledHFalse
			&& aux.i >= fractal->transformCommon.startIterationsH
			&& aux.i < fractal->transformCommon.stopIterationsH)
		z.z = -z.z;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	z += Offset;

	//aux.dist
	double d = z.Length();
	d = d / aux.DE;
	aux.dist = min(d, aux.dist);

	// aux->color
	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		aux.color += colAdd + fractal->foldColor.difs0000.w;
	}

}
