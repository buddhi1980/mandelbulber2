/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2021 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical fold MBox
 * This formula contains aux.color

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_transf_add_cpixel_sphere_fold.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 TransfAddCpixelSphereFoldIteration(
	REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL4 cv = aux->const_c;
	REAL rr = dot(cv, cv);
	cv += fractal->mandelbox.offset;

	if (rr < fractal->transformCommon.minR2p25)
	{
		cv *= fractal->transformCommon.maxR2d1 / fractal->transformCommon.minR2p25;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		cv *= fractal->transformCommon.maxR2d1 / rr;
	}
	// modes
	if (fractal->transformCommon.functionEnabledBFalse && rr >= fractal->transformCommon.maxR2d1)
		cv = cv * (1.0f - (fractal->transformCommon.maxR2d1 - rr) / rr);

	if (fractal->transformCommon.functionEnabledDFalse && rr >= fractal->transformCommon.maxR2d1)
		cv = cv * fractal->transformCommon.maxR2d1;

	if (fractal->transformCommon.functionEnabledCFalse && rr >= fractal->transformCommon.maxR2d1)
	{
		cv = cv * (1.0f - (rr - fractal->transformCommon.maxR2d1) / fractal->transformCommon.scale1);
	}

	if (fractal->transformCommon.functionEnabledAFalse)
	{
		cv.x = sign(cv.x) * min(fabs(cv.x), fractal->transformCommon.scaleA1);
		cv.y = sign(cv.y) * min(fabs(cv.y), fractal->transformCommon.scaleA1);
		cv.z = sign(cv.z) * min(fabs(cv.z), fractal->transformCommon.scaleA1);
	}

	cv -= fractal->mandelbox.offset;

	z += cv * fractal->transformCommon.constantMultiplier111;

	// Analytic DE tweak
	if (fractal->analyticDE.enabledFalse)
		aux->DE = aux->DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset0;

	// aux->color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		aux->color += fabs(cv.x * cv.y) * fractal->foldColor.difs0000.x;
		aux->color += (cv.x * cv.x + cv.y * cv.y) * fractal->foldColor.difs0000.y;
		aux->color += fabs(cv.z) * fractal->foldColor.difs0000.z;
	}
	return z;
}