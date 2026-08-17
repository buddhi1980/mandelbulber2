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

// Include lightweight math utilities (sign, clamp, smoothMin, etc.)
// These are pure C++ with no Qt/GSL dependencies — intended for formula code.
#include "math_utilities.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <algorithm>

#include "algebra.hpp"

extern unsigned int gRandomSeed;

// int abs(int v);
int RandomInt();
int Random(int max);

// ---------------------------------------------------------------------------
// Reflection / refraction — declared here, defined in common_math.cpp
// ---------------------------------------------------------------------------
CVector3 ReflectionVector(const CVector3 &normal, const CVector3 &incident);
CVector3 RefractVector(const CVector3 &normal, const CVector3 &incident, double n1, double n2);
double Reflectance(const CVector3 &normal, const CVector3 &incident, double n1, double n2);

// ---------------------------------------------------------------------------
// Interpolation — declared here, defined in common_math.cpp
// ---------------------------------------------------------------------------
float cubicInterpolate(float p[4], float x);
float bicubicInterpolate(float p[4][4], float x, float y);

// ---------------------------------------------------------------------------
// MagicRound — declared here, defined in common_math.cpp
// ---------------------------------------------------------------------------
double MagicRound(double val, double maxError);

// ---------------------------------------------------------------------------
// Hypercomplex operations — declared here, defined in common_math.cpp
// ---------------------------------------------------------------------------
CVector4 hypercomplex_mult(const CVector4 &vec1, const CVector4 &vec2);
CVector4 hypercomplex_pow_constant(const CVector4 &vec1, int n);
CVector4 hypercomplex_conj(const CVector4 &vec1);

// SmoothCVector template — declared here, defined in common_math.cpp
template <typename T>
T SmoothCVector(const T &v1, const T &v2, double k);

#endif /* MANDELBULBER2_SRC_COMMON_MATH_H_ */
