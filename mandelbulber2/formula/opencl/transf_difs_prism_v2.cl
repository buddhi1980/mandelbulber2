/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2025 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDIFSPrismV2Iteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_transf_difs_prism_v2.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 TransfDIFSPrismV2Iteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.functionEnabledxFalse) z.x = -fabs(z.x);
	if (fractal->transformCommon.functionEnabledyFalse) z.y = -fabs(z.y);
	if (fractal->transformCommon.functionEnabledzFalse) z.z = -fabs(z.z);

	REAL4 zc = z;

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		REAL temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}
	REAL tp = 0.0f;
	REAL absZ = fabs(zc.z);
	REAL len = fractal->transformCommon.offset1;
	REAL face = fractal->transformCommon.offset05;

	if (fractal->transformCommon.functionEnabledMFalse)
	{
		tp = absZ;
		if (fractal->transformCommon.functionEnabledCxFalse) tp *= tp;
		zc.y += tp * fractal->transformCommon.scale0;
	}

	if (fractal->transformCommon.functionEnabledNFalse)
	{
		tp = absZ;
		if (fractal->transformCommon.functionEnabledCyFalse) tp *= tp;
		len += tp * fractal->transformCommon.scaleA0;
	}

	if (fractal->transformCommon.functionEnabledOFalse)
	{
		tp = absZ;
		if (fractal->transformCommon.functionEnabledCzFalse) tp *= tp;
		face += tp * fractal->transformCommon.scaleB0;
	}
	REAL priX = max(fabs(zc.y) * SQRT_3_4_F + zc.z * 0.5f, -zc.z) - face;
	face = fabs(priX);
	if (fractal->transformCommon.functionEnabledFalse)
	{
		tp = face - fractal->transformCommon.offsetp01;
		priX = max(priX, tp);
	}

	REAL priD = max(fabs(zc.x) - len, priX);

	REAL colDist = aux->dist;
	aux->dist = min(aux->dist, priD / (aux->DE + fractal->analyticDE.offset0));

	if (fractal->foldColor.auxColorEnabledFalse && colDist != aux->dist
			&& aux->i >= fractal->foldColor.startIterationsA
			&& aux->i < fractal->foldColor.stopIterationsA)
	{
		REAL addCol = fractal->foldColor.difs0000.y;
		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (priX == tp) addCol = fractal->foldColor.difs0000.z;
			//	if (tp > fractal->transformCommon.offsetp01)
			//		addCol = fractal->foldColor.difs0000.z;
			if (fractal->transformCommon.offset1 - fractal->foldColor.difs0 < fabs(zc.x))
				addCol = fractal->foldColor.difs0000.w;
		}

		if (!fractal->foldColor.auxColorEnabledBFalse)
		{
			aux->color = addCol;
		}
		else
		{
			aux->color += addCol + fractal->foldColor.difs0000.x; // aux->color default 1
		}
	}
	return z;
}