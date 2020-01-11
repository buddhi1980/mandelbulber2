/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * ABoxMod12,
 * The Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * Variations from DarkBeam, Buddhi, Eiffie and mclarekin
 * based on DarkBeam's Mandelbulb3D code
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467
 * This formula contains aux.color and aux.actualScaleA
 */

#include "all_fractal_definitions.h"

cFractalAboxMod12::cFractalAboxMod12() : cAbstractFractal()
{
	nameInComboBox = "Abox - Mod 12";
	internalName = "abox_mod12";
	internalID = fractal::aboxMod12;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionABox;
}

void cFractalAboxMod12::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 c = aux.const_c;
	double colorAdd = 0.0;
	// tglad fold
	CVector4 oldZ = z;
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
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
			if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
			if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
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
	// swap
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		z = CVector4(z.z, z.y, z.x, z.w);
	}
	// spherical folds
	if (aux.i >= fractal->transformCommon.startIterationsS
			&& aux.i < fractal->transformCommon.stopIterationsS)
	{
		double minR2 = fractal->transformCommon.minR2p25;
		double addR = 0.0;
		double m = 1.0;
		oldZ = z;

		// spherical fold with xyz bias option
		if (fractal->transformCommon.functionEnabledAx)
		{
			if (aux.i >= fractal->transformCommon.startIterationsE
					&& aux.i < fractal->transformCommon.stopIterationsE)
			{
				CVector4 xyzBias = fractal->transformCommon.constantMultiplier000;
				if (fractal->transformCommon.functionEnabledAzFalse)
				{
					xyzBias *= aux.c * aux.c;
				}
				else
				{
					xyzBias *= fabs(aux.c);
				}
				addR = (xyzBias.x + xyzBias.y + xyzBias.z);
				minR2 += addR;
				if (fractal->transformCommon.functionEnabledAz && minR2 > fractal->transformCommon.maxR2d1)
				{ // stop overlapping potential
					minR2 = fractal->transformCommon.maxR2d1;
				}
			}
			double rr = z.Dot(z);
			z += fractal->transformCommon.offset000;
			if (rr < minR2)
			{
				m *= fractal->transformCommon.maxR2d1 / minR2;
				colorAdd += fractal->mandelbox.color.factorSp1;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				m *= fractal->transformCommon.maxR2d1 / rr;
				colorAdd += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->transformCommon.offset000;
			// scale
			z *= m;
			aux.DE *= fabs(m);
		}

		// cuboid
		if (fractal->transformCommon.functionEnabledAyFalse
				&& aux.i >= fractal->transformCommon.startIterationsB
				&& aux.i < fractal->transformCommon.stopIterationsB)
		{
			CVector4 temp3;
			CVector4 R2;
			double MinR2 = fractal->transformCommon.minR2p25;
			CVector4 limitMinR2 = fractal->transformCommon.scaleP222;

			double rr = z.Dot(z);
			z += fractal->transformCommon.offset000;

			// if (aux.i >= fractal->transformCommon.startIterationsD
			//		&& aux.i < fractal->transformCommon.stopIterationsD)
			//{
			if (fractal->transformCommon.functionEnabledAxFalse)
				temp3 = z * z;
			else
				temp3 = fabs(z);

			if (temp3.x < limitMinR2.x && temp3.y < limitMinR2.y && temp3.z < limitMinR2.z)
			{ // if inside cuboid
				R2.x = limitMinR2.x / temp3.x;
				R2.y = limitMinR2.y / temp3.y;
				R2.z = limitMinR2.z / temp3.z;
				double First = min(R2.x, min(R2.y, R2.z));
				MinR2 = rr * First;

				// if (fractal->transformCommon.functionEnabled && MinR2 > fractal->transformCommon.maxR2d1)

				if (MinR2 > fractal->transformCommon.maxR2d1)
				{ // stop overlapping potential
					MinR2 = fractal->transformCommon.maxR2d1;
				}

				m *= fractal->transformCommon.maxR2d1 / MinR2;
				aux.color += fractal->mandelbox.color.factorSp1;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				m *= fractal->transformCommon.maxR2d1 / rr;
				aux.color += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->transformCommon.offset000;
			// scale
			z *= m;
			aux.DE *= fabs(m);
		}
		// if not use standard Mbox sphere fold

		if (z.x == oldZ.x && z.y == oldZ.y && z.z == oldZ.z && z.w == oldZ.w)
		{
			double MinR2 = fractal->transformCommon.minR2p25;
			double rr = z.Dot(z);
			z += fractal->transformCommon.offset000;
			if (rr < MinR2)
			{
				m *= fractal->transformCommon.maxR2d1 / MinR2;
				aux.color += fractal->mandelbox.color.factorSp1;
			}
			else if (rr < fractal->transformCommon.maxR2d1)
			{
				m *= fractal->transformCommon.maxR2d1 / rr;
				aux.color += fractal->mandelbox.color.factorSp2;
			}
			z -= fractal->transformCommon.offset000;
			// scale
			z *= m;
			aux.DE *= fabs(m);
		}
	}

	double useScale = fractal->mandelbox.scale;
	if (fractal->transformCommon.functionEnabledXFalse
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		useScale += aux.actualScaleA;
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;

		// update actualScale for next iteration
		double vary = fractal->transformCommon.scaleVary0
									* (fabs(aux.actualScaleA) - fractal->transformCommon.scaleB1);
		if (fractal->transformCommon.functionEnabledMFalse)
			aux.actualScaleA = -vary;
		else
			aux.actualScaleA = aux.actualScaleA - vary;
	}
	else
	{
		z *= useScale;
		aux.DE = aux.DE * fabs(useScale) + 1.0;
	}

	// offset
	z += fractal->transformCommon.additionConstant000;
	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC)
	{
		CVector4 tempC = c;
		if (fractal->transformCommon.alternateEnabledFalse) // alternate
		{
			tempC = aux.c;
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(tempC.x, tempC.y, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(tempC.x, tempC.z, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(tempC.y, tempC.x, tempC.z, tempC.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(tempC.y, tempC.z, tempC.x, tempC.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(tempC.z, tempC.x, tempC.y, tempC.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(tempC.z, tempC.y, tempC.x, tempC.w); break;
			}
			aux.c = tempC;
		}
		else
		{
			switch (fractal->mandelbulbMulti.orderOfXYZ)
			{
				case multi_OrderOfXYZ_xyz:
				default: tempC = CVector4(c.x, c.y, c.z, c.w); break;
				case multi_OrderOfXYZ_xzy: tempC = CVector4(c.x, c.z, c.y, c.w); break;
				case multi_OrderOfXYZ_yxz: tempC = CVector4(c.y, c.x, c.z, c.w); break;
				case multi_OrderOfXYZ_yzx: tempC = CVector4(c.y, c.z, c.x, c.w); break;
				case multi_OrderOfXYZ_zxy: tempC = CVector4(c.z, c.x, c.y, c.w); break;
				case multi_OrderOfXYZ_zyx: tempC = CVector4(c.z, c.y, c.x, c.w); break;
			}
		}
		z += tempC * fractal->transformCommon.constantMultiplier111;
	}
	// rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	// color updated v2.13
	if (fractal->foldColor.auxColorEnabled)
	{
		aux.color += colorAdd;
	}
}
