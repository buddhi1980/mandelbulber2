/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______
 * Copyright (C) 2022 Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * Transform Menger Fold
 * Menger Sponge formula created by Knighty
 * @reference
 * http://www.fractalforums.com/ifs-iterated-function-systems/kaleidoscopic-(escape-time-ifs)/
 * analytic aux.DE

 * This file has been autogenerated by tools/populateUiInformation.php
 * from the file "fractal_transf_menger_fold_v2.cpp" in the folder formula/definition
 * D O    N O T    E D I T    T H I S    F I L E !
 */

REAL4 TransfMengerFoldV2Iteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)
{
	z = fabs(z);
	z += fractal->transformCommon.offset000;

	if (fractal->transformCommon.rotationEnabledFalse
			&& aux->i >= fractal->transformCommon.startIterationsR
			&& aux->i < fractal->transformCommon.stopIterationsR)
	{
		z = Matrix33MulFloat4(fractal->transformCommon.rotationMatrix, z);
	}

	REAL t;
	if (fractal->transformCommon.functionEnabledAx)
	{
		t = z.x - z.z;
		t = fractal->transformCommon.additionConstant0555.x * (t - fabs(t));
		z.x = z.x - t;
		z.z = z.z + t;
	}
	if (fractal->transformCommon.functionEnabledAy)
	{
		t = z.x - z.y;
		t = fractal->transformCommon.additionConstant0555.y * (t - fabs(t));
		z.x = z.x - t;
		z.y = z.y + t;
	}
	if (fractal->transformCommon.functionEnabledAz)
	{
		t = z.y - z.z;
		t = fractal->transformCommon.additionConstant0555.z * (t - fabs(t));
		z.y = z.y - t;
		z.z = z.z + t;
	}

	if (fractal->transformCommon.functionEnabledFalse)
	{
		z = Matrix33MulFloat4(fractal->transformCommon.rotationMatrix2, z);

		REAL useScale = fractal->transformCommon.scale3;
		if (fractal->transformCommon.functionEnabledXFalse
				&& aux->i >= fractal->transformCommon.startIterationsX
				&& aux->i < fractal->transformCommon.stopIterationsX)
		{
			useScale += aux->actualScaleA;
			// update actualScale for next iteration
			REAL vary = fractal->transformCommon.scaleVary0
									* (fabs(aux->actualScaleA) - fractal->transformCommon.scaleB1);
			aux->actualScaleA = -vary;
		}
		aux->DE = aux->DE * useScale + fractal->analyticDE.offset0;

		REAL sc1 = useScale - 1.0f;
		REAL sc2 = sc1 / useScale;
		z.z = z.z - fractal->transformCommon.offset1105.z * sc2;
		z.z = -fabs(z.z) + fractal->transformCommon.offset1105.z * sc2;
		z.x = useScale * z.x - fractal->transformCommon.offset1105.x * sc1;
		z.y = useScale * z.y - fractal->transformCommon.offset1105.y * sc1;
		z.z = useScale * z.z;
	}

	if (fractal->analyticDE.enabledFalse)
		aux->DE = aux->DE * fractal->analyticDE.scale1 + fractal->analyticDE.offset1;
	return z;
}