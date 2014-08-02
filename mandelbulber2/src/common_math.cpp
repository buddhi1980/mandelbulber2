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

template <class T>
void QuickSortZBuffer(sSortZ<T> *dane, int l, int p)
{
  int i,j; //robocze indeksy
  sSortZ<T> x,y; //robocze zmienne typu sortowany obiekt

  i=l;           //{ i : = minimalny klucz }
  j=p;           //{ j : = maksymalny klucz }
  x=dane[(l+p)/2];  //{ x - wyraz srodkowy sortowanej tablicy }
  do
  {             //{ cala historie bedziemy powtarzac tak dlugo jak . . . }
	while(dane[i].z < x.z)              //{ dopoki jestesmy na lewo od srodkowego }
	{
	  i++;                       //{ ...powiekszamy indeks i }
	}
	while(x.z < dane[j].z)                  //{ dopoki na prawo od srodkowego }
	{
	  j--;                       //{ ...zmniejszamy indeks j }
	}
	if(i<=j)                        //{ jezeli i <= j wtedy : }
	{                               //{ zamieniamy miejscami wyrazy i, j }
	  y=dane[i];                      //{ podstawienie do zmiennej roboczej }
	  dane[i]=dane[j];                    //{ pierwszy etap zamiany }
	  dane[j]=y;                      //{ drugi etap zamiany }
	  i++;                       //{ zwiekszenie indeksu i }
	  j--;                       //{ zmniejszenie indeksu j }
	}
  }
  while(i<=j);      //{ nie zostanie spelniony warunek i>j }
  if(l<j) QuickSortZBuffer(dane, l, j);
  if(i<p) QuickSortZBuffer(dane, i, p);
}
template void QuickSortZBuffer<double>(sSortZ<double> *dane, int l, int p);

CVector3 Projection3D(CVector3 point, CVector3 vp, CRotationMatrix mRot, params::enumPerspectiveType perspectiveType, double fov, double zoom)
{
	double perspFactor = 1.0 + point.y * fov;
	CVector3 vector1, vector2;

	if (perspectiveType == params::perspFishEye)
	{
		double r = sqrt(point.x * point.x + point.z * point.z);

		if(r == 0)
		{
			vector1.x = 0.0;
			vector1.z = 0.0;
			vector1.y = point.y;
		}
		else
		{
			vector1.x = point.x / r * sin(r*fov) * point.y;
			vector1.z = point.z / r * sin(r*fov) * point.y;
			vector1.y = cos(r*fov) * point.y;
		}

		//vector1.x = sin(fov * point.x) * point.y;
		//vector1.z = sin(fov * point.z) * point.y;
		//vector1.y = cos(fov * point.x) * cos(fov * point.z) * point.y;

	}
	else if(perspectiveType == params::perspEquirectangular)
	{
		vector1.x = sin(fov * point.x) * cos(fov * point.z) * point.y;
		vector1.z = sin(fov * point.z) * point.y;
		vector1.y = cos(fov * point.x) * cos(fov * point.z) * point.y;
	}
	else //tree-point perspective
	{
		vector1.x = point.x * perspFactor;
		vector1.y = point.y * zoom;
		vector1.z = point.z * perspFactor;
	}

	vector2 = mRot.RotateVector(vector1);

	CVector3 result = vector2 + vp;
	return result;
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
