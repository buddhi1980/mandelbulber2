/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2024 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsCylinderIteration  fragmentarium code, mdifs by knighty (jan 2012)
 * and http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_transf_difs_cylinder.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 TransfDIFSCylinderIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL4 zc = z;

	// swap axis
	if (fractal->transformCommon.functionEnabledSFalse)
	{
		REAL temp = zc.x;
		zc.x = zc.y;
		zc.y = temp;
	}

	// swap axis
	if (fractal->transformCommon.functionEnabledSwFalse)
	{
		REAL temp = zc.x;
		zc.x = zc.z;
		zc.z = temp;
	}

	REAL xyR = native_sqrt(zc.x * zc.x + zc.y * zc.y) - fractal->transformCommon.radius1;
	REAL cylR = xyR;
	if (fractal->transformCommon.functionEnabledFalse)
	{
		cylR = fabs(cylR) - fractal->transformCommon.offset0;
		if (fractal->transformCommon.functionEnabledAFalse) cylR = max(cylR, xyR);
	}

	REAL cylH = fabs(zc.z) - fractal->transformCommon.offsetA1;

	cylR = max(cylR, 0.0f);
	cylH = max(cylH, 0.0f);
	REAL cylD = native_sqrt(cylR * cylR + cylH * cylH);
	cylD = min(max(cylR, cylH), 0.0f) + cylD;

	REAL colDist = aux->dist;
	aux->dist = min(aux->dist, cylD / (aux->DE + 1.0f));

	if (fractal->foldColor.auxColorEnabledFalse && aux->i >= fractal->foldColor.startIterationsA
			&& aux->i < fractal->foldColor.stopIterationsA)
	{
		if (colDist != aux->dist) aux->color += fractal->foldColor.difs0000.x;

		if (fractal->foldColor.auxColorEnabledAFalse)
		{
			if (fractal->transformCommon.offsetA1 < fabs(zc.z))
				aux->color += fractal->foldColor.difs0000.y;
			if (xyR < -fractal->transformCommon.offset0) aux->color += fractal->foldColor.difs0000.z;
			if (fractal->transformCommon.offsetA1 - fractal->foldColor.difs0 < fabs(zc.z))
				aux->color += fractal->foldColor.difs0000.w;
		}
	}
	return z;
}