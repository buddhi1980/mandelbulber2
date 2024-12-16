/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * KochV2Iteration
 * Based on Knighty's Kaleidoscopic IFS 3D Fractals, described here:
 * http://www.fractalforums.com/3d-fractal-generation/kaleidoscopic-%28escape-time-ifs%29/
 */

#include "all_fractal_definitions.h"

cFractalKochV4::cFractalKochV4() : cAbstractFractal()
{
	nameInComboBox = "Koch V4";
	internalName = "koch_v4";
	internalID = fractal::kochV4;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalKochV4::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;

	if (fractal->transformCommon.functionEnabledAx) zc.x = fabs(zc.x);
	if (fractal->transformCommon.functionEnabledAy) zc.y = fabs(zc.y);
	if (fractal->transformCommon.functionEnabledAzFalse) zc.z = fabs(zc.z);
	if (fractal->transformCommon.functionEnabledCx)
		if (zc.y > zc.x) swap(zc.x, zc.y);

	// folds
	if (fractal->transformCommon.functionEnabledFalse)
	{
		// diagonal2
		if (fractal->transformCommon.functionEnabledCxFalse)
			if (zc.x > zc.y) swap(zc.x, zc.y);
		// polyfold
		if (fractal->transformCommon.functionEnabledPFalse)
		{
			zc.x = fabs(zc.x);
			double psi = M_PI / fractal->transformCommon.int6;
			psi = fabs(fmod(atan2(zc.y, zc.x) + psi, 2.0 * psi) - psi);
			double len = sqrt(zc.x * zc.x + zc.y * zc.y);
			zc.x = cos(psi) * len;
			zc.y = sin(psi) * len;
		}
		// abs offsets
		if (fractal->transformCommon.functionEnabledCFalse)
		{
			double xOffset = fractal->transformCommon.offsetC0;
			if (zc.x < xOffset) zc.x = fabs(zc.x - xOffset) + xOffset;
		}
		if (fractal->transformCommon.functionEnabledDFalse)
		{
			double yOffset = fractal->transformCommon.offsetD0;
			if (zc.y < yOffset) zc.y = fabs(zc.y - yOffset) + yOffset;
		}
	}

	double YOff = FRAC_1_3 * fractal->transformCommon.scale1;
	zc.y = YOff - fabs(zc.y - YOff);

	zc.x += FRAC_1_3;
	if (zc.z > zc.x) swap(zc.x, zc.z);
	zc.x -= FRAC_1_3;

	zc.x -= FRAC_1_3;
	if (zc.z > zc.x) swap(zc.x, zc.z);
	zc.x += FRAC_1_3;

	CVector4 Offset = fractal->transformCommon.offset100;
	double useScale = 1.0;
	useScale = (aux.actualScaleA + fractal->transformCommon.scale3);
	if (fractal->transformCommon.functionEnabledKFalse)
	{
		// update actualScaleA for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleC1);
		aux.actualScaleA = -vary;
	}
	zc = useScale * (zc - Offset) + Offset;
	aux.DE = aux.DE * fabs(useScale) + fractal->analyticDE.offset0;

	// rotation
	if (fractal->transformCommon.functionEnabledRFalse
			&& aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		zc = fractal->transformCommon.rotationMatrix.RotateVector(zc);
	}
	zc += fractal->transformCommon.offset000;

	//aux.dist
	CVector4 c = aux.const_c;
	double d;

	double a = fractal->transformCommon.offsetA0;
	if (!fractal->transformCommon.functionEnabledFFalse)
	{
		CVector4 b = fabs(zc) - CVector4(a, a, a, 0.0);
		d = max(b.x, max(b.y, b.z));
	}
	else
	{
		d = fabs(zc.Length() - a);
	}

	if (fractal->transformCommon.functionEnabledBFalse)
		d -= Offset.Length();

	// plane
	double g = fabs(zc.z - fractal->transformCommon.offsetR0) - fractal->transformCommon.offsetF0;

	g = min(g, d);

	// clip
	double e = fractal->transformCommon.offset2;
	if (!fractal->transformCommon.functionEnabledEFalse)
	{
		CVector4 f = fabs(c) - CVector4(e, e, e, 0.0);
		if (!fractal->transformCommon.functionEnabledIFalse)
			e = max(f.x, f.y); // sq
		else
			e = max(f.x, max(f.y, f.z)); // box
	}
	else
	{
		if (!fractal->transformCommon.functionEnabledIFalse)
			e = clamp(sqrt(c.x * c.x + c.y * c.y) - e, 0.0, 100.0); // circle
		else
			e = clamp(c.Length() - e, 0.0, 100.0); // sphere
	}

	d = max(g, e) / aux.DE;

	//aux.dist = d;
	aux.dist = min(d, aux.dist);

	// aux->color
	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
			&& aux.i < fractal->foldColor.stopIterationsA)
	{
		double addColor = 0.0;
		if (aux.dist == d)
			aux.color = fractal->foldColor.difs0000.x;

		else
		{
			addColor = fractal->foldColor.difs0000.y * fabs(zc.x)
					+ fractal->foldColor.difs0000.z * fabs(zc.z)
					+ fractal->foldColor.difs0000.w * g;

			if (!fractal->transformCommon.functionEnabledJFalse)
				aux.color = addColor;
			else
				aux.color += addColor;
		}
	}
	z = zc;
}
