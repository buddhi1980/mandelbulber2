/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * EiffieCar
 */

#include "all_fractal_definitions.h"

cFractalEiffieCar::cFractalEiffieCar() : cAbstractFractal()
{
	nameInComboBox = "Eiffie Car";
	internalName = "eiffie_car";
	internalID = fractal::eiffieCar;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalEiffieCar::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	Q_UNUSED(&z);

	CVector4 p0 = aux.const_c;
	CVector4 p = p0;

	p.z = p0.z + fractal->transformCommon.scale015;

	double r = sqrt(p.z * p.z + p.x * p.x);

	CVector4 tp = p0;
	tp.y = max(fabs(p.y) - fractal->transformCommon.scale05, 0.0);
	tp.z = max(r - fractal->transformCommon.offset2, 0.0);
	tp.x = max(-p.z + fractal->transformCommon.offset105, 0.0);
	double d = tp.Length() - fractal->transformCommon.offset01; // round
	d = max(d, p.x - fractal->transformCommon.scale1p1); // boot
	if (fractal->transformCommon.functionEnabledDFalse) d = 100.0;


	p.z = p0.z - fractal->transformCommon.offsetD0; // locate z
	p.x = p0.x + fractal->transformCommon.minR2p25; //wheels and guards locate x
	p.y = fabs(fabs(p.y) - fractal->transformCommon.minR06); // locate y
	p.x = fabs(p.x) - fractal->transformCommon.offsetR1; //wheels and guards dist apart
	r = sqrt(p.z * p.z + p.x * p.x);
	tp.x = max(r - fractal->transformCommon.scale025, 0.0); // guard rad
	tp.y = max(p.y - fractal->transformCommon.offsetBp01 - 0.05, 0.0);
	tp.z = max(-p.z - 0.08, 0.0); // mmmmmmmmmmmmmmmmmmmmm

	// IQ's smooth minium function.
	if (!fractal->transformCommon.functionEnabledEFalse)
	{
		d = -log(exp(-fractal->transformCommon.scale8 * d)
				 + exp(-fractal->transformCommon.scale8 * (tp.Length() - 0.04)))
				/ fractal->transformCommon.scale8;
	}
	else
	{

	//	double b = tp.Length() - 0.04;
	//	double h = clamp( 0.5 + 0.5 * (b - d) / 0.2, 0. , 1.0);
	//	d = mix(b, d, h) - h * (1.0 - h) * 0.2;
	}

	d = max(d, -max(p.y - 0.165, r - 0.24)); // guard and car
double zcd = 0.0;
	if (!fractal->transformCommon.functionEnabledFFalse)
	{
		CVector4 q = fabs(p0) - fractal->transformCommon.offset100;
		CVector4 zc = q;

		zc.x = max(zc.x, 0.0);

		zc.y -=  fractal->transformCommon.minR06 + 2.0f * (fractal->transformCommon.offsetBp01 - 0.05f);
		zc.y = max(zc.y, 0.0);
		zc.z = max(zc.z, 0.0);
		zcd = zc.Length();

		zcd -= fractal->transformCommon.offsetB0;
		d = min(zcd, d);
	}

	//wheels
	CVector4 zc = p;
	double cylR = sqrt(zc.x * zc.x + zc.z * zc.z) - fractal->transformCommon.offset02; // tyre OD is offseto2
	cylR = max(fabs(cylR) - fractal->transformCommon.offsetp05, cylR); // tyre
	double cylH = fabs(zc.y) - fractal->transformCommon.offsetp1; //tyre width

	cylR = max(cylR, 0.0);
	cylH = max(cylH, 0.0);
	double cylD = sqrt(cylR * cylR + cylH * cylH);
	cylD = min(max(cylR, cylH), 0.0) + cylD;

	double d2 = cylD - 0.02; // round tyre
	if (fractal->transformCommon.functionEnabledCFalse)  d2 = 100.;
//	REAL d2 = sqrt(tp.y * tp.y + tp.z * tp.z) - .02;

	d = min(d, d2);
	aux.dist = d;

	// aux->color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (aux.dist == d2) aux.color = 3.0 + fractal->foldColor.difs0000.y;
		else aux.color = 5.0 + fractal->foldColor.difs0000.x;

		if (aux.dist == zcd)
			aux.color = 4.0 + fractal->foldColor.difs0000.z;

	}

}
