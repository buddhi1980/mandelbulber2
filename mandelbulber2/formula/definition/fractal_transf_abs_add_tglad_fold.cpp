/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * abs add tglad fold,  z = fabs( z + constant) -fabs( z - constant) - z:
 * with a fold tweak option
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfAbsAddTgladFold::cFractalTransfAbsAddTgladFold() : cAbstractFractal()
{
	nameInComboBox = "T>Abs Add Tglad Fold";
	internalName = "transf_abs_add_tglad_fold";
	internalID = fractal::transfAbsAddTgladFold;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfAbsAddTgladFold::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 limit = fractal->transformCommon.additionConstant000;
	double colorAdd = 0.0;
	CVector4 oldZ = z;
	z = fabs(z + limit) - fabs(z - limit) - z;

	if (fractal->transformCommon.functionEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsA
			&& aux.i < fractal->transformCommon.stopIterationsA)
	{
		CVector4 length = 2.0 * limit;
		CVector4 tgladS = 1.0 / length;
		CVector4 Add = CVector4(0.0, 0.0, 0.0, 0.0);
		;
		if (fabs(z.x) < limit.x) Add.x = z.x * z.x * tgladS.x;
		if (fabs(z.y) < limit.y) Add.y = z.y * z.y * tgladS.y;
		if (fabs(z.z) < limit.z) Add.z = z.z * z.z * tgladS.z;

		if (fabs(z.x) > limit.x && fabs(z.x) < length.x)
			Add.x = (length.x - fabs(z.x)) * (length.x - fabs(z.x)) * tgladS.x;
		if (fabs(z.y) > limit.y && fabs(z.y) < length.y)
			Add.y = (length.y - fabs(z.y)) * (length.y - fabs(z.y)) * tgladS.y;
		if (fabs(z.z) > limit.z && fabs(z.z) < length.z)
			Add.z = (length.z - fabs(z.z)) * (length.z - fabs(z.z)) * tgladS.z;
		Add *= fractal->transformCommon.offset000;
		z.x = (z.x - (sign(z.x) * (Add.x)));
		z.y = (z.y - (sign(z.y) * (Add.y)));
		z.z = (z.z - (sign(z.z) * (Add.z)));
	}
	// aux.color mode 1
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (z.x != oldZ.x) colorAdd += fractal->mandelbox.color.factor.x;
		if (z.y != oldZ.y) colorAdd += fractal->mandelbox.color.factor.y;
		if (z.z != oldZ.z) colorAdd += fractal->mandelbox.color.factor.z;
	}
	// mode 2
	if (fractal->transformCommon.functionEnabledCxFalse)
	{
		if (fabs(z.x) > limit.x)
		{
			colorAdd += fractal->mandelbox.color.factor.x;
		}
		else
		{
			colorAdd += fractal->mandelbox.color.factor.x * (1.0 - (limit.x - fabs(z.x)) / limit.x);
		}

		if (fabs(z.y) > limit.y)
		{
			colorAdd += fractal->mandelbox.color.factor.y;
		}
		else
		{
			colorAdd += fractal->mandelbox.color.factor.y * (1.0 - (limit.y - fabs(z.y)) / limit.y);
		}

		if (fabs(z.z) > limit.z)
		{
			colorAdd += fractal->mandelbox.color.factor.z;
		}
		else
		{
			colorAdd += fractal->mandelbox.color.factor.z * (1.0 - (limit.z - fabs(z.z)) / limit.z);
		}
	}

	// mode 3
	if (fractal->transformCommon.functionEnabledCyFalse)
	{
		// double valMinusLim = limit;
		if (z.x != oldZ.x)
		{
			colorAdd += fractal->mandelbox.color.factor.x * (fabs(z.x) - limit.x) / limit.x;
		}
		if (z.y != oldZ.y)
		{
			colorAdd += fractal->mandelbox.color.factor.y * (fabs(z.y) - limit.y) / limit.y;
		}
		if (z.z != oldZ.z)
		{
			colorAdd += fractal->mandelbox.color.factor.z * (fabs(z.z) - limit.z) / limit.z;
		}
	}
	aux.color += colorAdd;
}
