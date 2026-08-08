/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * ABoxMod1 (mb3d1), a formula from Mandelbulb3D by DarkBeam
 * Based on the documented formula from ABoxMod1.m3f:
 *   Scale = Scale + Scale_vary*(abs(Scale)-1)
 *   x = Fold-abs(abs(x+FoldXM)-Fold)-abs(FoldXM)
 *   y = Fold-abs(abs(y+FoldYM)-Fold)-abs(FoldYM)
 *   z = Fold-abs(abs(z+FoldZM)-Fold)-abs(FoldZM)
 *   if rr < sqr(Min_R) then m = Scale/sqr(Min_R) else
 *   if rr < 1 then m = Scale/rr else m = Scale
 *   x = x * m + Cy
 *   y = y * m + Cx
 *   z = z * m + Cz
 */

#include "all_fractal_definitions.h"
#include "src/fractal.h"

cFractalAboxMb3d1::cFractalAboxMb3d1() : cAbstractFractal()
{
	nameInComboBox = "Abox - Mod1 (MB3D)";
	internalName = "abox_mb3d1";
	internalID = fractal::aboxMb3d1;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionIFS;
}

void cFractalAboxMb3d1::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// Step 1: Mandelbox scale variation
	// Scale = Scale + Scale_vary*(abs(Scale)-1)
	double actualScale =
		fractal->mandelbox.scale + fractal->mandelboxVary4D.scaleVary * (fabs(aux.actualScale) - 1.0);
	aux.actualScale = actualScale;

	// Step 2: Box fold with FoldX/Y/ZMod (additionConstant000)
	// x = Fold - abs(abs(x+FoldXM) - Fold) - abs(FoldXM)
	double fold = fractal->mandelbox.foldingValue;
	double foldXM = fractal->transformCommon.additionConstant000.x;
	double foldYM = fractal->transformCommon.additionConstant000.y;
	double foldZM = fractal->transformCommon.additionConstant000.z;

	CVector4 oldZ = z;
	z.x = fold - fabs(fabs(z.x + foldXM) - fold) - fabs(foldXM);
	z.y = fold - fabs(fabs(z.y + foldYM) - fold) - fabs(foldYM);

	if (fractal->transformCommon.functionEnabled)
	{
		z.z = fold - fabs(fabs(z.z + foldZM) - fold) - fabs(foldZM);
	}

	// Track color changes from box fold
	double colorAdd = 0.0;
	if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
	if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
	if (fractal->transformCommon.functionEnabled && z.z != oldZ.z)
		colorAdd += fractal->mandelbox.color.factor.z;

	// Step 3: Spherical inversion
	// rr = x^2 + y^2 + z^2
	// if rr < sqr(Min_R) then m = Scale/sqr(Min_R)
	// elif rr < 1 then m = Scale/rr
	// else m = 1
	double rr = z.Dot(z);
	double m = 1.0;
	double minRSqr = fractal->mandelbox.foldingSphericalMin * fractal->mandelbox.foldingSphericalMin;

	if (rr < minRSqr)
	{
		m = fractal->mandelbox.scale / minRSqr;
		colorAdd += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < 1.0)
	{
		m = fractal->mandelbox.scale / rr;
		colorAdd += fractal->mandelbox.color.factorSp2;
	}

	z *= m;
	aux.DE *= fabs(m);

	// Step 4: Scale by actualScale
	z *= actualScale;
	aux.DE = aux.DE * fabs(actualScale) + 1.0;

	// Step 5: Add constant c (with Cx/Cy swap - key ABox feature)
	// x = x * m + Cy  -> z.x += const_c.y
	// y = y * m + Cx  -> z.y += const_c.x
	// z = z * m + Cz  -> z.z += const_c.z
	z.x += aux.const_c.y * fractal->transformCommon.constantMultiplier111.x;
	z.y += aux.const_c.x * fractal->transformCommon.constantMultiplier111.y;
	z.z += aux.const_c.z * fractal->transformCommon.constantMultiplier111.z;

	// Color update
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux.color += colorAdd;
	}

	// Rotation
	if (fractal->transformCommon.rotationEnabled && aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}
}
