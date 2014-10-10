/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * definitions of primitive objects
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

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "algebra.hpp"
#include "color_structures.hpp"

namespace fractal
{
enum enumObjectType
{
	objNone = -1, objFractal = 0, objPlane = 1, objWater = 2, objSphere = 3, objSphereInv = 4, objBox = 5, objBoxInv = 6
};
}

struct sPrimitivePlane
{
	bool enable;
	bool onlyPlane;
	CVector3 centre;
	CVector3 normal;
	double reflect;
	sRGB colour;
};

struct sPrimitiveBox
{
	bool enable;
	CVector3 centre;
	CVector3 size;
	double reflect;
	sRGB colour;
};

struct sPrimitiveSphere
{
	bool enable;
	CVector3 centre;
	double radius;
	double reflect;
	sRGB colour;
};

struct sPrimitiveWater
{
	bool enable;
	double amplitude;
	double animSpeed;
	double level;
	double length;
	double reflect;
	double rotation;
	int iterations;
	sRGB colour;
};

struct sPrimitives
{
	sPrimitivePlane plane;
	sPrimitiveBox box;
	sPrimitiveBox invertedBox;
	sPrimitiveSphere sphere;
	sPrimitiveSphere invertedSphere;
	sPrimitiveWater water;
};

double PrimitivePlane(CVector3 point, CVector3 centre, CVector3 normal);
double PrimitiveInvertedBox(CVector3 point, CVector3 center, CVector3 size);
double PrimitiveBox(CVector3 point, CVector3 center, CVector3 size);
double PrimitiveSphere(CVector3 point, CVector3 center, double radius);
double PrimitiveInvertedSphere(CVector3 point, CVector3 center, double radius);
double PrimitiveWater(CVector3 point, double height, double amplitude, double length, double rotation, int iterations, double animSpeed, int frame);

#endif /* PRIMITIVES_HPP_ */
