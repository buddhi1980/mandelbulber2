/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * platonic solid - from DarkBeam's code
 * @reference
 * http://www.fractalforums.com/3d-fractal-generation/platonic-dimensions/msg36528/#msg36528
 */

#include "all_fractal_definitions.h"

cFractalTransfPlatonicSolid::cFractalTransfPlatonicSolid() : cAbstractFractal()
{
	nameInComboBox = "T>Platonic Solid";
	internalName = "transf_platonic_solid";
	internalID = fractal::transfPlatonicSolid;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfPlatonicSolid::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double platonicR = 0.0;
	double rho = 0.0;
	if (!fractal->transformCommon.functionEnabledFalse)
	{
		rho = sqrt(aux.r); // the radius
		double theta =
			cos(fractal->platonicSolid.frequency * z.x) * sin(fractal->platonicSolid.frequency * z.y)
			+ cos(fractal->platonicSolid.frequency * z.y) * sin(fractal->platonicSolid.frequency * z.z)
			+ cos(fractal->platonicSolid.frequency * z.z) * sin(fractal->platonicSolid.frequency * z.x);
		platonicR = theta * fractal->platonicSolid.amplitude + rho * fractal->platonicSolid.rhoMul;
		z *= platonicR;
	}
	else
	{
		double rho1 = 0.0;
		double rho2 = 0.0;
		double rho3 = 0.0;
		if (fractal->transformCommon.functionEnabledx) rho1 = sqrt(aux.r);
		if (fractal->transformCommon.functionEnabledyFalse) rho2 = aux.r;
		if (fractal->transformCommon.functionEnabledzFalse) rho3 = aux.r * aux.r;

		if (fractal->transformCommon.functionEnabledAxFalse)
			rho = rho1 + (rho2 - rho1) * fractal->transformCommon.scale1;
		else if (fractal->transformCommon.functionEnabledAyFalse)
			rho = rho2 + (rho3 - rho2) * fractal->transformCommon.scale1;
		else
			rho = rho1 + rho2 + rho3;

		double theta =
			cos(fractal->platonicSolid.frequency * z.x) * sin(fractal->platonicSolid.frequency * z.y)
			+ cos(fractal->platonicSolid.frequency * z.y) * sin(fractal->platonicSolid.frequency * z.z)
			+ cos(fractal->platonicSolid.frequency * z.z) * sin(fractal->platonicSolid.frequency * z.x);
		// theta is pos or neg && < 3.0

		if (fractal->transformCommon.functionEnabledAzFalse) theta = fabs(theta);

		platonicR = theta * fractal->platonicSolid.amplitude + rho * fractal->platonicSolid.rhoMul;

		z *= platonicR; //  can be neg
	}

	if (fractal->analyticDE.enabled)
	{
		aux.DE = aux.DE * fabs(platonicR) * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	}
}
