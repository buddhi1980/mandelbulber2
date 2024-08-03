/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical invert v4
 * from M3D. Baded on formula by Luca GN 2011, updated May 2012.
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalInvV4::cFractalTransfSphericalInvV4() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Invert V4";
	internalName = "transf_spherical_inv_v4";
	internalID = fractal::transfSphericalInvV4;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalInvV4::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	double RR = 1.0;
	z += fractal->mandelbox.offset;
	z *= fractal->transformCommon.scale;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale) + 1.0;

	if (!fractal->transformCommon.functionEnabledyFalse) RR = z.Dot(z);
	else RR = oldZ.Dot(oldZ);
	double mde = RR;

	if (!fractal->transformCommon.functionEnabledzFalse)
	{
		mde = 1.0 / mde;
		z *= mde;
		aux.DE *= mde;
	}
	else // conditional
	{
		z += fractal->transformCommon.offset000;
		double mn = 0.0;
		if (!fractal->transformCommon.functionEnabledxFalse) mn = fractal->transformCommon.minR0;
		else mn = 2.0 * fractal->transformCommon.minR0 - RR;


		if (fractal->transformCommon.functionEnabledAFalse) // Mode 2
		{
			if (RR < fractal->mandelbox.foldingSphericalFixed)
				mde = mn;
		}

		if (fractal->transformCommon.functionEnabledBFalse) // Mode 3
		{ // if RR < max && RR > min => mn else RR
			if (RR < fractal->mandelbox.foldingSphericalFixed && RR > fractal->transformCommon.minR0)
			//
				mde = mn;
		}


		if (fractal->transformCommon.functionEnabledCFalse) // Mode 4
		{ // if RR > maxRR => maxRR, if RR < minRR => mn, else RR
			if (RR > fractal->mandelbox.foldingSphericalFixed) mde = fractal->mandelbox.foldingSphericalFixed;
			if (RR < fractal->transformCommon.minR0) mde = mn;
		}

		if (fractal->transformCommon.functionEnabledDFalse) // Mode 5
		{
			if (RR > fractal->mandelbox.foldingSphericalFixed)
				mde = mn;
		}

		if (fractal->transformCommon.functionEnabledFalse) // Mode 1 minR0
		{ // if (RR < minRR) else RR
			if (RR < fractal->transformCommon.minR0) mde = 1.0 / mn;
			else if (RR < fractal->mandelbox.foldingSphericalFixed) mde = fractal->mandelbox.foldingSphericalFixed;
		}
		//if (fractal->transformCommon.functionEnabledEFalse) // Mode 5
		/*{
			if (RR < fractal->transformCommon.minR0) mde = 1.0;
			if (RR < fractal->mandelbox.foldingSphericalFixed && RR > fractal->transformCommon.minR0)
				mde = fractal->transformCommon.minR0;
			if (RR < fractal->mandelbox.foldingSphericalFixed && RR < fractal->transformCommon.minR0)
				mde = mn;
		}*/



		mde = 1.0 / mde;
		z *= mde;
		aux.DE *= fabs(mde);
		z -= fractal->transformCommon.offset000;

	}
	z -= fractal->mandelbox.offset + fractal->transformCommon.additionConstant000;

	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}

	// color added v2.32
	if (fractal->foldColor.auxColorEnabledFalse && aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addCol = 0.0f;
		addCol += fractal->foldColor.difs0000.x * mde;
		if (RR > fractal->mandelbox.foldingSphericalFixed) addCol += fractal->foldColor.difs0000.y;
		if (RR < fractal->transformCommon.minR0) addCol += fractal->foldColor.difs0000.z;

	//	aux.color = addCol;

		aux.color += addCol;
	}
}
