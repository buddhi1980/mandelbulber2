/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cRegion class - definition and translation between screen and offscreen region
 */

#ifndef MANDELBULBER2_SRC_REGION_HPP_
#define MANDELBULBER2_SRC_REGION_HPP_

#include "algebra.hpp"

template <typename T>
class cRegion
{
public:
	cRegion(T _x1, T _y1, T _x2, T _y2)
			: x1(_x1), y1(_y1), x2(_x2), y2(_y2), width(_x2 - _x1), height(_y2 - _y1){};
	cRegion() : x1(), y1(), x2(), y2(), width(), height(){};
	cRegion(CVector2<T> corner1, CVector2<T> corner2)
			: x1(corner1.x),
				y1(corner1.y),
				x2(corner1.x),
				y2(corner2.y),
				width(corner2.x - corner1.x),
				height(corner2.y - corner1.y){};
	void Set(T _x1, T _y1, T _x2, T _y2)
	{
		x1 = _x1;
		x2 = _x2;
		y1 = _y1;
		y2 = _y2;
		width = _x2 - _x1;
		height = _y2 - _y1;
	}

	// transpose point in local coordinates to given region
	template <typename TNew>
	CVector2<TNew> transpose(cRegion<TNew> region, CVector2<T> point) const
	{
		double scaleX = double(region.width) / double(width);
		double scaleY = double(region.height) / double(height);
		return CVector2<TNew>(scaleX * (point.x - x1) + region.x1, scaleY * (point.y - y1) + region.y1);
	}

	T x1;
	T y1;
	T x2;
	T y2;
	T width;
	T height;

private:
};

#endif /* MANDELBULBER2_SRC_REGION_HPP_ */
