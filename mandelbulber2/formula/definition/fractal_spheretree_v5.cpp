/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * formula by TGlad,
 * https://https://fractalforums.org/fractal-mathematics-and-new-theories/28/sphere-inversion-trees/5113
 */

#include "all_fractal_definitions.h"

cFractalSpheretreeV5::cFractalSpheretreeV5() : cAbstractFractal()
{
	nameInComboBox = "Spheretree V5";
	internalName = "spheretree_v5";
	internalID = fractal::spheretreeV5;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalSpheretreeV5::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double t = 0.0; // temp
	CVector4 tv = CVector4(0.0, 0.0, 0.0, 0.0); // temp vector
	CVector4 oldZ = z;
	double col = 0.0;
	CVector4 ColV = CVector4(0.0, 0.0, 0.0, 0.0);

	if (fractal->transformCommon.functionEnabledJFalse)
	{
		aux.DE = 1.0;
		z = aux.const_c;
	}

	z *= fractal->transformCommon.scaleG1; // master scale
	aux.DE *= fractal->transformCommon.scaleG1;

	CVector4 K3 = tv;

	int NumChildren = fractal->transformCommon.int8X;
	int n = NumChildren;
	double ang1 = M_PI / n;

	double o1 = 3.0;
	if (NumChildren <= 4)
		o1 = 5.0;
	else if (NumChildren <= 6)
		o1 = 4.0;
	double sec = 1.0 / cos(M_PI / o1);
	double csc = 1.0 / sin(M_PI / n);
	double r = sec / sqrt(csc * csc - sec * sec);
	double l = sqrt(1.0 + r * r);

	double o2 = fractal->transformCommon.offset3;
	t = sin(M_PI - M_PI / o2);
	double theta = asin(r * t / l);
	double L4 = l * sin(M_PI / o2 - theta) / t;
	double minr = L4 * L4 * fractal->transformCommon.scaleD1;
	double bend = fractal->transformCommon.scale08;
	double omega = 0.0;
	double bigR = 0.0;
	double d = 0.0;

	if (!fractal->transformCommon.functionEnabledzFalse)
	{
		omega = M_PI_2 - bend;
		bigR = 1.0 / cos(omega);
		d = tan(omega);
	}

	// iteration loop
	bool recurse = true;
	if (fractal->transformCommon.functionEnabledFalse) recurse = false;
	for (int c = 0; c < fractal->transformCommon.int16; c++)
	{
		if (fractal->transformCommon.functionEnabledzFalse)
		{
			omega = M_PI_2 - bend;
			bigR = 1.0 / cos(omega);
			d = tan(omega);
		}

		if (fractal->transformCommon.functionEnabledPFalse
				&& c >= fractal->transformCommon.startIterationsP
				&& c < fractal->transformCommon.stopIterationsP1)
		{
			if (fractal->transformCommon.functionEnabledCxFalse)
				z.x = fabs(z.x) + fractal->transformCommon.offsetA000.x;
			if (fractal->transformCommon.functionEnabledCyFalse)
				z.y = fabs(z.y) + fractal->transformCommon.offsetA000.y;
			if (fractal->transformCommon.functionEnabledCzFalse)
				z.z = fabs(z.z) + fractal->transformCommon.offsetA000.z;
		}

		if (recurse == true && c >= fractal->transformCommon.startIterationsC
				&& c < fractal->transformCommon.stopIterationsC)
		{
			z.z += d + bigR;

			double inv = 1.0 / z.Dot(z);
			K3 += z * aux.DE * inv;
			K3 -= 2.0 * z * K3.Dot(z) * inv;
			double sc = 4.0 * bigR * bigR * inv;

			z *= sc;
			aux.DE *= sc;
			z.z += -2.0 * bigR;
			z.z = -z.z;
			double invSize = (bigR + d) / (2.0 * bigR);
			aux.DE *= invSize;
			z *= invSize;
			recurse = false;
		}
		double angle = atan2(z.y, z.x);
		if (angle < 0.0) angle += M_PI_2x;

		angle = fmod(angle, M_PI_2x / n);
		double mag = sqrt(z.x * z.x + z.y * z.y);
		z.x = mag * cos(angle);
		z.y = mag * sin(angle);

		CVector4 circle_centre = l * CVector4(cos(ang1), sin(ang1), 0.0, 0.0);
		tv = z - circle_centre;
		double len = tv.Length();
		if (len < r)
		{
			ColV.x += 1.0;
			double sc = r * r / (len * len);
			tv *= sc;
			aux.DE *= sc;
		}
		z = tv + circle_centre;

		o2 = bend / 2.0;
	//	double d2 = minr * tan(o2);
		double R2 = minr / cos(o2);
	//	CVector3 mid_offset = CVector3(0.0, 0.0, d2);
		tv = z; // - mid_offset * fractal->transformCommon.scaleA1;
		tv.z -= minr * tan(o2) * fractal->transformCommon.scaleA1;
		double amp = tv.Length();
		//   double mag4 = sqrt(p[0]*p[0] + p[1]*p[1]);
		if (amp <= R2 - fractal->transformCommon.offsetA0
			&& c >= fractal->transformCommon.startIterationsB
			&& c < fractal->transformCommon.stopIterationsB) // || mag4 <= minr)
		{
			ColV.z += 1.0;
			t = 1.0 / minr;
			z *= t;
			aux.DE *= t;
			recurse = true;
		}
		else if (z.Length() < L4)
		{
			double inv = 1.0 / z.Dot(z);
			K3 += z * aux.DE * inv;
			K3 -= 2.0 * z * K3.Dot(z) * inv;
			double sc =  L4 * L4  * inv;

			z *= sc;
			aux.DE *= sc;
		}
		if (c >= fractal->transformCommon.startIterationsA
				&& c < fractal->transformCommon.stopIterationsA)
		{
			bend *= fractal->transformCommon.scaleB1;
			bend += fractal->transformCommon.offsetB0;
		}

		// post scale
		z *= fractal->transformCommon.scaleC1;
		aux.DE *= fabs(fractal->transformCommon.scaleC1);

		// DE tweaks
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

		if (fractal->foldColor.auxColorEnabled && c >= fractal->foldColor.startIterationsA
				&& c < fractal->foldColor.stopIterationsA)
		{
			t = z.Length();
			aux.temp1000 = min(aux.temp1000, t);
			ColV.y = aux.temp1000;
			ColV.w  = t;

			col += ColV.x * fractal->foldColor.difs0000.x + ColV.y * fractal->foldColor.difs0000.y
					+ ColV.z * fractal->foldColor.difs0000.z + ColV.w * fractal->foldColor.difs0000.w;
		}
	}

	double dt = 0.0;
	if (!fractal->transformCommon.functionEnabledDFalse)
	{
		if (!fractal->transformCommon.functionEnabledOFalse)
		{
			if (!fractal->transformCommon.functionEnabledEFalse)
				dt = z.Length();
			else
				dt = z.z + (z.Length() - z.z) * fractal->transformCommon.scaleC0;
		}
		else
		{
			bool negate = false;
			double den = K3.Length();

			double radius = bend + (z.Length() - bend) * fractal->transformCommon.scaleB0;
		//	double radius = bend;

			CVector4 target = CVector4(0.0, 0.0, 0.0, 0.0);
			if (den > 1e-13)
			{
				CVector4 offset = K3 / den;
				offset *= aux.DE; // since K is normalised to the scale
				double rad = offset.Length();
				offset += z;

				target -= offset;
				double mag = target.Length();
				if ((radius / mag) > 1.0) negate = true;
				t = radius / mag;

				CVector4 t1 = target * (1.0 - t);
				CVector4 t2 = target * (1.0 + t);
				t1 *= rad * rad / t1.Dot(t1);
				t2 *= rad * rad / t2.Dot(t2);
				CVector4 mid = (t1 + t2) / 2.0;
				tv = t1 - t2;
				radius = tv.Length() / 2.0;
				target = mid + offset;
			}
			tv = z - target;
			double dist = tv.Length() - radius;

			if (negate) dist = -dist;

			dt = dist + 1.0; // mmmmmmmmmm
		}
	}
	else
	{
		CVector4 zc = z - fractal->transformCommon.offset000;
		if (!fractal->transformCommon.functionEnabledFFalse) zc = fabs(zc);
		dt = max(max(zc.x, zc.y), zc.z);
	}

	dt -= fractal->transformCommon.offset0;

	if (!fractal->transformCommon.functionEnabledGFalse) dt /= aux.DE;
	if (fractal->transformCommon.functionEnabledCFalse) // clip
	{
		aux.const_c.z += fractal->transformCommon.offsetF0;
		double dst1 = aux.const_c.Length() - fractal->transformCommon.offsetR1;
		dt = max(dt, dst1);
		//dt = fabs(dt);
		if (fractal->transformCommon.functionEnabledGFalse) dt /= aux.DE;
	}

	if (!fractal->transformCommon.functionEnabledXFalse)
		aux.dist = min(aux.dist, dt);
	else
		aux.dist = dt;

	if (fractal->analyticDE.enabledFalse) z = oldZ;

	if (!fractal->transformCommon.functionEnabledKFalse)
		aux.color += col;
	else
		aux.color = col;
}
