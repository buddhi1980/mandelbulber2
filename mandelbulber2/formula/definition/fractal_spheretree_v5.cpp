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
	CVector3 tv = CVector3(0.0, 0.0, 0.0); // temp vector
	CVector4 oldZ = z;
	double col = 0.0;
	CVector4 ColV = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector3 p = CVector3(z.x, z.y, z.z); // convert to vec3
	if (fractal->transformCommon.functionEnabledDFalse) aux.DE = 1.0;

	double dist_to_sphere = p.Length() - fractal->transformCommon.radius1;

	p *= fractal->transformCommon.scaleG1; // master scale
	aux.DE *= fractal->transformCommon.scaleG1;

	CVector3 K3 = tv;

	int Iterations = fractal->transformCommon.int16;
	int NumChildren = fractal->transformCommon.int8X;
	int n = NumChildren;
	double ang1 = M_PI / n;

	// double BendAngle = fractal->transformCommon.scale08;

	double o1 = 3.0;
	if (NumChildren <= 4)
		o1 = 5.0;
	else if (NumChildren <= 6)
		o1 = 4.0;
	double o2 = fractal->transformCommon.offset3;

	double sec = 1.0 / cos(M_PI / o1);
	double csc = 1.0 / sin(M_PI / n);
	double r = sec / sqrt(csc * csc - sec * sec);

	double l = sqrt(1.0 + r * r);

	t = sin(M_PI - M_PI / o2);
	double theta = asin(r * t / l);
	double L4 = l * sin(M_PI / o2 - theta) / t;
	double minr = L4 * L4;

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
//	bool StartCurved = fractal->transformCommon.functionEnabledFalse;


//	bool recurse = StartCurved; // StartCurved;// mmmmmmmmmmmmmmmm??? x -

	bool recurse = false;
	if (fractal->transformCommon.functionEnabledFalse) recurse = true;
	for (int c = 0; c < Iterations; c++)
	{
		if (fractal->transformCommon.functionEnabledzFalse)
		{
			omega = M_PI_2 - bend;
			bigR = 1.0 / cos(omega);
			d = tan(omega);
		}

		if (recurse == true && c >= fractal->transformCommon.startIterationsC// mmmmmmmmmmmmmmmm??? x -
				&& c < fractal->transformCommon.stopIterationsC)
		{
			p.z += d + bigR;
			double sc = 4.0 * bigR * bigR / p.Dot(p);
			p *= sc;
			aux.DE *= sc;
			p.z += -2.0 * bigR;
			p.z = -p.z;
			double invSize = (bigR + d) / (2.0 * bigR);
			aux.DE *= invSize;
			p *= invSize;
			recurse = false;
		}
		double angle = atan2(p.y, p.x);
		if (angle < 0.0) angle += M_PI_2x;

	angle = fmod(angle, M_PI_2x / n); // mmmmmmmmmmmmmmmm??? x - y * trunc (x/y).
		double mag = sqrt(p.x * p.x + p.y * p.y);
		p.x = mag * cos(angle);
		p.y = mag * sin(angle);

	//	double ang1 = M_PI / n;
		CVector3 circle_centre = l * CVector3(cos(ang1), sin(ang1), 0.0);
		tv = p - circle_centre;
		double len = tv.Length();
		if (len < r)
		{
			ColV.x += 1.0;
			double sc = r * r / (len * len);
			tv *= sc;
			aux.DE *= sc;
		}
		p = tv + circle_centre;

		o2 = bend / 2.0;
		double d2 = minr * tan(o2);
		double R2 = minr / cos(o2);
		CVector3 mid_offset = CVector3(0.0, 0.0, d2);
		tv = p - mid_offset * fractal->transformCommon.scaleA1;
		double amp = tv.Length() * fractal->transformCommon.scaleD1;
		//   double mag4 = sqrt(p[0]*p[0] + p[1]*p[1]);
		if (amp <= R2 - fractal->transformCommon.offsetA0) // mmmmmmmmmmmmmmmmmmmmmmm // || mag4 <= minr)
		{
			ColV.z += 1.0;
			t = 1.0 / minr;
			p *= t;
			aux.DE *= t;
			recurse = true;
		}
		else if (p.Length() < L4)
		{
			ColV.w += 1.0;
			double sc = L4 * L4 / p.Dot(p);
			p *= sc;
			aux.DE *= sc;
		}
		if (c >= fractal->transformCommon.startIterationsA
				&& c < fractal->transformCommon.stopIterationsA)
		{

			bend *= fractal->transformCommon.scaleB1; //transformCommon.offsetB0
			bend += fractal->transformCommon.offsetB0;
		}


		// post scale
		p *= fractal->transformCommon.scaleC1;
		aux.DE *= fabs(fractal->transformCommon.scaleC1);
		// DE tweaks
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

		ColV.y += p.Length();

		if (fractal->foldColor.auxColorEnabled && c >= fractal->foldColor.startIterationsA
				&& c < fractal->foldColor.stopIterationsA)
		{
			col += ColV.x * fractal->foldColor.difs0000.x + ColV.y * fractal->foldColor.difs0000.y
					+ ColV.z * fractal->foldColor.difs0000.z + ColV.w * fractal->foldColor.difs0000.w;
		}
	}

	z = CVector4(p.x, p.y, p.z, z.w);

	double dt = 0.0;

	if (!fractal->transformCommon.functionEnabledEFalse)
		dt = (z.Length() - fractal->transformCommon.offset0) / aux.DE;
	else
		dt = (p.z - fractal->transformCommon.offset0) / aux.DE;



/*	double d;
	if (!fractal->transformCommon.functionEnabledSwFalse)
	{
		if (!fractal->transformCommon.functionEnabledEFalse)
		{
			d = k;
		}
		else
		{
			d = min(1.0f, k);
		}

		d = minr * fractal->transformCommon.scaleE1 * d;

		if (!fractal->transformCommon.functionEnabledOFalse)
		{
			d = (length(z) - d) / aux.DE;
		}
		else
		{
			bool negate = false;

			double den = length(K3);

			double radius = d;
			CVector3 target = (CVector3){0.0f, 0.0f, 0.0f};
			if (den > 1e-13f)
			{
				CVector3 offset = K3 / den;
				offset *= aux.DE; // since K is normalised to the scale
				double rad = length(offset);
				offset += p;

				target -= offset;
				double mag = length(target);
				if (fabs(radius / mag) > 1.0f) negate = true;

				CVector3 t1 = target * (1.0f - radius / mag);
				CVector3 t2 = target * (1.0f + radius / mag);
				t1 *= rad * rad / dot(t1, t1);
				t2 *= rad * rad / dot(t2, t2);
				CVector3 mid = (t1 + t2) / 2.0f;
				radius = length(t1 - t2) / 2.0f;
				target = mid + offset;
			}

			double dist = (length(p - target) - radius);

			if (negate) dist = -dist;

			d = dist / aux.DE;
		}
	}
	else
	{
		double4 zc = z - fractal->transformCommon.offset000;
		if (fractal->transformCommon.functionEnabledFFalse) zc = fabs(zc);
		d = max(max(zc.x, zc.y), zc.z);
		d = (d - minr * k) / aux.DE;
	}*/

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		double dst1 = aux.const_c.Length() - fractal->transformCommon.offsetR1;
		dt = max(dt, dst1);
		dt = fabs(dt);
	}


	if (fractal->transformCommon.functionEnabledYFalse) dt = max(dist_to_sphere, dt);

	if (!fractal->transformCommon.functionEnabledXFalse)
		aux.dist = min(aux.dist, dt);
	else
		aux.dist = dt;

	if (fractal->analyticDE.enabledFalse) z = oldZ;

	aux.color += col;
	//return z;
}
