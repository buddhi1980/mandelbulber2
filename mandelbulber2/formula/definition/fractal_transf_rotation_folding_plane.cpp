/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * rotation folding plane
 */

#include "all_fractal_definitions.h"

cFractalTransfRotationFoldingPlane::cFractalTransfRotationFoldingPlane() : cAbstractFractal()
{
	nameInComboBox = "T>Rotation Folding Plane";
	internalName = "transf_rotation_folding_plane";
	internalID = fractal::transfRotationFoldingPlane;
	DEType = analyticDEType;
	DEFunctionType = withoutDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionNone;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfRotationFoldingPlane::FormulaCode(
	CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zRot;
	// cast vector to array pointer for address taking of components in opencl
	double *zRotP = (double *)&zRot;
	const double *colP = (const double *)&fractal->mandelbox.color.factor;
	for (int dim = 0; dim < 3; dim++)
	{
		// handle each dimension x, y and z sequentially in pointer var dim
		double *rotDim = (dim == 0) ? &zRotP[0] : ((dim == 1) ? &zRotP[1] : &zRotP[2]);
		const double *colorFactor = (dim == 0) ? &colP[0] : ((dim == 1) ? &colP[1] : &colP[2]);

		zRot = fractal->mandelbox.rot[0][dim].RotateVector(z);
		if (*rotDim > fractal->mandelbox.foldingLimit)
		{
			*rotDim = fractal->mandelbox.foldingValue - *rotDim;
			z = fractal->mandelbox.rotinv[0][dim].RotateVector(zRot);
			aux.color += *colorFactor;
		}
		else
		{
			zRot = fractal->mandelbox.rot[1][dim].RotateVector(z);
			if (*rotDim < -fractal->mandelbox.foldingLimit)
			{
				*rotDim = -fractal->mandelbox.foldingValue - *rotDim;
				z = fractal->mandelbox.rotinv[1][dim].RotateVector(zRot);
				aux.color += *colorFactor;
			}
		}
	}
}
