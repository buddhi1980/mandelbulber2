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


#include "common_math.h"
#include <cstdlib>
#include "fractparams.hpp"

#ifdef __sgi
#include <stdlib.h>
#endif

//********** Random ******************************
int Random(int max)
{
#ifdef WIN32
	return (rand()+rand()*32768) % (max + 1);
#else
	return rand() % (max + 1);
#endif
}

double dMax(double a, double b, double c)
{
	if(a > b) {
		if (a > c)
			return a;
		return c;
	}
	if (b > c)
		return b;
	return c;
}

double dMin(double a, double b, double c)
{
	if(a < b) {
		if (a < c)
			return a;
		return c;
	}
	if (b < c)
		return b;
	return c;
}

CVector3 CalculateViewVector(CVector2<double> normalizedPoint, double fov, params::enumPerspectiveType perspType, const CRotationMatrix &mRot)
{
	CVector3 viewVector;

	switch(perspType)
	{
		case  params::perspFishEye: case params::perspFishEyeCut:
		{
			CVector2<double> v = normalizedPoint * M_PI;
			double r = v.Length();
			if(r == 0.0)
			{
				viewVector.x = 0.0;
				viewVector.z = 0.0;
				viewVector.y = 1.0;
			}
			else
			{
				viewVector.x = v.x / r * sin(r * fov);
				viewVector.z = v.y / r * sin(r * fov);
				viewVector.y = cos(r * fov);
			}
			viewVector.Normalize();
			break;
		}
		case params::perspEquirectangular:
		{
			CVector2<double> v = normalizedPoint * M_PI;
			viewVector.x = sin(fov * v.x) * cos(fov * v.y);
			viewVector.z = sin(fov * v.y);
			viewVector.y = cos(fov * v.x) * cos(fov * v.y);
			viewVector.Normalize();
			break;
		}
		case params::perspThreePoint:
		{
			viewVector.x = normalizedPoint.x * fov;
			viewVector.y = 1.0;
			viewVector.z = normalizedPoint.y * fov;
			viewVector.Normalize();
			break;
		}
	}
	viewVector = mRot.RotateVector(viewVector);

	return viewVector;
}


CVector3 InvProjection3D(CVector3 point, CVector3 vp, CRotationMatrix mRotInv, params::enumPerspectiveType perspectiveType, double fov, double zoom, double imgWidth, double imgHeight)
{
	CVector3 screenPoint;
	CVector3 baseZ(0.0, 1.0, 0.0);

	double aspectRatio = (double) imgWidth / imgHeight;

	CVector3 start;
	if (perspectiveType == params::perspFishEye || perspectiveType == params::perspEquirectangular)
	{
		start = vp;
	}
	else
	{
		start = vp - baseZ * (1.0 / fov * zoom);
	}
	CVector3 viewVector = point - start;
	viewVector = mRotInv.RotateVector(viewVector);

	double x, y, z;
	if (perspectiveType == params::perspFishEye)
	{
		z = viewVector.Length();
		if(viewVector.y < 0) z = -z;
		viewVector.Normalize();
		double r = sqrt(viewVector.x * viewVector.x + viewVector.z * viewVector.z);
		double r2 = asin(r) / (M_PI * 0.5);
		x = (viewVector.x / fov) * r2 / r / 2.0;
		y = (viewVector.z / fov) * r2 / r / 2.0;
	}
	else
	{

		viewVector.x /= viewVector.y;
		viewVector.z /= viewVector.y;
		x = viewVector.x / fov;
		y = viewVector.z / fov;
		z = viewVector.y;
	}
	screenPoint.x = (x / aspectRatio + 0.5) * imgWidth;
	screenPoint.y = (y + 0.5) * imgHeight;
	screenPoint.z = z;

	return screenPoint;
}

//----------------------------------------
//reference:
//http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
//Reflections and Refractions in Ray Tracing
//Bram de Greve (bram.degreve@gmail.com)
//November 13, 2006

CVector3 ReflectionVector(const CVector3 &normal, const CVector3 &incident)
{
	return incident - normal * incident.Dot(normal) * 2.0;
}

CVector3 RefractVector(const CVector3 &normal, const CVector3 &incident, double n1, double n2)
{
	const double n = n1 / n2;
	const double cosI = -normal.Dot(incident);
	const double sinT2 = n * n * (1.0 - cosI * cosI);
	if (sinT2 > 1.0) return CVector3(0.0, 0.0, 0.0); //total internal reflection
	const double cosT = sqrt(1.0 - sinT2);
	return incident * n + normal * (n * cosI - cosT);
}

double Reflectance(const CVector3 &normal, const CVector3 &incident, double n1, double n2)
{
	const double n = n1 / n2;
	const double cosI = -normal.Dot(incident);
	const double sinT2 = n * n * (1.0 - cosI * cosI);
	if (sinT2 > 1.0) return 1.0; //total internal reflection
	const double cosT = sqrt(1.0 - sinT2);
	const double r0rth = (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
	const double rPar = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);
	return (r0rth * r0rth + rPar * rPar) / 2.0;
}

//----------------------------------------

//Smooth transition between two vectors with vector length control
template <typename T>
T SmoothCVector(const T &v1, const T &v2, double k)
{
	T result;
	double nk = 1.0 - k;

	if(k <= 0.0)
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
		if(lengthTemp > 0.0)
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
template CVector2<double> SmoothCVector(const CVector2<double> &v1, const CVector2<double> &v2, double k);
template CVector3 SmoothCVector(const CVector3 &v1, const CVector3 &v2, double k);
template CVector4 SmoothCVector(const CVector4 &v1, const CVector4 &v2, double k);


