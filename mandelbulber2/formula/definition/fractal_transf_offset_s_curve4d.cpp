/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * offset s curve
 * This formula contains aux.color
 */

#include "all_fractal_definitions.h"

cFractalTransfOffsetSCurve4d::cFractalTransfOffsetSCurve4d() : cAbstractFractal()
{
	nameInComboBox = "T>Offset S-Curve 4D";
	internalName = "transf_offset_s_curve4d";
	internalID = fractal::transfOffsetSCurve4d;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfOffsetSCurve4d::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 limit = fractal->transformCommon.additionConstant0000 / 2.0;
	CVector4 oldZ = z;

	CVector4 temp = fractal->transformCommon.additionConstant0000 / -2.0;
	CVector4 temp2 = temp * temp;
	CVector4 offS = CVector4(0.0, 0.0, 0.0, 0.0);
	if (z.w < 1e-15 && z.w > -1e-15) z.w = (z.w > 0) ? 1e-15 : -1e-15;

	if (fractal->transformCommon.functionEnabledAx)
	{
		offS.x += ((8.0 * temp.x * temp2.x) / ((z.x * z.x) + (4.0 * temp2.x)) - 2.0 * temp.x)
							* sign(z.x) * fractal->transformCommon.scale1;
		z.x += offS.x;
	}

	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		offS.y += ((8.0 * temp.y * temp2.y) / ((z.y * z.y) + (4.0 * temp2.y)) - 2.0 * temp.y)
							* sign(z.y) * fractal->transformCommon.scale1;
		z.y += offS.y;
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		offS.z += ((8.0 * temp.z * temp2.z) / ((z.z * z.z) + (4.0 * temp2.z)) - 2.0 * temp.z)
							* sign(z.z) * fractal->transformCommon.scale1;
		z.z += offS.z;
	}

	if (fractal->transformCommon.functionEnabledAwFalse)
	{
		offS.w += ((8.0 * temp.w * temp2.w) / ((z.w * z.w) + (4.0 * temp2.w)) - 2.0 * temp.w)
							* sign(z.w) * fractal->transformCommon.scale1;
		z.w += offS.w;
	}

	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (!fractal->transformCommon.functionEnabledCxFalse)
		{
			if (z.x != oldZ.x) aux.color += fractal->mandelbox.color.factor4D.x * (fabs(z.x) - limit.x);
			if (z.y != oldZ.y) aux.color += fractal->mandelbox.color.factor4D.y * (fabs(z.y) - limit.y);
			if (z.z != oldZ.z) aux.color += fractal->mandelbox.color.factor4D.z * (fabs(z.z) - limit.z);
			if (z.w != oldZ.w) aux.color += fractal->mandelbox.color.factor4D.w * (fabs(z.w) - limit.w);
		}
		else
		{
			aux.color += fractal->mandelbox.color.factor4D.x * offS.x;
			aux.color += fractal->mandelbox.color.factor4D.y * offS.y;
			aux.color += fractal->mandelbox.color.factor4D.z * offS.z;
			aux.color += fractal->mandelbox.color.factor4D.w * offS.w;
		}
	}
}
