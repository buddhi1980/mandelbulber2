/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2024 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * T>Juliabox
 * Folds, scaling, rotation and offsets
 * from Mandelbox fractal known as AmazingBox or ABox, invented by Tom Lowe in 2010
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/amazing-fractal/msg12467/#msg12467

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_transf_juliabox.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 TransfJuliaboxIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL colorAdd = 0.0f;

	REAL4 oldZ = z;
	if (aux->i >= fractal->transformCommon.startIterationsB
			&& aux->i < fractal->transformCommon.stopIterationsB)
	{
		z.x = fabs(z.x + fractal->transformCommon.additionConstant111.x)
					- fabs(z.x - fractal->transformCommon.additionConstant111.x) - z.x;
		z.y = fabs(z.y + fractal->transformCommon.additionConstant111.y)
					- fabs(z.y - fractal->transformCommon.additionConstant111.y) - z.y;
		if (fractal->transformCommon.functionEnabled)
			z.z = fabs(z.z + fractal->transformCommon.additionConstant111.z)
						- fabs(z.z - fractal->transformCommon.additionConstant111.z) - z.z;
	}
	REAL4 zCol = z;

	// spherical fold
	REAL rrCol = 0.0f;
	REAL m = 1.0f;
	if (aux->i >= fractal->transformCommon.startIterationsS
			&& aux->i < fractal->transformCommon.stopIterationsS)
	{
		REAL rr = dot(z, z);
		rrCol = rr;
		if (rr < fractal->transformCommon.minR0)
			m = fractal->transformCommon.maxMinR0factor;
		else if (rr < fractal->transformCommon.maxR2d1)
			m = fractal->transformCommon.maxR2d1 / rr;
		z *= m;
		aux->DE *= m;
	}

	// scale
	if (aux->i >= fractal->transformCommon.startIterationsE
			&& aux->i < fractal->transformCommon.stopIterationsE)
	{
		z *= fractal->transformCommon.scale1;
		aux->DE = aux->DE * fabs(fractal->transformCommon.scale1) + fractal->analyticDE.offset0;
	}

	// rotation
	if (aux->i >= fractal->transformCommon.startIterationsR
			&& aux->i < fractal->transformCommon.stopIterationsR)
		z = Matrix33MulFloat4(fractal->transformCommon.rotationMatrix2, z);

	// julia offset
	if (aux->i >= fractal->transformCommon.startIterationsF
			&& aux->i < fractal->transformCommon.stopIterationsF)
		z += fractal->transformCommon.additionConstantA000;

	// aux->color
	if (fractal->foldColor.auxColorEnabledFalse)
	{
		if (aux->i >= fractal->foldColor.startIterationsA
				&& aux->i < fractal->foldColor.stopIterationsA)
		{
			zCol = fabs(zCol - oldZ);
			if (zCol.x > 0.0f) colorAdd += fractal->foldColor.difs0000.x * zCol.x;
			if (zCol.y > 0.0f) colorAdd += fractal->foldColor.difs0000.y * zCol.y;
			if (zCol.z > 0.0f) colorAdd += fractal->foldColor.difs0000.z * zCol.z;
		}

		if (rrCol > fractal->transformCommon.maxR2d1)
			colorAdd +=
				fractal->foldColor.difs0000.w * (rrCol - fractal->transformCommon.maxR2d1) / 100.0f;

		colorAdd += fractal->mandelbox.color.factorSp1 * m;

		aux->color += colorAdd;
	}
	return z;
}