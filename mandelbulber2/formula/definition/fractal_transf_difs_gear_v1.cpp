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

	if (fractal->transformCommon.rotation2EnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	double angle = M_PI_2x / (fractal->transformCommon.int16);
	double sector = round(atan2(z.x , z.y) / angle) + fractal->transformCommon.intA *1.0;
	CVector4 zc = z;
	double an = sector * angle;
	double sinan = sin(an);
	double cosan = cos(an);
	double temp = zc.x;

	zc.x = zc.x * cosan - zc.y * sinan;
	zc.y = temp * sinan + zc.y * cosan;
	zc.y -= fractal->transformCommon.offset1;
	zc.z -= fractal->transformCommon.offset0;

	double widthX = fractal->transformCommon.offset01;
	double lengthY = fractal->transformCommon.offset02;
	double heightZ = fractal->transformCommon.offset05;

	// chevron
	if (fractal->transformCommon.functionEnabledFalse)
		 zc.x -= fractal->transformCommon.scale0 * fabs(zc.y);

	// curve
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		double absZZ = zc.z * zc.z * fractal->transformCommon.scaleA0;
		widthX += absZZ;
		lengthY += absZZ;
	}

	// pyramid
	if (fractal->transformCommon.functionEnabledBFalse)
	{
		double subZ = fractal->transformCommon.scaleB0 * zc.z;
		widthX -= subZ;
		lengthY -= subZ;
	}

	// star
	if (fractal->transformCommon.functionEnabledCFalse)
				widthX -= (fractal->transformCommon.scaleC0 * zc.y);

	if (fractal->transformCommon.functionEnabledDFalse)
				zc.x -= fractal->transformCommon.scale05 * zc.y;

	zc.x = fabs(zc.x) - widthX;
	zc.y = fabs(zc.y) - lengthY;
	zc.z = fabs(zc.z) - heightZ;

	if (fractal->transformCommon.functionEnabledEFalse)
				zc.x *= -fractal->transformCommon.scaleE1 * zc.y;

	if (fractal->transformCommon.functionEnabledFFalse)
				zc.x += fractal->transformCommon.scaleF1 * zc.y;

	zc.x = max(zc.x, 0.0);
	zc.y = max(zc.y, 0.0);
	zc.z = max(zc.z, 0.0);
	double zcd = zc.Length();


	double sdTor = fabs(sqrt(z.x * z.x + z.y *z.y) - fractal->transformCommon.offsetA1
			+ fractal->transformCommon.offsetR0)
			- fractal->transformCommon.offsetR0;
	sdTor = max (sdTor , fabs(z.z) - fractal->transformCommon.offsetA05);

	double d = min(zcd, sdTor) - fractal->transformCommon.offset0005;

	aux.dist = min(aux.dist, d / (aux.DE + 1.0));



	/*	if (!fractal->transformCommon.functionEnabledEFalse)
			aux.dist = min(aux.dist, zcd / (aux.DE + 1.0));
		else
			aux.dist = min(aux.dist, zcd / (aux.DE + 1.0)) - fractal->transformCommon.offsetB0;
	}*/
}
