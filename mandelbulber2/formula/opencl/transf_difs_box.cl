/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2019 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
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
	REAL4 zc = z;
	REAL4 boxSize = fractal->transformCommon.additionConstant111;
	zc = fabs(zc) - boxSize;
	zc.x = max(zc.x, 0.0f);
	zc.y = max(zc.y, 0.0f);
	zc.z = max(zc.z, 0.0f);
	REAL zcd = length(zc);

	aux->dist = min(aux->dist, native_divide(zcd, (aux->DE + 1.0f)));
	return z;
}