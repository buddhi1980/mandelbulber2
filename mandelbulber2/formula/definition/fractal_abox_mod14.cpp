/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * ABoxMod14,
 * The Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * Hollow mandelbox idea from alexl (sanbase)
 * http://www.fractalforums.com/3d-fractal-generation/realtime-rendering-on-gpu/
 * Other variations by mclarekin
 * This formula contains aux.color and aux.actualScaleA
 */

#include "all_fractal_definitions.h"

cFractalAboxMod14::cFractalAboxMod14() : cAbstractFractal()
{
	nameInComboBox = "Abox - Mod 14";
	internalName = "abox_mod14";
	internalID = fractal::aboxMod14;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalAboxMod14::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	double m = 1.0;
	// tglad fold
	if (aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabled)
		{
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
		}
	}
	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		CVector4 limit = fractal->transformCommon.additionConstant111;
		CVector4 length = 2.0 * limit;
		CVector4 tgladS = 1.0 / length;
		CVector4 Add = CVector4(0.0, 0.0, 0.0, 0.0);
		if (fabs(z.x) < limit.x) Add.x = z.x * z.x * tgladS.x;
		if (fabs(z.y) < limit.y) Add.y = z.y * z.y * tgladS.y;
		if (fabs(z.z) < limit.z) Add.z = z.z * z.z * tgladS.z;
		if (fabs(z.x) > limit.x && fabs(z.x) < length.x)
			Add.x = (length.x - fabs(z.x)) * (length.x - fabs(z.x)) * tgladS.x;
		if (fabs(z.y) > limit.y && fabs(z.y) < length.y)
			Add.y = (length.y - fabs(z.y)) * (length.y - fabs(z.y)) * tgladS.y;
		if (fabs(z.z) > limit.z && fabs(z.z) < length.z)
			Add.z = (length.z - fabs(z.z)) * (length.z - fabs(z.z)) * tgladS.z;
		Add *= fractal->transformCommon.scale3D000;
		z.x = (z.x - (sign(z.x) * (Add.x)));
		z.y = (z.y - (sign(z.y) * (Add.y)));
		z.z = (z.z - (sign(z.z) * (Add.z)));
	}

	// standard offset
	z += fractal->transformCommon.offset000;

	// spherical fold
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double rr = z.Dot(z);
		if (rr < fractal->transformCommon.invert0) m = fractal->transformCommon.inv0;
		else if (rr < fractal->transformCommon.invert1) m = 1.0 / rr;
		else m = fractal->transformCommon.inv1;
		z *= m;
		aux.DE *= m;
	}

	double useScale =  fractal->transformCommon.scale2;
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		useScale += aux.actualScaleA;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + fractal->analyticDE.offset0;

		// update actualScale for next iteration
		double vary = fractal->transformCommon.scaleVary0
				* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
		aux.actualScaleA = -vary;
	}
	else
	{
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + fractal->analyticDE.offset0;
	}

	// offset options
	if (fractal->transformCommon.functionEnabledAxFalse
		&& aux.i >= fractal->transformCommon.startIterationsB
		&& aux.i < fractal->transformCommon.stopIterationsB)
	{
		CVector4 offset = aux.pos_neg * fractal->transformCommon.additionConstant000;

		if (fractal->transformCommon.functionEnabledAFalse)
			offset = CVector4(sign(z.x), sign(z.y), sign(z.z), 1.0) * offset;

		if (fractal->transformCommon.functionEnabledBFalse)
			offset = CVector4(sign(c.x), sign(c.y), sign(c.z), 1.0) * offset;

		z += offset;
		aux.pos_neg *= fractal->transformCommon.scale1; // update for next iter
	}

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabled
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse)
		{
			tempC.x = sign(z.x) * fabs(c.x);
			tempC.y = sign(z.y) * fabs(c.y);
			tempC.z = sign(z.z) * fabs(c.z);
		}

		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	// rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
	// color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		colorAdd += fractal->mandelbox.color.factorSp2 * m;
		aux.color += colorAdd;
	}
}
