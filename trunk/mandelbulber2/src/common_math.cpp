/*********************************************************
 /                   MANDELBULBER
 / some functions for different calculations
 /
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 ********************************************************/

/*
 * common_math.cpp
 *
 *  Created on: 2010-01-23
 *      Author: krzysztof
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
			viewVector.y = cos(fov * v.y) * cos(fov * v.y);
			viewVector.Normalize();
			break;
		}
		case params::perspThreePoint:
		{
			viewVector.x = normalizedPoint.x * fov;
			viewVector.y = 1.0;
			viewVector.z = normalizedPoint.y * fov;
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
