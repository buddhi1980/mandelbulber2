/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsGearV1Iteration based on example by Inigo Quilez,
 * https://www.iquilezles.org/
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSGearV1::cFractalTransfDIFSGearV1() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Gear V1";
	internalName = "transf_difs_gear_v1";
	internalID = fractal::transfDIFSGearV1;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSGearV1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

		//double d = 100.0;
	double angle = M_PI_2x /  (fractal->transformCommon.int8X);
	double sector = round(atan2(z.x , z.y) / angle);
	CVector4 zc = z;
	double an = sector * angle;
	double sinan = sin(an);
	double cosan = cos(an);
	double temp = zc.x;

	zc.x = zc.x * cosan - zc.y * sinan;
	zc.y = temp * sinan + zc.y * cosan;

	zc.y -= fractal->transformCommon.offset1;
		zc.z -= fractal->transformCommon.offset0;
		// zc.x -= fractal->transformCommon.scaleB1 * fabs(zc.y);
	CVector4 boxSize = fractal->transformCommon.additionConstant111;
		// zc.x -= fractal->transformCommon.scaleB1 * fabs(zc.y);
	// curve
	if (fractal->transformCommon.functionEnabledTFalse)
	{
		double absZZ = zc.z * zc.z * fractal->transformCommon.scale0;
		boxSize.x += absZZ;
		boxSize.y += absZZ;
	}

	// pyramid
	if (fractal->transformCommon.functionEnabledMFalse)
	{
		double subZ = fractal->transformCommon.scaleA0 * zc.z;
		boxSize.x -= subZ;
		boxSize.y -= subZ;
	}

	// star
	if (fractal->transformCommon.functionEnabledNFalse)
				boxSize.x -= (fractal->transformCommon.scaleB0 * zc.y);

	if (fractal->transformCommon.functionEnabledOFalse)
				zc.x -= fractal->transformCommon.scale05 * zc.y;

	zc = fabs(zc) - boxSize;


	if (fractal->transformCommon.functionEnabledPFalse)
				zc.x *= -fractal->transformCommon.scaleD1 * zc.y;

	if (fractal->transformCommon.functionEnabledRFalse)
				zc.x += fractal->transformCommon.scaleE1 * zc.y;

	zc.x = max(zc.x, 0.0);
	zc.y = max(zc.y, 0.0);
	zc.z = max(zc.z, 0.0);
	double zcd = zc.Length();


		double sdTor = fabs(sqrt(z.x * z.x + z.y *z.y) - fractal->transformCommon.offsetA1
				+ fractal->transformCommon.offsetR0)
				- fractal->transformCommon.offsetR0;
		sdTor= max (sdTor , fabs(z.z) - fractal->transformCommon.offsetA05);

		double d = min(zcd, sdTor) - fractal->transformCommon.offset0005;

	aux.dist = min(aux.dist, d / (aux.DE + 1.0));



	/*	if (!fractal->transformCommon.functionEnabledEFalse)
			aux.dist = min(aux.dist, zcd / (aux.DE + 1.0));
		else
			aux.dist = min(aux.dist, zcd / (aux.DE + 1.0)) - fractal->transformCommon.offsetB0;
	}*/
}
