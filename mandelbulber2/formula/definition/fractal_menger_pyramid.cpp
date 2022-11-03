/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsTorusTwistIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * M3D difs code by darkbeam
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
 */

#include "all_fractal_definitions.h"

cFractalMengerPyramid::cFractalMengerPyramid() : cAbstractFractal()
{
	nameInComboBox = "Menger Pyramid";
	internalName = "menger_pyramid";
	internalID = fractal::mengerPyramid;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalMengerPyramid::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	double temp;
	double ang;
	// polyfold
	if (fractal->transformCommon.functionEnabledCFalse
		&& aux.i >= fractal->transformCommon.startIterationsC
			&& aux.i < fractal->transformCommon.stopIterationsC1)
	{
		if (fractal->transformCommon.functionEnabledCxFalse)
		{
			z.y = fabs(z.y);
			ang = M_PI / fractal->transformCommon.int8X;
			ang = fabs(fmod(atan2(z.y, z.x) + ang, 2.0 * ang) - ang);
			temp = sqrt(z.x * z.x + z.y * z.y);
			z.x = cos(ang) * temp;
			z.y = sin(ang) * temp;
		}
		if (fractal->transformCommon.functionEnabledCyFalse)
		{
			z.z = fabs(z.z);
			ang = M_PI / fractal->transformCommon.int8Y;
			ang = fabs(fmod(atan2(z.z, z.y) + ang, 2.0 * ang) - ang);
			temp = sqrt(z.y * z.y + z.z * z.z);
			z.y = cos(ang) * temp;
			z.z = sin(ang) * temp;
		}
		if (fractal->transformCommon.functionEnabledCzFalse)
		{
			z.x = fabs(z.x);
			ang = M_PI / fractal->transformCommon.int8Z;
			ang = fabs(fmod(atan2(z.x, z.z) + ang, 2.0 * ang) - ang);
			temp = sqrt(z.z * z.z + z.x * z.x);
			z.z = cos(ang) * temp;
			z.x = sin(ang) * temp;
		}
		// addition constant
		z += fractal->transformCommon.additionConstant000;
	}

	if (aux.i >= fractal->transformCommon.startIterationsP
			&& aux.i < fractal->transformCommon.stopIterationsP1)
	{
		// pre abs
		if (fractal->transformCommon.functionEnabledx)
		{
			if (!fractal->transformCommon.functionEnabledBxFalse) z.x = fabs(z.x);
			else z.x = -fabs(z.x);
		}
		if (fractal->transformCommon.functionEnabledy)
		{
			if (!fractal->transformCommon.functionEnabledByFalse) z.y = fabs(z.y);
			else z.y = -fabs(z.y);
		}
		if (fractal->transformCommon.functionEnabledzFalse)
		{
			if (!fractal->transformCommon.functionEnabledBzFalse) z.z = fabs(z.z);
			else z.z = -fabs(z.z);
		}
		// addition constant
		z.x += fractal->transformCommon.offsetA05;
		z.y += fractal->transformCommon.offsetB05;
		z.z += fractal->transformCommon.offset0;

		z.x += z.z * fractal->transformCommon.scale05;
		z.y += z.z * fractal->transformCommon.offset05;

		if (fractal->transformCommon.functionEnabledAFalse)
		{

			aux.DE *= sqrt(z.x * z.x + z.y * z.y); // mmmmmmmmmmmmmmmmmmmm
		}
	}


	if (fractal->transformCommon.functionEnabledDFalse &&
			aux.i >= fractal->transformCommon.startIterationsD
			&& aux.i < fractal->transformCommon.stopIterationsD1)
	{
		if (!fractal->transformCommon.functionEnabledOFalse)
			ang = atan2(z.x, z.y) * fractal->transformCommon.scaleA1 * M_PI_2x_INV;
		else
			ang = atan2(z.y, z.x) * fractal->transformCommon.scaleA1 * M_PI_2x_INV;

		//if (fractal->transformCommon.functionEnabledTFalse)
			z.y = sqrt(z.x * z.x + z.y * z.y) - fractal->transformCommon.radius1;


		// vert helix
		if (fractal->transformCommon.functionEnabledAxFalse)
		{
			double Voff = fractal->transformCommon.offsetA2;
			temp = z.z - Voff * ang * fractal->transformCommon.int1 + Voff * 0.5;
			z.z = temp - Voff * floor(temp / (Voff)) - Voff * 0.5;
		}
		// stretch
		if (fractal->transformCommon.functionEnabledAyFalse)
		{
			if (!fractal->transformCommon.functionEnabledAy)
				z.x = fractal->transformCommon.offsetR1;
			else
			{
				temp = fractal->transformCommon.scaleA2 * ang + fractal->transformCommon.offsetR1;
				z.x = temp - 2.0 * floor(temp * 0.5) - 1.0;
			}
		}

		if (fractal->transformCommon.functionEnabledAzFalse)
		{
			if (fractal->transformCommon.functionEnabledAwFalse)
			{
				double Phi = z.z;
				double Rho = sqrt(z.x * z.x + z.y * z.y);

				ang = Rho * fractal->transformCommon.offsetA0 + Phi * fractal->transformCommon.offsetB0
							+ fractal->transformCommon.offsetC0;
			}
			double cosA = cos(ang);
			double sinB = sin(ang);
			if (!fractal->transformCommon.functionEnabledNFalse)
			{
				temp = z.x;
				z.x = z.y * cosA + z.x * sinB;
			}
			else
			{
				temp = z.z;
				z.z = z.y * cosA + z.z * sinB;
			}
			z.y = temp * cosA - z.y * sinB;
		}
	}

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		temp = z.x;
		z.x = z.y;
		z.y = temp;
	}

	// swap axis
	if (fractal->transformCommon.functionEnabledSFalse)
	{
		temp = z.x;
		z.x = z.z;
		z.z = temp;
	}

	// menger sponge
	CVector4 Offset = fractal->transformCommon.offset111;
	double Scale = fractal->transformCommon.scale3;

	z = fabs(z);
	// rotation
	if (aux.i >= fractal->transformCommon.startIterationsR
			&& aux.i < fractal->transformCommon.stopIterationsR1)
	{
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
	}

	double col = 0.0;
	if (z.x < z.y)
	{
		temp = z.y;
		z.y = z.x;
		z.x = temp;
		col += fractal->foldColor.difs0000.x;
	}
	if (z.x < z.z)
	{
		temp = z.z;
		z.z = z.x;
		z.x = temp;
		col += fractal->foldColor.difs0000.y;
	}
	if (z.y < z.z)
	{
		temp = z.z;
		z.z = z.y;
		z.y = temp;
		col += fractal->foldColor.difs0000.z;
	}
	if (fractal->foldColor.auxColorEnabledFalse
			&& aux.i >= fractal->foldColor.startIterationsA
					&& aux.i < fractal->foldColor.stopIterationsA)
	{
		aux.color += col;
	}

	// z = fractal->transformCommon.rotationMatrix2.RotateVector(z);

	z.z = fabs(z.z - FRAC_1_3 * Offset.z) + FRAC_1_3 * Offset.z;
	z = z * Scale - Offset * (Scale - 1.0);
	aux.DE = aux.DE * Scale;

	if (fractal->transformCommon.functionEnabledMFalse)
	{
		if ( z.z < (fractal->transformCommon.offsetA1 + 0.5) * fractal->transformCommon.offset2
			&& z.z > (fractal->transformCommon.offsetT1 + 0.5) * -fractal->transformCommon.offset2)
		{
			z.z -= round(z.z / fractal->transformCommon.offset2) * fractal->transformCommon.offset2;
			z.z = clamp(fabs(z.z), -fractal->transformCommon.offset1, fractal->transformCommon.offset1);

			// clip
			if (fractal->transformCommon.functionEnabledEFalse)
				z.z = max(aux.const_c.z - fractal->transformCommon.offsetE0, z.z);

			if (fractal->transformCommon.functionEnabledFFalse)
				z.z = min(aux.const_c.z + fractal->transformCommon.offsetF0, -z.z);
		}
	}

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	if (fractal->transformCommon.functionEnabledPFalse)
	{
		CVector4 zc = z;
		CVector4 d = fabs(zc);
		d.x = max(d.x - fractal->transformCommon.offsetA1, 0.0);
		d.y = max(d.y - fractal->transformCommon.offset01, 0.0);
		d.z = max(d.z - fractal->transformCommon.offsetp1, 0.0);

		double rDE;
		{
			if (!fractal->transformCommon.functionEnabledTFalse)
			{
				rDE = max(d.x, max(d.y, d.z));
			}
			else
			{
				rDE = d.Length();
			}
		}
		aux.dist = min(aux.dist, rDE / aux.DE);
	}

}
