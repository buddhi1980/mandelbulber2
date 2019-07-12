/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * auxiliary math functions
 */

#ifndef MANDELBULBER2_SRC_COMMON_MATH_H_
#define MANDELBULBER2_SRC_COMMON_MATH_H_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <QtCore>

#include "algebra.hpp"

extern unsigned int gRandomSeed;

#undef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

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

template <typename T>
inline T clamp(T x, T min, T max)
{
	return qBound(min, x, max);
}

// polynomial smooth min;
// reference: http://www.iquilezles.org/www/articles/smin/smin.htm
inline double dsmin(double a, double b, double k = 1)
{
	double ta = pow(a, k);
	double tb = pow(b, k);
	return pow((ta * tb) / (ta + tb), 1.0 / k);
}

template <typename T>
inline int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

// int abs(int v);
int RandomInt();
int Random(int max);
double dMax(double a, double b, double c);
double dMin(double a, double b, double c);

inline bool CheckNAN(double a)
{
	return !gsl_finite(a);
}
inline double SmoothConditionAGreaterB(double a, double b, double sharpness)
{
	return 1.0 / (1.0 + exp(sharpness * (b - a)));
}
inline double SmoothConditionALessB(double a, double b, double sharpness)
{
	return 1.0 / (1.0 + exp(sharpness * (a - b)));
}
CVector3 ReflectionVector(const CVector3 &normal, const CVector3 &incident);
CVector3 RefractVector(const CVector3 &normal, const CVector3 &incident, double n1, double n2);
double Reflectance(const CVector3 &normal, const CVector3 &incident, double n1, double n2);
inline double LimitAngle(double angle)
{
	return fmod(fmod(angle - 180.0, 360.0) + 360.0, 360.0) - 180.0;
}

// Smooth transition between two vectors with vector length control
template <typename T>
T SmoothCVector(const T &v1, const T &v2, double k);

double cubicInterpolate(double p[4], double x);
double bicubicInterpolate(double p[4][4], double x, double y);

CVector3 wrap(CVector3 x, const CVector3 &a, const CVector3 &s);

double MagicRound(double val, double maxError);

// hypercomplex multiplication taken from:
// https://www.physicsforums.com/threads/16-different-spherical-coordinate-systems.331883/
CVector4 hypercomplex_mult(const CVector4 &vec1, const CVector4 &vec2);
CVector4 hypercomplex_pow_constant(const CVector4 &vec1, int n);

// 3D analogon to the 2D complex conjugate
// mirroring a point on the x-axis
CVector4 hypercomplex_conj(const CVector4 &vec1);

#endif /* MANDELBULBER2_SRC_COMMON_MATH_H_ */
