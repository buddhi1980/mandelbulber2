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
	CVector3 lineDirection;
	double sphereRadius;
	enumFractalColoring coloringAlgorithm;
	bool color4dEnabledFalse;

	// trial
	bool extraColorEnabledFalse;
	double initialColorValue;
	bool initCondFalse;
	bool icRadFalse;
	bool icXYZFalse;
	bool icFabsFalse;
	double icRadWeight;
	CVector3 xyzC111;

	bool orbitTrapTrue;
	double orbitTrapWeight;
	double initialMinimumR;

	bool auxColorFalse;
	double auxColorWeight;
	double auxColorHybridWeight;

	bool radFalse;
	double radWeight;
	bool radSquaredFalse;
	bool radDiv1e13False;

	bool radDivDeFalse;
	bool radDivLogDeFalse;
	double radDivDeWeight;
	bool radDivDeSquaredFalse;

	bool xyzBiasEnabledFalse;
	CVector3 xyz000;
	double xyzIterScale;
	bool xyzXSqrdFalse;
	bool xyzYSqrdFalse;
	bool xyzZSqrdFalse;
	bool xyzFabsFalse;
	bool xyzDiv1e13False;

	bool iterGroupFalse;
	bool iterAddScaleTrue;
	bool iterScaleFalse;
	double iterAddScale;
	double iterScale;
	int iStartValue;

	// global palette controls
	bool globalPaletteFalse;

	bool addEnabledFalse;
	double addMax;
	double addSpread;
	double addStartValue;

	bool parabEnabledFalse;
	double parabScale;
	double parabStartValue;

	bool cosEnabledFalse;
	double cosPeriod;
	double cosAdd;
	double cosStartValue;

	bool roundEnabledFalse;
	double roundScale;

	double maxColorValue;
	double minColorValue;

	sFractalColoring();
};

double CalculateColorIndex(bool isHybrid, double r, CVector4 z, double minimumR,
	const sExtendedAux &extendedAux, const sFractalColoring &fractalColoring,
	fractal::enumColoringFunction coloringFunction, const sFractal *defaultFractal);

#endif /* MANDELBULBER2_SRC_FRACTAL_COLORING_HPP_ */
