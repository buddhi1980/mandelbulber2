/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Gnarl deformation in 2D (from Mark Townsend), and Luca 2011
 * @reference
 * http://www.fractalforums.com/mandelbulb-3d/custom-formulas-and-transforms-release-t17106/
 */

#include "all_fractal_definitions.h"

cFractalTransfGnarl::cFractalTransfGnarl() : cAbstractFractal()
{
	nameInComboBox = "T>Gnarl";
	internalName = "transf_gnarl";
	internalID = fractal::transfGnarl;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfGnarl::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(aux);
	CVector4 oldZ = z;
	CVector4 tempZ = z;
	double Scale = fractal->transformCommon.scale1;
	double stepX = fractal->transformCommon.scale3D000.x; //-0.1;
	double stepY = fractal->transformCommon.scale3D000.y;
	double stepZ = fractal->transformCommon.scale3D000.z;
	double Alpha = fractal->transformCommon.rotation.x; // 2.0;
	double Beta = fractal->transformCommon.rotation.y;	//-4.0;
	double Gamma = fractal->transformCommon.rotation.z; //-0.1;

	if (fractal->transformCommon.functionEnabledAx)
	{
		tempZ.x = z.x - stepX * sin(z.y + sin(Alpha * (z.y + sin(Beta * z.y))));
		tempZ.y = z.y - stepY * sin(z.x + sin(Alpha * (z.x + sin(Beta * z.x))));
		z = tempZ;
	}

	if (fractal->transformCommon.functionEnabledAxFalse)
	{
		double xx = z.x * z.x;
		tempZ.x = z.x + stepX * (sin(Gamma * (z.y - xx) + sin(Alpha * (z.y + Beta * cos(z.y)))));
		tempZ.y = z.y + stepY * (sin(Gamma * (z.y + xx) - Alpha * sin(xx + Beta * cos(xx))));
		z = tempZ;
	}

	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		double xx = z.x * z.x;
		double yy = z.y * z.y;
		tempZ.y = xx + stepY * (sin(yy * sqrt(fabs(z.y)) - Alpha * sin((yy + sin(Beta * yy)))));
		tempZ.x = yy - stepX * (sin(xx * sqrt(fabs(xx)) + sin(Alpha * (xx + sin(Beta * xx)))));
		z = tempZ;
	}

	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		tempZ.x = z.x - stepX * sin(z.z + sin(Alpha * (z.z + sin(Beta * z.z))));
		tempZ.y = z.y - stepY * sin(z.x + sin(Alpha * (z.x + sin(Beta * z.x))));
		tempZ.z = (z.z - stepZ * sin(z.y + sin(Alpha * (z.y + sin(Beta * z.y))))) * Scale;
		z = tempZ;
	}
	z.x *= Scale;
	z.y *= Scale;

	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
			aux.DE = aux.DE * fabs(Scale) * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		else
		{
			double avgScale = z.Length() / oldZ.Length();
			aux.DE = aux.DE * avgScale * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
		}
	}
}
