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

cFractalSphereCluster::cFractalSphereCluster() : cAbstractFractal()
{
	nameInComboBox = "Sphere Cluster";
	internalName = "sphere_cluster";
	internalID = fractal::sphereCluster;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalSphereCluster::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 oldZ = z;
	CVector3 p = CVector3(z.x, z.y, z.z); // convert to vec3
	//	double PackRatio = fractal->transformCommon.offset1;
	CVector4 ColV = CVector4(0.0, 0.0, 0.0, 0.0);
//	double phi = (1.0 + sqrt(5.0)) / 2.0;
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

	bool is_b = fractal->transformCommon.functionEnabledDFalse;
	double minr = 0.0;
	double l, r;
	CVector3 mid;
	aux.DE = 1.0 * fractal->transformCommon.scale1; // ,,,,,,,,,,,,,,,,,
int i;
	bool recurse = true;
	for (i = 0; i < fractal->transformCommon.int8X; i++)
	{
		if (recurse)
		{
			if (p.Length() > excess)
			{ p = CVector3(0.0, 0.0, 1e-15);
				// (p.Length() - 1.0) / aux.DE;
			}
			if (is_b)
			{
				minr = minrb;
			}
			else
			{
				minr = minra;
			}
			double sc = minr / p.Dot(p);
			p *= sc;
			aux.DE *= sc;
			recurse = false;
			ColV.z += 1.0;
		}
		if (is_b)
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
		else
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

		double dist = (p - mid * l).Length();
		if (dist < r || i == fractal->transformCommon.int8X - 1)
		{
			ColV.x += 1.0 * (i + 1);
			p -= mid * l;
			double sc = r * r / p.Dot(p);
			p *= sc;
			aux.DE *= sc;
			p += mid * l;

			double m = minr * k;
			if (p.Length() < minr)
			{
				ColV.y += 1.0 * (i + 1);
				p /= m;
				aux.DE /= m;

				if (fractal->transformCommon.functionEnabledTFalse) // toggle
					is_b = !is_b;
				recurse = true;
			}
		}
		// post scale
		p *= fractal->transformCommon.scaleF1;
		aux.DE *= fabs(fractal->transformCommon.scaleF1);
		// DE tweaks
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
	}


	z = CVector4{p.x, p.y, p.z, z.w};

double d;
	if (!fractal->transformCommon.functionEnabledSwFalse)
	{
		if (!fractal->transformCommon.functionEnabledEFalse) d = k;
		else d = min(1.0, k);
		d = (z.Length() - minr * fractal->transformCommon.scaleE1 * d) / aux.DE;
	}
	else
	{
		CVector4 zc = z - fractal->transformCommon.offset000;
		d = max(max(zc.x, zc.y), zc.z);
		d = (d - minr * k) / aux.DE;
	}


double dst1 = aux.const_c.Length() - fractal->transformCommon.offsetR1;
	d = max(d, dst1);

	if (!fractal->transformCommon.functionEnabledXFalse)
		aux.dist = min(aux.dist, d);
	else
		aux.dist = d;

	if (fractal->analyticDE.enabledFalse) z = oldZ;

	ColV.w += 1.0* aux.DE;

	// aux->color
	if (i >= fractal->foldColor.startIterationsA && i < fractal->foldColor.stopIterationsA)
	{
		aux.color += ColV.x * fractal->foldColor.difs0000.x + ColV.y * fractal->foldColor.difs0000.y
									+ ColV.z * fractal->foldColor.difs0000.z + ColV.w * fractal->foldColor.difs0000.w;
	}
}
