/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 * references:
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

void cFractalTransfDIFSClipPlane::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = aux.const_c;

	if (fractal->transformCommon.functionEnabledJFalse)
	{
		zc.x -= round(zc.x / fractal->transformCommon.offset2) * fractal->transformCommon.offset2;
		zc.y -= round(zc.y / fractal->transformCommon.offsetA2) * fractal->transformCommon.offsetA2;
	}

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

	zc.y -= fractal->transformCommon.offset0;
	zc.z -= fractal->transformCommon.offsetC0;


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


	if (fractal->transformCommon.functionEnabledAFalse)
		zc.x = zc.x + sign(zc.y) * .5 * fractal->transformCommon.intA;

	if (fractal->transformCommon.functionEnabledBFalse)
		zc.y = zc.y + sign(zc.x) * .5 * fractal->transformCommon.intB;

	zc.x *= fractal->transformCommon.scale3D111.x;
	zc.y *= fractal->transformCommon.scale3D111.y;
	zc.z /= fractal->transformCommon.scale3D111.z;

	if (fractal->transformCommon.functionEnabledFFalse)
		zc.x = zc.x + sin(zc.y) * fractal->transformCommon.scale3D000.x;
	if (fractal->transformCommon.functionEnabledGFalse)
		zc.y = zc.y + sin(zc.x) * fractal->transformCommon.scale3D000.y;


/*	if (fractal->transformCommon.functionEnabledFFalse)
		zc.x = zc.x + sin(zc.y) * fractal->transformCommon.scale3D000.x;
	if (fractal->transformCommon.functionEnabledGFalse)
		zc.y = zc.y + sin(zc.x) * fractal->transformCommon.scale3D000.y;

	// square
	if (fractal->transformCommon.functionEnabledBx) zc.x = max(fabs(zc.x), fabs(zc.y));
	// circle
	if (fractal->transformCommon.functionEnabledDFalse) zc.x = sqrt((zc.x * zc.x) + (zc.y * zc.y));

	if (fractal->transformCommon.functionEnabledKFalse)
		zc.x = zc.x + sin(zc.y) * fractal->transformCommon.scale3D000.z;

	double tD = 1000.0;
	tD = zc.x - round(zc.x);


	tD = sqrt(tD * tD + zc.z * zc.z) - fractal->transformCommon.offsetp05;

	if (fractal->transformCommon.functionEnabledOFalse)
		tD = max(
			fabs(tD) - fractal->transformCommon.offsetA0, fabs(zc.z) - fractal->transformCommon.offsetB0);*/


	// plane
	double plD = 1000.0;
	double d = 1000.0;
	if (fractal->transformCommon.functionEnabled)
		plD = fabs(zc.z - fractal->transformCommon.offsetF0);

//	aux.dist = (plD /(aux.DE + 1.0f));
	aux.dist = min(aux.dist, plD);



	// aux->color
	if (fractal->foldColor.auxColorEnabled)
	{
		if (aux.dist == plD) aux.color = fractal->foldColor.difs0000.x;
		//else aux.color = fractal->foldColor.difs0000.y
		//		+ fractal->foldColor.difs0000.z * zc.z
		//		+ fractal->foldColor.difs0000.w * zc.z * zc.z;
	}

	// clip plane
	CVector4 cir = zc;
	CVector4 rec = zc;
	if (fractal->transformCommon.functionEnabledTFalse)
	{
	// rec
		if (fractal->transformCommon.functionEnabledEFalse)
		rec.x = fabs(rec.x) - ((rec.y) * fractal->transformCommon.constantMultiplier000.y);

		if (fractal->transformCommon.functionEnabledXFalse)
		rec.x = rec.x - (fabs(rec.y) * fractal->transformCommon.constantMultiplier000.z);

		CVector4 f = fabs(rec);
		f -= fractal->transformCommon.offset111;
		d = max(f.x, max(f.y, f.z));

		// cir
		double e = fractal->transformCommon.offset3;
		if (fractal->transformCommon.functionEnabledCFalse)
			cir.y = cir.y - (fabs(cir.x) * fractal->transformCommon.constantMultiplier000.x);
		if (!fractal->transformCommon.functionEnabledYFalse)
			e = clamp(sqrt(cir.x * cir.x + cir.y * cir.y) - e, 0.0, 100.0); // circle,
		else
			e = clamp(cir.Length() - e, 0.0, 100.0); //a sphere

		e = min(e, d);

		// discs
		if (fractal->transformCommon.functionEnabledSFalse)
			e = sqrt(f.x * f.x + f.y * f.y) - fractal->transformCommon.offset3;

		d = max(aux.dist, e);
		//d = max(d, e);
	}


	if (fractal->transformCommon.functionEnabledzFalse) z = zc;
	if (fractal->analyticDE.enabledFalse)
		aux.dist = d;
	else
		aux.dist = min(aux.dist, d);





	/*double size = fractal->transformCommon.scale1;
	double grid = 0.0;



	aux.dist = min(aux.dist, (grid - fractal->transformCommon.offset0005) / (aux.DE + 1.0));*/
}
