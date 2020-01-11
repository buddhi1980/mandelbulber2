/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Surf Box Fold v2 4d
 * based on DarkBeam's code "Mixed fold by Luca 2016".
 * @reference
 * http:// http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/
 * httpwww-shaperich-comproshred-elite-review
 * This formula contains aux.color,
 */

#include "all_fractal_definitions.h"

cFractalTransfSurfBoxFoldV24d::cFractalTransfSurfBoxFoldV24d() : cAbstractFractal()
{
	nameInComboBox = "T>Surf Box_Fold V2 4D";
	internalName = "transf_surf_box_fold_v24d";
	internalID = fractal::transfSurfBoxFoldV24d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSurfBoxFoldV24d::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	double colorAdd = 0.0;
	CVector4 oldZ = z;
	CVector4 foldMod = fractal->transformCommon.offset2222;
	CVector4 fold = fractal->transformCommon.offset1111;
	CVector4 sg = z;
	sg.x = sign(z.x);
	sg.y = sign(z.y);
	sg.z = sign(z.z);
	sg.w = sign(z.w);

	CVector4 folder = z;
	CVector4 Tglad = z;
	Tglad = fabs(z + fold) - fabs(z - fold) - z;
	folder = sg * (z - Tglad);
	folder = fabs(folder);

	folder.x = min(folder.x, foldMod.x);
	folder.y = min(folder.y, foldMod.y);
	folder.z = min(folder.z, foldMod.z);
	folder.w = min(folder.w, foldMod.w);

	z -= sg * folder;

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
				colorAdd += fractal->mandelbox.color.factor4D.x
										* (fabs(zCol.x) - fractal->transformCommon.offset1111.x);
			if (zCol.y != oldZ.y)
				colorAdd += fractal->mandelbox.color.factor4D.y
										* (fabs(zCol.y) - fractal->transformCommon.offset1111.y);
			if (zCol.z != oldZ.z)
				colorAdd += fractal->mandelbox.color.factor4D.z
										* (fabs(zCol.z) - fractal->transformCommon.offset1111.z);
			if (zCol.w != oldZ.w)
				colorAdd += fractal->mandelbox.color.factor4D.w
										* (fabs(zCol.w) - fractal->transformCommon.offset1111.w);
		}
		else
		{
			if (zCol.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x;
			if (zCol.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y;
			if (zCol.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z;
			if (zCol.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w;
		}
		aux.color += colorAdd;
	}
}
