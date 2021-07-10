/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * amazing surf Mod4 based on Mandelbulber3D. Formula proposed by Kali, with features added by
 * DarkBeam
 * This formula has a c.x c.y SWAP
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalAmazingIfs::cFractalAmazingIfs() : cAbstractFractal()
{
	nameInComboBox = "Amazing IFS";
	internalName = "amazing_ifs";
	internalID = fractal::amazingIfs;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalAmazingIfs::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorDist = aux.DE;

	// sphere inversion
	if (fractal->transformCommon.sphereInversionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterations1)
	{
		z += fractal->transformCommon.offset000;
		double rr = z.Dot(z);
		z *= fractal->transformCommon.scaleG1 / rr;
		aux.DE *= (fractal->transformCommon.scaleG1 / rr);
		z += fractal->transformCommon.additionConstant000 - fractal->transformCommon.offset000;
		z *= fractal->transformCommon.scaleA1;
		aux.DE *= fractal->transformCommon.scaleA1;
	}

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAzFalse) z.z = fabs(z.z);
	}
	z += fractal->transformCommon.offsetA000;

	// polyfold
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		z.x = fabs(z.x);
		double psi = M_PI / fractal->transformCommon.int6;
		psi = fabs(fmod(atan(z.y / z.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(z.x * z.x + z.y * z.y);
		z.x = cos(psi) * len;
		z.y = sin(psi) * len;
	}


	CVector4 oldZ = z;

	if (fractal->transformCommon.functionEnabledCx)
			z.x = fabs(z.x + fractal->transformCommon.additionConstant0555.x)
				- fabs(z.x - fractal->transformCommon.additionConstant0555.x) - z.x;
	if (fractal->transformCommon.functionEnabledCy)
			z.y = fabs(z.y + fractal->transformCommon.additionConstant0555.y)
				- fabs(z.y - fractal->transformCommon.additionConstant0555.y) - z.y;
	if (fractal->transformCommon.functionEnabledCzFalse)
			z.z = fabs(z.z + fractal->transformCommon.additionConstant0555.z)
				- fabs(z.z - fractal->transformCommon.additionConstant0555.z) - z.z;
	CVector4 zCol = z;

	if (fractal->transformCommon.functionEnabledBxFalse)
		z.x = fractal->transformCommon.additionConstantA000.x - fabs(z.x);
	if (fractal->transformCommon.functionEnabledByFalse)
		z.y = fractal->transformCommon.additionConstantA000.y - fabs(z.y);
	if (fractal->transformCommon.functionEnabledBzFalse)
		z.z = fractal->transformCommon.additionConstantA000.z - fabs(z.z);



	double rr = z.Dot(z);
	double rrCol = rr;
	double MinRR = fractal->transformCommon.minR2p25;
	double dividend = rr < MinRR ? MinRR : min(rr, 1.0);

	// scale
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsB
			&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		useScale = (aux.actualScaleA + fractal->transformCommon.scale1) / dividend;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + fractal->analyticDE.offset0;
		if (fractal->transformCommon.functionEnabledKFalse)
		{
			// update actualScaleA for next iteration
			double vary = fractal->transformCommon.scaleVary0
										* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
			aux.actualScaleA -= vary;
		}
	}

	if (fractal->transformCommon.rotation2EnabledFalse)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}



	z = fractal->transformCommon.rotationMatrix2.RotateVector(z);


	if (fractal->transformCommon.functionEnabledGFalse)
	{
		aux.DE0 = z.Length() / aux.DE;
		if (!fractal->analyticDE.enabledFalse
				&& aux.i >= fractal->transformCommon.startIterationsC
				&& aux.i < fractal->transformCommon.stopIterationsC)
					aux.DE0 = min(aux.dist, aux.DE0);
		aux.dist = aux.DE0;
	}


	// aux.color
	if (fractal->foldColor.auxColorEnabled)
	{
		double colorAdd = 0.0;
		if (fractal->foldColor.auxColorEnabledA)
			if (colorDist != aux.DE) colorAdd += fractal->foldColor.difs1;

		if (fractal->foldColor.auxColorEnabledFalse)
		{
		if (zCol.x != oldZ.x)
			colorAdd += fractal->foldColor.difs0000.x
									* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
		if (zCol.y != oldZ.y)
			colorAdd += fractal->foldColor.difs0000.y
									* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
		if (zCol.z != oldZ.z)
			colorAdd += fractal->foldColor.difs0000.z
									* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);
		if (rrCol > fractal->transformCommon.minR2p25)
			colorAdd +=
				fractal->foldColor.difs0000.w * (rrCol - fractal->transformCommon.minR2p25) / 100.0;
		}
		aux.color += colorAdd;
	}
}
