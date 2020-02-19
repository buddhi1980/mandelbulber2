/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Based on a DarkBeam fold formula adapted by Knighty
 * refer http://www.fractalforums.com/amazing-box-amazing-surf-and-variations/'new'-fractal-type-mandalay/msg81348/#msg81348
 */

#include "all_fractal_definitions.h"

cFractalTransfMandalayFoldV1::cFractalTransfMandalayFoldV1() : cAbstractFractal()
{
	nameInComboBox = "T>Mandalay Fold V1";
	internalName = "transf_mandalay_fold_v1";
	internalID = fractal::transfMandalayFoldV1;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfMandalayFoldV1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);

	// darkbeam fold
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
}
