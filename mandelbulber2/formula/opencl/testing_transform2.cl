/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2019 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Testing transform2
 *

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the function "TestingTransform2Iteration" in the file fractal_formulas.cpp
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 TestingTransform2Iteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL colorAdd = 0.0f;
	REAL dd = 0.0f;

	if (fractal->transformCommon.functionEnabledAx) dd = length(z); // eucli norm

	if (fractal->transformCommon.functionEnabledAxFalse) dd = aux->DE0;

	if (fractal->transformCommon.functionEnabledAyFalse)
	{
		REAL4 absZ = fabs(z);
		dd = max(absZ.x, max(absZ.y, absZ.z)); // infnorm
	}
	if (fractal->transformCommon.functionEnabledAzFalse)
	{
		REAL4 zz = z * z;
		dd = max(native_sqrt(zz.x + zz.y), max(native_sqrt(zz.y + zz.z), native_sqrt(zz.z + zz.x)));
	}

	dd += fractal->transformCommon.offset0;

	aux->DE = aux->DE + fractal->analyticDE.offset0;

	if (fractal->transformCommon.functionEnabledAy)
	{
		dd = native_divide(dd, aux->DE); // same as an uncondtional aux->dist
	}
	if (fractal->transformCommon.functionEnabledBFalse)
	{
		REAL rxy = native_sqrt(mad(z.x, z.x, z.y * z.y));
		dd =
			maxnative_divide((rxy - aux->pseudoKleinianDE, native_divide(fabs(rxy * z.z), dd)), aux->DE);
	}
	if (fractal->transformCommon.functionEnabledCFalse)
	{
		dd = 0.5f * dd * native_divide(log(dd), aux->DE);
	}
	if (fractal->transformCommon.functionEnabledDFalse)
	{
		REAL logD = 0.5f * dd * native_divide(log(dd), aux->DE);
		REAL linD = native_divide(dd, aux->DE);
		dd = mad(fractal->transformCommon.scale1, (linD - logD), logD);
	}
	if (fractal->transformCommon.functionEnabledEFalse)
	{
		REAL rxy = native_sqrt(mad(z.x, z.x, z.y * z.y));
		REAL pkD =
			maxnative_divide((rxy - aux->pseudoKleinianDE, native_divide(fabs(rxy * z.z), dd)), aux->DE);
		REAL linD = native_divide(dd, aux->DE);
		dd = mad(fractal->transformCommon.scaleA1, (linD - pkD), pkD);
	}
	if (fractal->transformCommon.functionEnabledFFalse)
	{
		REAL rxy = native_sqrt(mad(z.x, z.x, z.y * z.y));
		REAL pkD =
			maxnative_divide((rxy - aux->pseudoKleinianDE, native_divide(fabs(rxy * z.z), dd)), aux->DE);
		REAL logD = 0.5f * dd * native_divide(log(dd), aux->DE);
		dd = mad(fractal->transformCommon.scaleB1, (pkD - logD), logD);
	}

	REAL colorDist = aux->dist;
	if (!fractal->transformCommon.functionEnabledFalse)
	{
		aux->colorHybrid = dd;
	}
	else
	{
		int tempC = fractal->transformCommon.int3X;
		if (aux->i < tempC || dd < aux->colorHybrid)
		{
			aux->colorHybrid = dd;
		}
	}
	aux->dist = aux->colorHybrid;

	// aux->color
	if (fractal->foldColor.auxColorEnabled)
	{
		if (fractal->foldColor.auxColorEnabledFalse)
		{
			colorAdd += fractal->foldColor.difs0000.x * fabs(z.x * z.y); // fabs(zc.x * zc.y)
			colorAdd += fractal->foldColor.difs0000.y * max(z.x, z.y);	 // max(z.x, z.y);
			colorAdd += fractal->foldColor.difs0000.z * (mad(z.x, z.x, z.y * z.y));
			// colorAdd += fractal->foldColor.difs0000.w * fabs(zc.x * zc.y);
		}
		colorAdd += fractal->foldColor.difs1;
		if (fractal->foldColor.auxColorEnabledA)
		{
			if (colorDist != aux->dist) aux->color += colorAdd;
		}
		else
			aux->color += colorAdd;
	}
	return z;
}