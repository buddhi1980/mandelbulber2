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
	double tp;

	double ang = atan2(zc.y, -zc.x) / M_PI_2x;
	zc.y = sqrt(zc.x * zc.x + zc.y * zc.y)
			- fractal->transformCommon.radius1 / M_PI_2x;
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		double Voff = fractal->transformCommon.scaleA2;
		tp = zc.z - 2.0 * Voff * ang + Voff;
		zc.z = tp - 2.0 * Voff * floor(tp / (2.0 * Voff)) - Voff;
	}

	if (fractal->transformCommon.functionEnabledBFalse)
	{
		tp = fractal->transformCommon.scale2 * ang + 1.0;
		zc.x = tp - 2.0 * floor(tp / 2.0) - 1.0;
	}

	ang *= fractal->transformCommon.scaleA1 * M_PI;
	double cosA = cos(ang);
	double sinB = sin(ang);
	double temp = zc.z;
	zc.z = zc.y * cosA + zc.z * sinB;
	zc.y = temp * cosA + zc.y * -sinB;


	double lenX = fractal->transformCommon.offset1;
	double lenY = fractal->transformCommon.offsetA0;
	/*CVector4 absZ = fabs(zc);

	if (fractal->transformCommon.functionEnabledMFalse)
		lenX += absZ.z * fractal->transformCommon.scale0;
	if (fractal->transformCommon.functionEnabledNFalse)
		lenY += absZ.z * fractal->transformCommon.scaleA0;
	if (fractal->transformCommon.functionEnabledOFalse)
		lenX += absZ.y * fractal->transformCommon.scaleB0;
	if (fractal->transformCommon.functionEnabledPFalse)
		lenY += absZ.x * fractal->transformCommon.scaleC0;




	CVector3 q = CVector3(max(absZ.y - lenY, 0.0), max(absZ.x - lenX, 0.0), zc.z);





	q *= q;


*/


	/*CVector4 d;
	d = fabs(zc);
	d -= fractal->transformCommon.offset111;
	double rDE = min(max(d.x, max(d.y, d.z)), 0.0);

	d.x = max(d.x, 0.0);
	d.y = max(d.y, 0.0);
	d.z = max(d.z, 0.0);

	//	rDE = rDE + length(d);

	aux.DE0 = d.Length();*/
		//aux.DE0 = sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.scaleA0;
//	aux->DE0 = length(zc) - 1.0;
	// aux->dist = DEscale * aux->DE0  / (aux->DE);
	//aux->dist = aux->DE0 / aux->DE;
	CVector4 absZ = fabs(zc);

		if (fractal->transformCommon.functionEnabledMFalse)
			lenX += absZ.z * fractal->transformCommon.scale0;
		if (fractal->transformCommon.functionEnabledNFalse)
			lenY += absZ.z * fractal->transformCommon.scaleA0;
		if (fractal->transformCommon.functionEnabledOFalse)
			lenX += absZ.y * fractal->transformCommon.scaleB0;
		if (fractal->transformCommon.functionEnabledPFalse)
			lenY += absZ.x * fractal->transformCommon.scaleC0;
	CVector3 q = CVector3(max(absZ.y - lenY, 0.0), max(absZ.x - lenX, 0.0), zc.z);
	q *= q;
	double streD = sqrt(q.x + q.y) - fractal->transformCommon.offsetR1;

	if (!fractal->transformCommon.functionEnabledJFalse)
		streD = sqrt(streD * streD + q.z);
	else
		streD = max(fabs(streD), fabs(zc.z));

	aux.dist = min(aux.dist, (streD - fractal->transformCommon.offsetA05) / (aux.DE + 1.0f));


	// aux.dist = min(aux.dist, aux.DE0 / aux.DE);

	if (fractal->transformCommon.functionEnabledXFalse)
		z = zc;
}
