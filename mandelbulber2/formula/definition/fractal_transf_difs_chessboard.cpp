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

cFractalTransfDIFSChessboard::cFractalTransfDIFSChessboard() : cAbstractFractal()
{
	nameInComboBox = "T>DIFS Chessboard";
	internalName = "transf_difs_chessboard";
	internalID = fractal::transfDIFSChessboard;
	DEType = analyticDEType;
	DEFunctionType = customDEFunction;
	cpixelAddition = cpixelDisabledByDefault;
	defaultBailout = 100.0;
	DEAnalyticFunction = analyticFunctionCustomDE;
	coloringFunction = coloringFunctionDefault;
}

void cFractalTransfDIFSChessboard::FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux)
{
	CVector4 zc = z;

	CVector4 col = zc + fractal->transformCommon.offset000;
	CVector4 repeats = fractal->transformCommon.scale3D444;
	repeats.x = floor(repeats.x * col.x);
	repeats.y = floor(repeats.y * col.y);
	repeats.z = floor(repeats.z * col.z);

	double auxCol;
	if (!fractal->transformCommon.functionEnabledCFalse)
	{
		auxCol = repeats.x + repeats.y;
	}
	else
	{
		auxCol = repeats.x + repeats.y + repeats.z;
	}
	auxCol = (auxCol * 0.5 - floor(auxCol * 0.5)) * 2.0;

	double rDE;
	if (!fractal->transformCommon.functionEnabledFalse)
	{
		zc = fabs(zc) - fractal->transformCommon.offset110; // pos = size
		rDE = max(zc.x, max(zc.y, zc.z));
	}
	else
		rDE = zc.z;
	zc = z;

	aux.dist = rDE;
	aux.color = auxCol;
}
