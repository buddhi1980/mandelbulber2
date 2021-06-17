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
	CVector4 signs ;
	signs.x = sign(aux.const_c.x);
	signs.y = sign(aux.const_c.y);
	signs.z = sign(aux.const_c.z);
	signs.w = 0.0;



	CVector4 offset = fractal->transformCommon.offset000;
	if (!fractal->transformCommon.functionEnabledEFalse) offset *= signs;


	CVector4 temp;
	if (!fractal->transformCommon.functionEnabledAFalse) temp = aux.const_c - offset;
	else temp = z - offset;

	double r =  temp.Dot(temp);

	if (fractal->transformCommon.functionEnabledBFalse) r = sqrt(r);


	//CVector4 offset1 = fractal->transformCommon.offset111;
	//if (fractal->transformCommon.functionEnabledCFalse)	offset1 *=	sign(c);
	//t = offset1 + t;

	CVector4 offset1 = fractal->transformCommon.offsetA000;
	if (fractal->transformCommon.functionEnabledCFalse)	offset1  *= signs;

	if (r > fractal->transformCommon.radius1)
	{
		temp = (temp * (1.0 - fractal->transformCommon.radius1 / r));
		//if (fractal->transformCommon.functionEnabledCFalse)	temp *= sign(z);
		z += temp + offset1;
		//	z = fabs(z);
	}
	else
	{
		if (fractal->transformCommon.functionEnabledDFalse)
			z += fractal->transformCommon.scale1 * temp / (fractal->transformCommon.radius1 / r - 1.0);
	}





	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
