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
#include <QtCore>

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


inline double dmix(double a, double b, double x)
{
	return a * (1.0 - x) + b * x;
}

template<typename T>
inline T clamp(T x, T min, T max)
{
	return (((x >= min) ? x : min) <= max) ? x : max;
}

// polynomial smooth min;
//reference: http://www.iquilezles.org/www/articles/smin/smin.htm
inline double dsmin(double a, double b, double k = 1)
{
	double ta = pow( a, k );
	double tb = pow( b, k );
	return pow( (ta*tb)/(ta+tb), 1.0/k );
}

//int abs(int v);
int Random(int max);
double dMax(double a, double b, double c);
double dMin(double a, double b, double c);

inline double SmoothConditionAGreaterB(double a, double b, double sharpness) {return 1.0 / (1.0 + exp(sharpness * (b - a)));}
inline double SmoothConditionALessB(double a, double b, double sharpness) {return 1.0 / (1.0 + exp(sharpness * (a - b)));}
CVector3 InvProjection3D(CVector3 point, CVector3 vp, CRotationMatrix mRotInv, params::enumPerspectiveType perspectiveType, double fov, double zoom, double imgWidth, double imgHeight);
CVector3 CalculateViewVector(CVector2<double> normalizedPoint, double fov, params::enumPerspectiveType perspType, const CRotationMatrix &mRot);
CVector3 ReflectionVector(const CVector3 &normal, const CVector3 &incident);
CVector3 RefractVector(const CVector3 &normal, const CVector3 &incident, double n1, double n2);
double Reflectance(const CVector3 &normal, const CVector3 &incident, double n1, double n2);
inline double LimitAngle(double angle) {return fmod(fmod(angle - 180.0, 360.0) + 360.0, 360.0) - 180.0;}

//Smooth transition between two vectors with vector length control
template <typename T> T SmoothCVector(const T &v1, const T &v2, double k);


#endif /* COMMON_MATH_H_ */
