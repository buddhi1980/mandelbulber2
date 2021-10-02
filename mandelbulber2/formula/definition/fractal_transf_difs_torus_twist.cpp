/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsTorusV3Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSTorusTwist::cFractalTransfDIFSTorusTwist() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Torus Twist";
	internalName = "transf_difs_torus_twist";
	internalID = fractal::transfDIFSTorusTwist;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSTorusTwist::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	if (fractal->transformCommon.functionEnabledxFalse) z.x = -fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = -fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = -fabs(z.z);

	CVector4 zc = z;

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		swap(zc.x, zc.z);
	}

	double ang = atan2(zc.y, -zc.x) / M_PI_2x;

	double tp;
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		double Voff = fractal->transformCommon.scaleA2;
		tp = zc.z - 2.0 * Voff * ang + Voff;
		zc.z = tp - 2.0 * Voff * floor(tp / (2.0 * Voff)) - Voff;
	}

	zc.y = sqrt(zc.x * zc.x + zc.y * zc.y)
			- fractal->transformCommon.radius1;

	//if (fractal->transformCommon.functionEnabledBFalse)
	//{
		//tp = fractal->transformCommon.scale2 * ang;
		//zc.x = ang - 2.0 * floor(ang / 2.0) - 1.0;
	//}

	ang *= fractal->transformCommon.int6 * M_PI_2;
	double cosA = cos(ang);
	double sinB = sin(ang);
	double temp = zc.z;
	zc.z = zc.y * cosA + zc.z * sinB;
	zc.y = temp * cosA + zc.y * -sinB;


	double lenY = fractal->transformCommon.offsetA0;
	double lenZ = fractal->transformCommon.offsetB0;
	CVector4 absZ = fabs(zc);


	if (fractal->transformCommon.functionEnabledPFalse)
		lenY += absZ.x * fractal->transformCommon.scaleC0;


	if (fractal->transformCommon.functionEnabledNFalse)
		lenY += absZ.z * fractal->transformCommon.scaleA0;
	if (fractal->transformCommon.functionEnabledMFalse)
		lenZ += absZ.z * fractal->transformCommon.scale0;

	if (fractal->transformCommon.functionEnabledOFalse)
		lenZ += absZ.y * fractal->transformCommon.scaleB0;

	CVector4 d = fabs(zc);

	d.x = 0.0;
	d.y -= fractal->transformCommon.offset01;
	d.z -= fractal->transformCommon.offsetp1;

	d.y = max(d.y - lenY, 0.0);
	d.z = max(d.z - lenZ, 0.0);
	aux.DE0 = d.Length() - fractal->transformCommon.offset0005;

	aux.dist = min(aux.dist, aux.DE0 / (aux.DE + fractal->analyticDE.offset0));

	if (fractal->transformCommon.functionEnabledXFalse)
		z = zc;
}
