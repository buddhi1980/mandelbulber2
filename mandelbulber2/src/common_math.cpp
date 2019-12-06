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

#include "common_math.h"

// custom includes
#ifdef __sgi
#include <stdlib.h>
#endif

//********** Random ******************************
// reference:
// http://csg.sph.umich.edu/abecasis/class/2006/615.14.pdf
// Park-Miller Generator Without any Divisions
// by D. Carta (1990)
unsigned int gRandomSeed = 1;
int RandomInt()
{
	// After calculation below, (hi << 16) + lo = seed * 16807
	unsigned int lo = 16807 * (gRandomSeed & 0xFFFF); // Multiply lower 16 bits by 16807
	unsigned int hi = 16807 * (gRandomSeed >> 16);		// Multiply higher 16 bits by 16807

	// After these lines, lo has the bottom 31 bits of result, hi has bits 32 and up
	lo += (hi & 0x7FFF) << 16; // Combine lower 15 bits of hi with lo's upper bits
	hi >>= 15;								 // Discard the lower 15 bits of hi

	// value % (231 - 1)) 	= ((231) * hi + lo) % (231 - 1)
	// 			= ((231 - 1) * hi + hi + lo) % (231-1)
	// 			= (hi + lo) % (231 - 1)
	lo += hi;

	// No division required, since hi + lo is always < 232 - 2
	if (lo > 2147483647) lo -= 2147483647;

	return (int(gRandomSeed = lo));
}

int Random(int max)
{
	return RandomInt() % (max + 1);
}

double dMax(double a, double b, double c)
{
	if (a > b)
	{
		if (a > c) return a;
		return c;
	}
	if (b > c) return b;
	return c;
}

double dMin(double a, double b, double c)
{
	if (a < b)
	{
		if (a < c) return a;
		return c;
	}
	if (b < c) return b;
	return c;
}

//----------------------------------------
// reference:
// http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
// Reflections and Refractions in Ray Tracing
// Bram de Greve (bram.degreve@gmail.com)
// November 13, 2006

CVector3 ReflectionVector(const CVector3 &normal, const CVector3 &incident)
{
	return incident - normal * incident.Dot(normal) * 2.0;
}

CVector3 RefractVector(const CVector3 &normal, const CVector3 &incident, double n1, double n2)
{
	const double n = n1 / n2;
	const double cosI = -normal.Dot(incident);
	const double sinT2 = n * n * (1.0 - cosI * cosI);
	if (sinT2 > 1.0) return CVector3(0.0, 0.0, 0.0); // total internal reflection
	const double cosT = sqrt(1.0 - sinT2);
	return incident * n + normal * (n * cosI - cosT);
}

double Reflectance(const CVector3 &normal, const CVector3 &incident, double n1, double n2)
{
	const double n = n1 / n2;
	const double cosI = -normal.Dot(incident);
	const double sinT2 = n * n * (1.0 - cosI * cosI);
	if (sinT2 > 1.0) return 1.0; // total internal reflection
	const double cosT = sqrt(1.0 - sinT2);
	const double r0rth = (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
	const double rPar = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);
	return (r0rth * r0rth + rPar * rPar) / 2.0;
}

//----------------------------------------

// Smooth transition between two vectors with vector length control
template <typename T>
T SmoothCVector(const T &v1, const T &v2, double k)
{
	T result;
	double nk = 1.0 - k;

	if (k <= 0.0)
	{
		result = v1;
	}
	else if (k >= 1.0)
	{
		result = v2;
	}
	else
	{
		double length1 = v1.Length();
		double length2 = v2.Length();
		double lenInterp = length1 * nk + length2 * k;
		T vTemp = v1 * nk + v2 * k;
		double lengthTemp = vTemp.Length();
		if (lengthTemp > 0.0)
		{
			result = (vTemp / lengthTemp) * lenInterp;
		}
		else
		{
			result = v1;
		}
	}
	return result;
}
template CVector2<double> SmoothCVector(
	const CVector2<double> &v1, const CVector2<double> &v2, double k);
template CVector3 SmoothCVector(const CVector3 &v1, const CVector3 &v2, double k);
template CVector4 SmoothCVector(const CVector4 &v1, const CVector4 &v2, double k);

double cubicInterpolate(double p[4], double x)
{
	return p[1]
				 + 0.5 * x
						 * (p[2] - p[0]
								 + x
										 * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3]
												 + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}

double bicubicInterpolate(double p[4][4], double x, double y)
{
	double yy[4];
	yy[0] = cubicInterpolate(p[0], y);
	yy[1] = cubicInterpolate(p[1], y);
	yy[2] = cubicInterpolate(p[2], y);
	yy[3] = cubicInterpolate(p[3], y);
	return cubicInterpolate(yy, x);
}

CVector3 wrap(CVector3 x, const CVector3 &a, const CVector3 &s)
{
	x -= s;
	CVector3 out(x.x - a.x * floor(x.x / a.x) + s.x, x.y - a.y * floor(x.y / a.y) + s.y,
		x.z - a.z * floor(x.z / a.z) + s.z);
	return out;
}

double MagicRound(double val, double maxError)
{
	if (!qIsNull(val))
	{
		double multiplier = pow(10.0, -int(log10(fabs(val))));
		double rounded = val * multiplier;
		for (int i = 0; i < 10; i++)
		{
			rounded = round(val * multiplier);
			double error = fabs(rounded / multiplier - val) / fabs(val);
			if (error < maxError) break;

			double rounded5 = rounded + 0.5;
			error = fabs(rounded5 / multiplier - val) / fabs(val);
			if (error < maxError)
			{
				rounded = rounded5;
				break;
			}

			rounded5 = rounded - 0.5;
			error = fabs(rounded5 / multiplier - val) / fabs(val);
			if (error < maxError)
			{
				rounded = rounded5;
				break;
			}

			rounded5 = rounded + 0.25;
			error = fabs(rounded5 / multiplier - val) / fabs(val);
			if (error < maxError)
			{
				rounded = rounded5;
				break;
			}

			rounded5 = rounded - 0.25;
			error = fabs(rounded5 / multiplier - val) / fabs(val);
			if (error < maxError)
			{
				rounded = rounded5;
				break;
			}

			multiplier *= 10.0;
		}
		return rounded / multiplier;
	}
	else
	{
		return val;
	}
}

/*CVector4 hypercomplex_mult(const CVector4 &vec1, const CVector4 &vec2)
{
	double r = sqrt(pow(vec1.x, 2) + pow(vec1.y, 2) + pow(vec1.z, 2));
	double theta = atan2(sqrt(pow(vec1.x, 2) + pow(vec1.y, 2)), vec1.z);
	double phi = atan2(vec1.y, vec1.x);

	double r2 = sqrt(pow(vec2.x, 2) + pow(vec2.y, 2) + pow(vec2.z, 2));
	double theta2 = atan2(sqrt(pow(vec2.x, 2) + pow(vec2.y, 2)), vec2.z);
	double phi2 = atan2(vec2.y, vec2.x);

	double new_x = (r * r2) * sin(theta + theta2) * cos(phi + phi2);
	double new_y = (r * r2) * sin(theta + theta2) * sin(phi + phi2);
	double new_z = (r * r2) * cos(theta + theta2);

	return CVector4(new_x, new_y, new_z, vec1.w);
}*/

CVector4 hypercomplex_pow_constant(const CVector4 &vec1, const int n)
{
	double r = sqrt(pow(vec1.x, 2) + pow(vec1.y, 2) + pow(vec1.z, 2));
	double theta = atan2(sqrt(pow(vec1.x, 2) + pow(vec1.y, 2)), vec1.z);
	double phi = atan2(vec1.y, vec1.x);

	double new_x = pow(r, n) * sin(theta * n) * cos(phi * n);
	double new_y = pow(r, n) * sin(theta * n) * sin(phi * n);
	double new_z = pow(r, n) * cos(theta * n);

	return CVector4(new_x, new_y, new_z, vec1.w);
}

/*CVector4 hypercomplex_conj(const CVector4 &vec1)
{
	return CVector4(vec1.x, -vec1.y, -vec1.z,-vec1.w);
}*/
