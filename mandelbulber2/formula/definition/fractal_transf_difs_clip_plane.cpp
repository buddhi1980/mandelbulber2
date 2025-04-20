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
	double temp;
	if (!fractal->transformCommon.functionEnabledDFalse) zc = c;
	else zc = z;

	// polyfold
	if (fractal->transformCommon.functionEnabledPFalse)
	{
		zc.y = fabs(z.y);
		double psi = M_PI / fractal->transformCommon.int6;
		psi = fabs(fmod(atan2(zc.y, zc.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(zc.x * zc.x + zc.y * zc.y);
		zc.x = cos(psi) * len;
		zc.y = sin(psi) * len;
	}
	// tile
	if (fractal->transformCommon.functionEnabledTFalse)
	{
		zc.x -= round(zc.x / fractal->transformCommon.offset2) * fractal->transformCommon.offset2;
		zc.y -= round(zc.y / fractal->transformCommon.offsetA2) * fractal->transformCommon.offsetA2;
	}

	// rot
	if (fractal->transformCommon.functionEnabledIFalse)
	{
		double angle = M_PI_2x / (fractal->transformCommon.int16);
		double sector = round(atan2(zc.x, zc.y) / angle);
		double an = sector * angle;
		double sinan = sin(an);
		double cosan = cos(an);
		temp = zc.x;
		zc.x = zc.x * cosan - zc.y * sinan;
		zc.y = temp * sinan + zc.y * cosan;
	}

	zc.x += fractal->transformCommon.offset000.x;
	zc.y += fractal->transformCommon.offset000.y;
	zc.z += fractal->transformCommon.offset000.z;

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		if (fractal->transformCommon.functionEnabledAxFalse) zc.x = fabs(zc.x);
		if (fractal->transformCommon.functionEnabledAyFalse) zc.y = fabs(zc.y);
		if (fractal->transformCommon.functionEnabledMFalse) zc.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledNFalse) zc.y = fabs(z.y);
		zc.x -= fractal->transformCommon.offsetA000.x;
		zc.y -= fractal->transformCommon.offsetA000.y;
	}

	// scales
	zc.x *= fractal->transformCommon.scale3D111.x;
	zc.y *= fractal->transformCommon.scale3D111.y;
	zc.z *= fractal->transformCommon.scale3D111.z;

	if (fractal->transformCommon.functionEnabledFFalse)
		zc.x = zc.x + sin(zc.y) * fractal->transformCommon.scale3D000.x;
	if (fractal->transformCommon.functionEnabledGFalse)
		zc.y = zc.y + sin(zc.x) * fractal->transformCommon.scale3D000.y;

	// square
	if (fractal->transformCommon.functionEnabledBxFalse) zc.x = max(fabs(zc.x), fabs(zc.y));
	// circle
	if (fractal->transformCommon.functionEnabledOFalse) zc.x = sqrt((zc.x * zc.x) + (zc.y * zc.y));

	// plane
	double plD = fabs(c.z - fractal->transformCommon.offsetF0)
			- fractal->transformCommon.offsetAp01;

	// rec clip plane
	double d = 1000.0;
	if (fractal->transformCommon.functionEnabledCy)
	{
		CVector4 rec = zc;
		if (fractal->transformCommon.functionEnabledEFalse)
			rec.x = fabs(rec.x) - ((rec.y) * fractal->transformCommon.scaleE1);

		if (fractal->transformCommon.functionEnabledXFalse)
			rec.x = rec.x - (fabs(rec.y) * fractal->transformCommon.scaleF1);

		CVector4 f = fabs(rec);
		f.x -= fractal->transformCommon.offset111.x;
		f.y -= fractal->transformCommon.offset111.y;
		f.z -= fractal->transformCommon.offsetBp01;
		d = max(f.x, max(f.y, f.z));

		// discs
		if (fractal->transformCommon.functionEnabledSFalse)
			d = sqrt(f.x * f.x + f.y * f.y) - fractal->transformCommon.offsetR1;
	}

	// cir clip plane
	double e = 1000.0;
	if (fractal->transformCommon.functionEnabledCxFalse)
	{
		CVector4 cir = zc;
		e = fractal->transformCommon.radius1;
		if (fractal->transformCommon.functionEnabledCFalse)
			cir.y = cir.y - (fabs(cir.x) * fractal->transformCommon.constantMultiplier000.x);

		if (!fractal->transformCommon.functionEnabledYFalse)
			e = clamp(sqrt(cir.x * cir.x + cir.y * cir.y * fractal->transformCommon.scaleA1) - e, 0.0, 100.0); // circle,
		else
			e = clamp(sqrt(cir.x * cir.x + cir.y * cir.y + cir.z * cir.z * fractal->transformCommon.scaleA1) - e, 0.0, 100.0); // sphere
	}

	aux.DE0 = min(e, d); //clip value

	// base plane
	double a = 1000.0;
	if (fractal->transformCommon.functionEnabledBFalse)
	{
		a = (c.z - fractal->transformCommon.offsetA0);
		aux.DE0 = min(aux.DE0, a);
	}

	// aux->color
	if (fractal->foldColor.auxColorEnabled)
	{
		double addColor = 0.0;
		if (e > d) addColor += fractal->foldColor.difs0000.x;
		if (e < d) addColor += fractal->foldColor.difs0000.y;
		if (aux.DE0 == a) addColor += fractal->foldColor.difs0000.z;

		if (!fractal->transformCommon.functionEnabledJFalse)
			aux.color = addColor;
		else
			aux.color += addColor;
	}



	aux.DE0 = (max(plD, aux.DE0) - fractal->transformCommon.offset0005)
			/ (aux.DE * fractal->analyticDE.scale1);

	if (!fractal->analyticDE.enabledFalse)
		aux.dist = aux.DE0;
	else
		aux.dist = min(aux.dist, aux.DE0);

	if (fractal->transformCommon.functionEnabledzFalse) z = zc;
}
