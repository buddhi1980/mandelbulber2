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


#include "primitives.h"
#include <math.h>

double PrimitivePlane(CVector3 point, CVector3 centre, CVector3 normal)
{
	CVector3 plane = normal;
	plane.Normalize();
	double planeDistance = plane.Dot(point - centre);
	return planeDistance;
}

double PrimitiveInvertedBox(CVector3 point, CVector3 center, CVector3 size)
{
	double distance, planeDistance;
	CVector3 corner1(center.x - 0.5*size.x, center.y - 0.5*size.y, center.z - 0.5*size.z);
	CVector3 corner2(center.x + 0.5*size.x, center.y + 0.5*size.y, center.z + 0.5*size.z);

	planeDistance = PrimitivePlane(point, corner1, CVector3(1,0,0));
	distance = planeDistance;
	planeDistance = PrimitivePlane(point, corner2, CVector3(-1,0,0));
	distance = (planeDistance < distance) ? planeDistance : distance;

	planeDistance = PrimitivePlane(point, corner1, CVector3(0,1,0));
	distance = (planeDistance < distance) ? planeDistance : distance;
	planeDistance = PrimitivePlane(point, corner2, CVector3(0,-1,0));
	distance = (planeDistance < distance) ? planeDistance : distance;

	planeDistance = PrimitivePlane(point, corner1, CVector3(0,0,1));
	distance = (planeDistance < distance) ? planeDistance : distance;
	planeDistance = PrimitivePlane(point, corner2, CVector3(0,0,-1));
	distance = (planeDistance < distance) ? planeDistance : distance;

	return distance;
}

double PrimitiveBox(CVector3 point, CVector3 center, CVector3 size)
{
	double distance, planeDistance;
	CVector3 corner1(center.x - 0.5*size.x, center.y - 0.5*size.y, center.z - 0.5*size.z);
	CVector3 corner2(center.x + 0.5*size.x, center.y + 0.5*size.y, center.z + 0.5*size.z);

	planeDistance = PrimitivePlane(point, corner1, CVector3(-1,0,0));
	distance = planeDistance;
	planeDistance = PrimitivePlane(point, corner2, CVector3(1,0,0));
	distance = (planeDistance > distance) ? planeDistance : distance;

	planeDistance = PrimitivePlane(point, corner1, CVector3(0,-1,0));
	distance = (planeDistance > distance) ? planeDistance : distance;
	planeDistance = PrimitivePlane(point, corner2, CVector3(0,1,0));
	distance = (planeDistance > distance) ? planeDistance : distance;

	planeDistance = PrimitivePlane(point, corner1, CVector3(0,0,-1));
	distance = (planeDistance > distance) ? planeDistance : distance;
	planeDistance = PrimitivePlane(point, corner2, CVector3(0,0,1));
	distance = (planeDistance > distance) ? planeDistance : distance;

	return distance;
}

double PrimitiveSphere(CVector3 point, CVector3 center, double radius)
{
	double distance = (point - center).Length() - radius;
	return distance;
}

double PrimitiveInvertedSphere(CVector3 point, CVector3 center, double radius)
{
	double distance = radius - (point - center).Length();
	return distance;
}

double PrimitiveWater(CVector3 point, double height, double amplitude, double length, double rotation, int iterations, double animSpeed, int frame)
{
	CVector3 plane(0,0,-1);
	CVector3 centre(0,0,height);
	plane.Normalize();
	double planeDistance = plane.Dot(point - centre);
	if(planeDistance < amplitude * 10.0)
	{
		CRotationMatrix rotMatrix;
		rotMatrix.RotateZ(rotation/180*M_PI);
		point = rotMatrix.RotateVector(point);

		double phase = animSpeed * frame;
		double k=0.23;
		double waveXtemp = point.x;
		double waveYtemp = point.y;
		double waveX = 0;
		double waveY = 0;
		double p = 1.0;
		double p2 = 0.05;
		for(int i=1; i<=iterations; i++)
		{
			float p3 = p * p2;
			double shift = phase / (i/3.0 + 1.0);
			waveXtemp = sin(i + 0.4*(waveX)*p3 + sin(k* point.y / length*p3) + point.x/length*p3 + shift)/p;
			waveYtemp = cos(i + 0.4*(waveY)*p3 + sin(point.x / length*p3) + k*point.y/length*p3 + shift*0.23)/p;
			waveX+=waveXtemp;
			waveY+=waveYtemp;
			p2 = p2 + (1.0 - p2) * 0.7;
			p *= 1.872;
		}

		planeDistance += (waveX + waveY) * amplitude;
	}

	return planeDistance;
}
