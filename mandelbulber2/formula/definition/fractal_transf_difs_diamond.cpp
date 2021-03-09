/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Created by Nrx in 2016-09-26
 * https://www.shadertoy.com/view/XlyGDm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSDiamond::cFractalTransfDIFSDiamond() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Diamond";
	internalName = "transf_difs_diamond";
	internalID = fractal::transfDIFSDiamond;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSDiamond::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 normalTopA = CVector4(0.0, 0.81373347121, 0.5812382, 0.0);
	CVector4 normalTopB = CVector4(0.0, SQRT_1_2, SQRT_1_2, 0.0);
	CVector4 normalTopC = CVector4(0.0, 0.4472135955, 0.8944272, 0.0);
	CVector4 normalBottomA = CVector4(0.0, SQRT_1_2, -SQRT_1_2, 0.0);
	CVector4 normalBottomB = CVector4(0.0, 0.848, -0.53, 0.0);
	CVector4 zc = z;

	double topCut = zc.z - fractal->transformCommon.offset1;
	double angleStep = M_PI / double(fractal->transformCommon.int16);
	double angle = angleStep * (0.5 + floor (atan2 (zc.x, zc.y) / angleStep));
	double co = cos(angle);
	double si = sin(angle);
	CVector4 q = zc;
	q.x = (co * zc.x - si * zc.y);
	q.y = (co * zc.y + si * zc.x);
	double topA = q.Dot(normalTopA) - fractal->transformCommon.offset2;
	double topC = q.Dot(normalTopC) - fractal->transformCommon.offset105;
	double bottomA = q.Dot( normalBottomA) - fractal->transformCommon.offsetA2 + 0.3;
	angle = -angleStep * 0.5;
	co = cos(angle);
	si = sin(angle);
	q.x = (co * zc.x - si * zc.y);
	q.y = (co * zc.y + si * zc.x);
	angle = angleStep * floor (atan2 (q.x, q.y) / angleStep);
	co = cos(angle);
	si = sin(angle);
	q.x = (co * zc.x - si * zc.y);
	q.y = (co * zc.y + si * zc.x);
	double topB = q.Dot(normalTopB) - fractal->transformCommon.offsetE2 + 0.15;
	double bottomB = q.Dot(normalBottomB) - fractal->transformCommon.offsetF2 + 0.1;


	aux.DE0 = max (topCut, max (topA, max (topB, max (topC, max (bottomA, bottomB)))));
	aux.dist = min(aux.dist, aux.DE0 / aux.DE);
}
