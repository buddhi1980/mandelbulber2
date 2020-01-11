/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.         ______
 * Copyright (C) 2020 Mandelbulber Team   _>]|=||i=i<,      / ____/ __    __
 *                                        \><||i|=>>%)     / /   __/ /___/ /_
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    / /__ /_  __/_  __/
 * The project is licensed under GPLv3,   -<>>=|><|||`    \____/ /_/   /_/
 * see also COPYING file in this folder.    ~+{i%+++
 *
 * TransfDifsBoxIteration  fragmentarium code, mdifs by knighty (jan 2012)
 */

#include "all_fractal_definitions.h"

cFractalTransfDIFSBox::cFractalTransfDIFSBox() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Box";
	internalName = "transf_difs_box";
	internalID = fractal::transfDIFSBox;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSBox::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;
	CVector4 boxSize = fractal->transformCommon.additionConstant111;
	zc = fabs(zc) - boxSize;
	zc.x = max(zc.x, 0.0);
	zc.y = max(zc.y, 0.0);
	zc.z = max(zc.z, 0.0);
	double zcd = zc.Length();

	aux.dist = min(aux.dist, zcd / (aux.DE + 1.0));
}
