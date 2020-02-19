/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Based on a DarkBeam fold formula adapted by Knighty
 * http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/'new'-fractal-type-mandalay/msg81348/#msg81348
 */

#include "all_fractal_definitions.h"

cFractalMandalayBoxV2::cFractalMandalayBoxV2() : cAbstractFractal()
{
	nameInComboBox = "Mandalay Box V2";
	internalName = "mandalay_box_v2";
	internalID = fractal::mandalayBoxV2;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandalayBoxV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double colorAdd = 0.0;
	double rrCol = 0.0;

	// tglad fold
	if (fractal->transformCommon.functionEnabledAFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabled)
		{
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		}
	}

	double signX = sign(z.x);
	double signY = sign(z.y);
	double signZ = sign(z.z);

	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP)
	{
		if (fractal->transformCommon.functionEnabledAx) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledAy) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledAz) z.z = fabs(z.z);
	}
	else
	{
		z = fabs(z);
	}

	CVector4 fo = fractal->transformCommon.additionConstant0555;
	CVector4 g = fractal->transformCommon.offsetA000;
	CVector4 p = z;
	CVector4 q = z;

	double t1, t2, v, v1;

	if(p.z > p.y) swap(p.y, p.z);
	t1 = p.x - 2.0 * fo.x;
	t2 = p.y - 4.0 * fo.x;
	v = max(fabs(t1 + fo.x) - fo.x, t2);
	v1 = max(t1 - g.x, p.y);
	v = min(v, v1);
	q.x = min(v, p.x);

	if (!fractal->transformCommon.functionEnabledSwFalse) p = z;
	else p = q;

	if(p.x > p.z) swap(p.z, p.x);
	t1 = p.y - 2.0 * fo.y;
	t2 = p.z - 4.0 * fo.y;
	v = max(fabs(t1 + fo.y) - fo.y, t2);
	v1 = max(t1 - g.y, p.z);
	v = min(v, v1);
	q.y = min(v, p.y);

	if (!fractal->transformCommon.functionEnabledSwFalse) p = z;
	else p = q;

	if(p.y > p.x) swap(p.x, p.y);
	t1 = p.z - 2.0 * fo.z;
	t2 = p.x - 4.0 * fo.z;
	v = max(fabs(t1 + fo.z) - fo.z, t2);
	v1 = max(t1 - g.z, p.x);
	v = min(v, v1);
	q.z = min(v, p.z);

	z = q;

	z.x *= signX;
	z.y *= signY;
	z.z *= signZ;

	// spherical fold
	double useScale = 1.0;
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{

		double rr = z.Dot(z);
		rrCol = rr;
		if (rr < fractal->transformCommon.minR2p25)
		{
			double tglad_factor1 = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
			z *= tglad_factor1;
			aux.DE *= tglad_factor1;
		}
		else if (rr < fractal->transformCommon.maxR2d1)
		{
			double tglad_factor2 = fractal->transformCommon.maxR2d1 / rr;
			z *= tglad_factor2;
			aux.DE *= tglad_factor2;
		}
	}

	// scale
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
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	if (fractal->analyticDE.enabledFalse)
		aux.DE =  aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (rrCol < fractal->transformCommon.maxR2d1)
		{
			colorAdd += fractal->mandelbox.color.factorSp2 * (fractal->transformCommon.maxR2d1 - rrCol);
			if (rrCol < fractal->transformCommon.minR2p25)
				colorAdd += fractal->mandelbox.color.factorSp1 * (fractal->transformCommon.minR2p25 - rrCol)
										+ fractal->mandelbox.color.factorSp2
												* (fractal->transformCommon.maxR2d1 - fractal->transformCommon.minR2p25);
		}
		aux.color += colorAdd;
	}

	 // temp code
	p = fabs(z);
	aux.dist = max(p.x, max(p.y, p.z));
	aux.dist = aux.dist / aux.DE;

}
