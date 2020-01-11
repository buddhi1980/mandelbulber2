/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSEllipsoidIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSEllipsoid::cFractalTransfDIFSEllipsoid() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Ellipsoid";
	internalName = "transf_difs_ellipsoid";
	internalID = fractal::transfDIFSEllipsoid;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSEllipsoid::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;
	CVector4 rads4 = fractal->transformCommon.additionConstant111;
	CVector3 rads3 = CVector3(rads4.x, rads4.y, rads4.z);

	CVector3 rV = CVector3(zc.x, zc.y, zc.z);
	rV /= rads3;

	CVector3 rrV = rV;
	rrV /= rads3;

	double rd = rV.Length();
	double rrd = rrV.Length();
	double ellD = rd * (rd - 1.0) / rrd;
	aux.dist = min(aux.dist, ellD / (aux.DE + 1.0));
}
