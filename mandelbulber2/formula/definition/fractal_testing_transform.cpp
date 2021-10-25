/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Based on a DarkBeam fold formula adapted by Knighty
 * MandalayBox  Fragmentarium /Examples/ Knighty Collection
 */

#include "all_fractal_definitions.h"

cFractalTestingTransform::cFractalTestingTransform() : cAbstractFractal()
{
	nameInComboBox = "Testing Transform";
	internalName = "testing_transform";
	internalID = fractal::testingTransform;
	DEType = analyticDEType;
	DEFunctionType = linearDEFunction;
	cpixelAddition = cpixelEnabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionLinear;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTestingTransform::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{

	if (fractal->transformCommon.functionEnabledFalse)
	{
		if (fractal->transformCommon.functionEnabledxFalse) z.x = fabs(z.x);
		if (fractal->transformCommon.functionEnabledyFalse) z.y = fabs(z.y);
		if (fractal->transformCommon.functionEnabledzFalse) z.z = fabs(z.z);
		z += fractal->transformCommon.offsetA000;
		z = fractal->transformCommon.rotationMatrix.RotateVector(z);
		z *= fractal->transformCommon.scale1;
		aux.DE *= fractal->transformCommon.scale1;
	}






	CVector4 signs = z;
	signs.x = sign(z.x);
	signs.y = sign(z.y);
	signs.z = sign(z.z);
	signs.w = sign(z.w);

	CVector4 out = CVector4(0.0, 0.0, 0.0, 0.0);
	CVector4 in = out;
	if (aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		out = fractal->transformCommon.offset000;
		if (fractal->transformCommon.functionEnabledAz)
			out *= signs;
	}
	if (aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
	{
		in = fractal->transformCommon.offsetF000;
		if (fractal->transformCommon.functionEnabledBz)
			in *= signs;
	}
	if (fractal->transformCommon.functionEnabledAx) z -= out;


	double m = 0.0f;
	double rr = z.Dot(z);
	if (fractal->transformCommon.functionEnabledAy) z -= in;


	if (rr < fractal->transformCommon.minR2p25)
	{
		m = fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
		z *= m;
		aux.DE *= m;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		m = fractal->transformCommon.maxR2d1 / rr;
		z *= m;
		aux.DE *= m;
	}

	if (fractal->transformCommon.functionEnabledBx) z += out;


	if (fractal->transformCommon.functionEnabledBy) z += in;

















	/*CVector4 signs ;
	signs.x = sign(aux.const_c.x);
	signs.y = sign(aux.const_c.y);
	signs.z = sign(aux.const_c.z);
	signs.w = 0.0;




	if (fractal->transformCommon.functionEnabledBx
			&& aux.i >= fractal->transformCommon.startIterationsX
			&& aux.i < fractal->transformCommon.stopIterationsX)
	{
		double sm = fractal->mandelbox.sharpness;
		double zk1 = SmoothConditionAGreaterB(z.x, fractal->mandelbox.foldingLimit, sm);
		double zk2 = SmoothConditionALessB(z.x, -fractal->mandelbox.foldingLimit, sm);
		z.x = z.x * (1.0 - zk1) + (fractal->mandelbox.foldingValue - z.x) * zk1;
		z.x = z.x * (1.0 - zk2) + (-fractal->mandelbox.foldingValue - z.x) * zk2;
		aux.color += (zk1 + zk2) * fractal->mandelbox.color.factor.x;

		double zk3 = SmoothConditionAGreaterB(z.y, fractal->mandelbox.foldingLimit, sm);
		double zk4 = SmoothConditionALessB(z.y, -fractal->mandelbox.foldingLimit, sm);
		z.y = z.y * (1.0 - zk3) + (fractal->mandelbox.foldingValue - z.y) * zk3;
		z.y = z.y * (1.0 - zk4) + (-fractal->mandelbox.foldingValue - z.y) * zk4;
		aux.color += (zk3 + zk4) * fractal->mandelbox.color.factor.y;

		if (fractal->transformCommon.functionEnabledBz)
		{
			double zk5 = SmoothConditionAGreaterB(z.z, fractal->mandelbox.foldingLimit, sm);
			double zk6 = SmoothConditionALessB(z.z, -fractal->mandelbox.foldingLimit, sm);
			z.z = z.z * (1.0 - zk5) + (fractal->mandelbox.foldingValue - z.z) * zk5;
			z.z = z.z * (1.0 - zk6) + (-fractal->mandelbox.foldingValue - z.z) * zk6;
			aux.color += (zk5 + zk6) * fractal->mandelbox.color.factor.z;
		}
	}

	if (fractal->transformCommon.functionEnabledBy
			&& aux.i >= fractal->transformCommon.startIterationsY
			&& aux.i < fractal->transformCommon.stopIterationsY)
	{
		double sms = fractal->transformCommon.scale3;
		double r2 = z.Dot(z);
		double tglad_factor2 = fractal->mandelbox.fR2 / r2;
		double rk1 = SmoothConditionALessB(r2, fractal->mandelbox.mR2, sms);
		double rk2 = SmoothConditionALessB(r2, fractal->mandelbox.fR2, sms);
		double rk21 = (1.0 - rk1) * rk2;

		z = z * (1.0 - rk1) + z * (fractal->mandelbox.mboxFactor1 * rk1);
		z = z * (1.0 - rk21) + z * (tglad_factor2 * rk21);
		aux.DE = aux.DE * (1.0 - rk1) + aux.DE * (fractal->mandelbox.mboxFactor1 * rk1);
		aux.DE = aux.DE * (1.0 - rk21) + aux.DE * (tglad_factor2 * rk21);
		aux.color += rk1 * fractal->mandelbox.color.factorSp1;
		aux.color += rk21 * fractal->mandelbox.color.factorSp2;
	}*/

	/*if (fractal->mandelbox.mainRotationEnabled) z = fractal->mandelbox.mainRot.RotateVector(z);
	z = z * fractal->mandelbox.scale;

	aux.DE = aux.DE * fabs(fractal->mandelbox.scale) + 1.0;*/





	if (fractal->analyticDE.enabledFalse)
		aux.DE = aux.DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;
}
