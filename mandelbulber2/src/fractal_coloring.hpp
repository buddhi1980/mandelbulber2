/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * fractal based coloring modes
 */

#ifndef MANDELBULBER2_SRC_FRACTAL_COLORING_HPP_
#define MANDELBULBER2_SRC_FRACTAL_COLORING_HPP_

#include "algebra.hpp"
#include "fractal_enums.h"

struct sExtendedAux;
struct sFractal;

enum enumFractalColoring
{
	fractalColoring_None = -1,
	fractalColoring_Standard = 0,
	fractalColoring_ZDotPoint = 1,
	fractalColoring_Sphere = 2,
	fractalColoring_Cross = 3,
	fractalColoring_Line = 4
};

struct sFractalColoring
{
	bool addEnabledFalse;
	bool auxColorFalse;
	bool color4dEnabledFalse;
	bool cosEnabledFalse;
	bool extraColorEnabledFalse;
	// global palette controls
	bool globalPaletteFalse;
	bool icFabsFalse;
	bool icRadFalse;
	bool icXYZFalse;
	bool initCondFalse;
	bool iterAddScaleTrue;
	bool iterGroupFalse;
	bool iterScaleFalse;
	bool orbitTrapTrue;
	bool parabEnabledFalse;
	bool radDiv1e13False;
	bool radDivDeFalse;
	bool radDivDeSquaredFalse;
	bool radDivLogDeFalse;
	bool radFalse;
	bool radSquaredFalse;
	bool roundEnabledFalse;
	bool xyzBiasEnabledFalse;
	bool xyzDiv1e13False;
	bool xyzFabsFalse;
	bool xyzXSqrdFalse;
	bool xyzYSqrdFalse;
	bool xyzZSqrdFalse;

	enumFractalColoring coloringAlgorithm;
	int iStartValue;

	CVector3 lineDirection;
	CVector3 xyz000;
	CVector3 xyzC111;

	double addMax;
	double addSpread;
	double addStartValue;
	double auxColorHybridWeight;
	double auxColorWeight;
	double cosAdd;
	double cosPeriod;
	double cosStartValue;
	double icRadWeight;
	double initialColorValue;
	double initialMinimumR;
	double iterAddScale;
	double iterScale;
	double maxColorValue;
	double minColorValue;
	double orbitTrapWeight;
	double parabScale;
	double parabStartValue;
	double radDivDeWeight;
	double radWeight;
	double roundScale;
	double sphereRadius;
	double xyzIterScale;

	sFractalColoring();
};

double CalculateColorIndex(bool isHybrid, double r, CVector4 z, double minimumR,
	const sExtendedAux &extendedAux, const sFractalColoring &fractalColoring,
	fractal::enumColoringFunction coloringFunction, const sFractal *defaultFractal);

#endif /* MANDELBULBER2_SRC_FRACTAL_COLORING_HPP_ */
