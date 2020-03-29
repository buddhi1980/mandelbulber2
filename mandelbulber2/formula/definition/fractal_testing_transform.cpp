/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Based on a DarkBeam fold formula adapted by Knighty
 * MandalayBox  Fragmentarium /Examples/ Knighty Collection
 */

#include "all_fractal_definitions.h"

cFractalTestingTransform::cFractalTestingTransform() : cAbstractFractal()
{
	nameInComboBox = "Testing Transform";
	internalName = "testing_transform";
	internalID = fractal::testingTransform;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingTransform::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	// CVector4 c = aux.const_c;
	// double colorAdd = 0.0;

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
	// CVector4 oldZ = z;
	z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
				- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
	z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
				- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
	if (fractal->transformCommon.functionEnabledJFalse)
		z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
				- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
	// CVector4 zCol = z;

	// offset
	z += fractal->transformCommon.offsetF000;

	CVector4 p = z;
	double dd = aux.DE;
	double m = 0.0;
	CVector4 signs = z;
	signs.x = sign(z.x);
	signs.y = sign(z.y);
	signs.z = sign(z.z);
	// signs.w = sign(z.w);
	z = fabs(z);
	z -= fractal->transformCommon.offsetA000;
	double trr = z.Dot(z);
	double tp = min(max(1.0 / trr, 1.0), 1.0 / fractal->transformCommon.minR2p25);
	z += fractal->transformCommon.offsetA000;
	z *= tp;
	aux.DE *= tp;
	z *= signs;

	if (fractal->transformCommon.functionEnabledNFalse)
	{
		double rr = p.Dot(p);
		p += fractal->mandelbox.offset;
		m = min(max(1.0 / rr, 1.0), 1.0 / fractal->transformCommon.scale025);
		p *= m;
		dd *= m;
		/*if (r2 < minR)
		{
			p *= 1.0 / minR;
			dd *= 1.0 / minR;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
			//	aux.color += fractal->mandelbox.color.factorSp1;
			}
		}
		else if (r2 < 1.0)
		{
			p *= 1.0 / r2;
			dd *= 1.0 / r2;
			if (fractal->foldColor.auxColorEnabledFalse)
			{
			//	aux.color += fractal->mandelbox.color.factorSp2;
			}
		}*/
		p -= fractal->mandelbox.offset;

		z = p + (z - p) * fractal->transformCommon.scale1;
		aux.DE = dd + (aux.DE - dd) * fractal->transformCommon.scale1;

	}
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux.color += tp * fractal->mandelbox.color.factorSp1;
		aux.color += m * fractal->mandelbox.color.factorSp2;
	}
	// scale
	double useScale = 1.0;
	useScale = aux.actualScaleA + fractal->transformCommon.scale2;
	z *= useScale;
	aux.DE = aux.DE * fabs(useScale) + 1.0;

	if (fractal->transformCommon.functionEnabledKFalse
			&& aux.i >= fractal->transformCommon.startIterationsK
			&& aux.i < fractal->transformCommon.stopIterationsK)
	{
		// update actualScaleA for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
		aux.actualScaleA -= vary;
	}

	// rotation
	z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->analyticDE.enabledFalse)
		aux.DE =  aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux.color += tp * fractal->mandelbox.color.factorSp1;
		aux.color += m * fractal->mandelbox.color.factorSp2;
	}


	 // temp code
	CVector4 q = fabs(z);
	aux.dist = max(q.x, max(q.y, q.z));
	aux.dist = aux.dist / aux.DE;

}
