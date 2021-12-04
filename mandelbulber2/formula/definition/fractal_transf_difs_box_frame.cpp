/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSBosFrameIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSBoxFrame::cFractalTransfDIFSBoxFrame() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Box Frame";
	internalName = "transf_difs_box_frame";
	internalID = fractal::transfDIFSBoxFrame;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSBoxFrame::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;
	double e = fractal->transformCommon.offset0005;
	zc = fabs(zc) - fractal->transformCommon.offsetC111;
	CVector4 q = CVector4(fractal->transformCommon.offsetp01,
						fractal->transformCommon.offsetAp01,
						fractal->transformCommon.offsetBp01, 0.0);
	q = fabs(zc + q) - q;

	double lenX = min(max(zc.x, max(q.y, q.z)), 0.0);
	double lenY = min(max(q.x, max(zc.y, q.z)), 0.0);
	double lenZ = min(max(q.x, max(q.y, zc.z)), 0.0);

	CVector4 mz = zc;
	mz.x = max(zc.x, 0.0);
	mz.y = max(zc.y, 0.0);
	mz.z = max(zc.z, 0.0);

	CVector4 mq = q;
	mq.x = max(q.x, 0.0);
	mq.y = max(q.y, 0.0);
	mq.z = max(q.z, 0.0);

	lenX += (CVector3(mz.x, mq.y, mq.z)).Length();
	lenY += (CVector3(mq.x, mz.y, mq.z)).Length();
	lenZ += (CVector3(mq.x, mq.y, mz.z)).Length();

	double D = min(min(lenX, lenY), lenZ);

	aux.dist = min(aux.dist, D);
}
