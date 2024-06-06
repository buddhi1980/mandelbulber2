/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Surf Box Fold
 * allows different fold types for each axis.
 * amazing surface if one axis has no folds enabled.
 * made from various folds found in the posts at fractalforums.com
 * This formula contains aux.color, analytic aux.DE and aux.r-dz
 */

#include "all_fractal_definitions.h"

cFractalTransfSurfBoxFold::cFractalTransfSurfBoxFold() : cAbstractFractal()
{
	nameInComboBox = "T>Surf Box_Fold";
	internalName = "transf_surf_box_fold";
	internalID = fractal::transfSurfBoxFold;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSurfBoxFold::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z; // aux.lastZ
	if (fractal->surfBox.enabledX1)
	{
		z.x =
			fabs(z.x + fractal->surfBox.offset1A111.x) - fabs(z.x - fractal->surfBox.offset1A111.x) - z.x;
	}
	if (fractal->surfBox.enabledX2False)
	{ // z = fold - fabs( fabs(z) - fold), Tglad fold
		z.x = fractal->surfBox.offset1B111.x - fabs(fabs(z.x) - fractal->surfBox.offset1B111.x);
	}
	if (fractal->surfBox.enabledX3False)
	{
		z.x = fabs(z.x + fractal->surfBox.offset2A111.x);
	}
	if (fractal->surfBox.enabledX4False)
	{ // if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
		if (z.x > fractal->surfBox.offset2B111.x)
		{
			z.x = fractal->surfBox.offset1A222.x - z.x;
			// aux.color += fractal->mandelbox.color.factor.x;
		}
		else if (z.x < -fractal->surfBox.offset2B111.x)
		{
			z.x = -fractal->surfBox.offset1A222.x - z.x;
			// aux.color += fractal->mandelbox.color.factor.x;
		}
	}
	if (fractal->surfBox.enabledX5False)
	{ // z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
		z.x = fractal->surfBox.offset1B222.x
					- fabs(fabs(z.x + fractal->surfBox.offset3A111.x) - fractal->surfBox.offset1B222.x)
					- fractal->surfBox.offset3A111.x;
	}

	if (fractal->surfBox.enabledY1)
	{
		z.y =
			fabs(z.y + fractal->surfBox.offset1A111.y) - fabs(z.y - fractal->surfBox.offset1A111.y) - z.y;
	}
	if (fractal->surfBox.enabledY2False)
	{ // z = fold - fabs( fabs(z) - fold)
		z.y = fractal->surfBox.offset1B111.y - fabs(fabs(z.y) - fractal->surfBox.offset1B111.y);
	}
	if (fractal->surfBox.enabledY3False)
	{
		z.y = fabs(z.y + fractal->surfBox.offset2A111.y);
	}
	if (fractal->surfBox.enabledY4False)
	{ // if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
		if (z.y > fractal->surfBox.offset2B111.y)
		{
			z.y = fractal->surfBox.offset1A222.y - z.y;
			// aux.color += fractal->mandelbox.color.factor.y;
		}
		else if (z.y < -fractal->surfBox.offset2B111.y)
		{
			z.y = -fractal->surfBox.offset1A222.y - z.y;
			// aux.color += fractal->mandelbox.color.factor.y;
		}
	}
	if (fractal->surfBox.enabledY5False)
	{ // z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)
		z.y = fractal->surfBox.offset1B222.y
					- fabs(fabs(z.y + fractal->surfBox.offset3A111.y) - fractal->surfBox.offset1B222.y)
					- fractal->surfBox.offset3A111.y;
	}

	if (fractal->surfBox.enabledZ1)
	{
		z.z =
			fabs(z.z + fractal->surfBox.offset1A111.z) - fabs(z.z - fractal->surfBox.offset1A111.z) - z.z;
	}
	if (fractal->surfBox.enabledZ2False)
	{ // z = fold - fabs( fabs(z) - fold)
		z.z = fractal->surfBox.offset1B111.z - fabs(fabs(z.z) - fractal->surfBox.offset1B111.z);
	}
	if (fractal->surfBox.enabledZ3False)
	{
		z.z = fabs(z.z + fractal->surfBox.offset2A111.z);
	}

	if (fractal->surfBox.enabledZ4False)
	{ // if z > limit) z =  Value -z,   else if z < limit) z = - Value - z,
		double limit = fractal->surfBox.offset2B111.z;
		double value = fractal->surfBox.offset1A222.z;
		double zLimit = limit * fractal->surfBox.scale1Z1;
		double zValue = value * fractal->surfBox.scale1Z1;
		if (z.z > zLimit)
		{
			z.z = zValue - z.z;
			// aux.color += fractal->mandelbox.color.factor.z;
		}
		else if (z.z < -zLimit)
		{
			z.z = -zValue - z.z;
			// aux.color += fractal->mandelbox.color.factor.z;
		}
	}

	if (fractal->surfBox.enabledZ5False)
	{ // z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)// DarkBeam's Abox mod1 fold
		z.z = fractal->surfBox.offset1B222.z
					- fabs(fabs(z.z + fractal->surfBox.offset3A111.z) - fractal->surfBox.offset1B222.z)
					- fractal->surfBox.offset3A111.z;
	}
	aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0; // tweak

	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
				&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double colorAdd = 0.0;
		CVector4 zCol = fabs(z - oldZ);

		if (zCol.x > 0.0f)
			colorAdd += fractal->foldColor.difs0000.x * zCol.x;
		if (zCol.y > 0.0f)
			colorAdd += fractal->foldColor.difs0000.y * zCol.y;
		if (zCol.z > 0.0f)
			colorAdd += fractal->foldColor.difs0000.z * zCol.z;

		aux.color += colorAdd;
	}
}
