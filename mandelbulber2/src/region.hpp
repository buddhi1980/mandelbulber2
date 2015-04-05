/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRegion class - definition and translation between screen and offscreen region
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef REGION_HPP_
#define REGION_HPP_

#include "algebra.hpp"

template <typename T>
class cRegion
{
public:
	cRegion(T _x1, T _y1, T _x2, T _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2), width(_x2 - _x1), height(_y2 - _y1) {};
	cRegion() : x1(), y1(), x2(), y2(), width(), height() {};
	cRegion(CVector2<T> corner1, CVector2<T> corner2) : x1(corner1.x), y1(corner1.y), x2(corner1.x), y2(corner2.y), width(corner2.x - corner1.x), height(corner2.y - corner1.y) {};
	void Set(T _x1, T _y1, T _x2, T _y2) {x1 = _x1; x2 = _x2; y1 = _y1; y2 = _y2; width = _x2 - _x1; height = _y2 - _y1;}

	//transpose point in local coordinates to given region
	template <typename TNew>
	CVector2<TNew> transpose(cRegion<TNew> region, CVector2<T> point) const
	{
		double scaleX = (double)region.width / (double)width;
		double scaleY = (double)region.height / (double)height;
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


#endif /* REGION_HPP_ */
