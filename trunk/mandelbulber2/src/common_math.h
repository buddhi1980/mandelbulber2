/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * auxiliary math functions
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

#ifndef COMMON_MATH_H_
#define COMMON_MATH_H_

#include "algebra.hpp"
#include "fractparams.hpp"

#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

struct sVectorsAround
{
	double alpha;
	double beta;
	CVector3 v;
	int R;
	int G;
	int B;
	bool notBlack;
};

struct sVector
{
	double x;
	double y;
	double z;
};




//int abs(int v);
int Random(int max);
double dMax(double a, double b, double c);
double dMin(double a, double b, double c);

inline double SmoothConditionAGreaterB(double a, double b, double sharpness) {return 1.0 / (1.0 + exp(sharpness * (b - a)));}
inline double SmoothConditionALessB(double a, double b, double sharpness) {return 1.0 / (1.0 + exp(sharpness * (a - b)));}
CVector3 InvProjection3D(CVector3 point, CVector3 vp, CRotationMatrix mRotInv, params::enumPerspectiveType perspectiveType, double fov, double zoom, double imgWidth, double imgHeight);
CVector3 CalculateViewVector(CVector2<double> normalizedPoint, double fov, params::enumPerspectiveType perspType, const CRotationMatrix &mRot);


#endif /* COMMON_MATH_H_ */
