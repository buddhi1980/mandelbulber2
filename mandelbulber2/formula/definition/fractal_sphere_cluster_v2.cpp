/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * formula by TGlad,
 * https://fractalforums.org/fractal-mathematics-and-new-theories/28/fractal-clusters/5107
 */

#include "all_fractal_definitions.h"

cFractalSphereClusterV2::cFractalSphereClusterV2() : cAbstractFractal()
{
	nameInComboBox = "Sphere Cluster V2";
	internalName = "sphere_cluster_v2";
	internalID = fractal::sphereClusterV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalSphereClusterV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double t = 0.0;
	CVector3 tv = CVector3(0.0, 0.0, 0.0);
	CVector4 oldZ = z;
	double col = 0.0;
	CVector4 ColV = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector3 p = CVector3(z.x, z.y, z.z); // convert to vec3

	p *= fractal->transformCommon.scaleG1; // master scale
	aux.DE *= fractal->transformCommon.scaleG1;

	CVector3 K3 = tv;

	double phi = (1.0 + sqrt(5.0)) / fractal->transformCommon.scale2;

	// Isocahedral geometry
	CVector3 ta0 = CVector3(0.0, 1.0, phi);
	CVector3 ta1 = CVector3(0.0, -1.0, phi);
	CVector3 ta2 = CVector3(phi, 0.0, 1.0);
	CVector3 na0 = ta0.Cross(ta1 - ta0);
	na0 = na0 / na0.Length();
	CVector3 na1 = ta1.Cross(ta2 - ta1);
	na1 = na1 / na1.Length();
	CVector3 na2 = ta2.Cross(ta0 - ta2);
	na2 = na2 / na2.Length();
	double mid_to_edgea = atan(phi / (1.0 + 2.0 * phi));
	double xxa = 1.0 / sin(mid_to_edgea);
	double ra = 2.0 / sqrt(-4.0 + xxa * xxa);
	double la = sqrt(1.0 + ra * ra);
	CVector3 mida = (ta0 + ta1 + ta2);
	mida = mida/ mida.Length();
	double minra = (la - ra * fractal->transformCommon.scaleC1) * fractal->transformCommon.scaleA1;

	// Dodecahedral geometry
	CVector3 tb0 = CVector3(1.0 / phi, 0.0, phi);
	CVector3 tb1 = CVector3(1.0, -1.0, 1.0);
	CVector3 tb2 = CVector3(phi, -1.0 / phi, 0.0);
	CVector3 tb3 = CVector3(phi, 1.0 / phi, 0.0);
	CVector3 tb4 = CVector3(1.0, 1.0, 1.0);

	CVector3 nb0 = (tb0.Cross(tb1 - tb0));
	nb0 = nb0 / nb0.Length();
	CVector3 nb1 = (tb1.Cross(tb2 - tb1));
	nb1 = nb1 / nb1.Length();
	CVector3 nb2 = (tb2.Cross(tb3 - tb2));
	nb2 = nb2 / nb2.Length();
	CVector3 nb3 = (tb3.Cross(tb4 - tb3));
	nb3 = nb3 / nb3.Length();
	CVector3 nb4 = (tb4.Cross(tb0 - tb4));
	nb4 = nb4 / nb4.Length();
	CVector3 dirb = (tb0 + tb1 + tb2 + tb3 + tb4);
	dirb = dirb /dirb.Length();
	double mid_to_edgeb = atan(dirb.z / dirb.x);
	double xxb = 1.0 / sin(mid_to_edgeb);
	double rb =sqrt(2.0) / sqrt(-2.0 + xxb * xxb);
	double lb = sqrt(1.0 + rb * rb);
	CVector3 midb = dirb;
	double minrb = (lb - rb * fractal->transformCommon.scaleD1) * fractal->transformCommon.scaleB1;

	double k = fractal->transformCommon.scale08; // PackRatio;
	double excess = fractal->transformCommon.offset105; // adds a skin width

	double minr = 0.0;
	double l, r;
	CVector3 mid;

	int n;
	bool recurse = true;
	for (n = 0; n < fractal->transformCommon.int16; n++)
	{
		if (fractal->transformCommon.functionEnabledPFalse
				&& n >= fractal->transformCommon.startIterationsP
				&& n < fractal->transformCommon.stopIterationsP1)
		{
			if (fractal->transformCommon.functionEnabledCxFalse) p.x = fabs(p.x) + fractal->transformCommon.offsetA000.x;
			if (fractal->transformCommon.functionEnabledCyFalse) p.y = fabs(p.y) + fractal->transformCommon.offsetA000.y;
			if (fractal->transformCommon.functionEnabledCzFalse) p.z = fabs(p.z) + fractal->transformCommon.offsetA000.z;
		}

		bool is = true;
		if (n >= fractal->transformCommon.startIterationsA
				&& n < fractal->transformCommon.stopIterationsA) is = false; // Isocahedral

		bool on = true;
		if (n >= fractal->transformCommon.startIterationsB
			&& n < fractal->transformCommon.stopIterationsB) on = false;

		if (recurse && n >= fractal->transformCommon.startIterationsC
				&& n < fractal->transformCommon.stopIterationsC)
		{
			if (p.Length() > excess)
			{
				if (fractal->transformCommon.functionEnabledAFalse)
				{
					tv = CVector3(z.x, z.y, z.z);
					p = fabs(tv);
				}
				if (!fractal->transformCommon.functionEnabledBFalse) break;
			}
			if (is == true)
			{
				minr = minrb; // Dodecahedral
			}
			else
			{
				minr = minra; // Isocahedral
			}
			if (on == false)
			{
				double inv = 1.0 / p.Dot(p);
				K3 += p * aux.DE * inv;
				K3 -= 2.0 * p * K3.Dot(p) * inv;
				double sc = minr * inv;
				p *= sc;
				aux.DE *= sc;
				ColV.z += 1.0;
				recurse = false;

			}
		}

		if (is == true) // Dodecahedral
		{
			l = lb;
			r = rb;
			mid = midb;
			minr = minrb;
			if (p.Dot(nb0) < 0.0)
				p -= 2.0 * nb0 * p.Dot(nb0);
			if (p.Dot(nb1) < 0.0)
				p -= 2.0 * nb1 * p.Dot(nb1);
			if (p.Dot(nb2) < 0.0)
				p -= 2.0 * nb2 * p.Dot(nb2);
			if (p.Dot(nb3) < 0.0)
				p -= 2.0 * nb3 * p.Dot(nb3);
			if (p.Dot(nb4) < 0.0)
				p -= 2.0 * nb4 * p.Dot(nb4);

			if (p.Dot(nb0) < 0.0)
				p -= 2.0 * nb0 * p.Dot(nb0);
			if (p.Dot(nb1) < 0.0)
				p -= 2.0 * nb1 * p.Dot(nb1);
			if (p.Dot(nb2) < 0.0)
				p -= 2.0 * nb2 * p.Dot(nb2);
			if (p.Dot(nb3) < 0.0)
				p -= 2.0 * nb3 * p.Dot(nb3);
			if (p.Dot(nb4) < 0.0)
				p -= 2.0 * nb4 * p.Dot(nb4);
		}
		else // Isocahedral
		{
			l = la;
			r = ra;
			mid = mida;
			minr = minra;
			if (p.Dot(na0) < 0.0)
				p -= 2.0 * na0 * p.Dot(na0);
			if (p.Dot(na1) < 0.0)
				p -= 2.0 * na1 * p.Dot(na1);
			if (p.Dot(na2) < 0.0)
				p -= 2.0 * na2 * p.Dot(na2);

			if (p.Dot(na0) < 0.0)
				p -= 2.0 * na0 * p.Dot(na0);
			if (p.Dot(na1) < 0.0)
				p -= 2.0 * na1 * p.Dot(na1);
			if (p.Dot(na2) < 0.0)
				p -= 2.0 * na2 * p.Dot(na2);

			if (p.Dot(na0) < 0.0)
				p -= 2.0 * na0 * p.Dot(na0);
			if (p.Dot(na1) < 0.0)
				p -= 2.0 * na1 * p.Dot(na1);
			if (p.Dot(na2) < 0.0)
				p -= 2.0 * na2 * p.Dot(na2);
		}

		double m = minr * k;
		t = 1.0 / m;
		CVector3 tv = p - mid * l;
		double dist = tv.Length();
		if (dist < r || n == fractal->transformCommon.int16 - 1)
		{
			p -= mid * l;

			double inv = 1.0 / p.Dot(p);
			K3 += p * aux.DE * inv;
			K3 -= 2.0 * p * (K3.Dot(p)) * inv;

			double sc = r * r;
			if (!fractal->transformCommon.functionEnabledMFalse)
				sc = sc * inv;
			else
				sc = (sc + (minr - sc) * fractal->transformCommon.scale0) * inv;

			p *= sc;
			aux.DE *= sc;
			p += mid * l;

			if (fractal->transformCommon.functionEnabledTFalse) m = minr;
			if (p.Length() < m * fractal->transformCommon.radius1 && on == false)
			{
				ColV.y += 1.0;
				p *= t;
				aux.DE *= t;
				recurse = true;
			}
		}

		if (!fractal->transformCommon.functionEnabledIFalse)
		{
			if (on == true && n >= fractal->transformCommon.startIterationsD
					&& n < fractal->transformCommon.stopIterationsD)
			{
				p *= t;
				aux.DE *= t;
			}
		}
		else
		{
			if (recurse && n >= fractal->transformCommon.startIterationsD
					&& n < fractal->transformCommon.stopIterationsD)
			{
				p *= t;
				aux.DE *= t;
			}
		}

		k *= fractal->transformCommon.scale1; // PackRatioScale;
		// post scale
		p *= fractal->transformCommon.scaleF1;
		aux.DE *= fabs(fractal->transformCommon.scaleF1);
		// DE tweaks
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

		if (fractal->foldColor.auxColorEnabled && n >= fractal->foldColor.startIterationsA
				&& n < fractal->foldColor.stopIterationsA)
		{
			t = z.Length();

			aux.temp1000 = min(aux.temp1000, t);
			ColV.x = aux.temp1000;

			col += ColV.x * fractal->foldColor.difs0000.x
					+ ColV.y * fractal->foldColor.difs0000.y
					+ ColV.z * fractal->foldColor.difs0000.z;
			if (fractal->foldColor.difs1 > dist) col += fractal->foldColor.difs0000.w;
		}
	}

	z = CVector4(p.x, p.y, p.z, z.w);
	double d;

	if (!fractal->transformCommon.functionEnabledEFalse)
	{
		d = k;
	}
	else
	{
		d = min(1.0, k);
	}

	d = minr * fractal->transformCommon.scaleE1 * d;

	if (!fractal->transformCommon.functionEnabledOFalse)
	{
		d = (z.Length() - d) / aux.DE;
	}
	else
	{
		bool negate = false;
		double radius = d;
		double den = K3.Dot(K3);
		CVector3 target = CVector3(0.0, 0.0, 0.0);
		if (den > 1e-13)
		{
			CVector3 offset = K3 / den;
			offset *= aux.DE; // since K is normalised to the scale
			double rad = offset.Length();
			offset += p;

			target -= offset;
			double mag = target.Length();
			if (fabs(radius / mag) > 1.0) negate = true;

			CVector3 t1 = target * (1.0 - radius / mag);
			CVector3 t2 = target * (1.0 + radius / mag);
			t1 *= rad * rad / t1.Dot(t1);
			t2 *= rad * rad / t2.Dot(t2);
			CVector3 mid = (t1 + t2) / 2.0;
			tv = t1 - t2;
			radius = tv.Length() / 2.0;
			target = mid + offset;
		}
		tv = p - target;
		double dist = tv.Length() - radius;

		if (negate) dist = -dist;
		d = dist / aux.DE;
	}

	if (fractal->transformCommon.functionEnabledCFalse)
	{
		double dst1 = aux.const_c.Length() - fractal->transformCommon.offsetR1;
		d = max(d, dst1);
		d = fabs(d);
	}

	if (!fractal->transformCommon.functionEnabledXFalse)
		aux.dist = min(aux.dist, d);
	else
		aux.dist = d;

	if (fractal->analyticDE.enabledFalse) z = oldZ;

	if (!fractal->transformCommon.functionEnabledGFalse)
		aux.color += col;
	else
		aux.color = col;
}
