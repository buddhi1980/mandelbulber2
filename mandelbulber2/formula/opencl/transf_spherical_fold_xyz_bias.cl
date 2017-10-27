/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2017 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * spherical fold XYZ Bias
 * deforms the MinR2 condition
 * This formula contains aux.color

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the function "TransfSphericalFoldXYZBiasIteration" in the file fractal_formulas.cpp
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 TransfSphericalFoldXYZBiasIteration(
	REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	REAL4 xyzBias;
	REAL minR2 = fractal->transformCommon.minR2p25;
	REAL MaxR2 = fractal->transformCommon.maxR2d1;
	REAL m = fractal->transformCommon.scale;

	if (aux->i >= fractal->transformCommon.startIterationsA
			&& aux->i < fractal->transformCommon.stopIterationsA)
	{
		xyzBias = fabs(aux->c) * fractal->transformCommon.constantMultiplier000;
		minR2 = minR2 + (xyzBias.x + xyzBias.y + xyzBias.z);
	}

	if (fractal->transformCommon.functionEnabledFalse
			&& aux->i >= fractal->transformCommon.startIterationsB
			&& aux->i < fractal->transformCommon.stopIterationsB)
	{
		REAL4 cSquared = aux->c * aux->c;
		xyzBias = cSquared * fractal->transformCommon.scale3D000;
		minR2 = minR2 + (xyzBias.x + xyzBias.y + xyzBias.z);
	}

	if (fractal->transformCommon.functionEnabled && minR2 > MaxR2)
	{
		minR2 = MaxR2; // stop overlapping potential
	}

	REAL rr = dot(z, z);
	z += fractal->transformCommon.offset000;

	if (rr < minR2)
	{
		m *= native_divide(fractal->transformCommon.maxR2d1, minR2);
		aux->color += fractal->mandelbox.color.factorSp1;
	}
	else if (rr < fractal->transformCommon.maxR2d1)
	{
		m *= native_divide(fractal->transformCommon.maxR2d1, rr);
		aux->color += fractal->mandelbox.color.factorSp2;
	}
	z -= fractal->transformCommon.offset000;

	z *= m;
	aux->DE = mad(aux->DE, fabs(m), 1.0f);
	return z;
}