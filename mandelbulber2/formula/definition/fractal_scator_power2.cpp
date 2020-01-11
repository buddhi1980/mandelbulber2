/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * quadratic iteration in real or imaginary scator algebra
 * @reference
 * http://www.fractalforums.com/new-theories-and-research/
 * ix-possibly-the-holy-grail-fractal-%28in-fff-lore%29
 * https://luz.izt.uam.mx/drupal/en/fractals/ix
 * @author Manuel Fernandez-Guasti
 * This formula contains aux.DE
 */

#include "all_fractal_definitions.h"

cFractalScatorPower2::cFractalScatorPower2() : cAbstractFractal()
{
	nameInComboBox = "Scator Power 2";
	internalName = "scator_power2";
	internalID = fractal::scatorPower2;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalScatorPower2::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	// r calc
	double r;
	CVector4 zz = z * z;
	if (fractal->transformCommon.functionEnabledXFalse)
	{
		r = aux.r;
	}
	else if (!fractal->transformCommon.functionEnabledYFalse)
	{
		r = sqrt(zz.x - zz.y - zz.z + (zz.y * zz.z) / zz.x);
	}
	else
	{ // this should be used for imaginary scators
		r = sqrt(zz.x + zz.y + zz.z + (zz.y * zz.z) / zz.x);
	}
	// Scator real enabled by default
	CVector4 newZ = z;
	// double temp1;
	if (!fractal->transformCommon.functionEnabledFalse)
	{ // scator real
		newZ.x = zz.x + zz.y + zz.z;
		newZ.y = z.x * z.y;
		newZ.z = z.x * z.z;
		newZ *= fractal->transformCommon.constantMultiplier122;
		// temp1 = newZ.Length();
		newZ.x += (zz.y * zz.z) / zz.x;
		newZ.y *= (1.0 + zz.z / zz.x);
		newZ.z *= (1.0 + zz.y / zz.x);
		// r = sqrt(zz.x - zz.y - zz.z + (zz.y * zz.z) / zz.x);
	}
	else
	{ // scator imaginary
		newZ.x = zz.x - zz.y - zz.z;
		newZ.y = z.x * z.y;
		newZ.z = z.x * z.z;
		newZ *= fractal->transformCommon.constantMultiplier122;
		// temp1 = newZ.Length();
		newZ.x += (zz.y * zz.z) / zz.x;
		newZ.y *= (1.0 - zz.z / zz.x);
		newZ.z *= (1.0 - zz.y / zz.x);
		// r = sqrt(zz.x + zz.y + zz.z + (zz.y * zz.z) / zz.x);
	}
	z = newZ;
	// double temp2 = newZ.Length();
	// temp2 = temp1 / temp2;
	/* aux.DE = aux.DE * 2.0 * aux.r;
	double newx = z.x * z.x - z.y * z.y - z.z * z.z;
	double newy = 2.0 * z.x * z.y;
	double newz = 2.0 * z.x * z.z;
	z.x = newx;
	z.y = newy;
	z.z = newz; */

	// addCpixel
	if (fractal->transformCommon.addCpixelEnabledFalse
			&& aux.i >= fractal->transformCommon.startIterationsE
			&& aux.i < fractal->transformCommon.stopIterationsE)
	{
		CVector4 c = aux.const_c;
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

	// analytic DE calc
	if (fractal->analyticDE.enabled)
	{
		if (!fractal->analyticDE.enabledFalse)
		{
			aux.DE = 2.0 * r * aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
		}
		else
		{ // vec3
			// rd calc
			double rd;
			zz = z * z;
			if (fractal->transformCommon.functionEnabledXFalse)
			{
				rd = z.Length();
			}
			else if (!fractal->transformCommon.functionEnabledYFalse)
			{
				rd = sqrt(zz.x - zz.y - zz.z + (zz.y * zz.z) / zz.x);
			}
			else
			{
				rd = sqrt(zz.x + zz.y + zz.z + (zz.y * zz.z) / zz.x);
			}
			double vecDE = fractal->transformCommon.scaleA1 * rd / r;
			aux.DE =
				max(r * 2.0, vecDE) * aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
		}
		aux.dist = 0.5 * r * log(r) / aux.DE;
	}
	// force bailout
	// double tp = fractal->transformCommon.scale1;
	// double tpz = fabs(z.z);
	// if (tpz < -tp && tpz > tp) tpz = (tpz > 0) ? z.z += 100000.0 : z.z -= 100000.0;
	if (r > fractal->transformCommon.scale2) z.z += 100000.0;
	aux.DE0 = r; // temp for testing
}
