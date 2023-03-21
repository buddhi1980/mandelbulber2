/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSClipCustom
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSClipCustom::cFractalTransfDIFSClipCustom() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Clip Custom";
	internalName = "transf_difs_clip_custom";
	internalID = fractal::transfDIFSClipCustom;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 1000.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSClipCustom::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	if (fractal->transformCommon.functionEnabledwFalse) aux.dist = z.Length() / aux.DE;

	// pre-box option
	if (fractal->transformCommon.functionEnabledAFalse)
	{
		CVector4 zc = z;
		zc += fractal->transformCommon.offsetA000;
		zc = fabs(zc) - fractal->transformCommon.additionConstant111;
		zc.x = max(zc.x, 0.0);
		zc.y = max(zc.y, 0.0);
		zc.z = max(zc.z, 0.0);
		//double zcd = zc.Length() / (aux.DE + fractal->analyticDE.offset0) - fractal->transformCommon.offsetB0;
		double zcd = zc.Length() * fractal->transformCommon.scale1 - fractal->transformCommon.offsetB0;
		if (!fractal->transformCommon.functionEnabledNFalse)
		{
			aux.dist = zcd;
		}
		else
		{
			aux.dist = min(aux.dist, zcd);
		}
	}

	// transform c
	CVector4 c = aux.const_c;
	if (fractal->transformCommon.functionEnabledFalse) c = z;

			// polyfold
	if (fractal->transformCommon.functionEnabledPFalse)
	{
		c.y = fabs(c.y);
		double psi = M_PI / fractal->transformCommon.int6;
		psi = fabs(fmod(atan2(c.y, c.x) + psi, 2.0 * psi) - psi);
		double len = sqrt(c.x * c.x + c.y * c.y);
		c.x = cos(psi) * len;
		c.y = sin(psi) * len;
	}

	if (fractal->transformCommon.functionEnabledAxFalse) c.x = fabs(c.x);
	if (fractal->transformCommon.functionEnabledAyFalse) c.y = fabs(c.y);
	if (fractal->transformCommon.functionEnabledAzFalse) c.z = fabs(c.z);

	c *= fractal->transformCommon.scale3D111;
	c += fractal->transformCommon.offset000;
	c = fractal->transformCommon.rotationMatrix.RotateVector(c);

	double dst = 0.0;
	if (fractal->transformCommon.functionEnabledBx)
	{
		CVector4 g = fabs(c) - fractal->transformCommon.offsetC111;
		if (!fractal->transformCommon.functionEnabledCFalse)
		{
			dst = max(max(g.x, g.y), g.z);
		}
		else
		{
			dst = c.Length() - g.Length();
		}
	}



	double dst1 = 0.0;
	if (fractal->transformCommon.functionEnabledBFalse) // sphere
	{

		if (!fractal->transformCommon.functionEnabledIFalse)
		{
			dst1 = c.Length() - fractal->transformCommon.offsetR1; // sphere
		}
		else // cyl or cone
		{
			if (!fractal->transformCommon.functionEnabledEFalse) // cyl
			{
				dst1 = sqrt(c.x * c.x + c.y * c.y) - fractal->transformCommon.offsetR1;
			}
			else // cones
			{
				double CZ = -c.z;
				if (fractal->transformCommon.functionEnabledFFalse) CZ = fabs(c.z);
				if (fractal->transformCommon.functionEnabledGFalse) CZ = c.z * c.z;
				dst1 = sqrt(c.x * c.x + c.y * c.y) - fractal->transformCommon.offsetR1 * CZ;
			}
		}

		if (!fractal->transformCommon.functionEnabledJFalse) // z clip
		{
			dst1 = max(fabs(c.z) - fractal->transformCommon.offset1, dst1);
		}

		if (!fractal->transformCommon.functionEnabledDFalse) dst = dst1;
		else dst = max(dst, dst1);
	}

	// aux->color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		double addColor = 0.0;
		if (dst > aux.dist) addColor += fractal->foldColor.difs0000.x;
		if (dst1 > aux.dist) addColor += fractal->foldColor.difs0000.y;
		if (!fractal->transformCommon.functionEnabledyFalse)
			aux.color = addColor;
		else
			aux.color += addColor;
	}

	if (!fractal->transformCommon.functionEnabledxFalse)
	{
		dst *= fractal->analyticDE.scale1;
	}
	else
	{
		dst *= fractal->analyticDE.scale1 / aux.DE;
	}

	dst = max(aux.dist, dst);

	if (!fractal->analyticDE.enabledFalse)
		aux.dist = dst;
	else
		aux.dist = min(dst, aux.dist);
}
