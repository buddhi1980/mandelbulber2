/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-22 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W , ][T"]C  §  § '§ e===~ U  !§[Z ]N
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
 * Lightweight math utilities — pure C++, no Qt, no GSL dependencies.
 * Intended for inclusion by formula definition files and other math-heavy code.
 */

#ifndef MANDELBULBER2_SRC_MATH_UTILITIES_H_
#define MANDELBULBER2_SRC_MATH_UTILITIES_H_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <algorithm>

// legacy MAX/MIN macros (used by some formula code)
#undef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// 3-element float vector (lightweight, no Qt)
struct sVector
{
	double x;
	double y;
	double z;
};

// ---------------------------------------------------------------------------
// sign — returns -1, 0, or +1
// ---------------------------------------------------------------------------
template <typename T>
inline int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

// ---------------------------------------------------------------------------
// clamp — C++17 fallback (project uses -std=gnu++1z)
// ---------------------------------------------------------------------------
#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
using std::clamp;
#else
template <typename T>
inline T clamp(T x, T min, T max)
{
	return (x < min) ? min : ((x > max) ? max : x);
}
#endif

// ---------------------------------------------------------------------------
// Smooth interpolation helpers (I-quake smin / smooth union)
// ---------------------------------------------------------------------------
inline double dmix(double a, double b, double x)
{
	return a * (1.0 - x) + b * x;
}

// reference: https://www.iquilezles.org/www/articles/smin/smin.htm
inline double smoothMin(double a, double b, double k)
{
	double h = std::max(k - fabs(a - b), 0.0) / k;
	return std::min(a, b) - h * h * h * k * (1.0 / 6.0);
}

// ref https://iquilezles.org/articles/distfunctions/
inline double opSmoothUnion(double d1, double d2, double k)
{
	double h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
	return dmix(d2, d1, h) - k * h * (1.0 - h);
}

// polynomial smooth min
// reference: http://www.iquilezles.org/www/articles/smin/smin.htm
inline double dsmin(double a, double b, double k = 1)
{
	double ta = pow(a, k);
	double tb = pow(b, k);
	return pow((ta * tb) / (ta + tb), 1.0 / k);
}

// ---------------------------------------------------------------------------
// NaN check
// ---------------------------------------------------------------------------
inline bool CheckNAN(double a)
{
	return !std::isfinite(a);
}

// ---------------------------------------------------------------------------
// Smooth step conditions (used by ABoxSmooth and similar formulas)
// ---------------------------------------------------------------------------
inline double SmoothConditionAGreaterB(double a, double b, double sharpness)
{
	return 1.0 / (1.0 + exp(sharpness * (b - a)));
}

inline double SmoothConditionALessB(double a, double b, double sharpness)
{
	return 1.0 / (1.0 + exp(sharpness * (a - b)));
}

// ---------------------------------------------------------------------------
// Angle normalization
// ---------------------------------------------------------------------------
inline double LimitAngle(double angle)
{
	return fmod(fmod(angle - 180.0, 360.0) + 360.0, 360.0) - 180.0;
}

// ---------------------------------------------------------------------------
// Three-argument max/min (declared here, defined in common_math.cpp)
// ---------------------------------------------------------------------------
double dMax(double a, double b, double c);
double dMin(double a, double b, double c);

// ---------------------------------------------------------------------------
// Vector wrap (modular repetition) — declared here, defined in common_math.cpp
// ---------------------------------------------------------------------------
class CVector3; // forward declaration
CVector3 wrap(CVector3 x, const CVector3 &a, const CVector3 &s);

#endif /* MANDELBULBER2_SRC_MATH_UTILITIES_H_ */
