/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2024 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsBoxIteration  fragmentarium code, mdifs by knighty (jan 2012)

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_transf_difs_box.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 TransfDIFSBoxIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL4 q = fabs(z) - fractal->transformCommon.additionConstant111;
	REAL4 zc = q;
	zc.x = max(zc.x, 0.0f);
	zc.y = max(zc.y, 0.0f);
	zc.z = max(zc.z, 0.0f);
	REAL zcd = length(zc);
	REAL colDist = aux->dist;
	aux->dist = min(aux->dist, zcd / (aux->DE + 1.0f) - fractal->transformCommon.offsetB0);

	if (fractal->foldColor.auxColorEnabledFalse && aux->i >= fractal->foldColor.startIterationsA
			&& aux->i < fractal->foldColor.stopIterationsA)
	{
		if (!fractal->foldColor.auxColorEnabledAFalse)
		{
			if (colDist != aux->dist) aux->color += fractal->foldColor.difs0000.x;
		}
		else
		{
			if (zc.x > max(zc.y, zc.z)) aux->color += fractal->foldColor.difs0000.y;
			if (zc.y > max(zc.x, zc.z)) aux->color += fractal->foldColor.difs0000.z;
			if (zc.z > max(zc.y, zc.x)) aux->color += fractal->foldColor.difs0000.w;
			/*if (fractal->foldColor.difs0000.y != 0.0f && zc.x == fabs(q.x))
				aux->color += fractal->foldColor.difs0000.y;
			if (fractal->foldColor.difs0000.z != 0.0f && zc.y == fabs(q.y))
				aux->color += fractal->foldColor.difs0000.z;
			if (fractal->foldColor.difs0000.w != 0.0f && zc.z == fabs(q.z))
				aux->color += fractal->foldColor.difs0000.w;*/

			if (fractal->foldColor.difs0 != 0.0f)
			{
				REAL t = z.x * z.y;
				if ((t > 0.0f && z.z > 0.0f) || (t < 0.0f && z.z < 0.0f))
					aux->color += fractal->foldColor.difs0;
			}
		}
	}
	return z;
}