/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Rayan Hitchman
 *
 * cObjectData class - generalized properties of any object
 *
 * This object can be a primitive or a fractal.
 */

#ifndef MANDELBULBER2_SRC_OBJECT_DATA_HPP_
#define MANDELBULBER2_SRC_OBJECT_DATA_HPP_

#include "algebra.hpp"
#include "object_types.hpp"

class cObjectData
{
public:
	cObjectData()
	{
		materialId = 0;
		objectType = fractal::objNone;
		smoothDeCombineEnable = false;
		smoothDeCombineDistance = 0.0f;
		wallThickness = 0.0f;
		usedForVolumetric = false;
	}

	bool smoothDeCombineEnable;
	bool usedForVolumetric;
	double smoothDeCombineDistance;
	double wallThickness;
	CVector3 position;
	CVector3 size;
	CVector3 repeat;
	int materialId;
	fractal::enumObjectType objectType;
	CRotationMatrix rotationMatrix;
	void SetRotation(const CVector3 &rot) { rotationMatrix.SetRotation2(rot * M_PI / 180.0); }

private:
	CVector3 rotation;
};

#endif /* MANDELBULBER2_SRC_OBJECT_DATA_HPP_ */
