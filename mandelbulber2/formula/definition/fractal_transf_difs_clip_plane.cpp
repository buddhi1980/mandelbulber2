/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * fragmentarium code, by knighty
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSClipPlane::cFractalTransfDIFSClipPlane() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Clip Plane";
	internalName = "transf_difs_clip_plane";
	internalID = fractal::transfDIFSClipPlane;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 1000.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSClipPlane::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	CVector4 zc;
	if (!fractal->transformCommon.functionEnabledDFalse) zc = c;
	else zc = z;

	// polyfold
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		zc.y = fabs(z.y);
		double psi = M_PI / fractal->transformCommon.int6;
		psi = fabs(fmod(atan2(zc.y, zc.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(zc.x * zc.x + zc.y * zc.y);
		zc.x = cos(psi) * len;
		zc.y = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledTFalse
			&& aux.i >= fractal->transformCommon.startIterationsT
			&& aux.i < fractal->transformCommon.stopIterationsT1)
	{
		zc.x += fractal->transformCommon.offsetD0;
		zc.x -= round(zc.x / fractal->transformCommon.offset2) * fractal->transformCommon.offset2;
		zc.y -= round(zc.y / fractal->transformCommon.offsetA2) * fractal->transformCommon.offsetA2;
	}

	/*if (fractal->transformCommon.functionEnabledDFalse)
	{

// ietate offset??
		CVector4 repeatPos = fractal->transformCommon.offsetA111;
		CVector4 repeatNeg = fractal->transformCommon.offsetB111;

		if (fractal->transformCommon.functionEnabledx && z.x < (repeatPos.x + 0.5) * sizeX
				&& z.x > (repeatNeg.x + 0.5) * -sizeX && sizeX != 0.0)
		{
			double sizeX = fractal->transformCommon.offset2;
			z.x -= round(z.x / sizeX) * sizeX;
			z.x = clamp(fabs(z.x), -t.x, t.x);
		}
		if (fractal->transformCommon.functionEnabledyFalse && z.y < (repeatPos.y + 0.5) * sizeY	// polyfold
	if (fractal->transformCommon.functionEnabledPFalse
			&& aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		zc.y = fabs(z.y);
		double psi = M_PI / fractal->transformCommon.int6;
		psi = fabs(fmod(atan2(zc.y, zc.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(zc.x * zc.x + zc.y * zc.y);
		zc.x = cos(psi) * len;
		zc.y = sin(psi) * len;
	}
				&& z.y > (repeatNeg.y + 0.5) * -sizeY && sizeY != 0.0)
		{
			double sizeY = fractal->transformCommon.offsetA2;
			z.y -= round(z.y / sizeY) * sizeY;
			z.y = clamp(fabs(z.y), -t.y, t.y);
		}
	}*/




	if (fractal->transformCommon.functionEnabledIFalse)
	{
		double angle = M_PI_2x / (fractal->transformCommon.int16);
		double sector = round(atan2(zc.x, zc.y) / angle);
		double an = sector * angle;
		double sinan = sin(an);
		double cosan = cos(an);
		double temp = zc.x;
		zc.x = zc.x * cosan - zc.y * sinan;
		zc.y = temp * sinan + zc.y * cosan;
	}



	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) zc.x = fabs(zc.x);
		if (fractal->transformCommon.functionEnabledAyFalse) zc.y = fabs(zc.y);
		if (fractal->transformCommon.functionEnabledAzFalse) zc.z = fabs(zc.z);
	}
	zc += fractal->transformCommon.offset000;

	zc.y -= fractal->transformCommon.offset0;
	zc.z -= fractal->transformCommon.offsetC0;

	zc.x -= fractal->transformCommon.offsetE0;

	// abs offset x
	if (fractal->transformCommon.functionEnabledMFalse)
	{
		zc.x += fractal->transformCommon.offsetA000.x;
		zc.x = fabs(z.x) - fractal->transformCommon.offsetA000.x;
	}
	// abs offset y
	if (fractal->transformCommon.functionEnabledNFalse)
	{
		zc.y += fractal->transformCommon.offsetA000.y;
		zc.y = fabs(z.y) - fractal->transformCommon.offsetA000.y;
	}
	// steps
//	if (fractal->transformCommon.functionEnabledAFalse)
//		zc.x = zc.x + sign(zc.y) * 0.5 * fractal->transformCommon.offsetD0;
//	if (fractal->transformCommon.functionEnabledBFalse)
//		zc.y = zc.y + sign(zc.x) * 0.5 * fractal->transformCommon.offsetE0;

	// scales
	zc.x *= fractal->transformCommon.scale3D111.x;
	zc.y *= fractal->transformCommon.scale3D111.y;
	// zc.z *= fractal->transformCommon.scale3D111.z; // mmmmmmmmmmmmmmmmmmmmmmmmmmm

	if (fractal->transformCommon.functionEnabledFFalse)
		zc.x = zc.x + sin(zc.y) * fractal->transformCommon.scale3D000.x;
	if (fractal->transformCommon.functionEnabledGFalse)
		zc.y = zc.y + sin(zc.x) * fractal->transformCommon.scale3D000.y;

	// square
	if (fractal->transformCommon.functionEnabledBxFalse) zc.x = max(fabs(zc.x), fabs(zc.y));
	// circle
	if (fractal->transformCommon.functionEnabledOFalse) zc.x = sqrt((zc.x * zc.x) + (zc.y * zc.y));

	if (fractal->transformCommon.functionEnabledKFalse)
		zc.x = zc.x + sin(zc.y) * fractal->transformCommon.scale3D000.z;

	// plane
	double plD = fabs(c.z - fractal->transformCommon.offsetF0)
			- fractal->transformCommon.offset0005;

	double b = min(aux.dist, plD / (aux.DE + fractal->analyticDE.offset0));

	// clip plane
	CVector4 cir = zc;
	CVector4 rec = zc;

	double d = 1000.0;
	double e = fractal->transformCommon.offset3;

	// rec
	if (fractal->transformCommon.functionEnabledCy)
	{
		if (fractal->transformCommon.functionEnabledEFalse)
			rec.x = fabs(rec.x) - ((rec.y) * fractal->transformCommon.constantMultiplier000.y);

		if (fractal->transformCommon.functionEnabledXFalse)
			rec.x = rec.x - (fabs(rec.y) * fractal->transformCommon.constantMultiplier000.z);

		CVector4 f = fabs(rec);
		f -= fractal->transformCommon.offset111;
		d = max(f.x, max(f.y, f.z));

		// discs
		if (fractal->transformCommon.functionEnabledSFalse)
			d = sqrt(f.x * f.x + f.y * f.y) - fractal->transformCommon.offsetR2;
	}

	// cir
	if (fractal->transformCommon.functionEnabledCxFalse)
	{
		if (fractal->transformCommon.functionEnabledCFalse)
			cir.y = cir.y - (fabs(cir.x) * fractal->transformCommon.constantMultiplier000.x);

		if (!fractal->transformCommon.functionEnabledYFalse)
			e = clamp(sqrt(cir.x * cir.x + cir.y * cir.y) - e, 0.0, 100.0); // circle,
		else
			e = clamp(sqrt(cir.x * cir.x + cir.y * cir.y + cir.z * cir.z * fractal->transformCommon.scaleA1) - e, 0.0, 100.0); // sphere
	}

	//d = min(d2, d);

	// aux->color
	if (fractal->foldColor.auxColorEnabled)
	{
		double addColor = 0.0;
		if (e > d) addColor += fractal->foldColor.difs0000.x;
		if (e < d) addColor += fractal->foldColor.difs0000.y;

			//addColor += fractal->foldColor.difs0000.z * zc.z
			//									+ fractal->foldColor.difs0000.w * zc.z * zc.z;
		if (!fractal->transformCommon.functionEnabledJFalse)
			aux.color = addColor;
		else
			aux.color += addColor;
	}

	e = min(e, d); //clip value
	// plane
	double a = 1000.;


	//if (fractal->transformCommon.functionEnabledDFalse)
	{
		a = fabs(aux.const_c.z - fractal->transformCommon.offsetA0)
				- fractal->transformCommon.offsetB0;
		//tp = min(tp, d);
		//if (tp == d) aux.color += fractal->foldColor.difs1;
	}



	aux.DE0 = max(b, e);
	//aux.DE0 = min(plD, a);


	//aux.DE0 = max(aux.DE0, e);

	if (!fractal->analyticDE.enabledFalse)
		aux.dist = aux.DE0;
	else
		aux.dist = min(aux.dist, aux.DE0);

	if (fractal->transformCommon.functionEnabledzFalse) z = zc;
}
