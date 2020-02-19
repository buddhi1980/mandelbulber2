/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Based on a DarkBeam fold formula adapted by Knighty
 * http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/'new'-fractal-type-mandalay/msg81348/#msg81348
 */

#include "all_fractal_definitions.h"

cFractalMandalayBoxV1::cFractalMandalayBoxV1() : cAbstractFractal()
{
	nameInComboBox = "Mandalay Box V1";
	internalName = "mandalay_box_v1";
	internalID = fractal::mandalayBoxV1;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMandalayBoxV1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// tglad fold
	if (fractal->transformCommon.functionEnabledAFalse)
		z = fabs(z + fractal->transformCommon.additionConstant111)
					- fabs(z - fractal->transformCommon.additionConstant111) - z;

	double signX = sign(z.x);
	double signY = sign(z.y);
	double signZ = sign(z.z);

	z = fabs(z);

	CVector4 fo = fractal->transformCommon.additionConstant0555;
	CVector4 g = fractal->transformCommon.offsetA000;
	CVector4 p = z;
	CVector4 q = z;

	double t1, t2, v, v1;

	if(p.z > p.y) swap(p.y, p.z);
	t1 = p.x - 2.0 * fo.x;
	t2 = p.y - 4.0 * fo.x;
	v = max(fabs(t1 + fo.x) - fo.x, t2);
	v1 = max(t1 - g.x, p.y);
	v = min(v, v1);
	q.x = min(v, p.x);

	if (!fractal->transformCommon.functionEnabledSwFalse) p = z;
	else p = q;

	if(p.x > p.z) swap(p.z, p.x);
	t1 = p.y - 2.0 * fo.y;
	t2 = p.z - 4.0 * fo.y;
	v = max(fabs(t1 + fo.y) - fo.y, t2);
	v1 = max(t1 - g.y, p.z);
	v = min(v, v1);
	q.y = min(v, p.y);

	if (!fractal->transformCommon.functionEnabledSwFalse) p = z;
	else p = q;

	if(p.y > p.x) swap(p.x, p.y);
	t1 = p.z - 2.0 * fo.z;
	t2 = p.x - 4.0 * fo.z;
	v = max(fabs(t1 + fo.z) - fo.z, t2);
	v1 = max(t1 - g.z, p.x);
	v = min(v, v1);
	q.z = min(v, p.z);

	z = q;

	z.x *= signX;
	z.y *= signY;
	z.z *= signZ;

	// spherical fold and scale
	double rr = z.Dot(z);
	double t = min(1.0 / fractal->transformCommon.minR2p25, max(1.0, 1.0 / rr));
	z *= fractal->transformCommon.scale2 * t;
	aux.DE = aux.DE * fabs(fractal->transformCommon.scale2) * t + 1.0;

	// rotation
	z = fractal->transformCommon.rotationMatrix.RotateVector(z);

	if (fractal->analyticDE.enabledFalse)
		aux.DE =  aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	 // temp code
	p = fabs(z);
	aux.dist = max(p.x, max(p.y, p.z));
	aux.dist = aux.dist / aux.DE;

}
