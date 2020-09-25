/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Described in http://www.fractalforums.com/general-discussion-b77/solids-many-many-solids/
 * fold and cut regular polyhedra Distance Estimator (knighty 2012)
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSPolyhedraV2::cFractalTransfDIFSPolyhedraV2() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Polyhedra V2";
	internalName = "transf_difs_polyhedra_v2";
	internalID = fractal::transfDIFSPolyhedraV2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSPolyhedraV2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	int Type = fractal->transformCommon.int3;
	// U 'barycentric' coordinate for the 'principal' node
	double U = fractal->transformCommon.constantMultiplier100.x;
	double V = fractal->transformCommon.constantMultiplier100.y;
	double W = fractal->transformCommon.constantMultiplier100.z;
	// vertex radius
	double VRadius = fractal->transformCommon.offset01;
	// segments radius
	double SRadius = fractal->transformCommon.offsetp05;

	double temp;
	// this block does not use z so could be inline precalc
	double cospin = M_PI / (double)(Type);
	cospin = cos(cospin);
	double scospin = sqrt(0.75 - cospin * cospin);
	CVector4 nc = CVector4(-0.5, -cospin, scospin, 0.0);
	CVector4 pab = CVector4(0.0, 0.0, 1.0, 0.0);
	CVector4 pbc = CVector4(scospin, 0.0, 0.5, 0.0);
	temp = pbc.Length();
	pbc = pbc / temp;
	CVector4 pca = CVector4(0.0, scospin, cospin, 0.0);
	temp = pca.Length();
	pca = pca / temp;

	CVector4 p = (U * pab + V * pbc + W * pca);
	temp = p.Length();
	p = p / temp;

	double d = 10000.0;
	CVector4 colVec = CVector4(1000.0, 1000.0, 1000.0, 1000.0);
	CVector4 zc = z;
	CVector4 zcv = zc;

	for (int m = 0; m < fractal->transformCommon.int6; m++)
	{
		double Scale = fractal->transformCommon.scale1;
		double powp = 1.0;
		powp = pow(Scale, (double)(-m));

		for (int n = 0; n < Type; n++)
		{
			zc.x = fabs(zc.x);
			zc.y = fabs(zc.y);
			double t = -2.0 * min(0.0, zc.Dot(nc));
			zc += t * nc;
		}
		zcv = zc;

		if (fractal->transformCommon.functionEnabledBxFalse)
		{ // faces
			double d0 = zc.Dot(pab) - p.Dot(pab);
			double d1 = zc.Dot(pbc) - p.Dot(pbc);
			double d2 = zc.Dot(pca) - p.Dot(pca);

			double df = max(max(d0, d1), d2);
			colVec.x = df;
			d = min(d, df * powp);
		}

		if (!fractal->transformCommon.functionEnabledByFalse)
		{ // Segments
			zcv = zcv - p;
			double minDL = fractal->transformCommon.offsetB0;
			double dla =  zcv.x - min(minDL, zcv.x);
			CVector4 temp4 = CVector4(dla, zcv.y, zcv.z, zcv.w);
			dla = temp4.Length();
			temp4 = zcv - zcv.Dot(nc) * nc;
			double dlc = temp4.Length();
			double ds = min(dla, dlc) - SRadius;
			colVec.y = ds;
			d = min(d, ds * powp);
		}

		if (!fractal->transformCommon.functionEnabledBzFalse)
		{ // Vertices
			double dv;
			if (!fractal->transformCommon.functionEnabledDFalse)
			{
				CVector4 temp4 = zcv - p;
				dv = temp4.Length() - VRadius;
			}
			else
			{
				CVector4 ff = fabs(zc - p);
				dv = max(max(ff.x, ff.y), ff.z) - VRadius;
			}
			colVec.z = dv;
			d = min(d, dv * powp);
		}
		zc *= Scale;
	}

	aux.dist = min(aux.dist, d) / aux.DE;
	if (fractal->transformCommon.functionEnabledzFalse) z = zc;

	if (fractal->foldColor.auxColorEnabled)
	{
		colVec.x *= fractal->foldColor.difs0000.x;
		colVec.y *= fractal->foldColor.difs0000.y;
		colVec.z *= fractal->foldColor.difs0000.z;
		if (!fractal->foldColor.auxColorEnabledFalse)
		{
			double colorAdd = 0.0;
			colorAdd += colVec.x;
			colorAdd += colVec.y;
			colorAdd += colVec.z;
			// colorAdd += colVec.w;
			aux.color = colorAdd * 256.0;
		}
		else
		{
			aux.color = min(min(colVec.x, colVec.y), colVec.z)
				* fractal->foldColor.difs1 * 1024.0;
		}
	}
}
