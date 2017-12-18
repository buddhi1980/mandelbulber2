/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

struct sFractalColoring
{
	enum enumFractalColoringAlgorithm
	{
		fractalColoringNone = -1,
		fractalColoringStandard = 0,
		fractalColoringZDotPoint = 1,
		fractalColoringSphere = 2,
		fractalColoringCross = 3,
		fractalColoringLine = 4
	};

	sFractalColoring()
	{
		sphereRadius = 0.0;
		coloringAlgorithm = fractalColoringNone;

		extraColorEnabledFalse = false;
		orbitTrapWeight = 1.0;
		initialMiniumuR = 0.0;

		auxColorWeight = 0.0;

		radFalse = false;
		radWeight = 1.0;
		radSquaredFalse = false;

		radDivDeFalse = false;
		radDivDeWeight = 1.0;

		xyzBiasEnabledFalse = false;
		xyzIterScale = 1.0;
		cModeEnabledFalse = false; // basic test

		iterAddScaleFalse = false;
		iterScaleFalse = false;
		iterAddScale = 0.0;
		iterScale = 1.0;
		iStartValue = 0;
		iSquaredEnabledFalse = false;
		iInvertEnabledFalse = false;

		// global palette controls
		cosEnabledFalse = false;
		cosPeriod = 1.0; //TODO calibrate
		cosAdd = 1.0; //TODO calibrate

		maxColorValue = 100000.0;
		minColorValue = 0.0;

	}

	CVector3 lineDirection;
	double sphereRadius;
	enumFractalColoringAlgorithm coloringAlgorithm;

	// trial
	bool extraColorEnabledFalse;
	double orbitTrapWeight;
	double initialMiniumuR;

	double auxColorWeight;

	bool radFalse;
	double radWeight;
	bool radSquaredFalse;

	bool radDivDeFalse;
	double radDivDeWeight;

	bool xyzBiasEnabledFalse;
	CVector3 xyz000;
	bool cModeEnabledFalse;
	double xyzIterScale;

	bool iterAddScaleFalse;
	bool iterScaleFalse;
	double iterAddScale;
	double iterScale;
	int iStartValue; // or start colorV??
	bool iSquaredEnabledFalse;
	bool iInvertEnabledFalse;

	// global palette controls
	bool cosEnabledFalse;
	double cosPeriod;
	double cosAdd;
	double maxColorValue;
	double minColorValue;
};

#endif /* MANDELBULBER2_SRC_FRACTAL_COLORING_HPP_ */
