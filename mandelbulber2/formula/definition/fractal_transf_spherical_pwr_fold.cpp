/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical pwr fold
 * This formula contains aux.color and analytic aux.DE
 */

#include "all_fractal_definitions.h"

cFractalTransfSphericalPwrFold::cFractalTransfSphericalPwrFold() : cAbstractFractal()
{
	nameInComboBox = "T>Spherical Pwr Fold";
	internalName = "transf_spherical_pwr_fold";
	internalID = fractal::transfSphericalPwrFold;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfSphericalPwrFold::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// if (z.x > -1e-21 && z.x < 1e-21) z.x = (z.x > 0) ? 1e-21 : -1e-21;
	// if (z.y > -1e-21 && z.y < 1e-21) z.y = (z.y > 0) ? 1e-21 : -1e-21;
	// if (z.z > -1e-21 && z.z < 1e-21) z.z = (z.z > 0) ? 1e-21 : -1e-21;
	double rr = pow(pow(z.x, fractal->transformCommon.pwr4) + pow(z.y, fractal->transformCommon.pwr4)
										+ pow(z.z, fractal->transformCommon.pwr4),
		fractal->transformCommon.pwr05);

	// if (rr < 1e-21 && rr > -1e-21)
	//	rr = (rr > 0) ? 1e-21 : -1e-21;
	if (rr < fractal->mandelbox.mR2)
	{
		z *= fractal->mandelbox.mboxFactor1;
		aux.DE *= fractal->mandelbox.mboxFactor1;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp1;
		}
	}
	else if (rr < fractal->mandelbox.fR2)
	{
		double tglad_factor2 = fractal->mandelbox.fR2 / rr;
		z *= tglad_factor2;
		aux.DE *= tglad_factor2;
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			aux.color += fractal->mandelbox.color.factorSp2;
		}
	}
}
