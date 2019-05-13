/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * data structure common for main parameters and fractal parameters
 */

#ifndef MANDELBULBER2_SRC_COMMON_PARAMS_HPP_
#define MANDELBULBER2_SRC_COMMON_PARAMS_HPP_

#include "algebra.hpp"

namespace params
{
enum enumFakeLightsShape
{
	fakeLightsShapePoint = 0,
	fakeLightsShapeLine = 1,
	fakeLightsShapeCircle = 2,
	fakeLightsShapeSquare = 3,
	fakeLightsShapeSphere = 4,
	fakeLightsShapeCube = 5
};
};

struct sFractalFoldings
{
	double boxLimit; // parameters of TGlad's folding
	double boxValue;
	double sphericalOuter;
	double sphericalInner;
	bool boxEnable;
	bool sphericalEnable;
};

struct sCommonParams
{
	bool iterThreshMode;

	int fakeLightsMaxIter;
	int fakeLightsMinIter;

	params::enumFakeLightsShape fakeLightsOrbitTrapShape;

	double fakeLightsOrbitTrapSize;
	double fakeLightsThickness;
	double linearDEOffset;

	CVector3 fakeLightsOrbitTrap;
	CVector3 fakeLightsRotation;
	CVector3 fractalPosition;
	CVector3 fractalRotation;
	CVector3 repeat;
	CRotationMatrix mRotFractalRotation;
	CRotationMatrix mRotFakeLightsRotation;

	sFractalFoldings foldings;
};

#endif /* MANDELBULBER2_SRC_COMMON_PARAMS_HPP_ */
