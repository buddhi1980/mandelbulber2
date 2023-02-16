/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * rotate about vec3
 */

#include "all_fractal_definitions.h"

cFractalTransfRotateAboutVec3::cFractalTransfRotateAboutVec3() : cAbstractFractal()
{
	nameInComboBox = "T>Rotation About Vec3";
	internalName = "transf_rotate_about_vec3";
	internalID = fractal::transfRotateAboutVec3;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfRotateAboutVec3::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double useAngle = fractal->transformCommon.angle0;

	if (fractal->transformCommon.functionEnabledEFalse)
	{
		if (aux.i > fractal->transformCommon.startIterationsRV)
		{
			useAngle *= (1.0
										- 1.0
												/ (1.0
													 + (aux.i - fractal->transformCommon.startIterationsRV)
															 / fractal->transformCommon.offset0))
									* fractal->transformCommon.scale1;
		}
	}

	useAngle *= M_PI_180;
	if (!fractal->transformCommon.functionEnabledFFalse)
	{
		CVector4 v = fractal->transformCommon.vec111;
		v = v / v.Length(); // normalise
		double c = cos(useAngle);
		double temp = 1.0 - c;
		double s = sin(useAngle);
		CVector4 rotVec = CVector4(0.0, 0.0, 0.0, z.w);

		rotVec.x = z.x * (c + temp * v.x * v.x) + z.y * (temp * v.x * v.y + s * v.z)
							 + z.z * (temp * v.x * v.z - s * v.y);
		rotVec.y = z.x * (temp * v.x * v.y - s * v.z) + z.y * (c + temp * v.y * v.y)
							 + z.z * (temp * v.y * v.z + s * v.x);
		rotVec.z = z.x * (temp * v.x * v.z + s * v.y) + z.y * (temp * v.y * v.z - s * v.x)
							 + z.z * (c + temp * v.z * v.z);
		z = rotVec;
	}
	else
	{
		z = z.RotateAroundVectorByAngle(fractal->transformCommon.vec111.GetXYZ(), -useAngle);
	}
	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
