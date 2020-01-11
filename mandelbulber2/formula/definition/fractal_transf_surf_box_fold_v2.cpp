/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Surf Box Fold v2
 * based on DarkBeam's code "Mixed fold by Luca 2016".
 * @reference
 * http:// http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/
 * httpwww-shaperich-comproshred-elite-review
 * This formula contains aux.color,
 */

#include "all_fractal_definitions.h"

cFractalTransfSurfBoxFoldV2::cFractalTransfSurfBoxFoldV2() : cAbstractFractal()
{
	nameInComboBox = "T>Surf Box_Fold V2";
	internalName = "transf_surf_box_fold_v2";
	internalID = fractal::transfSurfBoxFoldV2;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSurfBoxFoldV2::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double colorAdd = 0.0;
	CVector4 oldZ = z;
	CVector4 foldMod = fractal->transformCommon.offset222;
	CVector4 fold = fractal->transformCommon.additionConstant111;
	CVector4 sg = z;
	sg.x = sign(z.x);
	sg.y = sign(z.y);
	sg.z = sign(z.z);

	CVector4 folder = z;
	CVector4 Tglad = z;

	Tglad.x = fabs(z.x + fold.x) - fabs(z.x - fold.x) - z.x;
	Tglad.y = fabs(z.y + fold.y) - fabs(z.y - fold.y) - z.y;

	folder.x = sg.x * (z.x - Tglad.x);
	folder.y = sg.y * (z.y - Tglad.y);
	folder.x = fabs(folder.x);
	folder.y = fabs(folder.y);
	folder.x = min(folder.x, foldMod.x);
	folder.y = min(folder.y, foldMod.y);
	z.x -= sg.x * folder.x;
	z.y -= sg.y * folder.y;

	if (fractal->transformCommon.functionEnabled)
	{
		Tglad.z = fabs(z.z + fold.z) - fabs(z.z - fold.z) - z.z;
		folder.z = sg.z * (z.z - Tglad.z);
		folder.z = fabs(folder.z);
		folder.z = min(folder.z, foldMod.z);
		z.z -= sg.z * folder.z;
	}

	// analyic DE tweak
	if (fractal->analyticDE.enabledFalse)
	{
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}
	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		CVector4 zCol = z;
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			if (zCol.x != oldZ.x)
				colorAdd += fractal->mandelbox.color.factor.x
										* (fabs(zCol.x) - fractal->transformCommon.additionConstant111.x);
			if (zCol.y != oldZ.y)
				colorAdd += fractal->mandelbox.color.factor.y
										* (fabs(zCol.y) - fractal->transformCommon.additionConstant111.y);
			if (zCol.z != oldZ.z)
				colorAdd += fractal->mandelbox.color.factor.z
										* (fabs(zCol.z) - fractal->transformCommon.additionConstant111.z);
		}
		else
		{
			if (zCol.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (zCol.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (zCol.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
		}
		aux.color += colorAdd;
	}
}
