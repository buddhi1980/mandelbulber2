/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical fold MBox
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfAddCpixelSphereFold::cFractalTransfAddCpixelSphereFold() : cAbstractFractal()
{
	nameInComboBox = "T>Add Cpixel Sphere Fold";
	internalName = "transf_add_cpixel_sphere_fold";
	internalID = fractal::transfAddCpixelSphereFold;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAddCpixelSphereFold::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	CVector4 cv = aux.const_c;
	double rr = cv.Dot(cv);
	cv += fractal->mandelbox.offset;

	if (rr < fractal->transformCommon.minR2p25)
	{
		cv *= fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		cv *= fractal->transformCommon.maxR2d1 / rr;
	}

	if (fractal->transformCommon.functionEnabledBFalse && rr >= fractal->transformCommon.maxR2d1)
		cv = cv * (1.0 - (fractal->transformCommon.maxR2d1 - rr) / rr);

	if (fractal->transformCommon.functionEnabledCFalse && rr >= fractal->transformCommon.maxR2d1)
	{
		cv = cv * (1.0 - (rr - fractal->transformCommon.maxR2d1) / fractal->transformCommon.scale1);
	}
	if (fractal->transformCommon.functionEnabledDFalse && rr >= fractal->transformCommon.maxR2d1)
		cv = cv * fractal->transformCommon.maxR2d1;


	if (fractal->transformCommon.functionEnabledAFalse)
	{
		cv.x = sign(cv.x) * min(fabs(cv.x), fractal->transformCommon.scaleA1);
		cv.y = sign(cv.y) * min(fabs(cv.y), fractal->transformCommon.scaleA1);
		cv.z = sign(cv.z) * min(fabs(cv.z), fractal->transformCommon.scaleA1);
	}

	cv -= fractal->mandelbox.offset;

	z += cv * fractal->transformCommon.constantMultiplier111;

	// Analytic DE tweak

	if (fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fractal->analyticDE.scale1
								+ fractal->analyticDE.offset0;
	// aux->color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux.color += fabs(cv.x * cv.y) * fractal->foldColor.difs0000.x;
		aux.color += cv.y * fractal->foldColor.difs0000.y;
		aux.color += cv.z * fractal->foldColor.difs0000.z;
	}


}
