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

		CVector4 v = fractal->transformCommon.vec111;
		v = v / v.Length(); // normalise
		double c = cos(useAngle * M_PI_180);
		double s = sin(useAngle * M_PI_180);
		CVector4 rotVec = CVector4(0.0, 0.0, 0.0, z.w);

		rotVec.x = z.x * (c + (1.0 - c) * v.x * v.x) + z.y * ((1.0 - c) * v.x * v.y + s * v.z)
							 + z.z * ((1.0 - c) * v.x * v.z - s * v.y);
		rotVec.y = z.x * ((1.0 - c) * v.x * v.y - s * v.z) + z.y * (c + (1.0 - c) * v.y * v.y)
							 + z.z * ((1.0 - c) * v.y * v.z + s * v.x);
		rotVec.z = z.x * ((1.0 - c) * v.x * v.z + s * v.y) + z.y * ((1.0 - c) * v.y * v.z - s * v.x)
							 + z.z * (c + (1.0 - c) * v.z * v.z);
		z = rotVec;
	}
	else
	{
		CVector4 v = fractal->transformCommon.vec111;
		v = v / v.Length(); // normalise
		double c = cos(useAngle * M_PI_180);
		double s = sin(useAngle * M_PI_180);
		CVector4 rotVec = CVector4(0.0, 0.0, 0.0, z.w);

		rotVec.x = z.x * (c + (1.0 - c) * v.x * v.x) + z.y * ((1.0 - c) * v.x * v.y + s * v.z)
							 + z.z * ((1.0 - c) * v.x * v.z - s * v.y);
		rotVec.y = z.x * ((1.0 - c) * v.x * v.y - s * v.z) + z.y * (c + (1.0 - c) * v.y * v.y)
							 + z.z * ((1.0 - c) * v.y * v.z + s * v.x);
		rotVec.z = z.x * ((1.0 - c) * v.x * v.z + s * v.y) + z.y * ((1.0 - c) * v.y * v.z - s * v.x)
							 + z.z * (c + (1.0 - c) * v.z * v.z);
		z = rotVec;

		/*	CVector4 v = fractal->transformCommon.vec111;
			v = v / v.Length(); // normalise
			double c = cos(useAngle * M_PI_180);
			double s = sin(useAngle * M_PI_180);
			CVector4 rotVec = CVector4(0.0, 0.0, 0.0, z.w);

			CMatrix44 rotM = CMatrix44(c + (1.0 - c) * v.x * v.x, (1.0 - c) * v.x * v.y - s * v.z, (1.0 -
			c) * v.x * v.z + s * v.y, 0.0, (1.0 - c) * v.x * v.y + s * v.z, c + (1.0 - c) * v.y * v.y,
			(1.0
			- c) * v.y * v.z - s * v.x, 0.0, (1.0 - c) * v.x * v.z - s * v.y, (1.0 - c) * v.y * v.z + s *
			v.x, c + (1.0 - c) * v.z * v.z, 0.0, 0.0, 0.0, 0.0, 0.0
				);
			z *= rotM;*/
	}

	// DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
